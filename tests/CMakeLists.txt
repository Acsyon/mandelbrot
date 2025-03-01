# Use FetchContent to download Unity if not present
include(FetchContent)
FetchContent_Declare(
    Unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG master  # You can specify a version if needed
)
FetchContent_MakeAvailable(Unity)

# Link Unity and your source code
link_directories(${unity_BINARY_DIR})
include_directories("${unity_SOURCE_DIR}/src")
link_libraries(unity)

include_directories("${CMAKE_SOURCE_DIR}/src")

# Enable testing with CTest
enable_testing()

add_executable(test_color test_color.c)
add_test(NAME test_color COMMAND test_color)

