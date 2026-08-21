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

bool http_test_server_run(
    HttpTestServer *server,
    int status_code,
    const char *body
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

        // const ssize_t sent = send(
        //     client_fd,
        //     response,
        //     (size_t)response_length,
        //     0
        // );

        close(client_fd);
        close(server->server_fd);

        // if (sent != (ssize_t)response_length)
        // {
        //     _exit(EXIT_FAILURE);
        // }

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
