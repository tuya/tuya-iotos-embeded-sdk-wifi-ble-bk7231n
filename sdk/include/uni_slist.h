/***********************************************************
*  File: uni_slist.h
*  Author: nzy
*  Date: 180803
***********************************************************/
#ifndef _UNI_SLIST_H
#define _UNI_SLIST_H

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _UNI_SLIST_GLOBAL
    #define _UNI_SLIST_EXT 
#else
    #define _UNI_SLIST_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
typedef struct slist_head {
    struct slist_head *next;
}SLIST_HEAD;

// 定义LIST并静态初始化一个空的通用双向链表
#define SLIST_HEAD(name) \
SLIST_HEAD name = {NULL}

// 动态初始化一个空的通用双向链表
#define INIT_SLIST_HEAD(ptr) do { \
    (ptr)->next = NULL; \
} while (0)

#define NEW_SLIST_NODE(type,node) \
{\
    node = (type *)Malloc(sizeof(type));\
}

#define SLIST_ENTRY(ptr, type, member) CNTR_OF(ptr,type,member)

// 遍历链表
#define SLIST_FOR_EACH_ENTRY(tpos, type, pos, list, member) \
    for (pos = (list)->next;                     \
         pos && (tpos = SLIST_ENTRY(pos, type, member), 1); \
         pos = pos->next)

#define SLIST_FOR_EACH_ENTRY_SAFE(tpos, type, pos, n, list, member) \
    for (pos = (list)->next; \
         pos && (n = pos->next, 1) && \
        (tpos = SLIST_ENTRY(pos, type, member), 1); \
         pos = n)

#define SLIST_FOR_EACH(pos, list) \
    for (pos = (list)->next; pos ; \
         pos = pos->next)

#define SLIST_FOR_EACH_SAFE(pos, n, list) \
    for (pos = (list)->next; pos && ({ n = pos->next; 1; }); \
         pos = n)

#define FREE_SLIST_SAFE(tpos, type, pos, n, list, member) \
{\
    type *posnode; \
    SLIST_FOR_EACH_ENTRY_SAFE(tpos, type, pos, n, list, member) { \
        list->next = n; \
        posnode = tpos; \
        Free(posnode); \
    } \
}

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
STATIC INLINE VOID tuya_init_slist_node(INOUT SLIST_HEAD *node) 
{
    node->next = NULL;
} 

STATIC INLINE INT_T tuya_slist_empty(IN CONST SLIST_HEAD *list) 
{
    return !(list->next);
}

STATIC INLINE VOID tuya_slist_del(INOUT SLIST_HEAD *list,INOUT SLIST_HEAD *node) 
{
    SLIST_HEAD *pos = NULL;
    SLIST_HEAD *last = list;

    SLIST_FOR_EACH(pos, list) {
        if(pos != node) {
            last = pos;
            continue;
        }

        // success find and delete
        last->next = pos->next;
        node->next = NULL;
        break;
    }
}

STATIC INLINE VOID tuya_slist_add_head(INOUT SLIST_HEAD *list,INOUT SLIST_HEAD *n) 
{
    n->next = list->next;
    list->next = n;
}

/* next must be != NULL */
// insert n before next
STATIC INLINE VOID tuya_slist_add_tail(INOUT SLIST_HEAD *list,INOUT SLIST_HEAD *n) 
{
    SLIST_HEAD *pos = NULL;
    SLIST_HEAD *last = list;

    SLIST_FOR_EACH(pos, list) {
        last = pos;
    }
    last->next = n;   
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
