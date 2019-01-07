CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0 FATAL_ERROR)
set(PROJECT_NAME dedispersion-cpu)

file(GLOB_RECURSE SOURCE_FILES "src/main.c")

include_directories(include/)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
