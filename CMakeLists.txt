# CMake version and project name
cmake_minimum_required(VERSION 3.5...3.19)
project(mandelbrot LANGUAGES C)

# Set C standard and compiler flags
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_C_EXTENSIONS OFF)
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native -Wall -Wextra -Wpedantic")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNDEBUG -O3 \
        -flto=auto \
        -fno-signed-zeros -fno-math-errno \
    ")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -Werror")
endif ()
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    if (CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 8)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
            -fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}/src/= \
        ")
    endif ()
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fcx-limited-range")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wformat=1 -Wno-format-truncation")
endif ()
if (CMAKE_C_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_POSIX_C_SOURCE=199309L")
endif ()

# Set path for additional CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# Include OpenMP
find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
endif ()

# Include SDL
find_package(SDL2 REQUIRED)
if (SDL2_FOUND)
    include_directories(${SDL2_INCLUDE_DIRS})
    link_libraries(${SDL2_LIBRARIES})
endif ()

# Include GMP
find_package(GMP REQUIRED)
if (GMP_FOUND)
    include_directories(${GMP_INCLUDE_DIRS})
    link_libraries(${GMP_LIBRARIES})
endif ()

# Include cJSON
find_package(cJSON REQUIRED)
if (cJSON_FOUND)
    include_directories(${CJSON_INCLUDE_DIRS})
    link_libraries(${CJSON_LIBRARIES})
endif ()

# Add the source directory
add_subdirectory(src)

# Add the tests directory
add_subdirectory(tests)
