cmake_minimum_required(VERSION 3.25)

project(
    RedditAnalyzer
    VERSION 0.1.0
    DESCRIPTION "Reddit analyzer for some statistics in C"
    LANGUAGES C
)


# -------------------------------------------------------------------------
# Project configuration
# -------------------------------------------------------------------------

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")


# -------------------------------------------------------------------------
# Options
# -------------------------------------------------------------------------

option(REDDITANALYZER_ENABLE_TESTS
    "Build RedditAnalyzer tests"
    ON
)

option(REDDITANALYZER_ENABLE_SANITIZERS
    "Enable AddressSanitizer and UndefinedBehaviorSanitizer"
    OFF
)

option(REDDITANALYZER_ENABLE_CLANG_TIDY
    "Enable clang-tidy"
    OFF
)


# -------------------------------------------------------------------------
# Compiler warnings
# -------------------------------------------------------------------------

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wshadow
        -Wstrict-prototypes
        -Wmissing-prototypes
        -Wformat=2
        -Wnull-dereference
        -Wdouble-promotion
        -Wundef
        -Werror=return-type
    )
endif()


# -------------------------------------------------------------------------
# Sanitizers
# -------------------------------------------------------------------------

if(REDDITANALYZER_ENABLE_SANITIZERS)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(
            -fsanitize=address,undefined
            -fno-omit-frame-pointer
        )

        add_link_options(
            -fsanitize=address,undefined
        )
    else()
        message(FATAL_ERROR
            "Sanitizers are currently supported only with GCC and Clang."
        )
    endif()
endif()


# -------------------------------------------------------------------------
# clang-tidy
# -------------------------------------------------------------------------

if(REDDITANALYZER_ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXECUTABLE NAMES clang-tidy)

    if(NOT CLANG_TIDY_EXECUTABLE)
        message(FATAL_ERROR
            "REDDITANALYZER_ENABLE_CLANG_TIDY is ON, but clang-tidy was not found."
        )
    endif()

    set(CMAKE_C_CLANG_TIDY
        "${CLANG_TIDY_EXECUTABLE}"
        "-p=${CMAKE_BINARY_DIR}"
    )
endif()


# -------------------------------------------------------------------------
# Include directories
# -------------------------------------------------------------------------

add_library(redditanalyzer_options INTERFACE)

target_include_directories(
    redditanalyzer_options
    INTERFACE
        "${PROJECT_SOURCE_DIR}/include"
)


# -------------------------------------------------------------------------
# Curl
# -------------------------------------------------------------------------

find_package(CURL REQUIRED)


# -------------------------------------------------------------------------
# Core library
# -------------------------------------------------------------------------

add_library(redditanalyzer_core STATIC
    src/model/post.c
    src/model/post_list.c
    src/model/subreddit.c
    src/reddit/client.c
    src/utils/error.c
    src/utils/logger.c
    src/utils/string.c
)

target_link_libraries(
    redditanalyzer_core
    PUBLIC
        redditanalyzer_options
)

target_compile_features(
    redditanalyzer_core
    PUBLIC
        c_std_17
)


# -------------------------------------------------------------------------
# Http library
# -------------------------------------------------------------------------

add_library(redditanalyzer_http STATIC
    src/http/client.c
)

target_link_libraries(
    redditanalyzer_http
    PUBLIC
        redditanalyzer_options
        redditanalyzer_core
        CURL::libcurl
)

target_compile_features(
    redditanalyzer_http
    PUBLIC
        c_std_17
)


# -------------------------------------------------------------------------
# Main executable
# -------------------------------------------------------------------------

add_executable(redditanalyzer
    src/main.c
)

target_link_libraries(
    redditanalyzer
    PRIVATE
        redditanalyzer_core
        redditanalyzer_http
)

target_compile_features(
    redditanalyzer
    PRIVATE
        c_std_17
)


# -------------------------------------------------------------------------
# Tests
# -------------------------------------------------------------------------

if(REDDITANALYZER_ENABLE_TESTS)
    enable_testing()

    add_subdirectory(tests)
endif()


# -------------------------------------------------------------------------
# Installation
# -------------------------------------------------------------------------

include(GNUInstallDirs)

install(
    TARGETS redditanalyzer
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

install(
    DIRECTORY include/
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)