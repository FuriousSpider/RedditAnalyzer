#define _POSIZ_C_SOURCE 200809L

#include "redditanalyzer/json/post_parser.h"
#include "redditanalyzer/utils/string.h"

#include <cjson/cJSON.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static char *duplicate_json_string(
    const cJSON *object,
    const char *name
)
{
    const cJSON *item = cJSON_GetObjectItemCaseSensitive(object, name);

    if (!cJSON_IsString(item) || item->valuestring == NULL)
    {
        return NULL;
    }

    return ra_strdup(item->valuestring);
}

RaError post_from_json(
    const char *json,
    Post **post
)
{
    if (json == NULL || post == NULL)
    {
        return RA_ERR_INVALID_ARGUMENT;
    }

    *post = NULL;

    cJSON *root = cJSON_Parse(json);

    if (root == NULL)
    {
        return RA_ERR_JSON;
    }

    const cJSON *data = cJSON_GetObjectItemCaseSensitive(root, "data");

    if (!cJSON_IsObject(data))
    {
        cJSON_Delete(root);
        return RA_ERR_JSON;
    }

    Post *result = post_create();

    if (result == NULL)
    {
        cJSON_Delete(root);
        return RA_ERR_OUT_OF_MEMORY;
    }

    result->id = duplicate_json_string(data, "id");

    if (result->id == NULL)
    {
        post_destroy(result);
        cJSON_Delete(root);
        return RA_ERR_JSON;
    }

    result->title = duplicate_json_string(data, "title");

    if (result->title == NULL)
    {
        post_destroy(result);
        cJSON_Delete(root);
        return RA_ERR_JSON;
    }

    result->author = duplicate_json_string(data, "author");

    if (result->author == NULL)
    {
        post_destroy(result);
        cJSON_Delete(root);
        return RA_ERR_JSON;
    }

    const cJSON *score = cJSON_GetObjectItemCaseSensitive(data, "score");

    if (cJSON_IsNumber(score))
    {
        result->score = (int64_t)score->valuedouble;
    }

    const cJSON * comments = cJSON_GetObjectItemCaseSensitive(data, "num_comments");

    if (cJSON_IsNumber(comments) && comments->valuedouble >= 0.0)
    {
        result->comments = (uint64_t)comments->valuedouble;
    }

    const cJSON *created_utc = cJSON_GetObjectItemCaseSensitive(data, "created_utc");

    if (cJSON_IsNumber(created_utc) && created_utc->valuedouble >= 0.0)
    {
        result->created_at = (time_t)created_utc->valuedouble;
    }

    const cJSON *is_video = cJSON_GetObjectItemCaseSensitive(data, "is_video");

    if (cJSON_IsBool(is_video))
    {
        result->is_video = cJSON_IsTrue(is_video);
    }

    const cJSON *is_self = cJSON_GetObjectItemCaseSensitive(data, "is_self");

    if (cJSON_IsBool(is_self))
    {
        result->is_self = cJSON_IsTrue(is_self);
    }

    *post = result;

    cJSON_Delete(root);

    return RA_OK;
}
