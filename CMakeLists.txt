# CMake version and project name
cmake_minimum_required(VERSION 3.5...3.31)
project(mandelbrot LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_C_EXTENSIONS OFF)

# Set compiler flags (UNIX)
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    string(APPEND CMAKE_C_FLAGS_RELEASE " -fcx-limited-range")
    string(APPEND CMAKE_C_FLAGS_DEBUG " -Wformat=1")
    string(APPEND CMAKE_C_FLAGS_DEBUG " -Wno-format-truncation")
endif ()

if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    string(APPEND CMAKE_C_FLAGS " -D_POSIX_C_SOURCE=199309L")
endif ()

if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    string(APPEND CMAKE_C_FLAGS " -march=native")
    string(APPEND CMAKE_C_FLAGS " -Wall")
    string(APPEND CMAKE_C_FLAGS " -Wextra")
    string(APPEND CMAKE_C_FLAGS " -Wpedantic")
    string(APPEND CMAKE_C_FLAGS_RELEASE " -DNDEBUG")
    string(APPEND CMAKE_C_FLAGS_RELEASE " -flto=auto")
    string(APPEND CMAKE_C_FLAGS_RELEASE " -fno-signed-zeros")
    string(APPEND CMAKE_C_FLAGS_RELEASE " -fno-math-errno")
    string(APPEND CMAKE_C_FLAGS_DEBUG " -O0")
    string(APPEND CMAKE_C_FLAGS_DEBUG " -g")
    string(APPEND CMAKE_C_FLAGS_DEBUG " -Werror")
    string(APPEND CMAKE_C_FLAGS_RELWITHDEBINFO " ${CMAKE_C_FLAGS_RELEASE}")
    string(APPEND CMAKE_C_FLAGS_RELWITHDEBINFO " -O2")
    string(APPEND CMAKE_C_FLAGS_RELWITHDEBINFO " -g")
    string(APPEND CMAKE_C_FLAGS_RELEASE " -O3")
endif ()

# Set compiler flags (MSVC)
if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    string(APPEND CMAKE_C_FLAGS " /W4")
    string(APPEND CMAKE_C_FLAGS_RELEASE " /O2")
    string(APPEND CMAKE_C_FLAGS_RELEASE " /DNDEBUG")
    string(APPEND CMAKE_C_FLAGS_RELEASE " /MT")
    string(APPEND CMAKE_C_FLAGS_DEBUG " /WX")
    string(APPEND CMAKE_C_FLAGS_DEBUG " /Od")
    string(APPEND CMAKE_C_FLAGS_DEBUG " /Zi")
    string(APPEND CMAKE_C_FLAGS_DEBUG " /MTd")
    string(APPEND CMAKE_C_FLAGS_RELWITHDEBINFO " ${CMAKE_C_FLAGS_RELEASE}")
    string(APPEND CMAKE_C_FLAGS_RELWITHDEBINFO " /Zi")
endif ()

# Set paths for local installations and additional CMake modules
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/.local" CACHE PATH "Local installation directory"
                                                            FORCE
)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/.local/lib")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Set output path for binaries
set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# Options
option(MANDELBROT_DISTRIBUTED
       "Build in 'distributed' mode, i.e., use static libraries and position-independent code" OFF
)

# On Windows, we can only run in distributed mode
if (WIN32)
    set(MANDELBROT_DISTRIBUTED TRUE)
endif ()

# Set name of auxiliary static library (for tests)
set(AUXILIARY_STATIC_LIBRARY "auxlib")

# Include unity already here to prevent cutil submodule from being chosen
if (ENABLE_TESTS)
    # Include unity subdirectory
    add_subdirectory(submodules/Unity unity EXCLUDE_FROM_ALL)

    # Set unity output directory
    set_target_properties(
        unity PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
                         LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    )
endif ()

# Add the source directory
add_subdirectory(src)

# Add the tests directory
include(CTest)
