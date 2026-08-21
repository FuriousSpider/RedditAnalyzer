#include "redditanalyzer/reddit/client.h"

#include <stdlib.h>

struct RedditClient
{
    int dummy;
};

RedditClient *reddit_client_create(void)
{
    RedditClient *client = calloc(1, sizeof(*client));

    if (client == NULL)
    {
        return NULL;
    }

    return client;
}

void reddit_client_destroy(RedditClient *client)
{
    free(client);
}

RaError reddit_client_get_subreddit(RedditClient *client, const char *name)
{
    if (client == NULL || name == NULL)
    {
        return RA_ERR_INVALID_ARGUMENT;
    }

    return RA_OK;
}
