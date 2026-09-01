#ifndef REDDITANALYZER_JSON_SUBREDDIT_PARSER_H
#define REDDITANALYZER_JSON_SUBREDDIT_PARSER_H

#include "redditanalyzer/model/subreddit.h"
#include "redditanalyzer/utils/error.h"

RaError subreddit_from_json(
    const char *json,
    Subreddit **subreddit
);

#endif /* REDDITANALYZER_JSON_SUBREDDIT_PARSER_H */
