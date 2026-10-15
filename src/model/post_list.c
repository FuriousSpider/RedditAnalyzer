#include "redditanalyzer/model/post_list.h"

#include <stdlib.h>

#define POST_LIST_INITIAL_CAPACITY 8U
#define POST_LIST_MAX_AFTER_SIZE 8U

RaError post_list_init(PostList *list)
{
    if (list == NULL)
    {
        return RA_ERR_INVALID_ARGUMENT;
    }

    list->items = (Post **) calloc(
        POST_LIST_INITIAL_CAPACITY,
        sizeof(*list->items)
    );

    if (list->items == NULL)
    {
        return RA_ERR_OUT_OF_MEMORY;
    }

    list->count = 0U;
    list->capacity = POST_LIST_INITIAL_CAPACITY;
    list->after = NULL;

    return RA_OK;
}

RaError post_list_append(PostList *list, Post *post)
{
    if (list == NULL || post == NULL)
    {
        return RA_ERR_INVALID_ARGUMENT;
    }

    if (list->count == list->capacity)
    {
        const size_t new_capacity = list->capacity * 2U;

        Post **new_items = (Post **) realloc(
            (void *) list->items,
            new_capacity * sizeof(*new_items)
        );

        if (new_items == NULL)
        {
            return RA_ERR_OUT_OF_MEMORY;
        }

        list->items = new_items;
        list->capacity = new_capacity;
    }

    list->items[list->count] = post;
    list->count++;

    return RA_OK;
}

void post_list_clear(PostList *list)
{
    if (list == NULL)
    {
        return;
    }

    for (size_t i = 0U; i < list->count; i++)
    {
        post_destroy(list->items[i]);
    }

    list->count = 0;
}

void post_list_destroy(PostList *list)
{
    if (list == NULL)
    {
        return;
    }

    post_list_clear(list);

    if (list->items != NULL) {
        free((void *) list->items);
    }
    
    if (list->after != NULL)
    {
        free(list->after);
    }
    
    list->items = NULL;
    list->after = NULL;
    list->capacity = 0U;
}
