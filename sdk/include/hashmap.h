/*
 * Generic map implementation.
 */

#ifndef __HASHMAP_H__
#define __HASHMAP_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAP_MISSING -2  /* No such element */
#define MAP_OMEM -1 	/* Out of Memory */
#define MAP_OK 0 	/* OK */

/*
 * any_t is a pointer.  This allows you to put arbitrary structures in
 * the hashmap.
 */
typedef void *any_t;

/*
 * map_t is a pointer to an internally maintained data structure.
 * Clients of this package do not need to know how hashmaps are
 * represented.  They see and manipulate only map_t's.
 */
typedef any_t map_t;    

/*
 * Return an empty hashmap. Returns NULL if empty.
 * malloc_fn MUST SET TO Malloc
 * free_fn   MUST SET TO Free
*/
extern map_t hashmap_new(unsigned int table_size);

/*
 * Add an element to the hashmap. Return MAP_OK or MAP_OMEM.
 * For same key, it does not replace it. it is inserted in the head of the list
 */
extern int hashmap_put(map_t in, const char* key ,const any_t data);

/*
 * Get an element from the hashmap. Return MAP_OK or MAP_MISSING.
 *arg is the first value that the key matches
 */
extern int hashmap_get(map_t in, const char* key, any_t *arg);

/*
 * traverse all data with same key
 */

typedef any_t * any_t_iter;
extern int hashmap_data_traversal(map_t in, const char* key, any_t_iter *arg_iterator);
#define HASHMAP_FOR_EACH_DATA(in, key, data_iter) \
    for(data_iter = NULL; hashmap_data_traversal(in, key, (any_t_iter *)&data_iter) == MAP_OK; /*empty*/)

/*
 * Remove an element from the hashmap. Return MAP_OK or MAP_MISSING.
 * if data is NULL,then delete the first note match key.if data is not null,
 * then delete the node match key and data.
 */
extern int hashmap_remove(map_t in, char* key, any_t data);

/*
 * Free the hashmap
 */
extern void hashmap_free(map_t in);

/*
 * Get the current size of a hashmap
 */
extern int hashmap_length(map_t in);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __HASHMAP_H__
