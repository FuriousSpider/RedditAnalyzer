#ifndef REDDITANALYZER_JSON_POST_LIST_PARSER_H
#define REDDITANALYZER_JSON_POST_LIST_PARSER_H

#include "redditanalyzer/model/post_list.h"
#include "redditanalyzer/utils/error.h"

RaError post_list_from_json(
    const char *json,
    PostList *post_list
);

#endif /* REDDITANALYZER_JSON_POST_LIST_PARSER_H */
