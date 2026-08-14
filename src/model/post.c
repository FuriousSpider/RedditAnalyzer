#include "redditanalyzer/model/post.h"

#include <stdlib.h>

Post *post_create(void)
{
    return calloc(1, sizeof(Post));
}

void post_destroy(Post *post)
{
    if (post == NULL)
    {
        return;
    }

    free(post->id);
    free(post->title);
    free(post->author);

    free(post);
}