#include "redditanalyzer/json/post_parser.h"
#include "redditanalyzer/utils/string.h"

#include <cjson/cJSON.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static RaError parse_post_string(
    const cJSON *data,
    const char *field_name,
    char **destination
)
{
    const cJSON *item = cJSON_GetObjectItemCaseSensitive(data, field_name);

    if (!cJSON_IsString(item) || item->valuestring == NULL)
    {
        return RA_ERR_JSON;
    }

    *destination = ra_strdup(item->valuestring);

    if (*destination == NULL)
    {
        return RA_ERR_OUT_OF_MEMORY;
    }

    return RA_OK;
}

static RaError parse_post_number(
    const cJSON *data,
    const char *field_name,
    int64_t *destination
)
{
    const cJSON *item = cJSON_GetObjectItemCaseSensitive(data, field_name);

    if (!cJSON_IsNumber(item))
    {
        return RA_ERR_JSON;
    }

    *destination = (int64_t)item->valuedouble;

    return RA_OK;
}

static void parse_post_optional_numbers(const cJSON *data, Post *post)
{
    const cJSON *comments = cJSON_GetObjectItemCaseSensitive(data, "num_comments");

    if (cJSON_IsNumber(comments) && comments->valuedouble >= 0.0)
    {
        post->comments = (uint64_t)comments->valuedouble;
    }

    const cJSON *created_utc = cJSON_GetObjectItemCaseSensitive(data, "created_utc");

    if (cJSON_IsNumber(created_utc) && created_utc->valuedouble >= 0.0)
    {
        post->created_at = (time_t)created_utc->valuedouble;
    }
}

static void parse_post_flags(const cJSON *data, Post *post)
{
    const cJSON *is_video = cJSON_GetObjectItemCaseSensitive(data, "is_video");

    if (cJSON_IsBool(is_video))
    {
        post->is_video = cJSON_IsTrue(is_video);
    }

    const cJSON *is_self = cJSON_GetObjectItemCaseSensitive(data, "is_self");

    if (cJSON_IsBool(is_self))
    {
        post->is_self = cJSON_IsTrue(is_self);
    }
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

    RaError error = parse_post_string(data, "id", &result->id);

    if (error != RA_OK)
    {
        post_destroy(result);
        cJSON_Delete(root);
        return error;
    }

    error = parse_post_string(data, "title", &result->title);

    if (error != RA_OK)
    {
        post_destroy(result);
        cJSON_Delete(root);
        return error;
    }

    error = parse_post_string(data, "author", &result->author);

    if (error != RA_OK)
    {
        post_destroy(result);
        cJSON_Delete(root);
        return error;
    }

    int64_t score = 0;

    error = parse_post_number(data, "score", &score);

    if (error == RA_OK)
    {
        result->score = score;
    }

    parse_post_optional_numbers(data, result);

    parse_post_flags(data, result);

    *post = result;

    cJSON_Delete(root);

    return RA_OK;
}
