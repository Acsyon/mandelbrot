# Set additional compiler flags (UNIX)
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 8)
    string(APPEND CMAKE_C_FLAGS " -fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}/= ")
endif ()

if (MANDELBROT_DISTRIBUTED)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    include(ExternalProject)
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
    net/connection.c
    net/package.c
    net/package/settings.c
    net/package/string.c
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
    ExternalProject_Add(
        sdl_submodule
        SOURCE_DIR "${CMAKE_SOURCE_DIR}/submodules/SDL"
        BINARY_DIR "${CMAKE_BINARY_DIR}/sdl-build"
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
                   -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DSDL_STATIC=ON -DSDL_SHARED=OFF
                   -DSDL_TEST=OFF
        INSTALL_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --target install
    )
    add_dependencies("${AUXILIARY_STATIC_LIBRARY}" sdl_submodule)

    find_package(SDL2 PATHS "${CMAKE_INSTALL_PREFIX}" NO_DEFAULT_PATH)
else ()
    find_package(SDL2 REQUIRED)
endif ()
if (SDL2_FOUND)
    target_link_libraries("${AUXILIARY_STATIC_LIBRARY}" PRIVATE SDL2::SDL2)
endif ()

# Include GMP / MPIR
if (MANDELBROT_DISTRIBUTED)
    if (NOT WIN32)
        set(GMP_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}")
        file(MAKE_DIRECTORY "${GMP_INSTALL_DIR}")

        set(GMP_INCLUDE_DIR "${GMP_INSTALL_DIR}/include")
        set(GMP_LIB_DIR "${GMP_INSTALL_DIR}/lib")
        set(GMP_LIB_FILE "${GMP_LIB_DIR}/libgmp.a")
        set(GMP_HEADER_FILE "${GMP_INCLUDE_DIR}/gmp.h")

        add_custom_command(
            OUTPUT "${GMP_LIB_FILE}" "${GMP_HEADER_FILE}"
            COMMAND ./.bootstrap
            COMMAND ./configure --disable-shared "--prefix=${GMP_INSTALL_DIR}"
                    "--libdir=${GMP_LIB_DIR}" --with-pic
            COMMAND make "-j${nproc}"
            COMMAND make install
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/submodules/gmp"
            COMMENT "Building GMP and installing to ${GMP_INSTALL_DIR}"
        )

        add_custom_target(gmp_submodule ALL DEPENDS "${GMP_LIB_FILE}" "${GMP_HEADER_FILE}")

        if (EXISTS "${GMP_LIB_FILE}" AND EXISTS "${GMP_HEADER_FILE}")
            message(STATUS "Found GMP library at: ${GMP_LIB_FILE}")
        else ()
            message(STATUS "GMP will be built during compilation")
        endif ()

        add_dependencies("${AUXILIARY_STATIC_LIBRARY}" gmp_submodule)
        target_include_directories("${AUXILIARY_STATIC_LIBRARY}" PRIVATE "${GMP_INCLUDE_DIR}")
        target_link_libraries("${AUXILIARY_STATIC_LIBRARY}" PRIVATE "${GMP_LIB_FILE}")
    else ()
        # MPIR configuration for Windows using MSBuild
        set(MPIR_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}")
        file(MAKE_DIRECTORY "${MPIR_INSTALL_DIR}")

        set(MPIR_INCLUDE_DIR "${MPIR_INSTALL_DIR}/include")
        set(MPIR_LIB_DIR "${MPIR_INSTALL_DIR}/lib")
        set(MPIR_HEADER_FILE "${MPIR_INCLUDE_DIR}/gmp.h")

        # Library paths - these might need adjustment based on your MPIR version
        set(MPIR_LIB_FILE "${MPIR_LIB_DIR}/mpir.dll") # Dynamic lib
        set(MPIR_STATIC_LIB "${MPIR_LIB_DIR}/mpir.lib") # Static lib

        # Choose which version you want (static or dynamic)
        set(MPIR_LIB_TO_USE "${MPIR_STATIC_LIB}") # Change to MPIR_LIB_FILE if you want dynamic

        # Determine build platform (x64 or Win32) and set build type
        if (CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(MPIR_PLATFORM "x64")
        else ()
            set(MPIR_PLATFORM "Win32")
        endif ()
        set(MPIR_BUILD_TYPE "Release")

        # Path to the build.vc directory
        set(MPIR_BUILD_VC_DIR "${CMAKE_SOURCE_DIR}/submodules/mpir/msvc/vs22")

        # Path of the Windows build output
        set(MIPR_OUT_DIR
            "${CMAKE_SOURCE_DIR}/submodules/mpir/lib/${MPIR_PLATFORM}/${MPIR_BUILD_TYPE}"
        )

        add_custom_command(
            OUTPUT "${MPIR_LIB_TO_USE}" "${MPIR_HEADER_FILE}"
            COMMAND msbuild.bat gc LIB ${MPIR_PLATFORM} ${MPIR_BUILD_TYPE}
            WORKING_DIRECTORY "${MPIR_BUILD_VC_DIR}"
            COMMENT "Building MPIR for ${MPIR_PLATFORM} platform"
        )

        # Copy files to proper directory
        file(COPY "${MIPR_OUT_DIR}/mpir.lib" DESTINATION "${MPIR_LIB_DIR}")
        file(COPY "${MIPR_OUT_DIR}/gmp.h" DESTINATION "${MPIR_INCLUDE_DIR}")

        add_custom_target(mpir_submodule ALL DEPENDS "${MPIR_LIB_TO_USE}" "${MPIR_HEADER_FILE}")

        if (EXISTS "${MPIR_LIB_TO_USE}" AND EXISTS "${MPIR_HEADER_FILE}")
            message(STATUS "Found MPIR library at: ${MPIR_LIB_TO_USE}")
        else ()
            message(STATUS "MPIR will be built during compilation")
        endif ()

        add_dependencies("${AUXILIARY_STATIC_LIBRARY}" mpir_submodule)
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
    ExternalProject_Add(
        cjson_submodule
        SOURCE_DIR "${CMAKE_SOURCE_DIR}/submodules/cJSON"
        BINARY_DIR "${CMAKE_BINARY_DIR}/cjson-build"
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
                   -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_POLICY_VERSION_MINIMUM=3.5
                   -DBUILD_SHARED_LIBS=OFF -DENABLE_CJSON_TEST=OFF -DENABLE_TARGET_EXPORT=ON
        INSTALL_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --target install
    )
    add_dependencies("${AUXILIARY_STATIC_LIBRARY}" cjson_submodule)

    find_package(cJSON PATHS "${CMAKE_INSTALL_PREFIX}" NO_DEFAULT_PATH)
else ()
    find_package(cJSON REQUIRED)
endif ()
if (cJSON_FOUND)
    target_link_libraries("${AUXILIARY_STATIC_LIBRARY}" PRIVATE cjson)
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
