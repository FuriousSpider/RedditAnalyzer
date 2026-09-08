#include "redditanalyzer/reddit/client.h"

#include "redditanalyzer/http/client.h"

#include "redditanalyzer/json/post_list_parser.h"
#include "redditanalyzer/json/subreddit_parser.h"

#include "redditanalyzer/utils/string.h"


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct RedditClient
{
    char *base_url;
};

RedditClient *reddit_client_create(const RedditClientConfig *config)
{
    if (config == NULL || config->base_url == NULL)
    {
        return NULL;
    }

    RedditClient *client = calloc(1, sizeof(*client));

    if (client == NULL)
    {
        return NULL;
    }

    client->base_url = ra_strdup(config->base_url);

    if (client->base_url == NULL)
    {
        free(client);
        return NULL;
    }

    return client;
}

void reddit_client_destroy(RedditClient *client)
{
    if (client == NULL)
    {
        return;
    }
    free(client->base_url);
    free(client);
}

RaError reddit_client_get_subreddit(RedditClient *client, const char *name, Subreddit **subreddit)
{
    if (client == NULL || name == NULL || subreddit == NULL)
    {
        return RA_ERR_INVALID_ARGUMENT;
    }

    *subreddit = NULL;

    size_t base_length = strlen(client->base_url);
    size_t name_length = strlen(name);

    if (name_length == 0U)
    {
        return RA_ERR_INVALID_ARGUMENT;
    }

    const size_t suffix_lenght = strlen("/r//about.json");

    if (base_length > SIZE_MAX - name_length || base_length + name_length > SIZE_MAX - suffix_lenght - 1U)
    {
        return RA_ERR_INTERNAL;
    }

    const size_t url_size = base_length + strlen("/r/") + name_length + strlen("/about.json") + 1U;

    char *url = malloc(url_size);

    if (url == NULL)
    {
        return RA_ERR_OUT_OF_MEMORY;
    }

    int written = snprintf(
        url,
        url_size,
        "%s/r/%s/about.json",
        client->base_url,
        name
    );

    if (written < 0 || (size_t)written >= url_size)
    {
        free(url);
        return RA_ERR_INTERNAL;
    }

    HttpResponse response = {0};

    RaError error = http_get(url, &response);

    free(url);

    if (error != RA_OK)
    {
        http_response_destroy(&response);
        return error;
    }

    error = subreddit_from_json(response.data, subreddit);

    http_response_destroy(&response);

    return error;
}

RaError reddit_client_get_posts(
    RedditClient *client,
    const char *name,
    size_t limit,
    PostList *post_list
)
{
    if (client == NULL || name == NULL || limit == 0U || post_list == NULL)
    {
        return RA_ERR_INVALID_ARGUMENT;
    }

    if (name[0] == '\0')
    {
        return RA_ERR_INVALID_ARGUMENT;
    }

    RaError error = post_list_init(post_list);

    if (error != RA_OK)
    {
        return error;
    }

    char *after = NULL;

    while (post_list->count < limit)
    {
        size_t remaining = limit - post_list->count;
        size_t page_limit = remaining;

        if (page_limit > 100U)
        {
            page_limit = 100U;
        }

        PostList page = {0};

        error = reddit_client_get_posts_page(
            client,
            name,
            page_limit,
            after,
            &page
        );

        free(after);
        after = NULL;

        if (error != RA_OK)
        {
            post_list_destroy(&page);
            post_list_destroy(post_list);

            return error;
        }

        for (size_t i = 0U; i < page.count; i++)
        {
            error = post_list_append(
                post_list,
                page.items[i]
            );

            if (error != RA_OK)
            {
                page.items[i] = NULL;

                post_list_destroy(&page);
                post_list_destroy(post_list);

                return error;
            }

            page.items[i] = NULL;
        }

        if (page.after == NULL || page.after[0] == '\0')
        {
            post_list_destroy(&page);
            break;
        }

        after = ra_strdup(page.after);

        post_list_destroy(&page);

        if (after == NULL)
        {
            post_list_destroy(post_list);
            return RA_ERR_OUT_OF_MEMORY;
        }
    }

    free(after);

    return RA_OK;
}

RaError reddit_client_get_posts_page(
    RedditClient *client,
    const char *name,
    size_t limit,
    const char *after,
    PostList *post_list
)
{
    if (client == NULL || name == NULL ||  limit == 0U || post_list == NULL)
    {
        return RA_ERR_INVALID_ARGUMENT;
    }

    if (name[0] == '\0')
    {
        return RA_ERR_INVALID_ARGUMENT;
    }

    size_t base_length = strlen(client->base_url);
    size_t name_length = strlen(name);

    const char *after_parameter = "";

    if (after != NULL && after[0] != '\0')
    {
        after_parameter = "&after=";
    }

    size_t after_length = strlen(after_parameter);

    if (after != NULL)
    {
        after_length += strlen(after);
    }

    const size_t url_size = 
        base_length + 
        strlen("/r/") + 
        name_length + 
        strlen(".json?limit=") + 
        20U + 
        after_length + 
        1U;

    char *url = malloc(url_size);

    if (url == NULL)
    {
        return RA_ERR_OUT_OF_MEMORY;
    }

    int written;

    if (after != NULL && after[0] != '\0')
    {
        written = snprintf(
            url,
            url_size,
            "%s/r/%s.json?limit=%zu&after=%s",
            client->base_url,
            name,
            limit,
            after
        );
    }
    else
    {
        written = snprintf(
            url,
            url_size,
            "%s/r/%s.json?limit=%zu",
            client->base_url,
            name,
            limit
        );
    }

    if (written < 0 || (size_t)written >= url_size)
    {
        free(url);
        return RA_ERR_INTERNAL;
    }

    HttpResponse response = {0};

    RaError error = http_get(url, &response);

    free(url);

    if (error != RA_OK)
    {
        http_response_destroy(&response);
        return error;
    }

    error = post_list_from_json(
        response.data,
        post_list
    );

    http_response_destroy(&response);

    return error;
}
