#include "redditanalyzer/model/subreddit.h"
#include "redditanalyzer/utils/string.h"

#include <stdlib.h>

Subreddit *subreddit_create(const char *name)
{
    if (name == NULL)
    {
        return NULL;
    }

    Subreddit *subreddit = calloc(1, sizeof(*subreddit));

    if (subreddit == NULL)
    {
        return NULL;
    }

    subreddit->name = ra_strdup(name);

    if (subreddit->name == NULL)
    {
        free(subreddit);
        return NULL;
    }

    return subreddit;
}

void subreddit_destroy(Subreddit *subreddit)
{
    if (subreddit == NULL)
    {
        return;
    }

    free(subreddit->name);
    free(subreddit->display_name);
    free(subreddit->description);

    free(subreddit);
}
