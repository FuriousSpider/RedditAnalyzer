#ifndef REDDITANALYZER_TEST_HTTP_SERVER_H
#define REDDITANALYZER_TEST_HTTP_SERVER_H

#include <stdbool.h>
#include <sys/types.h>

typedef struct
{
    int server_fd;
    int port;
    pid_t server_pid;
    char request[4096];
} HttpTestServer;

bool http_test_server_start(HttpTestServer *server);

bool http_test_server_run(
    HttpTestServer *server,
    int status_code,
    const char *body
);

bool http_test_server_run_expect_request(
    HttpTestServer *server,
    int status_code,
    const char *body,
    const char *expected_request
);

void http_test_server_stop(HttpTestServer *server);

#endif /* REDDITANALYZER_TEST_HTTP_SERVER_H */
