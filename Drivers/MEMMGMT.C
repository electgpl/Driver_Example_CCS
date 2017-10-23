///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

#if defined(__PCH__)
 #define _MEMMGMT_CSIZE 32768
 #define _MEMMGMT_POS 15
#elif defined(__PCD__)
   #define _MEMMGMT_CSIZE 32768
   #define _MEMMGMT_POS 15
#else
   #define _MEMMGMT_CSIZE 127
   #define _MEMMGMT_POS 7
#endif

node_t * create_node(unsigned int16 size, node_t *ptr) // create node at given location
{
   node_t *result;
   result =ptr;
   result->size=size;
   result->next = NULL;
   return result;
}
void update_node(node_t *node, unsigned int16 size) // update the size of given node
{
   node->size=size;
}

/* Insert node immediately after place */ //old,new
void insert_node_after(node_t *place, node_t *node)// place the node after another given node
{
    if (place->next==NULL)
       node->next= NULL;
    else
       node->next=place->next;
    place->next=node;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void remove_node(node_t *node) {// remove the given node from the memlist
   node_t *ptr;
   for(ptr=__DYNAMIC_HEAD;ptr->next!=node;ptr=ptr->next);
   ptr->next=node->next;
   node=NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(__DO_DEBUG_STDLIBM)
void print_list() { // print the current memlist
    node_t *node;
    debug_stdlibm("\r\nThe list is \n\r");
    for(node = __DYNAMIC_HEAD; node != NULL; node = node->next)
    {
        sprintf(g_DebugStdlibmStr, "H:0x%lx S:0x%lx N:0x%lx\n\r", node, node->size, node->next);
        debug_stdlibm(g_DebugStdlibmStr);
        if (INVALID_MEMORY_LOCATION(node->next))
        {
           debug_stdlibm("Breaking because of invalid next node\r\n");
           break;
        }   
    }
    sprintf(g_DebugStdlibmStr, "size of node_t %u\r\n",sizeof(node_t));
    debug_stdlibm(g_DebugStdlibmStr);
}
#endif


