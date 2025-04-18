# Set additional compiler flags (UNIX)
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 8)
    string(APPEND CMAKE_C_FLAGS " -fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}/= ")
endif ()

if (MANDELBROT_DISTRIBUTED)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
else ()
    set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries" FORCE)
endif ()

# Set source files
set(SOURCE_FILES
    app/app.c
    app/data.c
    app/key.c
    app/settings.c
    app/video.c
    data/chunk.c
    data/image.c
    data/pixel.c
    util/json.c
    util/num.c
    util/sys.c
    util/util.c
    visuals/color.c
    visuals/palette.c
    visuals/view.c
)

# Compile auxiliary library
add_library("${AUXILIARY_STATIC_LIBRARY}" STATIC "${SOURCE_FILES}")
target_include_directories(
    "${AUXILIARY_STATIC_LIBRARY}" PUBLIC "${MANDELBROT_INCLUDE_DIR}"
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
)
if (NOT WIN32)
    target_link_libraries("${AUXILIARY_STATIC_LIBRARY}" PRIVATE m)
endif ()
set_target_properties("${AUXILIARY_STATIC_LIBRARY}" PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")

# Create main executable
add_executable("${PROJECT_NAME}" main.c)
target_link_libraries("${PROJECT_NAME}" PRIVATE "${AUXILIARY_STATIC_LIBRARY}")

# Include OpenMP
find_package(OpenMP REQUIRED)
if (OpenMP_C_FOUND)
    target_link_libraries("${AUXILIARY_STATIC_LIBRARY}" PRIVATE OpenMP::OpenMP_C)
endif ()

# Include SDL
if (MANDELBROT_DISTRIBUTED)
    set(SDL_LIBRARY_FILE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libSDL2.a")

    if (NOT EXISTS "${SDL_LIBRARY_FILE}")
        message(STATUS "Building SDL from source...")
        set(SDL_STATIC ON CACHE BOOL "" FORCE)
        set(SDL_SHARED OFF CACHE BOOL "" FORCE)
        set(SDL_TEST OFF CACHE BOOL "" FORCE)
        set(SDL2_DISABLE_UNINSTALL ON CACHE BOOL "" FORCE)

        add_subdirectory("${CMAKE_SOURCE_DIR}/submodules/SDL" SDL EXCLUDE_FROM_ALL)
        target_link_libraries("${AUXILIARY_STATIC_LIBRARY}" PRIVATE SDL2-static)
    else ()
        target_link_libraries("${AUXILIARY_STATIC_LIBRARY}" PRIVATE "${SDL_LIBRARY_FILE}")
    endif ()
else ()
    find_package(SDL2 REQUIRED)
    if (SDL2_FOUND)
        target_link_libraries("${AUXILIARY_STATIC_LIBRARY}" PRIVATE SDL2::SDL2)
    endif ()
endif ()

# Include GMP
if (MANDELBROT_DISTRIBUTED)
    set(GMP_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/gmp-install")
    file(MAKE_DIRECTORY "${GMP_INSTALL_DIR}")

    set(GMP_INCLUDE_DIR "${GMP_INSTALL_DIR}/include")
    set(GMP_LIB_FILE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libgmp.a")
    set(GMP_HEADER_FILE "${GMP_INCLUDE_DIR}/gmp.h")

    add_custom_command(
        OUTPUT "${GMP_LIB_FILE}" "${GMP_HEADER_FILE}"
        COMMAND ./.bootstrap
        COMMAND ./configure --disable-shared "--prefix=${GMP_INSTALL_DIR}"
                "--libdir=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" --with-pic
        COMMAND make "-j${nproc}"
        COMMAND make install
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/submodules/gmp"
        COMMENT "Building GMP and installing to ${GMP_INSTALL_DIR}"
    )

    add_custom_target(build_gmp ALL DEPENDS "${GMP_LIB_FILE}" "${GMP_HEADER_FILE}")

    if (EXISTS "${GMP_LIB_FILE}" AND EXISTS "${GMP_HEADER_FILE}")
        message(STATUS "Found GMP library at: ${GMP_LIB_FILE}")
    else ()
        message(STATUS "GMP will be built during compilation")
    endif ()

    add_dependencies("${AUXILIARY_STATIC_LIBRARY}" build_gmp)
    target_include_directories("${AUXILIARY_STATIC_LIBRARY}" PRIVATE "${GMP_INCLUDE_DIR}")
    target_link_libraries("${AUXILIARY_STATIC_LIBRARY}" PRIVATE "${GMP_LIB_FILE}")
else ()
    find_package(GMP REQUIRED)
    if (GMP_FOUND)
        target_link_libraries("${AUXILIARY_STATIC_LIBRARY}" PRIVATE "${GMP_LIBRARIES}")
    endif ()
endif ()

# Include cJSON
if (MANDELBROT_DISTRIBUTED)
    set(CMAKE_POLICY_VERSION_MINIMUM "3.5")

    set(ENABLE_TARGET_EXPORT OFF CACHE BOOL "" FORCE)
    set(ENABLE_CJSON_TEST OFF CACHE BOOL "" FORCE)
    set(ENABLE_CJSON_UNINSTALL OFF CACHE BOOL "" FORCE)
    set(cJSON_STATIC_BUILD ON CACHE BOOL "" FORCE)
    set(cJSON_SHARED_BUILD OFF CACHE BOOL "" FORCE)
    set(ENABLE_CJSON_VERSION_SO OFF CACHE BOOL "" FORCE)

    add_subdirectory("${CMAKE_SOURCE_DIR}/submodules/cJSON" cJSON EXCLUDE_FROM_ALL)
    target_link_libraries("${AUXILIARY_STATIC_LIBRARY}" PRIVATE cjson)
else ()
    find_package(cJSON REQUIRED)
    if (cJSON_FOUND)
        target_link_libraries("${AUXILIARY_STATIC_LIBRARY}" PRIVATE cjson)
    endif ()
endif ()

# Include cutil
add_subdirectory("${CMAKE_SOURCE_DIR}/submodules/cutil" cutil EXCLUDE_FROM_ALL)
if (MANDELBROT_DISTRIBUTED)
    target_link_libraries("${AUXILIARY_STATIC_LIBRARY}" PRIVATE cutil-static)
    target_link_libraries("${PROJECT_NAME}" PRIVATE cutil-static)
else ()
    target_link_libraries("${AUXILIARY_STATIC_LIBRARY}" PRIVATE cutil)
    target_link_libraries("${PROJECT_NAME}" PRIVATE cutil)
endif ()
