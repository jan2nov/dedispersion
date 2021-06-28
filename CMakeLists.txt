CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0 FATAL_ERROR)

set(CMAKE_C_COMPILER "icc")
set(PROJECT_NAME dedispersion-cpu)

# Status information
message("Using: ${CMAKE_C_COMPILER} with compiler ID ${CMAKE_C_COMPILER_ID} and compiler version ${CMAKE_C_COMPILER_VERSION}")

file(GLOB_RECURSE SOURCE_FILES "src/*.c")

include_directories(include/)

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS})

# Flags for Knights Landing in case of using GCC
#string(APPEND CMAKE_C_FLAGS "-std=c99 -O3 -Wall -Wpedantic -Wextra -fopenmp -fcilkplus -mavx512f -mavx512cd -mavx512er -mavx512pf -march=knl -ffast-math -mfma -malign-data=cacheline -finline-functions -lm")

# update for AMD EPYC
#list(APPEND CMAKE_C_FLAGS "-std=c99 -O2 -Wall -Wextra -qopenmp -march=core-avx2 -qopt-prefetch -fma -ftz -fomit-frame-pointer -finline-functions -qopt-streaming-stores=never")
#list(APPEND CMAKE_C_FLAGS "-std=c99 -O3 -Wall -Wextra -qopenmp -march=core-avx2 -fma -ftz -fomit-frame-pointer -finline-functions -qopt-streaming-stores=never -ffast-math -qopt-prefetch")
#list(APPEND CMAKE_C_FLAGS "-std=c99 -O2 -Wall -Wextra -march=broadwell -qopenmp -qopt-prefetch -fma -ftz -fomit-frame-pointer -finline-functions -qopt-streaming-stores=never")

#AMD EPYC for gcc
#list(APPEND CMAKE_C_FLAGS "-std=c99 -O3 -Wall -Wextra -march=znver1 -mtune=znver1 -mfma -mavx2 -m3dnow -fopenmp -ffast-math -ftree-vectorize -fprefetch-loop-arrays -mprefetchwt1")

#AMD clang
#list(APPEND CMAKE_C_FLAGS "-std=c99 -O2 -Wall -Wextra -march=znver1 -fvectorize -mfma -mavx2 -m3dnow -fuse-ld=lld -fopenmp -fcilkplus")

#Flags for Knights Landing in case of use intel ICC
#list(APPEND CMAKE_C_FLAGS "-qopenmp -g -fp-model fast=2 -std=c99 -O2 -fma -xMIC-AVX512 -align -finline-functions -no-prec-div -ipo -DOPEMP_SPEC -qopt-streaming-stores=never")

# Generic flags for GCC compiler
#list(APPEND CMAKE_C_FLAGS "-std=c99 -O3 -Wall -Wpedantic -Wextra -fopenmp -fcilkplus -ffast-math -lm")

#Generic flags for ICC compiler
list(APPEND CMAKE_C_FLAGS "-qopenmp -g -fp-model fast=2 -std=c99 -O2 -march=core-avx2 -fma -align -finline-functions -no-prec-div -ipo -DOPEMP_SPEC -qopt-streaming-stores=never -lgsl -lgslcblas")

if(DEFINED NSAMPLES)
	string(APPEND CMAKE_C_FLAGS " -DNSAMPLES=${NSAMPLES}")
endif()

if(DEFINED NCHAN)
	string(APPEND CMAKE_C_FLAGS " -DDIVINCHAN=${NCHAN}")
endif()
message(STATUS "Using C flags ${CMAKE_C_FLAGS}")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} m)
