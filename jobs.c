/**
* @file         jobs.c
* @brief		This is the implement of backgound jobs part of shell
* @details	    It use elastic list data structure, use jobs_init() to initiallize and jobs_destroy() to free memory
* @author		Haoyu Yan
* @date		    2020/4/7
* @version	    v1.0
*/

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "jobs.h"
#include "logger.h"

#define DEFAULT_INIT_SZ 10
#define RESIZE_MULTIPLIER 2

struct elist {
    size_t capacity;         /*!< Storage space allocated for list items */
    size_t size;             /*!< The actual number of items in the list */
    size_t item_sz;          /*!< Size of the items stored in the list */
    char *element_storage;   /*!< Pointer to the beginning of the array */
};

bool idx_is_valid(struct elist *list, size_t idx);

struct elist *elist_create(size_t list_sz, size_t item_sz)
{
    struct elist *list = malloc(sizeof(struct elist));
    if (list == NULL)
    {
        return NULL;
    }
    if (list_sz == 0)
    {
        list_sz = DEFAULT_INIT_SZ;
    }

    list->capacity = list_sz;
    list->item_sz = item_sz;
    list->size = 0;

    LOG("Initializing new elist: capacity=[%zu], item_sz=[%zu]\n",
        list->capacity, list->item_sz);
    
    list->element_storage = malloc(list->capacity * list->item_sz);
    if (list->element_storage == NULL)
    {
        perror("Malloc Error");
        free(list);
        return NULL;
    }
    
    
    return list;
}

void elist_destroy(struct elist *list)
{
    free(list->element_storage);
    free(list);
}

int elist_set(struct elist *list, size_t idx, char *item)
{
    if (!idx_is_valid(list, idx)) {
        return -1;
    }
    if (item == NULL)
    {
        return -1;
    }
    
    char *set_item = list->element_storage + idx * list->item_sz;
    memcpy(set_item, item, list->item_sz);
    return 0;
}

ssize_t elist_add(struct elist *list, char *item)
{
    if (list->size >= list->capacity)
    {
        LOG("Resizing the list. New capacity: %zu\n", list->capacity);
        list->element_storage = realloc(list->element_storage, list->item_sz * list->capacity * RESIZE_MULTIPLIER);
        if (list->element_storage == NULL)
        {
            return -1;
        }
        list->capacity = list->capacity * RESIZE_MULTIPLIER;
    }
    size_t idx = list->size++;
    char *item_ptr = list->element_storage + idx * list->item_sz;
    memcpy(item_ptr, item, list->item_sz);
    return idx;
}

size_t elist_capacity(struct elist *list)
{
    return list->capacity;
}

void *elist_add_new(struct elist *list)
{
    if (list->size >= list->capacity)
    {
        list->capacity = list->capacity + 1;
        LOG("Resizing the list. New capacity: %zu\n", list->capacity);
        list->capacity = list->capacity + 1;
        list->element_storage = realloc(list->element_storage, list->item_sz * list->capacity);
        if (list->element_storage == NULL)
        {
            return NULL;
        }
    }
    size_t idx = list->size++;
    char *item_ptr = list->element_storage + idx * list->item_sz;
    return item_ptr;
}

char *elist_get(struct elist *list, size_t idx)
{
    if (idx >= list->size) {
        return NULL;
    }
    return list->element_storage + idx * list->item_sz;
}

size_t elist_size(struct elist *list)
{
    return list->size;
}

int elist_remove(struct elist *list, size_t idx)
{
    if (idx >= list->size)
    {
        return -1;
    }
    for (int i = idx; i < list->size - 1; i++)
    {
        char *next = elist_get(list, i+1);
        elist_set(list, i, next);
    }
    list->size--;
    return 0;
}

void elist_clear(struct elist *list)
{
    list->size = 0;
}

void elist_clear_mem(struct elist *list)
{
    list->element_storage = calloc(list->capacity, list->item_sz);
    
    elist_clear(list);
}

ssize_t elist_index_of(struct elist *list, void *item)
{
    for (int i = 0; i < list->size; i++)
    {
        if (memcmp(list->element_storage + i * list->item_sz, item, list->item_sz) == 0)
        {
            return i;
        }
        
    }
    return -1;
}

bool idx_is_valid(struct elist *list, size_t idx)
{
    if (idx >= list->size)
    {
        return false;
    }
    return true;
}

//methods

struct elist *el;

void jobs_init() 
{
    el = elist_create(10, 100);
}

void jobs_destroy() 
{
    elist_destroy(el);
}

void jobs_add(char *item)
{
    elist_add(el, item);
}

void jobs_dele(char *item) 
{
    int i = elist_index_of(el, item);
    elist_remove(el, i);
}


void jobs_print() 
{
    for (int i = 0; i < el->size; i++)
    {
        printf("%s\n", el->element_storage + i * el->item_sz);
        fflush(stdout);
    }
}

