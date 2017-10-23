/////////////////////////////////////////////////////////////////////////
////                        XTEA.h                                   ////
////                   XTEA cipher library                           ////
////                                                                 ////
//// This library implements the Extended Tiny Encryption Algorithm  ////
////                                                                 ////
//// XTEA_encrypt(*v,*k) Encrypts 64b long data with 128b long key   ////
////                                                                 ////
//// XTEA_decrypt(*v,*k) Decrypts 64b long data with 128b long key   ////
////                                                                 ////
//// XTEA_encrypt_string() - Encrypts a string                       //// ////                                                                 ////     
//// XTEA_decrypt_string() - Decrypts a string                       ////
////                                                                 ////     
//// See function below for more details                             ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2006 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#define XTEA_ROUNDS         ((unsigned int)64)
#define XTEA_DELTA          ((unsigned int32)0x9E3779B9)

/*
void XTEA_encrypt(int32* v, int32* k);
This function encrypts 64b long data with 128b long key
PARAM v:    - Data to Encrypt
PARAM k    - key
RETURNS: none
*/
void XTEA_encrypt(unsigned int32* v, unsigned int32* k)
{
    unsigned int32 v0, v1;
    unsigned int i;
    unsigned int32 sum=0;

    v0=v[0];
    v1=v[1];

    for(i=0; i < XTEA_ROUNDS; i++)
    {
        v0 += ((v1 << (unsigned int32)4 ^ v1 >> (unsigned int32)5) + v1) ^ (sum + k[sum & (unsigned int32)3]);
        sum += XTEA_DELTA;
        v1 += ((v0 << (unsigned int32)4 ^ v0 >> (unsigned int32)5) + v0) ^ (sum + k[sum>>11 & (unsigned int32)3]);
    }
    v[0]=v0; v[1]=v1;
}

/*
void XTEA_decrypt(int32* v, int32* k);
This function decrypts 64b long data with 128b long key
PARAM v:    - Decripted data
PARAM k    - key
RETURNS: none
*/
void XTEA_decrypt(unsigned int32* v, unsigned int32* k)
{
    unsigned int32 v0, v1;
    int i;
    unsigned int32 sum;

    v0=v[0];
    v1=v[1];

    sum = XTEA_ROUNDS * XTEA_DELTA;

    for(i=0; i < XTEA_ROUNDS; i++) {
        v1 -= ((v0 << 4 ^ v0 >> 5) + v0) ^ (sum + k[sum>>11 & 3]);
        sum -= XTEA_DELTA;
        v0 -= ((v1 << 4 ^ v1 >> 5) + v1) ^ (sum + k[sum & 3]);
    }
    v[0]=v0; v[1]=v1;
}

/*
void XTEA_encrypt_string (char* data_plain, int length_plain, char* data_crypt, int length_crypt,  int32* key);
This function will encrypt a string of data with 128b long key
PARAM data_plain: String to be Encripted
PARAM length_plain: Length of string
PARAM data_crypt: Encrypted data
PARAM length_crypt: Length of encrypted data
PARAM key: key
RETURNS: none
*/
void XTEA_encrypt_string (char* data_plain, unsigned int length_plain, char* data_crypt, unsigned int length_crypt, unsigned int32* key)
{
   char tmp[8];
   unsigned int8 i,j,m;

   for (i=0; i < length_crypt; i+=8) 
   {
      for (j=0; j<8; j++) 
      {
         m = i+j;

         if (m < length_plain) 
         {
            tmp[j]=data_plain[m];
         }
         else
            tmp[j]=0;
      }

      XTEA_encrypt(tmp, key);

      for (j=0; j<8; j++) 
      {
         m = i+j;
         
         data_crypt[m]=tmp[j];
      }
   }
}

/*
void XTEA_decrypt_string (char* data_plain, int length_plain, char* data_crypt, int length_crypt,  int32* key);
This function will decrypt a string of data with a 128b long key
PARAM data_plain: String to be decripted
PARAM length_plain: Length of string
PARAM data_crypt: decrypted data
PARAM length_crypt: Length of decrypted data
PARAM key: key
RETURNS: none
*/
void XTEA_decrypt_string (char* data_plain, unsigned int length_plain, char* data_crypt, unsigned int length_crypt,  unsigned int32* key)
{
   char tmp[8];
   unsigned int8 i,j,m;

   for (i=0; i < length_crypt; i+=8) 
   {
      for (j=0; j<8; j++) 
      {
         m = i+j;
         tmp[j]=data_crypt[m];
      }

      XTEA_decrypt(tmp, key);

      for (j=0; j < 8; j++) 
      {
         m = i+j;
         if (m < length_plain) 
         {
            data_plain[m]=tmp[j];
         }
      }
   }
}

