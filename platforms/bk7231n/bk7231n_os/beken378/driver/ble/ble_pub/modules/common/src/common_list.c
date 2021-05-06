/**
 ****************************************************************************************
 *
 * @file common_list.c
 *
 * @brief List management functions
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup COMMON_LIST
 * @{
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <string.h>      // for mem* functions
#include "common_list.h"     // common list definitions

#include "kernel_mem.h"
#include "architect.h"        // for assertions

/*
 * FUNCTION DEFINTIONS
 ****************************************************************************************
 */
void common_list_init(struct common_list *list)
{
    list->first = NULL;
    list->last = NULL;

    #if (KERNEL_PROFILING)
    list->cnt = 0;
    list->maxcnt = 0;
    list->mincnt = 0xFFFFFFFF;
    #endif //KERNEL_PROFILING
}

void common_list_pool_init(struct common_list *list,
                       void *pool,
                       size_t elmt_size,
                       uint32_t elmt_cnt,
                       void *default_value,
                       uint8_t list_type)
{
    uint32_t i;

    // initialize the free list relative to the pool
    common_list_init(list);

    // Add each element of the pool to this list, and init them one by one
    for (i = 0; i < elmt_cnt; i++)
    {
        if (default_value)
        {
            memcpy(pool, default_value, elmt_size);
        }
        if((i == (elmt_cnt - 1)) && (list_type != POOL_LINKED_LIST))
        {
            struct common_list_hdr *list_hdr =(struct common_list_hdr *) pool;
            // Sanity check
            ASSERT_ERR(list_hdr != NULL);

            // check if list is empty
            if (common_list_is_empty(list))
            {
                // list empty => pushed element is also head
                list->first = list_hdr;
            }
            else
            {
                // list not empty => update next of last
                list->last->next = list_hdr;
            }

            // add element at the end of the list
            list->last = list_hdr;
            list_hdr->next = NULL;

            #if (KERNEL_PROFILING)
            list->cnt++;
            if(list->maxcnt < list->cnt)
            {
                list->maxcnt = list->cnt;
            }
            #endif //KERNEL_PROFILING
        }
        else
        {
            common_list_push_back(list, (struct common_list_hdr *) pool);
        }

        // move to the next pool element
        pool = (void *)((uint8_t *)pool + (uint32_t)elmt_size);
    }
}

void common_list_push_back(struct common_list *list,
                       struct common_list_hdr *list_hdr)
{
    // Sanity check
    ASSERT_ERR(list_hdr != NULL);

    // check if list is empty
    if (common_list_is_empty(list))
    {
        // list empty => pushed element is also head
        list->first = list_hdr;
    }
    else
    {
        // list not empty => update next of last
        list->last->next = list_hdr;
    }

    // add element at the end of the list
    list->last = list_hdr;
    list_hdr->next = NULL;

    #if (KERNEL_PROFILING)
    list->cnt++;
    if(list->maxcnt < list->cnt)
    {
        list->maxcnt = list->cnt;
    }
    #endif //KERNEL_PROFILING
}

void common_list_push_front(struct common_list *list,
                        struct common_list_hdr *list_hdr)
{
    // Sanity check
    ASSERT_ERR(list_hdr != NULL);

    // check if list is empty
    if (common_list_is_empty(list))
    {
        // list empty => pushed element is also head
        list->last = list_hdr;
    }

    // add element at the beginning of the list
    list_hdr->next = list->first;
    list->first = list_hdr;

    #if (KERNEL_PROFILING)
    list->cnt++;
    if(list->maxcnt < list->cnt)
    {
        list->maxcnt = list->cnt;
    }
    #endif //KERNEL_PROFILING
}

struct common_list_hdr *common_list_pop_front(struct common_list *list)
{
    struct common_list_hdr *element;

    // check if list is empty
    element = list->first;
    if (element != NULL)
    {

        // The list isn't empty : extract the first element
        list->first = list->first->next;

        if(list->first == NULL)
        {
            list->last = list->first;
        }

        #if (KERNEL_PROFILING)
        list->cnt--;
        if(list->mincnt > list->cnt)
        {
            list->mincnt = list->cnt;
        }
        #endif //KERNEL_PROFILING
    }
    return element;
}

bool common_list_extract(struct common_list *list, struct common_list_hdr *list_hdr, uint8_t nb_following)
{
    bool found = false;

    // sanity check
    ASSERT_ERR(list != NULL);

    struct common_list_hdr *prev = NULL;
    struct common_list_hdr *curr = list->first;

    // Search for the element
    while(curr != NULL)
    {
        // Check element
        if(curr == list_hdr)
        {
            found = true;
            break;
        }

        // Move pointers
        prev = curr;
        ASSERT_INFO(list->first != curr->next, ((uint32_t)curr)>>16, ((uint32_t)curr)&0xFFFF);
        ASSERT_INFO(curr != curr->next, ((uint32_t)curr)>>16, ((uint32_t)curr)&0xFFFF);
        curr = curr->next;
    }

    if(found)
    {
        struct common_list_hdr *last = curr;
        int i = 0;
        // Get the last element to extract
        for(i = nb_following; i > 0; i--)
        {
            // Jump to next
            last = last->next;
            ASSERT_INFO(last != NULL, i, nb_following);
            #if (KERNEL_PROFILING)
            // Following element extracted
            list->cnt--;
            #endif //KERNEL_PROFILING
        }

        // Check if the element is first
        if(prev == NULL)
        {
            // Extract element
            list->first = last->next;
        }
        else
        {
            // Extract element
            prev->next = last->next;
        }

        // Check if the element is last
        if(last == list->last)
        {
            // Update last pointer
            list->last = prev;
        }

        #if (KERNEL_PROFILING)
        // Reference element extracted
        list->cnt--;
        if(list->mincnt > list->cnt)
        {
            list->mincnt = list->cnt;
        }
        #endif //KERNEL_PROFILING
    }

    return found;
}

void common_list_extract_after(struct common_list *list, struct common_list_hdr *elt_ref_hdr, struct common_list_hdr *elt_to_rem_hdr)
{
    // sanity check
    ASSERT_ERR(list != NULL);
    ASSERT_ERR(elt_to_rem_hdr != NULL);

    // Check if the element is first
    if(elt_ref_hdr == NULL)
    {
        ASSERT_ERR(elt_to_rem_hdr == list->first);

        // The list isn't empty : extract the first element
        list->first = list->first->next;
    }
    else
    {
        ASSERT_ERR(elt_to_rem_hdr == elt_ref_hdr->next);

        // Extract element
        elt_ref_hdr->next = elt_to_rem_hdr->next;
    }

    // Check if the element is last
    if(elt_to_rem_hdr == list->last)
    {
        // Update last pointer
        list->last = elt_ref_hdr;
    }

    #if (KERNEL_PROFILING)
    // Reference element extracted
    list->cnt--;
    if(list->mincnt > list->cnt)
    {
        list->mincnt = list->cnt;
    }
    #endif //KERNEL_PROFILING
}

bool common_list_find(struct common_list *list,
                  struct common_list_hdr *list_hdr)
{
    struct common_list_hdr *tmp_list_hdr;

    // Go through the list to find the element
    tmp_list_hdr = list->first;

    while ((tmp_list_hdr != list_hdr) && (tmp_list_hdr != NULL))
    {
        tmp_list_hdr = tmp_list_hdr->next;
    }

    return (tmp_list_hdr == list_hdr);
}

void common_list_merge(struct common_list *list1,
                   struct common_list *list2)
{
    // Sanity check: list2 is not supposed to be empty
    ASSERT_ERR(!common_list_is_empty(list2));

    // just copy list elements
    if(common_list_is_empty(list1))
    {
        list1->first = list2->first;
        list1->last  = list2->last;
    }
    // merge lists
    else
    {
        // Append list2 to list1
        list1->last->next = list2->first;
        list1->last = list2->last;

    }

    // Empty list2
    list2->first = NULL;

    #if (KERNEL_PROFILING)
    list1->cnt += list2->cnt;
    list2->cnt = 0;
    #endif //KERNEL_PROFILING
}

void common_list_insert_before(struct common_list *list,
                        struct common_list_hdr *elt_ref_hdr, struct common_list_hdr *elt_to_add_hdr)
{
    // Sanity check
    ASSERT_ERR(elt_to_add_hdr != NULL);
    // If no element referenced
    if(elt_ref_hdr == NULL)
    {
        common_list_push_front(list,elt_to_add_hdr);
    }
    else
    {
        struct common_list_hdr *tmp_list_prev_hdr = NULL;
        struct common_list_hdr *tmp_list_curr_hdr;

        // Go through the list to find the element
        tmp_list_curr_hdr = list->first;

        while ((tmp_list_curr_hdr != elt_ref_hdr) && (tmp_list_curr_hdr != NULL))
        {
            // Save previous element
            tmp_list_prev_hdr = tmp_list_curr_hdr;
            // Get the next element of the list
            tmp_list_curr_hdr = tmp_list_curr_hdr->next;
        }
        // If only one element is available
        if(tmp_list_prev_hdr == NULL)
        {
            common_list_push_front(list,elt_to_add_hdr);
        }
        else
        {
            tmp_list_prev_hdr->next = elt_to_add_hdr;
            elt_to_add_hdr->next = tmp_list_curr_hdr;
            #if (KERNEL_PROFILING)
            list->cnt++;
            if(list->maxcnt < list->cnt)
            {
                list->maxcnt = list->cnt;
            }
            #endif //KERNEL_PROFILING
        }
    }
}

void common_list_insert_after(struct common_list *list,
                        struct common_list_hdr *elt_ref_hdr, struct common_list_hdr *elt_to_add_hdr)
{
    // Sanity check
    ASSERT_ERR(elt_to_add_hdr != NULL);
    // If no element referenced
    if(elt_ref_hdr == NULL)
    {
        common_list_push_back(list,elt_to_add_hdr);
    }
    else
    {
        struct common_list_hdr *tmp_list_curr_hdr;

        // Go through the list to find the element
        tmp_list_curr_hdr = list->first;

        while ((tmp_list_curr_hdr != elt_ref_hdr) && (tmp_list_curr_hdr != NULL))
        {
             // Get the next element of the list
            tmp_list_curr_hdr = tmp_list_curr_hdr->next;
        }
        // If only one element is available
        if(tmp_list_curr_hdr == NULL)
        {
            common_list_push_back(list,elt_to_add_hdr);
        }
        else
        {
            // Check if the found element was the last of the list
            if (!tmp_list_curr_hdr->next)
            {
                // Update last pointer
                list->last = elt_to_add_hdr;
            }

            elt_to_add_hdr->next = tmp_list_curr_hdr->next;
            tmp_list_curr_hdr->next = elt_to_add_hdr;

            #if (KERNEL_PROFILING)
            list->cnt++;
            if(list->maxcnt < list->cnt)
            {
                list->maxcnt = list->cnt;
            }
            #endif //KERNEL_PROFILING
        }
    }
}

uint16_t common_list_size(struct common_list *list)
{
    uint16_t count = 0;
    struct common_list_hdr *tmp_list_hdr = list->first;

    // browse list to count number of elements
    while (tmp_list_hdr != NULL)
    {
        tmp_list_hdr = tmp_list_hdr->next;
        count++;
    }

    return count;
}

bool common_list_check_size_available(struct common_list *list, uint8_t free_size_needed)
{
    uint16_t count = 0;
    struct common_list_hdr *tmp_list_hdr = list->first;

    // browse list to count number of elements
    while (tmp_list_hdr != NULL)
    {
        tmp_list_hdr = tmp_list_hdr->next;
        count++;
        if(count == free_size_needed)
            return true;
    }

    return false;
}

/// @} COMMON_LIST
