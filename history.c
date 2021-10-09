/**
* @file         history.c
* @brief		This is the implement of history part of shell
* @details	    It use circle list data structure, use hist_init() to initiallize and hist_destroy() to free memory
* @author		Haoyu Yan
* @date		    2020/4/7
* @version	    v1.0
*/

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "history.h"
#include "logger.h"

/** 
 * @brief		This is the struct of circle list
 * @details	    clist is used to build the history part
 */

struct hlist {
    size_t insertion;
    size_t capacity;
    size_t item_sz;
    char *element_storage;
};

/**
 * @brief		This is the function used to create an clist 
 * @param[in]	size an unsign int of clist capacity
 * @param[out]	clist a clist been created 
 * @retval		hlist *		success
 * @retval		NULL        fail
 */

struct hlist *hlist_create (unsigned int size) {
    struct hlist *list = malloc(sizeof(struct hlist));
    if (list == NULL) {
        free(list);
        perror("hlist malloc error");
        return NULL;
    }
    if (size == 0) {
        free(list);
        return NULL;
    }

    list->capacity = size;
    list->insertion = 0;
    list->item_sz = 50;

    list->element_storage = malloc(list->capacity * list->item_sz);
    if (list->element_storage == NULL) {
        perror("Malloc error");
        free(list);
        return NULL;
    }
    return list;
}

/**
 * @brief		This is the function used to add item into clist
 * @param[in]	list a point to a clist
 * @param[in]	line a point to a string
 * @param[out]	int to see the fuction work correctly or not
 * @retval		0           success
 * @retval		-1          fail
 */

int hlist_add(struct hlist *list, char *line) {
    if (list == NULL) {
        return -1;
    }
    size_t idx = list->insertion % list->capacity;
    char *ptr = list->element_storage + idx * list->item_sz;
    memcpy(ptr, line, strlen(line)+1);
    list->insertion++;
    return 0;
}

/**
 * @brief		This is the function used to get item from clist
 * @param[in]	list a point to a clist
 * @param[in]	idx the index of item in clist
 * @param[out]	char the string get from list 
 * @retval		hlist *		success
 * @retval		NULL        fail
 */

char *hlist_get (struct hlist *list, size_t idx) {
    if (list == NULL || idx >= list->insertion || ((list->insertion > list->capacity) && (idx < list->insertion - list->capacity))) {
        return NULL;
    }
    size_t real_idx = idx % list->capacity;
    return list->element_storage + real_idx * list->item_sz;
}

//hist
struct hlist *l;
int history_index;

/**
 * @brief		public method initialize the clist of history
 * @param[in]	limit the size of clist
 */

void hist_init(unsigned int limit)
{
    // TODO: set up history data structures, with 'limit' being the maximum
    // number of entries maintained.
    l = hlist_create(limit);
}

/**
 * @brief		public method destory the clist of history
 */

void hist_destroy(void)
{
    if (l != NULL) {
        free(l->element_storage);
        free(l);
    }
}

/**
 * @brief		This is the function used to add item to clist
 * @param[in]	cmd a pointer of char item
 */

void hist_add(const char *cmd)
{
    char *tmp = malloc(strlen(cmd)+1);
    strcpy(tmp, cmd);
    tmp[strlen(cmd)] = '\0';
    if (hlist_add(l, tmp) == -1) {
        perror("add fail");
    } 
    free(tmp);
}

/**
 * @brief		This is the function used to print all item in clist
 * @param[out]	print out all the element in clist
 */

void hist_print(void)
{
    if (l->insertion <= l->capacity) {
        for(int i = 0; i < l->insertion; i++) {
            printf("    %d  %s\n", i+1, hlist_get(l, i));
        }
    } else {
        for (int i = l->insertion-l->capacity; i < l->insertion; i++) {
            printf("    %d  %s\n", i+1, hlist_get(l, i));
        }     
    }
    fflush(stdout); 
}

/**
 * @brief		This is the function search the item prefix from clist
 * @param[in]	prefix a pointer to char, store a string of prefix
 * @retval		p       the search result
 */

const char *hist_search_prefix(char *prefix)
{
    // TODO: Retrieves the most recent command starting with 'prefix', or NULL
    // if no match found.
    const char *p = NULL;
    if (l->insertion < l->capacity) {
        for (int i = 0; i < l->insertion; i++) {
            if (strncmp(prefix, hist_search_cnum(i), strlen(prefix)) == 0) {
                p = hist_search_cnum(i);
            }
        }
    } else {
        for (int i = l->insertion-l->capacity; i < l->insertion; i++) {
            if (strncmp(prefix, hist_search_cnum(i), strlen(prefix)) == 0) {
                p = hist_search_cnum(i);
            }
        }
    }
    return p;
}

/**
 * @brief		This is the function search the item index from clist
 * @param[in]	command_number get element from clist 
 * @retval		char*       the search result
 */

const char *hist_search_cnum(int command_number)
{
    // TODO: Retrieves a particular command number. Return NULL if no match
    // found.
    return hlist_get(l, command_number);
}

/**
 * @brief		This is the function to get last element's index in clsit
 * @retval		usign int       last index of element
 */

unsigned int hist_last_cnum(void)
{
    // TODO: Retrieve the most recent command number.
    if (l->insertion == 0) {
        return 0;
    }
    return l->insertion-1;
}

/**
 * @brief		Init the iterator index every time when prompt a new line
 */

void hist_iter_init(void) {
    //history_index = l->insertion;
    history_index = 0;
}

/**
 * @brief		Get the next element of history index
 * @retval		char*       the search result
 */

char *hist_next(void) {
    if (history_index <= 1) {
        history_index = 0;
        return "";
    } else {
        history_index --;
        return hlist_get(l, l->insertion - history_index);
    }
}

/**
 * @brief		Get the perivous element of history index
 * @retval		char*       the search result
 */

char *hist_prev(void) {
    if (l->insertion <= history_index) {
        return hlist_get(l, 0);
    }
    history_index ++;
    return hlist_get(l, l->insertion - history_index);
}