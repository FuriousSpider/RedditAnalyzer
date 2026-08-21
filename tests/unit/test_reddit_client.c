#include "redditanalyzer/reddit/client.h"

#include <assert.h>
#include <stddef.h>

static void test_create_destroy(void)
{
    RedditClient *client = reddit_client_create();

    assert(client != NULL);

    reddit_client_destroy(client);
}

static void test_invalid_arguments(void)
{
    assert(reddit_client_get_subreddit(NULL, "programming") == RA_ERR_INVALID_ARGUMENT);

    RedditClient *client = reddit_client_create();

    assert(client != NULL);

    assert(reddit_client_get_subreddit(client, NULL) == RA_ERR_INVALID_ARGUMENT);

    reddit_client_destroy(client);
}

int main(void)
{
    test_create_destroy();
    test_invalid_arguments();

    return 0;
}

