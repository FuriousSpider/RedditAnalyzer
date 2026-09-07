#include "redditanalyzer/model/subreddit.h"
#include "redditanalyzer/reddit/client.h"
#include "redditanalyzer/utils/error.h"
#include "support/http_test_server.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static void test_create_destroy(void)
{
    RedditClientConfig config = {
        .base_url = "http://127.0.0.1"
    };

    RedditClient *client = reddit_client_create(&config);

    assert(client != NULL);

    reddit_client_destroy(client);
}

static void test_invalid_arguments(void)
{
    assert(reddit_client_create(NULL) == NULL);

    Subreddit *subreddit = NULL;

    assert(reddit_client_get_subreddit(NULL, "programming", &subreddit) == RA_ERR_INVALID_ARGUMENT);

    RedditClientConfig config = {
        .base_url = "http://127.0.0.1"
    };

    RedditClient *client = reddit_client_create(&config);

    assert(client != NULL);

    assert(reddit_client_get_subreddit(client, NULL, &subreddit) == RA_ERR_INVALID_ARGUMENT);

    assert(reddit_client_get_subreddit(client, "", NULL) == RA_ERR_INVALID_ARGUMENT);

    reddit_client_destroy(client);
}

static void test_get_subreddit(void)
{
    HttpTestServer server;

    assert(http_test_server_start(&server));

    char base_url[64];

    int written = snprintf(
        base_url,
        sizeof(base_url),
        "http://127.0.0.1:%u",
        server.port
    );

    assert(written > 0);
    assert((size_t)written < sizeof(base_url));

    RedditClientConfig config = {
        .base_url = base_url
    };

    RedditClient *client = reddit_client_create(&config);

    assert(client != NULL);

    const char *body =
        "{"
        "\"kind\":\"t5\","
        "\"data\":{"
            "\"display_name\":\"programming\","
            "\"public_description\":\"A subreddit about programming.\","
            "\"subscribers\":123456,"
            "\"accounts_active\":789"
        "}"
        "}";

    assert(
        http_test_server_run(
            &server,
            200,
            body
        )
    );

    Subreddit *subreddit = NULL;

    RaError error = reddit_client_get_subreddit(
        client,
        "programming",
        &subreddit
    );

    fprintf(
        stderr,
        "reddit_client_get_subreddit() returned: %d (%s)\n",
        error,
        ra_error_string(error)
    );

    assert(error == RA_OK);
    assert(subreddit != NULL);

    assert(strcmp(subreddit->name, "programming") == 0);
    assert(strcmp(subreddit->display_name, "programming") == 0);
    assert(strcmp(subreddit->description, "A subreddit about programming.") == 0);

    assert(subreddit->subscribers == 123456);
    assert(subreddit->active_users == 789);

    subreddit_destroy(subreddit);
    reddit_client_destroy(client);

    http_test_server_stop(&server);
}

static void test_get_posts(void)
{
    HttpTestServer server = {0};

    assert(http_test_server_start(&server));

    char base_url[64];

    int written = snprintf(
        base_url,
        sizeof(base_url),
        "http://127.0.0.1:%d",
        server.port
    );

    assert(written > 0);
    assert((size_t)written < sizeof(base_url));

    RedditClientConfig config = {
        .base_url = base_url
    };

    RedditClient *client = reddit_client_create(&config);

    assert(client != NULL);

    const char *body =
        "{"
        "\"kind\":\"Listing\","
        "\"data\":{"
            "\"children\":["
                "{"
                    "\"kind\":\"t3\","
                    "\"data\":{"
                        "\"id\":\"abc123\","
                        "\"title\":\"First post\","
                        "\"author\":\"user_one\","
                        "\"score\":100,"
                        "\"num_comments\":10,"
                        "\"created_utc\":1750000000,"
                        "\"is_video\":false,"
                        "\"is_self\":true"
                    "}"
                "},"
                "{"
                    "\"kind\":\"t3\","
                    "\"data\":{"
                        "\"id\":\"def456\","
                        "\"title\":\"Second post\","
                        "\"author\":\"user_two\","
                        "\"score\":250,"
                        "\"num_comments\":20,"
                        "\"created_utc\":1750000100,"
                        "\"is_video\":true,"
                        "\"is_self\":false"
                    "}"
                "}"
            "]"
        "}"
        "}";

    assert(
        http_test_server_run(
            &server,
            200,
            body
        )
    );

    PostList list = {0};

    RaError error = reddit_client_get_posts(
        client,
        "programming",
        2U,
        &list
    );

    assert(error == RA_OK);
    assert(list.count == 2U);

    assert(strcmp(list.items[0]->id, "abc123") == 0);
    assert(strcmp(list.items[0]->title, "First post") == 0);
    assert(list.items[0]->score == 100);

    assert(strcmp(list.items[1]->id, "def456") == 0);
    assert(strcmp(list.items[1]->title, "Second post") == 0);
    assert(list.items[1]->score == 250);

    post_list_destroy(&list);
    reddit_client_destroy(client);
    http_test_server_stop(&server);
}

static void test_get_posts_page(void)
{
    HttpTestServer server = {0};

    assert(http_test_server_start(&server));

    char base_url[64];

    int written = snprintf(
        base_url,
        sizeof(base_url),
        "http://127.0.0.1:%d",
        server.port
    );

    assert(written > 0);
    assert((size_t)written < sizeof(base_url));

    RedditClientConfig config = {
        .base_url = base_url
    };

    RedditClient *client = reddit_client_create(&config);

    assert(client != NULL);

    const char *body =
        "{"
        "\"kind\":\"Listing\","
        "\"data\":{"
            "\"after\":\"t3_next\","
            "\"children\":["
                "{"
                    "\"kind\":\"t3\","
                    "\"data\":{"
                        "\"id\":\"abc123\","
                        "\"title\":\"Next page post\","
                        "\"author\":\"test_user\","
                        "\"score\":321,"
                        "\"num_comments\":12,"
                        "\"created_utc\":1750000000,"
                        "\"is_video\":false,"
                        "\"is_self\":true"
                    "}"
                "}"
            "]"
        "}"
        "}";

    assert(
        http_test_server_run_expect_request(
            &server,
            200,
            body,
            "GET /r/programming.json?limit=2&after=t3_previous"
        )
    );

    PostList list = {0};

    RaError error = reddit_client_get_posts_page(
        client,
        "programming",
        2U,
        "t3_previous",
        &list
    );

    assert(error == RA_OK);
    assert(list.count == 1U);

    assert(strcmp(list.items[0]->id, "abc123") == 0);
    assert(strcmp(list.items[0]->title, "Next page post") == 0);
    assert(list.items[0]->score == 321);

    assert(list.after != NULL);
    assert(strcmp(list.after, "t3_next") == 0);

    post_list_destroy(&list);
    reddit_client_destroy(client);
    http_test_server_stop(&server);
}

int main(void)
{
    test_create_destroy();
    test_invalid_arguments();
    test_get_subreddit();
    test_get_posts();
    test_get_posts_page();

    return 0;
}

