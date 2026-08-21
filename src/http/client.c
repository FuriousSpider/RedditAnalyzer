#include "redditanalyzer/http/client.h"

#include <curl/curl.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define HTTP_CONNECT_TIMEOUT_SECONDS 5L
#define HTTP_REQUEST_TIMEOUT_SECONDS 10L

typedef struct
{
    char *data;
    size_t size;
} ResponseBuffer;

static size_t write_callback(
    char *data,
    size_t size,
    size_t count,
    void *userdata
)
{
    ResponseBuffer *buffer = userdata;
    
    if (count != 0U && size > SIZE_MAX / count)
    {
        return 0U;
    }

    const size_t bytes = size * count;

    if (bytes > SIZE_MAX - buffer->size - 1U)
    {
        return 0U;
    }

    char *new_data = realloc(
        buffer->data,
        buffer->size + bytes + 1U
    );

    if (new_data == NULL)
    {
        return 0U;
    }

    buffer->data = new_data;

    memcpy(
        buffer->data + buffer->size,
        data,
        bytes
    );

    buffer->size += bytes;
    buffer->data[buffer->size] = '\0';

    return bytes;
}

static RaError http_status_to_error(long status_code)
{
    if (status_code >= 200L && status_code < 300L)
    {
        return RA_OK;
    }

    if (status_code == 404L)
    {
        return RA_ERR_NOT_FOUND;
    }

    if (status_code == 429L)
    {
        return RA_ERR_RATE_LIMIT;
    }

    if (status_code >= 400L)
    {
        return RA_ERR_HTTP;
    }

    return RA_ERR_HTTP;
}

static RaError curl_result_to_error(CURLcode result)
{
    if (result == CURLE_OK)
    {
        return RA_OK;
    }

    if (result == CURLE_OPERATION_TIMEDOUT)
    {
        return RA_ERR_TIMEOUT;
    }

    return RA_ERR_NETWORK;
}

RaError http_get(
    const char *url,
    HttpResponse *response
)
{
    if (url == NULL || response == NULL)
    {
        return RA_ERR_INVALID_ARGUMENT;
    }

    response->data = NULL;
    response->size = 0U;
    response->status_code = 0L;

    CURL *curl = curl_easy_init();

    if (curl == NULL)
    {
        return RA_ERR_INTERNAL;
    }

    ResponseBuffer buffer = {
        .data = NULL,
        .size = 0U
    };

    CURLcode result = curl_easy_setopt(
        curl,
        CURLOPT_URL,
        url
    );

    if (result != CURLE_OK)
    {
        curl_easy_cleanup(curl);
        return RA_ERR_INTERNAL;
    }

    result = curl_easy_setopt(
        curl,
        CURLOPT_WRITEFUNCTION,
        write_callback
    );

    if (result != CURLE_OK)
    {
        curl_easy_cleanup(curl);
        return RA_ERR_INTERNAL;
    }

    result = curl_easy_setopt(
        curl,
        CURLOPT_WRITEDATA,
        &buffer
    );

    if (result != CURLE_OK)
    {
        curl_easy_cleanup(curl);
        return RA_ERR_INTERNAL;
    }

    result = curl_easy_setopt(
        curl,
        CURLOPT_FOLLOWLOCATION,
        1L
    );

    if (result != CURLE_OK)
    {
        curl_easy_cleanup(curl);
        return RA_ERR_INTERNAL;
    }

    result = curl_easy_setopt(
        curl,
        CURLOPT_CONNECTTIMEOUT,
        HTTP_CONNECT_TIMEOUT_SECONDS
    );

    if (result != CURLE_OK)
    {
        curl_easy_cleanup(curl);
        free(buffer.data);
        return RA_ERR_INTERNAL;
    }

    result = curl_easy_setopt(
        curl,
        CURLOPT_TIMEOUT,
        HTTP_REQUEST_TIMEOUT_SECONDS
    );

    if (result != CURLE_OK)
    {
        curl_easy_cleanup(curl);
        free(buffer.data);
        return RA_ERR_INTERNAL;
    }

    result = curl_easy_setopt(
        curl,
        CURLOPT_USERAGENT,
        "RedditAnalyzer/0.1"
    );

    if (result != CURLE_OK)
    {
        free(buffer.data);
        curl_easy_cleanup(curl);
        return RA_ERR_INTERNAL;
    }

    result = curl_easy_perform(curl);

    if (result != CURLE_OK)
    {
        free(buffer.data);
        curl_easy_cleanup(curl);

        return curl_result_to_error(result);
    }

    long status_code = 0L;

    result = curl_easy_getinfo(
        curl,
        CURLINFO_RESPONSE_CODE,
        &status_code
    );

    curl_easy_cleanup(curl);

    if (result != CURLE_OK)
    {
        free(buffer.data);
        return RA_ERR_INTERNAL;
    }

    response->data = buffer.data;
    response->size = buffer.size;
    response->status_code = status_code;

    return http_status_to_error(status_code);
}

void http_response_destroy(HttpResponse *response)
{
    if (response == NULL)
    {
        return;
    }

    free(response->data);

    response->data = NULL;
    response->size = 0U;
    response->status_code = 0L;
}
