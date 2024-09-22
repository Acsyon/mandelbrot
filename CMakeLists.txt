# CMake version (all should work) and project name
cmake_minimum_required(VERSION 3.5...3.19)
project(mandelbrot LANGUAGES C)

if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
endif ()
if (
        CMAKE_C_COMPILER_ID STREQUAL "GNU" AND 
        CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 8
    )
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
        -fmacro-prefix-map=${CMAKE_SOURCE_DIR}/src/= \
    ")
endif ()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Set C standard and compiler flags
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_C_EXTENSIONS OFF)
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
    if (CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 8)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
            -fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}/src/= \
        ")
    endif ()
    set(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -O3 \
        -flto=auto \
        -fcx-limited-range -fno-signed-zeros -fno-math-errno \
    ")
    set(CMAKE_C_FLAGS_DEBUG "-g -Wformat=1 -Wno-format-truncation -Werror")
endif ()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

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
if (CJSON_UTILS_FOUND)
    include_directories(${CJSON_INCLUDE_DIRS})
    link_libraries(${CJSON_LIBRARIES})
endif ()

# Set source files
set(SOURCE_FILES
    src/main.c
    src/color.c
    src/data.c
    src/log.c
    src/settings.c
    src/sys.c
    src/video.c
)

# Compile
link_libraries(m)
add_executable("${PROJECT_NAME}" ${SOURCE_FILES})
