include(FetchContent)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.9.4
)

FetchContent_MakeAvailable(googlebenchmark)

add_executable(lob_benchmarks "lob-benchmark.cpp")

target_compile_options(lob_benchmarks PRIVATE -fno-omit-frame-pointer)

target_link_libraries(lob_benchmarks PRIVATE LOBLib benchmark::benchmark)
