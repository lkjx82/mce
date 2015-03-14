#ifndef __LBH_C_LIST_H__
#define __LBH_C_LIST_H__

#include <stdlib.h>
#include <string.h>
#include <stddef.h>

// -----------------------------------------------------------------------------

#define list_offset(type, member) (((size_t) &((type *)0xFF)->member) -0xFF)


#define list_container_of(nodeptr, type, member) 		\
			((type *)((char *)(nodeptr) - list_offset(type, member)))

#define list_node_of(contptr, type, member)            \
			((C_List_Node *)((char *)(contptr) + list_offset(type, member)))



//#define list_node_of(ptr, type, member) ({
//    const typeof( ((type *)0)->member ) *__mptr = (ptr);
//    (type *)( (char *)__mptr - offsetof(type,member) );})

// -----------------------------------------------------------------------------

struct C_List_Node
{
    struct C_List_Node * prev;
    struct C_List_Node * next;
};

// -----------------------------------------------------------------------------

struct C_List
{
    C_List_Node origin;
    //size_t     size;
};

// -----------------------------------------------------------------------------

extern size_t list_size (C_List * l);

// 把 data插到 n 前
extern C_List_Node * list_insert (C_List_Node * n, void * data);

extern C_List_Node * list_erase (C_List_Node * n);

#define list_clear(list)    do {    \
    (list)->origin.next = &(list)->origin;  \
    (list)->origin.prev = &(list)->origin;  \
}while (0);

#define list_begin(list) ((list)->origin.next)
#define list_end(list) (&(list)->origin)

#define list_push_front(list, data) list_insert (list_begin (list), data);
#define list_push_back(list, data) list_insert (list_end (list), data);
#define list_pop_front(list) list_erase (list_front (list));
#define list_pop_back(list) list_erase (list_back (list));

#define list_empty(list) (list_begin(list) == list_end(list))

#define list_front(list) ((list)->origin.next)
#define list_back(list) ((list)->origin.prev)

#define list_node_next(node) ((node)->next)
#define list_node_prev(node) ((node)->prev)

// -----------------------------------------------------------------------------

#endif // __LBH_C_LIST_H__
