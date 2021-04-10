/***********************************************************
*  File: uni_hlist.h
*  Author: nzy
*  Date: 180721
***********************************************************/
#ifndef _UNI_HLIST_H
#define _UNI_HLIST_H

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _UNI_HLIST_GLOBAL
    #define _UNI_HLIST_EXT 
#else
    #define _UNI_HLIST_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
typedef struct hlist_node {
    struct hlist_node *next,**pprev;
}HLIST_NODE;

typedef struct  hlist_head{
    struct hlist_node *first;
}HLIST_HEAD;

#define HLIST_HEAD_INIT { .first = NULL}
#define HLIST_HEAD(name) HLIST_HEAD name = {.first = NULL}
#define INIT_HLIST_HEAD(ptr) ((ptr->first)=NULL)

#define HLIST_ENTRY(ptr, type, member) CNTR_OF(ptr,type,member)
#define HLIST_FOR_EACH_ENTRY(tpos, type, pos, head, member) \
    for (pos = (head)->first;                     \
         pos && (tpos = HLIST_ENTRY(pos, type, member), 1); \
         pos = pos->next)

#define HLIST_FOR_EACH_ENTRY_CURR(tpos, type, pos, curr, member) \
    for (pos = (curr)->next;                     \
         pos && (tpos = HLIST_ENTRY(pos, type, member), 1); \
         pos = pos->next)

#define HLIST_FOR_EACH_ENTRY_SAFE(tpos, type, pos, n, head, member) \
    for (pos = (head)->first; \
         pos && (n = pos->next, 1) && \
        (tpos = HLIST_ENTRY(pos, type, member), 1); \
         pos = n)

#define HLIST_FOR_EACH(pos, head) \
    for (pos = (head)->first; pos ; \
         pos = pos->next)

#define HLIST_FOR_EACH_SAFE(pos, n, head) \
    for (pos = (head)->first; pos && ({ n = pos->next; 1; }); \
         pos = n)

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
STATIC INLINE VOID tuya_init_hlist_node(INOUT HLIST_NODE *h) 
{
    h->next = NULL;
    h->pprev = NULL;
} 

STATIC INLINE INT_T tuya_hlist_empty(IN CONST HLIST_HEAD *h) 
{
    return !h->first;
}

STATIC INLINE INT_T tuya_hlist_unhashed(IN CONST HLIST_NODE *h) 
{
    return !h->pprev;
}

STATIC INLINE VOID __tuya_hlist_del(INOUT HLIST_NODE *n) 
{
    HLIST_NODE *next =  n->next;  
    HLIST_NODE **pprev = n->pprev;
    *pprev = next;
    if (next)
        next->pprev = pprev;
}

STATIC INLINE VOID tuya_hlist_del(INOUT HLIST_NODE *n) 
{
    __tuya_hlist_del(n);
    // n->next = NULL;
    // n->pprev = NULL;
}

STATIC INLINE VOID tuya_hlist_del_init(INOUT HLIST_NODE *n) 
{
    if (!tuya_hlist_unhashed(n)) {
        __tuya_hlist_del(n);
        tuya_init_hlist_node(n);
    }
}

STATIC INLINE VOID tuya_hlist_add_head(INOUT HLIST_NODE *n, INOUT HLIST_HEAD *h) 
{
    struct hlist_node *first = h->first;
    n->next = first;
    if (first)
        first->pprev = &n->next;
    h->first = n;
    n->pprev = &h->first;
}

/* next must be != NULL */
// insert n before next
STATIC INLINE VOID tuya_hlist_add_before(INOUT HLIST_NODE *n,\
                                         INOUT HLIST_NODE *next) 
{
    n->pprev = next->pprev;
    n->next = next;
    next->pprev = &n->next;
    *(n->pprev) = n;
}

// insert next after n
STATIC INLINE VOID tuya_hlist_add_after(INOUT HLIST_NODE *n,
                                        INOUT HLIST_NODE *next) 
{
    next->next = n->next;
    n->next = next;
    next->pprev = &n->next;

    if(next->next)
        next->next->pprev  = &next->next;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
