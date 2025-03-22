# Set additional compiler flags (UNIX)
if(CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_C_COMPILER_VERSION
                                          VERSION_GREATER_EQUAL 8)
    string(APPEND CMAKE_C_FLAGS
           " -fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}/= ")
endif()

# Set source files
include_directories(${MANDELBROT_INCLUDE_DIR})
set(SOURCE_FILES
    app/app.c
    app/data.c
    app/key.c
    app/settings.c
    app/video.c
    data/chunk.c
    data/image.c
    data/pixel.c
    util/getopt.c
    util/json.c
    util/log.c
    util/num.c
    util/sys.c
    util/util.c
    visuals/color.c
    visuals/palette.c
    visuals/view.c)

# Set output path for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Compile
link_libraries(m)
add_library(${AUXILIARY_STATIC_LIBRARY} STATIC ${SOURCE_FILES})
set_property(TARGET ${AUXILIARY_STATIC_LIBRARY} PROPERTY OUTPUT_NAME
                                                         ${PROJECT_NAME})

link_libraries(${AUXILIARY_STATIC_LIBRARY})
add_executable(${PROJECT_NAME} main.c)
