#ifndef REDDITANALYZER_REDDIT_CLIENT_H
#define REDDITANALYZER_REDDIT_CLIENT_H

#include "redditanalyzer/utils/error.h"

typedef struct RedditClient RedditClient;


RedditClient *reddit_client_create(void);

void reddit_client_destroy(RedditClient *client);

RaError reddit_client_get_subreddit(
    RedditClient *client,
    const char *name
);

#endif /* REDDITANALYZER_REDDIT_CLIENT_H */
