#define _POSIX_C_SOURCE 200809L

#include "redditanalyzer/json/subreddit_parser.h"

#include <cjson/cJSON.h>
#include <stdlib.h>
#include <string.h>

static char *duplicate_json_string(const cJSON *object, const char *name)
{
    const cJSON *item = cJSON_GetObjectItemCaseSensitive(object, name);

    if (!cJSON_IsString(item) || item->valuestring == NULL)
    {
        return NULL;
    }

    return strdup(item->valuestring);
}

RaError subreddit_from_json(
    const char *json,
    Subreddit **subreddit
)
{
    if (json == NULL || subreddit == NULL)
    {
        return RA_ERR_INVALID_ARGUMENT;
    }

    *subreddit = NULL;

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

    char *name = duplicate_json_string(data, "display_name");

    if (name == NULL)
    {
        cJSON_Delete(root);
        return RA_ERR_JSON;
    }

    Subreddit *result = subreddit_create(name);

    free(name);

    if (result == NULL)
    {
        cJSON_Delete(root);
        return RA_ERR_OUT_OF_MEMORY;
    }

    result->display_name = duplicate_json_string(data, "display_name");

    if (result->display_name == NULL)
    {
        subreddit_destroy(result);
        cJSON_Delete(root);
        return RA_ERR_JSON;
    }

    result->description = duplicate_json_string(data, "public_description");

    if (result->description == NULL)
    {
        result->description = strdup("");
    }

    if (result->description == NULL)
    {
        subreddit_destroy(result);
        cJSON_Delete(root);
        return RA_ERR_OUT_OF_MEMORY;
    }

    const cJSON *subscribers = cJSON_GetObjectItemCaseSensitive(data, "subscribers");

    if (cJSON_IsNumber(subscribers) && subscribers->valuedouble >= 0.0)
    {
        result->subscribers = (uint64_t)subscribers->valuedouble;
    }

    const cJSON *active_users = cJSON_GetObjectItemCaseSensitive(data, "accounts_active");

    if (cJSON_IsNumber(active_users) && active_users->valuedouble >= 0.0)
    {
        result->active_users = (uint64_t)active_users->valuedouble;
    }

    *subreddit = result;

    cJSON_Delete(root);

    return RA_OK;
}
