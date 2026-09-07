#include "redditanalyzer/json/post_list_parser.h"

#include "redditanalyzer/json/post_parser.h"

#include <cjson/cJSON.h>
#include <stdlib.h>

RaError post_list_from_json(
    const char *json,
    PostList *post_list
)
{
    if (json == NULL || post_list == NULL)
    {
        return RA_ERR_INVALID_ARGUMENT;
    }

    RaError error = post_list_init(post_list);

    if (error != RA_OK)
    {
        return error;
    }

    cJSON *root = cJSON_Parse(json);

    if (root == NULL)
    {
        post_list_destroy(post_list);
        return RA_ERR_JSON;
    }

    const cJSON *data = cJSON_GetObjectItemCaseSensitive(root, "data");

    if (!cJSON_IsObject(data))
    {
        cJSON_Delete(root);
        post_list_destroy(post_list);
        return RA_ERR_JSON;
    }

    const cJSON *children = cJSON_GetObjectItemCaseSensitive(data, "children");

    if (!cJSON_IsArray(children))
    {
        cJSON_Delete(root);
        post_list_destroy(post_list);
        return RA_ERR_JSON;
    }

    const cJSON *child = NULL;

    cJSON_ArrayForEach(child, children)
    {
        const cJSON *post_data = cJSON_GetObjectItemCaseSensitive(child, "data");

        if (!cJSON_IsObject(post_data))
        {
            cJSON_Delete(root);
            post_list_destroy(post_list);
            return RA_ERR_JSON;
        }

        cJSON *post_root = cJSON_CreateObject();

        if (post_root == NULL)
        {
            cJSON_Delete(root);
            post_list_destroy(post_list);
            return RA_ERR_OUT_OF_MEMORY;
        }

        cJSON *post_data_copy = cJSON_Duplicate(post_data, 1);

        if (post_data_copy == NULL || !cJSON_AddItemToObject(post_root, "data", post_data_copy))
        {
            cJSON_Delete(post_data_copy);
            cJSON_Delete(post_root);
            cJSON_Delete(root);
            post_list_destroy(post_list);
            return RA_ERR_OUT_OF_MEMORY;
        }

        char *post_json = cJSON_PrintUnformatted(post_root);

        cJSON_Delete(post_root);

        if (post_json == NULL)
        {
            cJSON_Delete(root);
            post_list_destroy(post_list);
            return RA_ERR_OUT_OF_MEMORY;
        }

        Post *post = NULL;

        error = post_from_json(post_json, &post);

        free(post_json);

        if (error != RA_OK)
        {
            cJSON_Delete(root);
            post_list_destroy(post_list);
            return error;
        }

        error = post_list_append(post_list, post);

        if (error != RA_OK)
        {
            post_destroy(post);
            cJSON_Delete(root);
            post_list_destroy(post_list);
            return error;
        }
    }

    cJSON_Delete(root);

    return RA_OK;
}
