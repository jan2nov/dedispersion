CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0 FATAL_ERROR)

set(CMAKE_C_COMPILER "gcc")
set(PROJECT_NAME dedispersion-cpu)

file(GLOB_RECURSE SOURCE_FILES "src/*.c")

include_directories(include/)

list(APPEND CMAKE_C_FLAGS "-std=c99 -O2 -lm -Wall -Wpedantic -Wextra -fopenmp -fcilkplus -lcilkrts")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
