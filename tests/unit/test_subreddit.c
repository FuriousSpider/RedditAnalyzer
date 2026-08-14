#include "redditanalyzer/model/subreddit.h"

#include <assert.h>
#include <stdbool.h>
#include <string.h>


static void test_create_valid_subreddit(void)
{
    Subreddit *subreddit = subreddit_create("programming");

    assert(subreddit != NULL);
    assert(subreddit->name != NULL);
    assert(strcmp(subreddit->name, "programming") == 0);

    assert(subreddit->display_name == NULL);
    assert(subreddit->description == NULL);

    assert(subreddit->subscribers == 0);
    assert(subreddit->active_users == 0);
    assert(subreddit->over18 == false);

    subreddit_destroy(subreddit);
}

static void test_create_null_name(void)
{
    Subreddit *subreddit = subreddit_create(NULL);

    assert(subreddit == NULL);
}

static void test_create_empty_name(void)
{
    Subreddit *subreddit = subreddit_create("");

    assert(subreddit != NULL);
    assert(subreddit->name != NULL);
    assert(strcmp(subreddit->name, "") == 0);

    subreddit_destroy(subreddit);
}

static void test_destroy_null(void)
{
    subreddit_destroy(NULL);
}

int main(void)
{
    test_create_valid_subreddit();
    test_create_null_name();
    test_create_empty_name();
    test_destroy_null();

    return 0;
}
