add_executable(
    redditanalyzer_tests
    unit/test_smoke.c
)

target_link_libraries(
    redditanalyzer_tests
    PRIVATE
        redditanalyzer_core
)

target_compile_features(
    redditanalyzer_tests
    PRIVATE
        c_std_17
)

add_test(
    NAME redditanalyzer_smoke
    COMMAND redditanalyzer_tests
)


add_executable(
    redditanalyzer_subreddit_tests
    unit/test_subreddit.c
)

target_link_libraries(
    redditanalyzer_subreddit_tests
    PRIVATE
        redditanalyzer_core
)

target_compile_features(
    redditanalyzer_subreddit_tests
    PRIVATE
        c_std_17
)

add_test(
    NAME redditanalyzer_subreddit
    COMMAND redditanalyzer_subreddit_tests
)