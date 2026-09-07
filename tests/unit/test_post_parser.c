#include "redditanalyzer/json/post_parser.h"

#include <assert.h>
#include <string.h>

static void test_valid_json(void)
{
    const char *json = 
        "{"
        "\"kind\":\"t3\","
        "\"data\":{"
            "\"id\":\"abc123\","
            "\"title\":\"Hello Reddit\","
            "\"author\":\"test_user\","
            "\"score\":123,"
            "\"num_comments\":45,"
            "\"created_utc\":1750000000,"
            "\"is_video\":true,"
            "\"is_self\":false"
        "}"
        "}";

    Post *post = NULL;

    RaError error = post_from_json(json, &post);

    assert(error == RA_OK);
    assert(post != NULL);

    assert(strcmp(post->id, "abc123") == 0);
    assert(strcmp(post->title, "Hello Reddit") == 0);
    assert(strcmp(post->author, "test_user") == 0);

    assert(post->score == 123);
    assert(post->comments == 45);
    assert(post->created_at == (time_t)1750000000);
    assert(post->is_video == true);
    assert(post->is_self == false);

    post_destroy(post);
}

static void test_invalid_arguments(void)
{
    Post *post = NULL;

    assert(post_from_json(NULL, &post) == RA_ERR_INVALID_ARGUMENT);

    assert(post_from_json("{}", NULL) == RA_ERR_INVALID_ARGUMENT);
}

static void test_invalid_json(void)
{
    Post *post = NULL;

    assert(post_from_json("{invalid json}", &post) == RA_ERR_JSON);

    assert(post == NULL);
}

static void test_missin_data(void)
{
    Post *post = NULL;

    assert(
        post_from_json(
            "{\"kind\":\"t3\"}",
            &post
        ) == RA_ERR_JSON
    );
}

static void test_missing_required_field(void)
{
    const char *json = 
        "{"
        "\"data\":{"
            "\"id\":\"abc123\","
            "\"title\":\"Hello Reddit\""
        "}"
        "}";

    Post *post = NULL;

    assert(post_from_json(json, &post) == RA_ERR_JSON);

    assert(post == NULL);
}

int main(void)
{
    test_valid_json();
    test_invalid_arguments();
    test_invalid_json();
    test_missin_data();
    test_missing_required_field();

    return 0;
}
