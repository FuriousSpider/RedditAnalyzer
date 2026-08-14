#include "redditanalyzer/model/post_list.h"

#include <stdlib.h>

#define POST_LIST_INITIAL_CAPACITY 8U

RaError post_list_init(PostList *list)
{
    if (list == NULL)
    {
        return RA_ERR_INVALID_ARGUMENT;
    }

    list->items = calloc(
        POST_LIST_INITIAL_CAPACITY,
        sizeof(*list->items)
    );

    if (list->items == NULL)
    {
        return RA_ERR_OUT_OF_MEMORY;
    }

    list->count = 0U;
    list->capacity = POST_LIST_INITIAL_CAPACITY;

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

        Post **new_items = realloc(
            list->items,
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

    free(list->items);

    list->items = NULL;
    list->capacity = 0U;
}
