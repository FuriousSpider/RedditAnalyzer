function(add_redditanalyzer_test test_name source_file)
    set(test_target "redditanalyzer_${test_name}_tests")

    add_executable(
        ${test_target}
        ${source_file}
    )

    target_link_libraries(
        ${test_target}
        PRIVATE
            redditanalyzer_core
    )

    target_compile_features(
        ${test_target}
        PRIVATE
            c_std_17
    )

    add_test(
        NAME "redditanalyzer_${test_name}"
        COMMAND ${test_target}
    )
endfunction()

function(add_redditanalyzer_http_test test_name source_file)
    set(test_target "redditanalyzer_${test_name}_tests")

    add_executable(
        ${test_target}
        ${source_file}
        support/http_test_server.c
    )

    target_include_directories(
        ${test_target}
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/support"
    )

    target_link_libraries(
        ${test_target}
        PRIVATE
            redditanalyzer_http
    )

    target_compile_features(
        ${test_target}
        PRIVATE
            c_std_17
    )

    add_test(
        NAME "redditanalyzer_${test_name}"
        COMMAND ${test_target}
    )
endfunction()


add_redditanalyzer_test(
    smoke
    unit/test_smoke.c
)

add_redditanalyzer_test(
    subreddit
    unit/test_subreddit.c
)

add_redditanalyzer_test(
    post
    unit/test_post.c
)

add_redditanalyzer_test(
    post_list
    unit/test_post_list.c
)

add_redditanalyzer_http_test(
    http
    unit/test_http.c
)

set_tests_properties(
    redditanalyzer_http
    PROPERTIES
        ENVIRONMENT
            "LSAN_OPTIONS=suppressions=${PROJECT_SOURCE_DIR}/sanitizers/lsan.supp"
)