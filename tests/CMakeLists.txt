# Set additional compiler flags (UNIX)
if(CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_C_COMPILER_VERSION
                                          VERSION_GREATER_EQUAL 8)
    string(APPEND CMAKE_C_FLAGS " -fmacro-prefix-map=${CMAKE_SOURCE_DIR}/= ")
endif()

# Use FetchContent to download Unity if not present
include(FetchContent)
FetchContent_Declare(
    Unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG master # You can specify a version if needed
)
FetchContent_MakeAvailable(Unity)

# Link Unity and your source code
link_directories(${unity_BINARY_DIR})
include_directories("${unity_SOURCE_DIR}/src")
link_libraries(unity)

# Link and include auxlib
include_directories(${MANDELBROT_INCLUDE_DIR})
link_libraries(${AUXILIARY_STATIC_LIBRARY})

# Enable testing with CTest
enable_testing()

add_executable(test_key app/test_key.c)
add_test(NAME test_key COMMAND test_key)

add_executable(test_settings app/test_settings.c)
add_test(NAME test_settings COMMAND test_settings)

add_executable(test_chunk data/test_chunk.c)
add_test(NAME test_chunk COMMAND test_chunk)

add_executable(test_pixel data/test_pixel.c)
add_test(NAME test_pixel COMMAND test_pixel)

add_executable(test_json util/test_json.c)
add_test(NAME test_json COMMAND test_json)

add_executable(test_num util/test_num.c)
add_test(NAME test_num COMMAND test_num)

add_executable(test_sys util/test_sys.c)
add_test(NAME test_sys COMMAND test_sys)

add_executable(test_util util/test_util.c)
add_test(NAME test_util COMMAND test_util)

add_executable(test_color visuals/test_color.c)
add_test(NAME test_color COMMAND test_color)

add_executable(test_palette visuals/test_palette.c)
add_test(NAME test_palette COMMAND test_palette)