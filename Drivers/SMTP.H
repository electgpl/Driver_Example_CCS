///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

struct {
   int1 open;
   int8 socket;
} email_status;

void email_init(void);
s7600_ec email_open(int32 ip, int16 port, int8 * from, int8 * to, int8 * subject);
s7600_ec email_add(char * c);
s7600_ec email_close();

/********************************************************************
//
// function: email_init
//
// description: Initializes the e-mail code
//
// input: none
// output: global variable, email_status, changed
// calls: none
//
//*******************************************************************/
void email_init(void)
{
   email_status.open=0;
}

/********************************************************************
//
// function: email_open
//
// description: Opens a connection to an SMTP server and initializes the email
//              message.  Must be called before other data can be sent to email.
//
// input: ip (IP address of SMTP server to connect to), port (port number of
//        SMTP server, usually 25), from (null terminated char array of the sender's
//        email address), to (null terminated char array of the receiving user's
//        email address), subject (null terminated char array of the subject line).
//
// output: s7600_ec (enumerated data type that details error).
// calls: tcp_find_free_socket(), tcp_connect(), email_add(), tcp_flush_socket()
//
//*******************************************************************/
s7600_ec email_open(int32 ip, int16 port, int8 * from, int8 * to, int8 * subject)
{
   s7600_ec ec;
   int8 string[12];
   int8 crlf[]="\r\n";

   email_status.socket=tcp_find_free_socket();
   if (email_status.socket == 0xFF) {return(NO_FREE_SOCKET);}   //tcp socket didn't connect right

   else {                                          //tcp socket connected right
      ec=tcp_connect(ip,port,email_status.socket);
      if (ec == OK) {
         //printf(debug_lcd,"\fSocket %u open...",email_status.socket);
         delay_ms(1000);
         email_status.open=1;

         sprintf(string,"mail from:");
         email_add(string);
         email_add(from);
         email_add(crlf);

         sprintf(string,"rcpt to:");
         email_add(string);
         email_add(to);
         email_add(crlf);

         sprintf(string,"DATA");
         email_add(string);
         email_add(crlf);

         sprintf(string,"From: ");
         email_add(string);
         email_add(from);
         email_add(crlf);

         sprintf(string,"To: ");
         email_add(string);
         email_add(to);
         email_add(crlf);

         sprintf(string,"Subject: ");
         email_add(string);
         email_add(subject);
         email_add(crlf);

         tcp_flush_socket(email_status.socket);

         return(OK);
      }
      else {
         //printf(debug_lcd,"\fCouldn't open socket %U! %U",email_status.socket,ec);
         return(ec);
      }
   }
}

/********************************************************************
//
// function: email_add
//
// description: Adds more data to the already opened email message
//
// input: c (null terminated char array of the data to add to message)
// output: s7600_ec (enumerated data type that details the error)
// calls: strlen(), tcp_putd()
//
//*******************************************************************/
s7600_ec email_add(char * c)
{
   int8 len;
   s7600_ec ec=SOCKET_NOT_CONNECTED;

   if (email_status.open) {
      len=strlen(c);
      ec=tcp_putd(email_status.socket, c, len);
   }

   return(ec);
}

/********************************************************************
//
// function: email_close
//
// description: Finishes the email message and closes the socket.
//
// input: none
// output: s7600_ec (enumerated data type that details the error)
// calls: tcp_putd(), tcp_flush_socket(), tcp_close()
//
//*******************************************************************/
s7600_ec email_close()
{
   int8 end[]="\r\n.\r\n";
   s7600_ec ec=SOCKET_NOT_CONNECTED;

   if (email_status.open) {
      ec=tcp_putd(email_status.socket,end,5);
      if (ec != OK) {return(ec);}

      tcp_close(email_status.socket);

      tcp_flush_socket(email_status.socket);

      email_status.open=0;
   }
   return(ec);
}

