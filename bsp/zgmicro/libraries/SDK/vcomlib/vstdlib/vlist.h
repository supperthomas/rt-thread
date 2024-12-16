
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef __VLIST_H__
#define __VLIST_H__

#include "vtypes.h"

/// Support extend feature of list operation
#define VLIST_EXT_ENABLE    0


typedef void *v_list_data_t;

/// Doubly-Linked-Lists
typedef struct v_list_s {
	struct v_list_s *prev;
	struct v_list_s *next;
	v_list_data_t *data;
}v_list_t;

typedef struct v_slist_s {
	struct v_slist_s *next;
	v_list_data_t *data;
}v_slist_t;

typedef unsigned char list_data_t;

/// Binary Tree Doubly-Linked List
struct bilist_head_s {
	struct bilist_node_s *next;
	struct bilist_node_s *prev;
};

struct bilist_node_s {
	struct bilist_head_s head;
	list_data_t data[1]; // a binary raw data
};

typedef struct bilist_head_s   bilist_head_t;
typedef struct bilist_node_s        bilist_node_t;

typedef struct bilist_s {
	list_data_t *head;
	list_data_t *tail;
	UINT32 num;
}bilist_t;

/// Signal-Linked List
struct sglist_head_s {
	struct sglist_node_s *next;
};
struct sglist_node_s {
	struct sglist_head_s head;
	list_data_t data[1];  // a binary raw data
};

typedef struct sglist_head_s   sglist_head_t;
typedef struct sglist_head_s   sglist_tail_t;
typedef struct sglist_node_s   sglist_node_t;

typedef struct sglist_s {
	sglist_node_t *head;
	sglist_node_t *tail;
	UINT32 num;
}sglist_t;

/* From memory start ptr go to userdata */
list_data_t *bilist_data(bilist_node_t *node);

/* From userdata go to memory start ptr */
bilist_node_t *bilist_addr(list_data_t *data);

/* From userdata ptr go to next memory start ptr */
bilist_node_t *bilist_next_from_data(list_data_t *data);

/* From userdata go to prev memory start ptr */
bilist_node_t *bilist_prev_from_data(list_data_t *data);

void bilist_append_data(bilist_t *list, list_data_t *data);

void bilist_add_head(bilist_t *list, list_data_t *data);

void bilist_insert_data_at(bilist_t *list, list_data_t *pos, list_data_t *data);

list_data_t *bilist_remove_headnode(bilist_t *list);

list_data_t *bilist_remove_tailnode(bilist_t *list);

void bilist_remove_data_at(bilist_t *list, list_data_t *data);


list_data_t* sglist_data_new(UINT32 size);
void sglist_data_del(list_data_t* data);
list_data_t *sglist_data(sglist_node_t *node);
sglist_node_t *sglist_addr(list_data_t *data);
void sglist_append_data(sglist_t *list, list_data_t *data);
void sglist_add_head(sglist_t *list, list_data_t *data);
list_data_t* sglist_remove_data_at(sglist_t *list, list_data_t *data);
list_data_t* sglist_remove_data_check(sglist_t *list, UINT32 val);


typedef int            (*GCompareFunc)         (const void  *a, const void *b);
typedef int            (*GCompareDataFunc)     (const void *a, const void *b, void *user_data);
typedef int            (*GEqualFunc)           (const void *a, const void *b);
typedef void           (*GDestroyNotify)       (void *data);
typedef void           (*GFunc)                (v_list_data_t * data, v_list_data_t * user_data);

/* 
 *  Singly-Linked Lists — linked lists that can be iterated in one direction
 */


/// Allocates space for one v_slist_t element. 
/// It is called by the g_slist_append(), g_slist_prepend(), g_slist_insert() 
/// and g_slist_insert_sorted() functions and so is rarely used on its own.
/// 
/// @return : a pointer to the newly-allocated v_slist_t element.
v_slist_t *            v_slist_alloc                       (void);

/// Adds a new element on to the end of the list. 
/// 
/// @list   a v_slist_t, NULL is acceptable
/// @data   the data pointer for the new element
/// @return the new start of the v_slist_t
/// 
/// Note: The return value is the new start of the list, which may have 
///       changed, so make sure you store the new value. 
///       Note that v_slist_append() has to traverse the entire list to find 
///       the end, which is inefficient when adding multiple elements. A 
///       common idiom to avoid the inefficiency is to prepend the elements 
///       and reverse the list when all elements have been added. 
v_slist_t *            v_slist_append                      (v_slist_t *list, v_list_data_t * data);

/// Adds a new element on to the start of the list.
/// 
/// @list   a v_slist_t, NULL is acceptable
/// @data   the data pointer for the new element
/// @return the new start of the v_slist_t
/// 
/// Note: The return value is the new start of the list, which may have 
///       changed, so make sure you store the new value. 
v_slist_t *            v_slist_prepend                     (v_slist_t *list, v_list_data_t * data);

/// Removes an element from a v_slist_t. If two elements contain the same data, 
/// only the first is removed. If none of the elements contain the 
/// data, the v_slist_t is unchanged.
/// @list : a v_slist_t
/// @data : the data of the element to remove
/// @return : the new start of the v_slist_t
v_slist_t *            v_slist_remove                      (v_slist_t *list, const v_list_data_t * data);

/// Removes an element from a v_slist_t, without freeing the element. 
/// The removed element's next link is set to NULL, so that it becomes 
/// a self-contained list with one element.
/// 
/// list : a v_slist_t
/// link_ : an element in the v_slist_t
/// @return : the new start of the v_slist_t, without the element
v_slist_t *            v_slist_remove_link                 (v_slist_t *list, v_slist_t *link_);

///  Removes the node link_ from the list and frees it. Compare this to v_slist_remove_link() which removes the node without freeing it.
/// list : a v_slist_t
/// link_ : node to delete

/// @return : the new head of list
v_slist_t *            v_slist_delete_link                 (v_slist_t *list, v_slist_t *link_);

/// Removes all list nodes with data equal to data. @return the new head of the list.
/// Contrast with v_slist_remove() which removes only the first node matching 
/// the given data.
/// 
/// list : a v_slist_t
/// data : data to remove
/// @return : new head of list
v_slist_t *            v_slist_remove_all                  (v_slist_t *list, const v_list_data_t * data);

/// Frees all of the memory used by a v_slist_t. The freed elements are returned to the slice allocator.
/// 
/// list :a v_slist_t
/// Note : 
///    If list elements contain dynamically-allocated memory, you should either 
///    use v_slist_free_full() or free them manually first.
void                   v_slist_free                        (v_slist_t *list);

/// Frees one v_slist_t element. It is usually used after v_slist_remove_link().
/// Parameters

/// list ： a v_slist_t element
void                   v_slist_node_free              (v_slist_t *list);

///  Convenience method, which frees all the memory used by a v_slist_t, and calls the specified destroy function on every element's data.
/// list : a pointer to a v_slist_t
/// free_func : the function to be called to free each element's data
void                v_slist_free_full                   (v_slist_t *list, GDestroyNotify free_func);

/// Gets the number of elements in a v_slist_t.

/// This function iterates over the whole list to count its elements. To check whether the list is non-empty, it is faster to check list against NULL.
/// Parameters

/// list : a v_slist_t
	 
/// @return

///  Calls a function for each element of a v_slist_t.
/// list : a v_slist_t
/// func : the function to call with each element's data
/// user_data : user data to pass to the function
void                v_slist_foreach                     (v_slist_t *list, GFunc func, v_list_data_t * user_data);

/// Gets the last element in a v_slist_t.
/// 
/// This function iterates over the whole list.
/// Parameters
/// list : a v_slist_t	 
/// @return : the last element in the v_slist_t, or NULL if the v_slist_t has no elements
v_slist_t *            v_slist_last                        (v_slist_t *list);

/// A convenience macro to get the next element in a v_slist_t. Note that it is considered perfectly acceptable to access slist->next directly.
/// Parameters
/// slist
/// an element in a v_slist_t.
/// @return
/// the next element, or NULL if there are no more elements.
#define             v_slist_next(slist)    (list->next)


#endif //#ifndef __VLIST_H__
