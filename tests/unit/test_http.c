#include "redditanalyzer/http/client.h"

#include <assert.h>

static void test_invalid_arguments(void)
{
    HttpResponse response = {0};

    assert(http_get(NULL, &response) == RA_ERR_INVALID_ARGUMENT);
    
    assert(http_get("https://example.com", NULL) == RA_ERR_INVALID_ARGUMENT);
}

static void test_response_destroy_null(void)
{
    http_response_destroy(NULL);
}

static void test_response_destroy(void)
{
    HttpResponse response = {
        .data = NULL,
        .size = 0U,
        .status_code = 0L
    };

    http_response_destroy(&response);

    assert(response.data == NULL);
    assert(response.size == 0U);
    assert(response.status_code == 0L);
}

int main(void)
{
    test_invalid_arguments();
    test_response_destroy_null();
    test_response_destroy();
}
