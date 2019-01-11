CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0 FATAL_ERROR)

set(CMAKE_C_COMPILER "gcc")
set(PROJECT_NAME dedispersion-cpu)

file(GLOB_RECURSE SOURCE_FILES "src/*.c")

include_directories(include/)

list(APPEND CMAKE_C_FLAGS "-std=c99 -O3 -Wall -Wpedantic -Wextra -fopenmp -fcilkplus -mavx512f -mavx512cd -mavx512er -mavx512pf -march=knl -ffast-math -mfma -malign-data=cacheline -finline-functions -lm")
#list(APPEND CMAKE_C_FLAGS "-qopenmp -g -fp-model fast=2 -std=c99 -O2 -fma -xMIC-AVX512 -align -finline-functions -no-prec-div -ipo -DOPEMP_SPEC -qopt-streaming-stores=never")
#list(APPEND CMAKE_C_FLAGS "-qopenmp -std=c99 -O2")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} m)
