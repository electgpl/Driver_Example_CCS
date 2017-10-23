/////////////////////////////////////////////////////////////////////////
////                             dns.c                               ////
////                                                                 ////
//// Example driver for performing DNS resolving.                    ////
////                                                                 ////
//// Provides one function:                                          ////
////   dns_resolve_name(socket, dns, port, hostname)                 ////
////     socket - internal socket to use for connection              ////
////     dns - IP address of DNS server                              ////
////     port - TCP/IP Port to connect to on DNS server              ////
////     hostname - Hostname to find IP address, a null terminated   ////
////                string.                                          ////
////                                                                 ////
////     Returns the structury type dns_return_type which contains   ////
////     both the IP address of hostname.  If there was an error     ////
////     then the IP will be 0, and the error condition byte will    ////
////     be set to denote the error which occurred.                  ////
////                                                                 ////
//// Requires s7600.h or compatable TCP/IP stack.                    ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////


typedef struct {
   IPAddr   ip;
   s7600_ec ec;
} dns_return_type;

#define DNS_MAX_RETRY            4
#define DNS_REPLY_WAIT_TIME      5000

s7600_ec dns_gobble_name(int8 socket) {
   int8  t,i,count=0;
	while(1)
	{
		t=tcp_getc(socket);
		count++;

		if(t==0)
		{
			return(OK);
		}
		else if(t==0xc0)
		{
			tcp_getc(socket);
			count++;
			return(OK);
		}
		else
		{
			for(i=0;i<t;i++)
			{
				tcp_getc(socket);
				count++;
				if(count>250)
					return(BAD);
			}
		}
	}
}


dns_return_type dns_resolve_name(int8 socket,  IPAddr dns, int16 port, char * hostname) {
   dns_return_type ret;
   int16 len;
   int16 to, st;
   static int16 dns_id;

   char * ptr;
   int i,j,namesize, retries;

   ret.ec=BAD;
   ret.ip=0;

   for (retries=0; retries<DNS_MAX_RETRY; retries++) {
      tcp_flush_socket(socket);

      ret.ec=tcp_connect(dns,53,socket);

      if (ret.ec==OK) {

         dns_id++;
         len=18 + strlen(hostname);


         tcp_putc(socket, make8(len,1));     tcp_putc(socket, make8(len,0));  //when using TCP, first send 2 byte length field
         tcp_putc(socket, make8(dns_id,1));  tcp_putc(socket, make8(dns_id,0));  //send dns_id
         tcp_putc(socket, 1);                tcp_putc(socket, 0); //
         tcp_putc(socket, 0);                tcp_putc(socket, 1); //qcount = 1
         tcp_putc(socket, 0);                tcp_putc(socket, 0); //ancount = 0
         tcp_putc(socket, 0);                tcp_putc(socket, 0); //nscount = 0
         tcp_putc(socket, 0);                tcp_putc(socket, 0); //arcount = 0

         namesize=1;
         while(*hostname)
         {
            ptr=hostname;
            i=0;
            while(*ptr && (*ptr !='.'))
            {
               i++;
               ptr++;
            }

            tcp_putc(socket, i);
            namesize++;

            while(i--)
            {
               tcp_putc(socket,*hostname);
               hostname++;
               namesize++;
            }

            // Skip period if necessary
            if(*hostname)
            {
               hostname++;
            }
         }
         tcp_putc(socket, 0);
         tcp_putc(socket, 0);    tcp_putc(socket, 1); //qtype = 1
         tcp_putc(socket, 0);    tcp_putc(socket, 1); //qclass = 1

         to=DNS_REPLY_WAIT_TIME;
         while (to && (!tcp_kbhit(socket))) {
            delay_ms(1);
            to--;
         }
         if (!to) {
            ret.ec=TIMEOUT;
         }
         else {
            //tcp_ip length of message
            len=tcp_getc(socket) << 8;
            len+=tcp_getc(socket);

            to=tcp_getc(socket) << 8; //id
            to+=tcp_getc(socket);

            if (to != dns_id) {
               ret.ec=ID_FAIL;
            }
            else {
               st=tcp_getc(socket) << 8; //status
               st+=tcp_getc(socket);

               if (!(st & 0x8000)) {
                  ret.ec=SERVER_ERROR;
               }
               else {

                  for(i=0;i<(12+namesize);i++)  //toss rest of the header
                     tcp_getc(socket);

                  while (tcp_kbhit(socket)) {
                     if (BAD==dns_gobble_name(socket))
                        break;
                     j=tcp_getc(socket);
                     i=tcp_getc(socket);
                     if( (j==0) && (i==1) )
                     {

                        for(i=0;i<8;i++)
                           tcp_getc(socket);
                        ret.ip=make32(tcp_getc(socket), tcp_getc(socket), tcp_getc(socket), tcp_getc(socket));
                        ret.ec=OK;
                        tcp_close(socket);

                        return(ret);
                     }
                     else
                     {
                        // Eat this section
                        for(i=0;i<8;i++)
                           j=tcp_getc(socket);

                        for(i=0;i<j;i++)
                           tcp_getc(socket);
                     }
                  } //strip response
               } //ok status
            } //ok ID
         } //got a response
      } //connected ok
      tcp_close(socket);
   } //retries
   return(ret);
}
