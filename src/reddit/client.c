#include "redditanalyzer/reddit/client.h"

#include "redditanalyzer/http/client.h"

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

RaError reddit_client_get_subreddit(RedditClient *client, const char *name)
{
    if (client == NULL || name == NULL)
    {
        return RA_ERR_INVALID_ARGUMENT;
    }

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
    http_response_destroy(&response);

    return error;
}
