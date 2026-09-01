#include "redditanalyzer/json/subreddit_parser.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

static void test_parse_valid_subreddit(void)
{
    const char *json = 
        "{"
        "\"kind\":\"t5\","
        "\"data\":{"
        "\"display_name\":\"programming\","
        "\"public_description\":\"A subreddit about programming.\","
        "\"subscribers\":123456,"
        "\"accounts_active\":789"
        "}"
        "}";

    Subreddit *subreddit = NULL;

    RaError error = subreddit_from_json(json, &subreddit);

    assert(error == RA_OK);
    assert(subreddit != NULL);

    assert(subreddit->name != NULL);
    assert(strcmp(subreddit->name, "programming") == 0);

    assert(subreddit->display_name != NULL);
    assert(strcmp(subreddit->display_name, "programming") == 0);

    assert(subreddit->description != NULL);
    assert(strcmp(subreddit->description, "A subreddit about programming.") == 0);

    assert(subreddit->subscribers == UINT64_C(123456));
    assert(subreddit->active_users == UINT64_C(789));

    subreddit_destroy(subreddit);
}

static void test_invalid_arguments(void)
{
    Subreddit *subreddit = NULL;

    assert(subreddit_from_json(NULL, &subreddit) == RA_ERR_INVALID_ARGUMENT);
    assert(subreddit_from_json("{}", &subreddit) == RA_ERR_JSON);
}

static void test_invalid_json(void)
{
    Subreddit *subreddit = NULL;
    RaError error = subreddit_from_json("{invalid json}", &subreddit);

    assert(error = RA_ERR_JSON);
    assert(subreddit == NULL);
}

static void test_missing_data(void)
{
    Subreddit *subreddit = NULL;

    RaError error = subreddit_from_json(
        "{\"kind\":\"t5\"}",
        &subreddit
    );

    assert(error == RA_ERR_JSON);
    assert(subreddit == NULL);
}

static void test_missing_description(void)
{
    const char *json = 
        "{"
        "\"kind\":\"t5\","
        "\"data\":{"
        "\"display_name\":\"programming\","
        "\"subscribers\":100,"
        "\"accounts_active\":10"
        "}"
        "}";

    Subreddit *subreddit = NULL;

    RaError error = subreddit_from_json(json, &subreddit);

    assert(error == RA_OK);
    assert(subreddit != NULL);

    assert(strcmp(subreddit->name, "programming") == 0);
    assert(strcmp(subreddit->display_name, "programming") == 0);
    assert(strcmp(subreddit->description, "") == 0);

    assert(subreddit->subscribers == UINT64_C(100));
    assert(subreddit->active_users == UINT64_C(10));

    subreddit_destroy(subreddit);
}

int main(void)
{
    test_parse_valid_subreddit();
    test_invalid_arguments();
    test_invalid_json();
    test_missing_data();
    test_missing_description();

    return 0;
}
