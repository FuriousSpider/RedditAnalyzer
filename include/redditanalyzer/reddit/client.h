#ifndef REDDITANALYZER_REDDIT_CLIENT_H
#define REDDITANALYZER_REDDIT_CLIENT_H

#include "redditanalyzer/utils/error.h"

typedef struct RedditClient RedditClient;

typedef struct
{
    const char *base_url;
} RedditClientConfig;

RedditClient *reddit_client_create(const RedditClientConfig *config);

void reddit_client_destroy(RedditClient *client);

RaError reddit_client_get_subreddit(
    RedditClient *client,
    const char *name
);

#endif /* REDDITANALYZER_REDDIT_CLIENT_H */
