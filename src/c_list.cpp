#include "../include/c_list.h"

// -----------------------------------------------------------------------------

size_t list_size (C_List * l)
{
    C_List_Node    * pos = 0;
    size_t      size = 0;

    for (pos = list_begin (l);
        pos != list_end (l);
        pos = list_node_next (pos)) {
            size ++;
    }
    return size;
}

// -----------------------------------------------------------------------------

C_List_Node * list_insert (C_List_Node * n, void * data)
{
    C_List_Node * ret = (C_List_Node *) data;
    ret->prev = n->prev;
    ret->next = n;
    ret->prev->next = ret;
    n->prev = ret;
    return ret;
}

// -----------------------------------------------------------------------------

C_List_Node * list_erase (C_List_Node * n)
{
    n->prev->next = n->next;
    n->next->prev = n->prev;
    return n;
}

// -----------------------------------------------------------------------------
