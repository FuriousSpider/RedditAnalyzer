#ifndef REDDITANALYZER_MODEL_SUBREDDIT_H
#define REDDITANALYZER_MODEL_SUBREDDIT_H

#include <stdbool.h>
#include <stdint.h>

typedef struct subreddit
{
    char *name;
    char *display_name;
    char *description;

    uint64_t subscribers;
    uint64_t active_users;

    bool over18;
} Subreddit;

Subreddit *subreddit_create(const char *name);

void subreddit_destroy(Subreddit *subreddit);

#endif