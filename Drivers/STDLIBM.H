///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2011 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////


/*********************************************************************/
#ifndef _STDLIBM
#define _STDLIBM
#USE DYNAMIC_MEMORY
/* Memory Management Functions*/
#include <stddef.h>
#if defined(__PCB__)
typedef struct nodet {
   unsigned int8 size;
   unsigned int8 next; }node_t;
#elif defined(__PCM__)
typedef struct nodet {
   unsigned int8 size;
   unsigned int16 next; }node_t;
#elif defined(__PCH__)
typedef struct nodet {
   unsigned int16 size;
   unsigned int16 next; }node_t;
#elif defined(__PCD__)
typedef struct nodet {
   unsigned int16 size;
   unsigned int16 next; }node_t;
#endif


#ifndef debug_stdlibm
   #define debug_stdlibm(s)
#else
   #define __DO_DEBUG_STDLIBM
   
   char g_DebugStdlibmStr[50];
   
   #if defined(__PCD__)
      #if (defined(__PIC24E__)||defined(__dsPIC33E__))
         #define INVALID_MEMORY_LOCATION(x) ((x >= (getenv("RAM")+0x800)) || bit_test(x, 0))
      #else
         #define INVALID_MEMORY_LOCATION(x) ((x >= (getenv("RAM")+0x1000)) || bit_test(x, 0))
      #endif
   #else
      #define INVALID_MEMORY_LOCATION(x) (x >= getenv("RAM"))
   #endif
#endif

#include <memmgmt.c>

void traverse()
{
   node_t *node,*temp;
   unsigned int16 nsize,nextsize;
   node=__DYNAMIC_HEAD;
   while(node!=NULL)
   {
     #if defined(__DO_DEBUG_STDLIBM)
      if (INVALID_MEMORY_LOCATION(node))
      {
         sprintf(g_DebugStdlibmStr, "traverse() invalid node (0x%LX)\r\n", node);
         debug_stdlibm(g_DebugStdlibmStr);
         break;
      }   
     #endif
     if(!bit_test(node->size,_MEMMGMT_POS))// node free
      {
         nsize=node->size;
         temp=(unsigned int16)node->next;
        #if defined(__DO_DEBUG_STDLIBM)
         if (INVALID_MEMORY_LOCATION(temp))
         {
            sprintf(g_DebugStdlibmStr, "traverse() invalid temp (0x%LX)\r\n", node);
            debug_stdlibm(g_DebugStdlibmStr);
            break;
         }
        #endif
         if(!bit_test(temp->size,_MEMMGMT_POS)&& (temp==((unsigned int16)node+nsize+sizeof(node_t))))//next node free and consecutive, so combine
         {
            nextsize=temp->size;
            nsize+=nextsize+sizeof(node_t);
            remove_node(temp);
            update_node(node,nsize);
         }
         else
         node=node->next;
      }
      else
      node=node->next;
   }
}

char *malloc(size_t size)
{
   node_t *node,*new;
   unsigned int16 nsize;
   #if defined(__PCD__)
   if (size % 2)
      size++;
   #endif
   node=__DYNAMIC_HEAD;
   
  #if defined(__DO_DEBUG_STDLIBM)
   sprintf(g_DebugStdlibmStr, "malloc() size=%lu\r\n", size);
   debug_stdlibm(g_DebugStdlibmStr);
  #endif
        
   while(node!=NULL) // chk until end of memlist
   {
     #if defined(__DO_DEBUG_STDLIBM)
      if (INVALID_MEMORY_LOCATION(node))
      {
         sprintf(g_DebugStdlibmStr, "malloc() invalid node (0x%LX)\r\n", node);
         debug_stdlibm(g_DebugStdlibmStr);
         print_list();
         return(0);
      }
     #endif
      if(!bit_test(node->size,_MEMMGMT_POS) && node->size >=size) // node is free and > = req size
      {
         nsize=node->size;    //nsize = size of the node that "node" being pointed at
         if(nsize>size +sizeof(node_t)) //node > req size, so split and add new node to memlist
         {
            new=create_node(nsize-size-sizeof(node_t),(unsigned int16)node+sizeof(node_t)+size);
            insert_node_after(node,new);
            update_node(node,size+_MEMMGMT_CSIZE);
         }
         else//not enough space for new node so use original size
         update_node(node,nsize+_MEMMGMT_CSIZE);
         //end if
         break;
      }//end if
      node=node->next;
   }//end while
   if(node==NULL)// reached end without finding an appropriate node
   {
      debug_stdlibm("Not enough memory for mallocation\r\n");
      return NULL;
   }
   else
   return (char *)node+sizeof(node_t); // return pounsigned int8er to allocated space
}

char *calloc(size_t nmemb,size_t size)
{
   node_t *node,*new;
   unsigned int16 nsize,resize;
   node=__DYNAMIC_HEAD;
   resize=nmemb*size;
   #if defined(__PCD__)
   if(resize%2)
      resize++;
   #endif
   while(node!=NULL) // chk until end of memlist
   {
     #if defined(__DO_DEBUG_STDLIBM)
      if (INVALID_MEMORY_LOCATION(node))
      {
         sprintf(g_DebugStdlibmStr, "malloc() invalid node (0x%LX)\r\n", node);
         debug_stdlibm(g_DebugStdlibmStr);
         print_list();
         return(0);
      }
     #endif
      if(!bit_test(node->size,_MEMMGMT_POS) && node->size >=resize)// node is free and > = req size
      {
         nsize=node->size;
         if(nsize>resize+sizeof(node_t))//node > req size, so split and add new node to memlist
         {
            new=create_node(nsize-resize-sizeof(node_t),(unsigned int16)node+sizeof(node_t)+resize);
            insert_node_after(node,new);
            update_node(node,resize+_MEMMGMT_CSIZE);
         }
         else//not enough space for new node so use original size
         update_node(node,nsize+_MEMMGMT_CSIZE);
         //end if
         break;
      }//end if
      node=node->next;
   }//end while
   if(node==NULL)// reached end without finding an appropriate node
   {
      debug_stdlibm("Not enough memory for callocation\r\n");
      return NULL;
   }
   else
   {
      memset((unsigned int16)node+sizeof(node_t),0,resize);// initialize to 0
      return (char *)(unsigned int16)node+sizeof(node_t);// return pounsigned int8er to allocated space
   }
}
void free( void * ptr)
{
   node_t *node;
   unsigned int16 nsize;

  #if defined(__DO_DEBUG_STDLIBM)
   sprintf(g_DebugStdlibmStr, "free() 0x%LX\r\n", ptr);
   debug_stdlibm(g_DebugStdlibmStr);
  #endif

   if(ptr==NULL) // not a valid pounsigned int8er
      return;
   else
   {
      node=ptr-sizeof(node_t);
      if(bit_test(node->size,_MEMMGMT_POS))// node occupied
      {
         nsize=node->size-_MEMMGMT_CSIZE;
         update_node(node,nsize);
         ptr=NULL;

      }
      else // wrong input, return
      {
         ptr=NULL;
         return;
      }
   }
   traverse();
}

char *realloc(void *ptr,size_t size)
{
   node_t *node,*new,*temp;
   unsigned int16 nsize,nextsize;
   char *newptr;
   #if defined(__PCD__)
   if(size %2)
      size++;
   #endif
   if(ptr==NULL)// null pounsigned int8er, so malloc the req memory
      return(malloc(size));
   else if(size==0)
   {
      free(ptr);
      return(NULL);
   }
   else
   {
      node=ptr-sizeof(node_t);
      if(bit_test(node->size,_MEMMGMT_POS))// chk if valid pounsigned int8er
      {
         nsize=node->size-_MEMMGMT_CSIZE;
         temp=(unsigned int16)node->next;
         if(nsize>size)// block > req size
         {
       
               if(!bit_test(temp->size,_MEMMGMT_POS) && (temp==((unsigned int16)node+nsize+sizeof(node_t))))// next block free and consecutive
               {
                  update_node(node,size+_MEMMGMT_CSIZE); // update block
                  nextsize=temp->size;
                  remove_node(temp);
                  new=create_node(nextsize+(nsize-size),(unsigned int16)node+size+sizeof(node_t));
                  insert_node_after(node,new);
                  

               }
               else if (nsize>size +sizeof(node_t))//node > req size, so split and add new node to memlist
               {
                  update_node(node,size+_MEMMGMT_CSIZE); // update block
                  new=create_node(nsize-size-sizeof(node_t),(unsigned int16)node+sizeof(node_t)+size);
                  insert_node_after(node,new);
               }
               else//not enough space for new node so use original size
               update_node(node,nsize+_MEMMGMT_CSIZE); // update block

         }
         else // block < req size
         {
            if(!bit_test(temp->size,_MEMMGMT_POS) && (temp==((unsigned int16)node+nsize+sizeof(node_t))))// next block free and consecutive
            {
               nextsize=temp->size;
               if(nextsize>=size-nsize) // next block >=difference
               {
                  if(nextsize>size-nsize+sizeof(node_t))//next node > req size, so split and add new node to memlist
                  {
                      update_node(node,size+_MEMMGMT_CSIZE);// update block
                      remove_node(temp);
                      new=create_node(nextsize-(size-nsize),(unsigned int16)node+size+sizeof(node_t));
                      insert_node_after(node,new);
                  }
                  else//not enough space for new node in next node, so use original size
                  {
                      update_node(node,nsize+nextsize+_MEMMGMT_CSIZE);// update block
                      remove_node(temp);
                  }
               }
               else  //next block free but too small for new size
               {
                  newptr = malloc(size);  //use malloc to find new block
                  if(newptr == NULL)
                     return(NULL);        //return NULL if malloc was unable to find new block
                   
                  memcpy(newptr, ptr, nsize);   //copy original data to new block
                  free(ptr);                    //free original block
                  return(newptr);               //return new pointer
               }
            }
            else  //next block not free
            {
               newptr = malloc(size);  //use malloc to find new block
               if(newptr == NULL)      
                  return(NULL);        //return NULL if malloc was unable to find new block
               
               memcpy(newptr, ptr, nsize);   //copy original data to new block
               free(ptr);                    //free original block
               return(newptr);               //return new pointer
            }
         }
         return (char *)node+sizeof(node_t); // return pounsigned int8er to the reallocated block
      }
      else // not allocated use malloc
      {
         return(malloc(size));
      }
   }
 }
#ENDIF
