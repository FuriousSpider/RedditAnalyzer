#ifndef REDDITANALYZER_MODEL_POST_LIST_H
#define REDDITANALYZER_MODEL_POST_LIST_H

#include <stddef.h>

#include "redditanalyzer/model/post.h"
#include "redditanalyzer/utils/error.h"

typedef struct
{
    Post **items;
    size_t count;
    size_t capacity;
} PostList;

RaError post_list_init(PostList *list);
RaError post_list_append(PostList *list, Post *post);
void post_list_clear(PostList *list);
void post_list_destroy(PostList *list);

#endif