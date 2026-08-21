#include "redditanalyzer/reddit/client.h"

#include <assert.h>
#include <stddef.h>

static void test_create_destroy(void)
{
    RedditClientConfig config = {
        .base_url = "http://127.0.0.1"
    };

    RedditClient *client = reddit_client_create(&config);

    assert(client != NULL);

    reddit_client_destroy(client);
}

static void test_invalid_arguments(void)
{
    assert(reddit_client_create(NULL) == NULL);

    assert(reddit_client_get_subreddit(NULL, "programming") == RA_ERR_INVALID_ARGUMENT);

    RedditClientConfig config = {
        .base_url = "http://127.0.0.1"
    };

    RedditClient *client = reddit_client_create(&config);

    assert(client != NULL);

    assert(reddit_client_get_subreddit(client, NULL) == RA_ERR_INVALID_ARGUMENT);

    assert(reddit_client_get_subreddit(client, "") == RA_ERR_INVALID_ARGUMENT);

    reddit_client_destroy(client);
}

int main(void)
{
    test_create_destroy();
    test_invalid_arguments();

    return 0;
}

