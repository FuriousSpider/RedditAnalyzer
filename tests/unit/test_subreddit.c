#include "redditanalyzer/model/subreddit.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    Subreddit *subreddit = subreddit_create("programming");

    assert(subreddit != NULL);
    assert(subreddit->name != NULL);
    assert(strcmp(subreddit->name, "programming") == 0);

    subreddit_destroy(subreddit);

    return 0;
}