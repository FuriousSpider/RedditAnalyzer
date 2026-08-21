#include "redditanalyzer/http/client.h"
#include "../support/http_test_server.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

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

static void test_invalid_url(void)
{
    HttpResponse response = {0};

    RaError error = http_get("http://this-host-does-not-exist.invalid", &response);

    assert(error == RA_ERR_NETWORK);
    assert(response.data == NULL);
    assert(response.size == 0U);
    assert(response.status_code == 0L);

    http_response_destroy(&response);
}

static void test_server_start_stop(void)
{
    HttpTestServer server = {
        .server_fd = -1,
        .port = 0
    };

    assert(http_test_server_start(&server));
    assert(server.server_fd >= 0);
    assert(server.port > 0);

    http_test_server_stop(&server);

    assert(server.server_fd == -1);
    assert(server.port == 0);
}

static void test_http_get_ok(void)
{
    HttpTestServer server = {
        .server_fd = -1,
        .port = 0,
        .server_pid = 0
    };

    assert(http_test_server_start(&server));

    char url[128];

    int written = snprintf(
        url,
        sizeof(url),
        "http://127.0.0.1:%d",
        server.port
    );

    assert(written > 0);
    assert((size_t)written < sizeof(url));

    assert(http_test_server_run(&server, 200, "{\"status\":\"ok\"}"));

    HttpResponse response = {0};

    // RaError error = http_get(url, &response);

    // assert(error == RA_OK);

    RaError error = http_get(url, &response);
    fprintf(stderr, "http_get() returned: %d (%s), HTTP status: %ld\n", error, ra_error_string(error), response.status_code);
    assert(error == RA_OK);


    assert(response.status_code == 200L);
    assert(response.data != NULL);
    assert(strcmp(response.data, "{\"status\":\"ok\"}") == 0);

    http_response_destroy(&response);
    http_test_server_stop(&server);
}

static void test_http_get_not_found(void)
{
    HttpTestServer server = {
        .server_fd = -1,
        .port = 0,
        .server_pid = 0
    };

    assert(http_test_server_start(&server));

    char url[128];

    int written = snprintf(
        url,
        sizeof(url),
        "http://127.0.0.1:%d/not-found",
        server.port
    );

    assert(written > 0);
    assert((size_t)written < sizeof(url));

    assert(http_test_server_run(&server, 404, "{\"error\":\"not found\"}"));

    HttpResponse response = {0};

    RaError error = http_get(url, &response);

    assert(error == RA_ERR_NOT_FOUND);
    assert(response.status_code == 404L);
    assert(response.data != NULL);
    assert(strcmp(response.data, "{\"error\":\"not found\"}") == 0);

    http_response_destroy(&response);
    http_test_server_stop(&server);
}

static void test_http_get_rate_limit(void)
{
    HttpTestServer server = {
        .server_fd = -1,
        .port = 0,
        .server_pid = 0
    };

    assert(http_test_server_start(&server));

    char url[128];

    int written = snprintf(
        url,
        sizeof(url),
        "http://127.0.0.1:%d/rate-limit",
        server.port
    );

    assert(written > 0);
    assert((size_t)written < sizeof(url));

    assert(http_test_server_run(&server, 429, "{\"error\":\"rate limited\"}"));

    HttpResponse response = {0};

    RaError error = http_get(url, &response);

    assert(error == RA_ERR_RATE_LIMIT);
    assert(response.status_code == 429L);
    assert(response.data != NULL);
    assert(strcmp(response.data, "{\"error\":\"rate limited\"}") == 0);

    http_response_destroy(&response);
    http_test_server_stop(&server);
}

static void test_http_get_server_error(void)
{
    HttpTestServer server = {
        .server_fd = -1,
        .port = 0,
        .server_pid = 0
    };

    assert(http_test_server_start(&server));

    char url[128];

    int written = snprintf(
        url,
        sizeof(url),
        "http://127.0.0.1:%d/server-error",
        server.port
    );

    assert(written > 0);
    assert((size_t)written < sizeof(url));

    assert(http_test_server_run(&server, 500, "{\"error\":\"internal server error\"}"));

    HttpResponse response = {0};

    RaError error = http_get(url, &response);

    assert(error == RA_ERR_HTTP);
    assert(response.status_code == 500L);
    assert(response.data != NULL);
    assert(strcmp(response.data, "{\"error\":\"internal server error\"}") == 0);

    http_response_destroy(&response);
    http_test_server_stop(&server);
}

int main(void)
{
    test_invalid_arguments();
    test_response_destroy_null();
    test_response_destroy();
    test_invalid_url();
    test_server_start_stop();
    test_http_get_ok();
    test_http_get_not_found();
    test_http_get_rate_limit();
    test_http_get_server_error();

    return 0;
}
