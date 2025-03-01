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
    view.c
)

# Set path for binary
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Compile
link_libraries(m)
add_executable("${PROJECT_NAME}" ${SOURCE_FILES})

