#include "redditanalyzer/model/post.h"

#include <assert.h>
#include <stdbool.h>

static void test_create(void)
{
    Post *post = post_create();

    assert(post != NULL);

    assert(post->id == NULL);
    assert(post->title == NULL);
    assert(post->author == NULL);

    assert(post->score == 0);
    assert(post->comments == 0);
    assert(post->created_at == 0);

    assert(post->is_video == false);
    assert(post->is_self == false);

    post_destroy(post);
}

static void test_destroy_null(void)
{
    post_destroy(NULL);
}

int main(void)
{
    test_create();
    test_destroy_null();

    return 0;
}