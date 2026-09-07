#ifndef REDDITANALYZER_REDDIT_CLIENT_H
#define REDDITANALYZER_REDDIT_CLIENT_H

#include "redditanalyzer/model/post_list.h"
#include "redditanalyzer/model/subreddit.h"
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
    const char *name,
    Subreddit **subreddit
);

RaError reddit_client_get_posts(
    RedditClient *client,
    const char *name,
    PostList *post_list
);

#endif /* REDDITANALYZER_REDDIT_CLIENT_H */
