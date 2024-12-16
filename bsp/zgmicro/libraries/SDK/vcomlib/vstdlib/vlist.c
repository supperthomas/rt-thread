/*------------------------------------------------------------------------------
                    COPYRIGHT (C) 2009, Vimicro Corporation
                              ALL RIGHTS RESERVED

This source code has been made available to you by VIMICRO on an AS-IS basis.
Anyone receiving this source code is licensed under VIMICRO copyrights to use
it in any way he or she deems fit, including copying it, modifying it,
compiling it, and redistributing it either with or without modifications. Any
person who transfers this source code or any derivative work must include the
VIMICRO copyright notice and this paragraph in the transferred software.

Module:
  vlist

Description:
  linked list data structure

Revision History:
  Date                  Author                   Description
  2018-09-01             ming.m                  create
  2018-09-05             ming.m                  add v_slist_remove()
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vlist.h"

#define v_list_mem_alloc(sz)  VSYS_MEM_Malloc(sz)
#define v_list_mem_free(p)    VSYS_MEM_Free(p)

/// Allocates space for one v_slist_t element. 
/// It is called by the g_slist_append(), g_slist_prepend(), g_slist_insert() 
/// and g_slist_insert_sorted() functions and so is rarely used on its own.
/// 
/// @return : a pointer to the newly-allocated v_slist_t element.
v_slist_t *            v_slist_alloc                       (void){

	return (v_slist_t *)v_list_mem_alloc(sizeof(v_slist_t));
}

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
v_slist_t *            v_slist_append                      (v_slist_t *list, v_list_data_t * data) {
	v_slist_t *last, *element;


	element = v_slist_alloc();
	if(element != NULL) {
		element->data = data;
		element->next = NULL;
	}
	else
	{
		// create element fail
		return NULL;
	}

	last = v_slist_last(list);
	if(last != NULL) {
		last->next = element;
		return list;
	}
	else
	{
		return element;
	}
}

/// Adds a new element on to the start of the list.
/// 
/// @list   a v_slist_t, NULL is acceptable
/// @data   the data pointer for the new element
/// @return the new start of the v_slist_t
/// 
/// Note: The return value is the new start of the list, which may have 
///       changed, so make sure you store the new value. 
v_slist_t *            v_slist_prepend                     (v_slist_t *list, v_list_data_t * data){
  v_slist_t *new_list;

  new_list = v_slist_alloc ();
  if(new_list == NULL)
  {
  	// fail to alloc new element
  	return list;
  }
  new_list->data = data;
  new_list->next = list;

  return new_list;
}


/**
 * g_slist_remove:
 * @list: a #GSList
 * @data: the data of the element to remove
 *
 * Removes an element from a #GSList.
 * If two elements contain the same data, only the first is removed.
 * If none of the elements contain the data, the #GSList is unchanged.
 *
 * Returns: the new start of the #GSList
 */
v_slist_t*
v_slist_remove (v_slist_t *list, const v_list_data_t * data)
{
  v_slist_t *tmp, *prev = NULL;

  tmp = list;
  while (tmp)
    {
      if (tmp->data == data)
        {
          if (prev)
            prev->next = tmp->next;
          else
            list = tmp->next;

          v_slist_node_free (tmp);
          break;
        }
      prev = tmp;
      tmp = prev->next;
    }

  return list;
}

/// Removes an element from a v_slist_t, without freeing the element. 
/// The removed element's next link is set to NULL, so that it becomes 
/// a self-contained list with one element.
/// 
/// list : a v_slist_t
/// link_ : an element in the v_slist_t
/// @return : the new start of the v_slist_t, without the element
v_slist_t *            v_slist_remove_link                 (v_slist_t *list, v_slist_t *link){
  v_slist_t *tmp;
  v_slist_t *prev;

  prev = NULL;
  tmp = list;

  while (tmp)
    {
      if (tmp == link)
        {
          if (prev)
            prev->next = tmp->next;
          if (list == tmp)
            list = list->next;

          tmp->next = NULL;
          break;
        }

      prev = tmp;
      tmp = tmp->next;
    }

  return list;
}

///  Removes the node link_ from the list and frees it. Compare this to v_slist_remove_link() which removes the node without freeing it.
/// list : a v_slist_t
/// link_ : node to delete

/// @return : the new head of list
v_slist_t *            v_slist_delete_link                 (v_slist_t *list, v_slist_t *link_) {
	list = v_slist_remove_link (list, link_);
	v_slist_node_free (link_);

	return list;
}


/// Frees all of the memory used by a v_slist_t. The freed elements are returned to the slice allocator.
/// 
/// list a v_slist_t
/// Note
///    If list elements contain dynamically-allocated memory, you should either 
///    use v_slist_free_full() or free them manually first.
void                   v_slist_free                        (v_slist_t *list) {
	v_slist_t *next;
  while (list)
  {
    next = list->next;
    v_slist_node_free(list);
    list = next;
  }
}

/// Frees one v_slist_t node. It is usually used after v_slist_remove_link().
/// Parameters

/// list a v_slist_t element
void                   v_slist_node_free              (v_slist_t *list) {
	v_list_mem_free(list);
}

///  Convenience method, which frees all the memory used by a v_slist_t, and calls the specified destroy function on every element's data.
/// list : a pointer to a v_slist_t
/// free_func : the function to be called to free each element's data
void                v_slist_free_full                   (v_slist_t *list, GDestroyNotify free_func){
  v_slist_foreach (list, (GFunc) free_func, NULL);
  v_slist_free (list);
}



///  Calls a function for each element of a v_slist_t.
/// list : a v_slist_t
/// func : the function to call with each element's data
/// user_data : user data to pass to the function
void                v_slist_foreach                     (v_slist_t *list, GFunc func, v_list_data_t * user_data){
	v_slist_t *next;
  while (list)
    {
      next = list->next;
      (*func) (list->data, user_data);
      list = next;
    }
}


/// Gets the last element in a v_slist_t.
/// 
/// This function iterates over the whole list.
/// Parameters
/// list : a v_slist_t	 
/// @return : the last element in the v_slist_t, or NULL if the v_slist_t has no elements
v_slist_t *            v_slist_last                        (v_slist_t *list) {
	v_slist_t *next;

	next = list;

	while(next != NULL) {
		list = next;
		next = v_slist_next(list);
	}

	return list;
}

/* From memory start ptr go to userdata */
 list_data_t *bilist_data(bilist_node_t *node) {
	return &node->data[0];
}

/* From userdata go to memory start ptr */
bilist_node_t *bilist_addr(list_data_t *data) {
	return ((bilist_node_t *)((UINT8 *)(data) - sizeof(bilist_head_t)));
}

/* From userdata ptr go to next memory start ptr */
 bilist_node_t *bilist_next_from_data(list_data_t *data) {
	return ((bilist_node_t *)((UINT8 *)(data) - sizeof(bilist_head_t)))->head.next;
}
 
/* From userdata go to prev memory start ptr */
bilist_node_t *bilist_prev_from_data(list_data_t *data) {
	return ((bilist_node_t *)((UINT8 *)data - sizeof(bilist_head_t)))->head.prev;
}

/* From memory start ptr go to userdata */
list_data_t *sglist_data(sglist_node_t *node) {
	return &node->data[0];
}

/* From userdata go to memory start ptr */
sglist_node_t *sglist_addr(list_data_t *data) {
	return ((sglist_node_t *)((UINT8 *)(data) - sizeof(struct sglist_head_s)));
}


void bilist_append_data(bilist_t *list, list_data_t *data)
{
//  list->num++;
//  if (list->tail == NULL) {
//    list->head = node;
//    LPREV(node) = 0;
//    LNEXT(node) = NULL;
//  } else {
//    LNEXT(list->tail) = node;
//    LPREV(node) = list->tail;
//    LNEXT(node) = NULL;

//  }
//  list->tail = node;
  bilist_node_t *p_node;
  
  list->num++;
  if (list->tail == NULL) {
    list->head = data;
    //LPREV(node) = 0;
    p_node = bilist_addr(data);
    p_node->head.prev = NULL;

    //LNEXT(node) = NULL;
    p_node->head.next = NULL;

  } else {
    //LNEXT(list->tail) = node;
    p_node = bilist_addr(list->tail);
    p_node->head.next = (bilist_node_t *)data;

    //LPREV(node) = list->tail;
     p_node = bilist_addr(data);
    p_node->head.prev = (bilist_node_t *)list->tail;

    //LNEXT(node) = NULL;
    p_node->head.next = NULL;
  }
  list->tail = data;
}


void bilist_add_head(bilist_t *list, list_data_t *data)
{
	bilist_node_t *p_node;
	
	list->num++;
	if (list->head == NULL) {
		list->tail = data;
	} else {
//		LPREV(list->head) = node;
        p_node = bilist_addr(list->head);
        p_node->head.prev = (bilist_node_t *)data;

//		LPREV(node) = NULL;/* No need? */
        p_node = bilist_addr(data);
        p_node->head.prev = NULL;

//		LNEXT(node) = list->head;
        p_node->head.next = (bilist_node_t *)list->head;
	}
	list->head = data;
}


void bilist_insert_data_at(bilist_t *list, list_data_t *pos, list_data_t *data) 
{
	bilist_node_t *pre_node, *p_node;

	
	if (pos != NULL) {
		//void *prev = LPREV(pos);
		pre_node = bilist_prev_from_data(pos);
		
		list->num++;
		if (pre_node == NULL) {
			list->head = data;
		} else {
			//LNEXT(prev) = node;
			pre_node->head.next = (bilist_node_t *)data;
		}
		//LPREV(node) = prev;
		p_node = bilist_addr(data);
		p_node->head.prev = (bilist_node_t *)&pre_node->data;
		
		//LNEXT(node) = pos;
		p_node->head.next = (bilist_node_t *)pos;
		
		//LPREV(pos) = node;
		p_node = bilist_addr(pos);
		p_node->head.prev = (bilist_node_t *)data;
		
	} else {
		//List_AddTail(list, node);
		bilist_append_data(list, data);
	}
}

list_data_t *bilist_remove_headnode(bilist_t *list) 
{
	bilist_node_t *p_node;
	
	list_data_t *data = list->head;
	
	if (data != NULL) {
		list->num--;
		//list->head = LNEXT(node);
		p_node = bilist_addr(data);
		list->head = (list_data_t *)p_node->head.next;
		if (list->head != NULL) {
//			LPREV(list->head) = NULL;
			p_node = bilist_addr(list->head);
			p_node->head.prev = NULL;
		} else {
			list->tail = NULL;
		}
	}

	return data;
  
}

list_data_t *bilist_remove_tailnode(bilist_t *list) 
{
	bilist_node_t *p_node;

	list_data_t *data = list->tail;
	
	if (data != NULL) {
		list->num--;
		//list->tail = LPREV(node);
		p_node = bilist_addr(data);
		list->tail = (list_data_t *)p_node->head.prev;
		if (list->tail != NULL) {
//			LNEXT(list->tail) = NULL;
			p_node = bilist_addr(list->tail);
			p_node->head.next = NULL;
		} else {
			list->head = NULL;
		}
	}

	return data;
}


void bilist_remove_data_at(bilist_t *list, list_data_t *data)
{
//	void *next = LNEXT(node);
//	void *prev = LPREV(node);
	bilist_node_t *p_node;
	list_data_t *next_data;
	list_data_t *prev_data;
	
	p_node = bilist_addr(data);
	next_data = (list_data_t *)p_node->head.next;
	prev_data = (list_data_t *)p_node->head.prev;
	
	list->num--;
	if (prev_data != NULL) {
//		LNEXT(prev) = next;
		p_node = bilist_addr(prev_data);
		p_node->head.next = (bilist_node_t *)next_data;
	} else {/* node is head */
		list->head = next_data;
	}
	if (next_data != NULL) {
//		LPREV(next) = prev;
		p_node = bilist_addr(next_data);
		p_node->head.prev = (bilist_node_t *)prev_data;
	} else {/* node is tail */
		list->tail = prev_data;
	}
} 

list_data_t* sglist_data_new(UINT32 size)
{
	sglist_node_t *p_node = v_list_mem_alloc(size + sizeof(struct sglist_head_s));
	if(p_node){
		return sglist_data(p_node);
	}
	return NULL;
}

void sglist_data_del(list_data_t* data)
{
	sglist_node_t *p_node = sglist_addr(data);
	v_list_mem_free(p_node);
}

void sglist_append_data(sglist_t *list, list_data_t *data)
{
	sglist_node_t *p_node = sglist_addr(data);
	
	list->num++;
	if (list->tail == NULL) {/* Null list */
		list->head = p_node;
	} else {
//		SNEXT(list->tail) = node;
		list->tail->head.next = p_node;		
//		SNEXT(node) = NULL;/* No need? */
		p_node->head.next = NULL;
	}
	list->tail = p_node;
}

void sglist_add_head(sglist_t *list, list_data_t *data)
{
	sglist_node_t *p_node = sglist_addr(data);
	
	list->num++;
	if (list->head == NULL) {/* Null list */
		list->tail = p_node;
	} else {
//		SNEXT(node) = list->head;
		p_node->head.next = list->head;
	}
	list->head = p_node;
}


list_data_t* sglist_remove_data_at(sglist_t *list, list_data_t *data)
{
	sglist_node_t *p_node = sglist_addr(data), *p_n, *p_r = NULL;
	for(p_n = list->head; p_n != NULL; p_n = p_n->head.next)
	{
		if(p_n == p_node){
			if(p_r){
				p_r->head.next = p_n->head.next;
			}else{
				list->head = p_n->head.next;
			}
			list->num--;
			break;
		}	
		p_r = p_n;
	}
	return p_n?data:NULL;
}

list_data_t* sglist_remove_data_check(sglist_t *list, UINT32 val)
{
	sglist_node_t *p_n, *p_r=NULL;
	for(p_n = list->head; p_n != NULL; p_n = p_n->head.next)
	{
		if(*(UINT32*)p_n->data == val){
			if(p_r){
				p_r->head.next = p_n->head.next;
			}else{
				list->head = p_n->head.next;
			}
			list->num--;
			break;
		}	
		p_r = p_n;
	}
	return p_n?sglist_data(p_n):NULL;
}
