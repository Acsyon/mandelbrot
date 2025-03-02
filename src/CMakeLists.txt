# Set additional compiler flags (UNIX)
if(CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_C_COMPILER_VERSION
                                          VERSION_GREATER_EQUAL 8)
    string(APPEND CMAKE_C_FLAGS
           " -fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}/= ")
endif()

# Set source files
set(SOURCE_FILES
    main.c
    app.c
    color.c
    data.c
    getopt.c
    json.c
    log.c
    settings.c
    sys.c
    util.c
    video.c
    view.c)

# Set output path for binary
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Compile
link_libraries(m)
add_executable("${PROJECT_NAME}" ${SOURCE_FILES})
