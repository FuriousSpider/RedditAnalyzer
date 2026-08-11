# RedditAnalyzer

Reddit analytics desktop application written in C17.


## Project status

Early development


## Features

Currently implemented:

- C17 project structure
- CMake build system
- GCC and Clang support
- Logging subsystem
- Unit test infrastructure
- AddressSanitizer / UndefinedBehaviorSanitizer support
- clang-tidy integration


Planned:

- Reddit API integration
- HTTP client
- JSON parsing
- Reddit data models
- Statistical analysis
- SQLite cache
- GTK4 graphical interface
- Data visualization
- Asynchronous data fetching
- Related community analysis


## Required

- CMake >= 3.25
- Ninja
- GCC or Clang
- clang-format
- clang-tidy
- CTest


## Build

### GCC Debug
cmake --preset debug
cmake --build --preset debug

### Clang Debug
cmake --preset debug-clang
cmake --build --preset debug-clang

### Release
cmake --preset release
cmake --build --preset release


## Run
After building the project run:
./build/debug/bin/redditanalyzer

For the Clang build:
./build/debug-clang/bin/redditanalyzer


## Tests
Run the test suite with:
ctest --test-dir build/debug --output-on-failure

For the Clang build:
ctest --test-dir build/debug-clang --output-on-failure


## Sanitizers
Build the project with AddressSanitizer and UndefinedBehaviorSanitizer:
cmake --preset debug-sanitize
cmake --build --preset debug-sanitize

Run the tests:
ctest --test-dir build/debug-sanitize --output-on-failure