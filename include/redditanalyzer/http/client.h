#ifndef REDDITANALYZER_HTTP_CLIENT_H
#define REDDITANALYZER_HTTP_CLIENT_H

#include <stddef.h>

#include "redditanalyzer/utils/error.h"

typedef struct
{
    char *data;
    size_t size;
    long status_code;
} HttpResponse;

RaError http_get(
    const char *url,
    HttpResponse *response
);

void http_response_destroy(HttpResponse *response);

#endif /* REDDITANALYZER_HTTP_CLIENT_H */
