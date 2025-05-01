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
    set_target_properties("${AUXILIARY_STATIC_LIBRARY}" PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")
else ()
    set_target_properties("${AUXILIARY_STATIC_LIBRARY}" PROPERTIES OUTPUT_NAME "lib${PROJECT_NAME}")
endif ()

# Create main executable
add_executable("${PROJECT_NAME}" main.c)
target_link_libraries("${PROJECT_NAME}" PRIVATE "${AUXILIARY_STATIC_LIBRARY}")

# Treating warnings as errors would prevent compilation for MSVC
if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    string(REPLACE " /WX" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
endif ()

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

# Include GMP / MPIR
if (MANDELBROT_DISTRIBUTED)
    if (NOT WIN32)
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
        # MPIR configuration for Windows using MSBuild
        set(MPIR_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/mpir-install")
        file(MAKE_DIRECTORY "${MPIR_INSTALL_DIR}")

        # Paths to the build.vc directory
        set(MPIR_BUILD_VC_DIR "${CMAKE_SOURCE_DIR}/submodules/mpir/msvc/vs22")

        # Library paths - these might need adjustment based on your MPIR version
        set(MPIR_LIB_FILE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mpir.dll") # Dynamic lib
        set(MPIR_STATIC_LIB "${CMAKE_SOURCE_DIR}/submodules/mpir/lib/x64/Release/mpir.lib"
        )# Static lib

        # Choose which version you want (static or dynamic)
        set(MPIR_LIB_TO_USE "${MPIR_STATIC_LIB}") # Change to MPIR_LIB_FILE if you want dynamic

        set(MPIR_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/submodules/mpir")
        set(MPIR_HEADER_FILE "${MPIR_INCLUDE_DIR}/mpir.h")

        # Determine build platform (x64 or Win32)
        if (CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(MPIR_PLATFORM "x64")
        else ()
            set(MPIR_PLATFORM "Win32")
        endif ()

        add_custom_command(
            OUTPUT "${MPIR_LIB_TO_USE}" "${MPIR_HEADER_FILE}"
            COMMAND msbuild.bat gc LIB ${MPIR_PLATFORM} Release
                    /p:OutDir=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            WORKING_DIRECTORY "${MPIR_BUILD_VC_DIR}"
            COMMENT "Building MPIR for ${MPIR_PLATFORM} platform"
        )

        add_custom_target(build_mpir ALL DEPENDS "${MPIR_LIB_TO_USE}" "${MPIR_HEADER_FILE}")

        if (EXISTS "${MPIR_LIB_TO_USE}" AND EXISTS "${MPIR_HEADER_FILE}")
            message(STATUS "Found MPIR library at: ${MPIR_LIB_TO_USE}")
        else ()
            message(STATUS "MPIR will be built during compilation")
        endif ()

        add_dependencies("${AUXILIARY_STATIC_LIBRARY}" build_mpir)
        target_include_directories("${AUXILIARY_STATIC_LIBRARY}" PRIVATE "${MPIR_INCLUDE_DIR}")
        target_include_directories("${PROJECT_NAME}" PRIVATE "${MPIR_INCLUDE_DIR}")
        target_link_libraries("${AUXILIARY_STATIC_LIBRARY}" PRIVATE "${MPIR_LIB_TO_USE}")

        # For compatibility with code expecting GMP headers
        target_compile_definitions("${AUXILIARY_STATIC_LIBRARY}" PRIVATE USE_MPIR=1)
    endif ()
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
    target_include_directories(
        "${AUXILIARY_STATIC_LIBRARY}" PRIVATE "${CMAKE_SOURCE_DIR}/submodules/cJSON"
    )
    target_include_directories("${PROJECT_NAME}" PRIVATE "${CMAKE_SOURCE_DIR}/submodules/cJSON")
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
