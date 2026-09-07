#include "redditanalyzer/json/post_list_parser.h"

#include <assert.h>
#include <string.h>

static void test_valid_json(void)
{
    const char *json =
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

    PostList list = {0};

    RaError error = post_list_from_json(json, &list);

    assert(error == RA_OK);
    assert(list.count == 2U);

    assert(strcmp(list.items[0]->id, "abc123") == 0);
    assert(strcmp(list.items[0]->title, "First post") == 0);
    assert(strcmp(list.items[0]->author, "user_one") == 0);
    assert(list.items[0]->score == 100);
    assert(list.items[0]->comments == 10U);
    assert(list.items[0]->is_video == false);
    assert(list.items[0]->is_self == true);

    assert(strcmp(list.items[1]->id, "def456") == 0);
    assert(strcmp(list.items[1]->title, "Second post") == 0);
    assert(strcmp(list.items[1]->author, "user_two") == 0);
    assert(list.items[1]->score == 250);
    assert(list.items[1]->comments == 20U);
    assert(list.items[1]->is_video == true);
    assert(list.items[1]->is_self == false);

    post_list_destroy(&list);
}

static void test_invalid_arguments(void)
{
    PostList list = {0};

    assert(post_list_from_json(NULL, &list) == RA_ERR_INVALID_ARGUMENT);

    assert(post_list_from_json("{}", NULL) == RA_ERR_INVALID_ARGUMENT);
}

static void test_invalid_json(void)
{
    PostList list = {0};

    assert(post_list_from_json("{invalid json}", &list) == RA_ERR_JSON);

    assert(list.items == NULL);
    assert(list.count == 0U);
}

static void test_missing_data(void)
{
    PostList list = {0};

    assert(
        post_list_from_json(
            "{\"kind\":\"Listing\"}",
            &list
        ) == RA_ERR_JSON
    );

    assert(list.items == NULL);
    assert(list.count == 0U);
}

static void test_missing_children(void)
{
    PostList list = {0};

    assert(
        post_list_from_json(
            "{\"data\":{}}",
            &list
        ) == RA_ERR_JSON
    );

    assert(list.items == NULL);
    assert(list.count == 0U);
}

static void test_empty_children(void)
{
    PostList list = {0};

    assert(
        post_list_from_json(
            "{\"data\":{\"children\":[]}}",
            &list
        ) == RA_OK
    );

    assert(list.count == 0U);

    post_list_destroy(&list);
}

int main(void)
{
    test_valid_json();
    test_invalid_arguments();
    test_invalid_json();
    test_missing_data();
    test_missing_children();
    test_empty_children();

    return 0;
}
