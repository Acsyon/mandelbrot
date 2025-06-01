# Set additional compiler flags (UNIX)
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 8)
    string(APPEND CMAKE_C_FLAGS " -fmacro-prefix-map=${CMAKE_SOURCE_DIR}/= ")
endif ()

# Set output path for binaries
if (WIN32)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
endif ()

# Set test source files
set(TEST_SOURCES
    app/test_key.c
    app/test_settings.c
    data/test_chunk.c
    data/test_pixel.c
    net/test_connection.c
    net/test_package.c
    util/test_json.c
    util/test_num.c
    util/test_sys.c
    util/test_util.c
    visuals/test_color.c
    visuals/test_palette.c
)

# Set test binary output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

if (MANDELBROT_DISTRIBUTED)
    find_package(SDL2 PATHS "${CMAKE_INSTALL_PREFIX}" NO_DEFAULT_PATH)
    find_package(cJSON PATHS "${CMAKE_INSTALL_PREFIX}" NO_DEFAULT_PATH)
else ()
    find_package(SDL2 REQUIRED)
    find_package(cJSON REQUIRED)
endif ()

# Loop over all tests
foreach (TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME "${TEST_SOURCE}" NAME_WE)

    # Add tests and executables
    add_executable("${TEST_NAME}" "${TEST_SOURCE}")
    add_test(NAME "${TEST_NAME}" COMMAND "${TEST_NAME}")
    list(APPEND MANDELBROT_TEST_TARGETS "${TEST_NAME}")

    # Link and include Unity
    target_link_libraries("${TEST_NAME}" PRIVATE unity)

    # Link and include auxlib
    target_link_libraries("${TEST_NAME}" PRIVATE "${AUXILIARY_STATIC_LIBRARY}")

    # Link GMP / MPIR, SDL and cJSON
    target_include_directories("${TEST_NAME}" PRIVATE "${CMAKE_SOURCE_DIR}/submodules/cutil/lib/include")
    if (MANDELBROT_DISTRIBUTED)
        target_include_directories("${TEST_NAME}" PRIVATE "${CMAKE_INSTALL_PREFIX}/include")
        target_link_directories("${TEST_NAME}" PRIVATE "${CMAKE_INSTALL_PREFIX}/lib")
    endif ()
    if (NOT WIN32)
        target_link_libraries("${TEST_NAME}" PRIVATE gmp)
    else ()
        target_link_libraries("${TEST_NAME}" PRIVATE mpir)
    endif ()
    target_link_libraries("${TEST_NAME}" PRIVATE SDL2::SDL2 cjson cutil)
endforeach ()

# "check" target that runs the tests
add_custom_target(
    check
    COMMAND "${CMAKE_CTEST_COMMAND}" "--output-on-failure"
    DEPENDS "${MANDELBROT_TEST_TARGETS}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Running mandelbrot tests"
)

set_target_properties(test_connection PROPERTIES C_STANDARD 11)
set_target_properties(test_package PROPERTIES C_STANDARD 11)

# Fix cutil test binary locations
if (TARGET cutil-check)
    foreach (test_target ${CUTIL_TEST_TARGETS})
        set_target_properties(
            ${test_target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/cutil"
        )
    endforeach ()
endif ()

# "check-all" target that runs "mandelbrot" and "cutil" tests
add_custom_target(
    check-all
    DEPENDS check cutil-check
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Running all tests"
)
