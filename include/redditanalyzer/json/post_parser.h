#ifndef REDDITANALYZER_JSON_POST_PARSER_H
#define REDDITANALYZER_JSON_POST_PARSER_H

#include "redditanalyzer/model/post.h"
#include "redditanalyzer/utils/error.h"

RaError post_from_json(
    const char *json,
    Post **post
);

#endif /* REDDITANALYZER_JSON_POST_PARSER_H */
