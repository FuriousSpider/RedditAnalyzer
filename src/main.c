#include <stdio.h>
#include "redditanalyzer/utils/logger.h"

int main(void)
{
    ra_log_info("Starting RedditAnalyzer");

    printf("RedditAnalyzer v0.1.0\n");
    printf("Reddit analyzer written in C.\n");

    ra_log_info("RedditAnalyzer shutting down");

    return 0;
}
