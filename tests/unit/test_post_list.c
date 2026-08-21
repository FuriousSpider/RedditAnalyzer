#include "redditanalyzer/model/post_list.h"

#include <assert.h>


static void test_init(void)
{
    PostList list;

    RaError error = post_list_init(&list);

    assert(error == RA_OK);
    assert(list.items != NULL);
    assert(list.count == 0U);
    assert(list.capacity > 0U);

    post_list_destroy(&list);
}

static void test_append(void)
{
    PostList list;

    assert(post_list_init(&list) == RA_OK);

    Post *post = post_create();

    assert(post != NULL);

    assert(post_list_append(&list, post) == RA_OK);

    assert(list.count == 1U);
    assert(list.items[0] == post);

    post_list_destroy(&list);
}

static void test_multiple_posts(void)
{
    PostList list;

    assert(post_list_init(&list) == RA_OK);

    Post *post1 = post_create();
    Post *post2 = post_create();
    Post *post3 = post_create();

    assert(post1 != NULL);
    assert(post2 != NULL);
    assert(post3 != NULL);

    assert(post_list_append(&list, post1) == RA_OK);
    assert(post_list_append(&list, post2) == RA_OK);
    assert(post_list_append(&list, post3) == RA_OK);

    assert(list.count == 3U);

    assert(list.items[0] == post1);
    assert(list.items[1] == post2);
    assert(list.items[2] == post3);

    post_list_destroy(&list);
}

static void test_append_invalid_arguments(void)
{
    PostList list;

    assert(post_list_append(NULL, NULL) == RA_ERR_INVALID_ARGUMENT);

    assert(post_list_init(&list) == RA_OK);

    assert(post_list_append(&list, NULL) == RA_ERR_INVALID_ARGUMENT);

    post_list_destroy(&list);
}

static void test_destroy_null(void)
{
    post_list_destroy(NULL);
}

static void test_grow(void)
{
    PostList list;

    assert(post_list_init(&list) == RA_OK);

    const size_t initial_capacity = list.capacity;
    const size_t post_count = initial_capacity + 1U;

    for (size_t i = 0U; i < post_count; i++)
    {
        Post *post = post_create();

        assert(post != NULL);
        assert(post_list_append(&list, post) == RA_OK);
    }

    assert(list.count == post_count);
    assert(list.capacity > initial_capacity);

    post_list_destroy(&list);
}

int main(void)
{
    test_init();
    test_append();
    test_multiple_posts();
    test_append_invalid_arguments();
    test_destroy_null();
    test_grow();

    return 0;
}
