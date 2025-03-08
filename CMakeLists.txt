# CMake version and project name
cmake_minimum_required(VERSION 3.5...3.31)
project(mandelbrot LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_C_EXTENSIONS OFF)

# Set compiler flags (UNIX)
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    string(APPEND CMAKE_C_FLAGS_RELEASE " -fcx-limited-range")
    string(APPEND CMAKE_C_FLAGS_DEBUG " -Wformat=1 -Wno-format-truncation")
endif()
if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    string(APPEND CMAKE_C_FLAGS " -D_POSIX_C_SOURCE=199309L")
endif()
if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    string(APPEND CMAKE_C_FLAGS " -march=native -Wall -Wextra -Wpedantic")
    string(APPEND CMAKE_C_FLAGS_RELEASE " -DNDEBUG \
        -flto=auto \
        -fno-signed-zeros -fno-math-errno")
    string(APPEND CMAKE_C_FLAGS_DEBUG " -O0 -g -Werror")
    string(APPEND CMAKE_C_FLAGS_RELWITHDEBINFO
           " ${CMAKE_C_FLAGS_RELEASE} -O2 -g")
    string(APPEND CMAKE_C_FLAGS_RELEASE " -O3")
endif()

# Set path for additional CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# Include OpenMP
find_package(OpenMP REQUIRED)
if(OPENMP_FOUND)
    string(APPEND CMAKE_C_FLAGS " ${OpenMP_C_FLAGS}")
endif()

# Include SDL
find_package(SDL2 REQUIRED)
if(SDL2_FOUND)
    include_directories(${SDL2_INCLUDE_DIRS})
    link_libraries(${SDL2_LIBRARIES})
endif()

# Include GMP
find_package(GMP REQUIRED)
if(GMP_FOUND)
    include_directories(${GMP_INCLUDE_DIRS})
    link_libraries(${GMP_LIBRARIES})
endif()

# Include cJSON
find_package(cJSON REQUIRED)
if(cJSON_FOUND)
    include_directories(${CJSON_INCLUDE_DIRS})
    link_libraries(${CJSON_LIBRARIES})
endif()

# Set name of auxiliary static library (for tests)
set(AUXILIARY_STATIC_LIBRARY "auxlib")

# Add the source directory
add_subdirectory(src)

# Add the tests directory
add_subdirectory(tests)
