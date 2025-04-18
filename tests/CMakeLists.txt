# Set additional compiler flags (UNIX)
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 8)
    string(APPEND CMAKE_C_FLAGS " -fmacro-prefix-map=${CMAKE_SOURCE_DIR}/= ")
endif ()

# Include unity subdirectory
add_subdirectory(../submodules/Unity unity EXCLUDE_FROM_ALL)

# Set unity output directory to test directory
set_target_properties(
    unity PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
                     LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

# Set test source files
set(TEST_SOURCES
    app/test_key.c
    app/test_settings.c
    data/test_chunk.c
    data/test_pixel.c
    util/test_json.c
    util/test_num.c
    util/test_sys.c
    util/test_util.c
    visuals/test_color.c
    visuals/test_palette.c
)

# Set test binary output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

# Loop over all tests
foreach (TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME "${TEST_SOURCE}" NAME_WE)

    # Add tests and executables
    add_executable("${TEST_NAME}" "${TEST_SOURCE}")
    add_test(NAME "${TEST_NAME}" COMMAND "${TEST_NAME}")

    # Link and include Unity
    target_link_libraries("${TEST_NAME}" PRIVATE unity)

    # Link and include auxlib
    target_link_libraries("${TEST_NAME}" PRIVATE "${AUXILIARY_STATIC_LIBRARY}")
endforeach ()
