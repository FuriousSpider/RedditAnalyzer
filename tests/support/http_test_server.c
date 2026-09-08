#define _POSIX_C_SOURCE 200809L

#include "http_test_server.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#define HTTP_TEST_REQUEST_BUFFER_SIZE 4096U
#define HTTP_TEST_RESPONSE_BUFFER_SIZE 4096U

bool http_test_server_start(HttpTestServer *server)
{
    if (server == NULL)
    {
        return false;
    }

    server->server_fd = -1;
    server->port = 0;
    server->server_pid = 0;

    server->server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server->server_fd < 0)
    {
        perror("socket");
        return false;
    }

    struct sockaddr_in address = {0};

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = htons(0);

    if (bind(
        server->server_fd,
        (struct sockaddr *)&address,
        sizeof(address)
    ) < 0)
    {
        perror("bind");
        close(server->server_fd);
        server->server_fd = -1;
        return false;
    }

    if (listen(server->server_fd, 1) < 0)
    {
        perror("listen");
        close(server->server_fd);
        server->server_fd = -1;
        return false;
    }

    socklen_t address_length = sizeof(address);

    if (getsockname(
        server->server_fd,
        (struct sockaddr *)&address,
        &address_length
    ) < 0)
    {
        perror("getsockname");
        close(server->server_fd);
        server->server_fd = -1;
        return false;
    }

    server->port = (int)ntohs(address.sin_port);

    if (server->port <= 0)
    {
        fprintf(stderr, "Invalid server port: %d\n", server->port);

        close(server->server_fd);
        server->server_fd = -1;

        return false;
    }

    return true;
}

static bool http_test_server_run_internal(
    HttpTestServer *server,
    int status_code,
    const char *body,
    const char *expected_request
)
{
    if (server == NULL || body == NULL)
    {
        return false;
    }

    if (server->server_fd < 0)
    {
        return false;
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return false;
    }

    if (pid == 0)
    {
        fprintf(stderr, "HTTP SERVER: waiting for connection...\n");

        int client_fd = accept(
            server->server_fd,
            NULL,
            NULL
        );

        if (client_fd < 0)
        {
            perror("accept");
            _exit(EXIT_FAILURE);
        }

        fprintf(stderr, "HTTP SERVER: client connected\n");

        char request[HTTP_TEST_REQUEST_BUFFER_SIZE];

        ssize_t received = recv(client_fd, request, sizeof(request) -1U, 0);

        if (received <= 0)
        {
            perror("recv");
            close(client_fd);
            close(server->server_fd);
            _exit(EXIT_FAILURE);
        }

        request[received] = '\0';

        fprintf(stderr, "HTTP SERVER: received request:\n%s\n", request);

        if (expected_request != NULL && strstr(request, expected_request) == NULL)
        {
            fprintf(stderr, "HTTP SERVER: expected request fragment not found: %s\n", expected_request);

            close(client_fd);
            close(server->server_fd);
            _exit(EXIT_FAILURE);
        }

        const char *status_text = "Internal Server Error";

        if (status_code == 200)
        {
            status_text = "OK";
        }
        else if (status_code == 404)
        {
            status_text = "Not Found";
        }
        else if (status_code == 429)
        {
            status_text = "Too Many Requests";
        }

        char response[HTTP_TEST_RESPONSE_BUFFER_SIZE];

        int response_length = snprintf(
            response,
            sizeof(response),
            "HTTP/1.1 %d %s\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %zu\r\n"
            "Connection: close\r\n"
            "\r\n"
            "%s",
            status_code,
            status_text,
            strlen(body),
            body
        );

        if (response_length < 0 || (size_t)response_length >= sizeof(response))
        {
            close(client_fd);
            close(server->server_fd);
            _exit(EXIT_FAILURE);
        }

        fprintf(stderr, "HTTP SERVER: sending response (%d bytes)\n", response_length);

        size_t total_sent = 0U;

        while (total_sent < (size_t)response_length)
        {
            ssize_t sent = send(client_fd, response + total_sent, (size_t)response_length - total_sent, 0);
            if (sent <= 0)
            {
                perror("send");
                close(client_fd);
                close(server->server_fd);
                _exit(EXIT_FAILURE);
            }

            total_sent += (size_t)sent;
        }

        close(client_fd);
        close(server->server_fd);

        _exit(EXIT_SUCCESS);
    }

    server->server_pid = pid;

    return true;
}

static bool http_test_server_send_response(
    int client_fd,
    int status_code,
    const char *body
)
{
    const char *status_text = "Internal Server Error";

    if (status_code == 200)
    {
        status_text = "OK";
    }
    else if (status_code == 404)
    {
        status_text = "Not Found";
    }
    else if (status_code == 429)
    {
        status_text = "Too Many Requests";
    }

    char response[HTTP_TEST_RESPONSE_BUFFER_SIZE];

    int response_length = snprintf(
        response,
        sizeof(response),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        status_code,
        status_text,
        strlen(body),
        body
    );

    if (response_length < 0 || (size_t)response_length >= sizeof(response))
    {
        return false;
    }

    size_t total_sent = 0U;

    while (total_sent < (size_t)response_length)
    {
        ssize_t sent = send(
            client_fd,
            response + total_sent,
            (size_t)response_length - total_sent,
            0
        );

        if (sent <= 0)
        {
            return false;
        }

        total_sent += (size_t)sent;
    }

    return true;
}

bool http_test_server_run_sequence(
    HttpTestServer *server,
    int first_status_code,
    const char *first_body,
    const char *first_expected_request,
    int second_status_code,
    const char *second_body,
    const char *second_expected_request
)
{
    if (server == NULL || first_body == NULL || first_expected_request == NULL || second_body == NULL || second_expected_request == NULL)
    {
        return false;
    }

    if (server->server_fd < 0)
    {
        return false;
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return false;
    }

    if (pid == 0)
    {
        const int status_codes[2] = {
            first_status_code,
            second_status_code
        };

        const char *bodies[2] = {
            first_body,
            second_body
        };

        const char *expected_requests[2] = {
            first_expected_request,
            second_expected_request
        };

        for (size_t i = 0U; i < 2U; i++)
        {
            fprintf(stderr, "HTTP SEVER: waiting for connection %zu...\n", i + 1U);

            int client_fd = accept(server->server_fd, NULL, NULL);

            if (client_fd < 0)
            {
                perror("accept");
                _exit(EXIT_FAILURE);
            }

            char request[HTTP_TEST_REQUEST_BUFFER_SIZE];

            ssize_t received = recv(client_fd, request, sizeof(request) - 1U, 0);

            if (received <= 0)
            {
                perror("recv");
                close(client_fd);
                close(server->server_fd);
                _exit(EXIT_FAILURE);
            }

            request[received] = '\0';

            fprintf(stderr, "HTTP SERVER: received request %zu:\n%s\n", i + 1U, request);

            if (strstr(request, expected_requests[i]) == NULL)
            {
                fprintf(stderr, "HTTP SERVER: expected request fragment not found: %s\n", expected_requests[i]);

                close(client_fd);
                close(server->server_fd);
                _exit(EXIT_FAILURE);
            }

            if (
                !http_test_server_send_response(
                    client_fd,
                    status_codes[i],
                    bodies[i]
                )
            ) 
            {
                close(client_fd);
                close(server->server_fd);
                _exit(EXIT_FAILURE);
            }

            close(client_fd);
        }

        close(server->server_fd);
        _exit(EXIT_SUCCESS);
    }

    server->server_pid = pid;

    return true;
}

void http_test_server_stop(HttpTestServer *server)
{
    if (server == NULL)
    {
        return;
    }

    if (server->server_pid > 0)
    {
        int status = 0;

        if(waitpid(server->server_pid, &status, 0) < 0)
        {
            kill(server->server_pid, SIGTERM);
            (void)waitpid(server->server_pid, NULL, 0);
        }

        server->server_pid = 0;
    }


    if (server->server_fd >= 0)
    {
        close(server->server_fd);
        server->server_fd = -1;
    }

    server->port = 0;
}

bool http_test_server_run(
    HttpTestServer *server,
    int status_code,
    const char *body
)
{
    return http_test_server_run_internal(
        server,
        status_code,
        body,
        NULL
    );
}

bool http_test_server_run_expect_request(
    HttpTestServer *server,
    int status_code,
    const char *body,
    const char *expected_request
)
{
    if (expected_request == NULL)
    {
        return false;
    }

    return http_test_server_run_internal(
        server,
        status_code,
        body,
        expected_request
    );
}
