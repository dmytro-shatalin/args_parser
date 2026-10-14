cmake_minimum_required(VERSION 4.2)

project(args_parser VERSION 1.0.0 LANGUAGES NONE)

# Header-only library: INTERFACE means it has nothing to compile itself, it only
# passes usage requirements (include path, C standard) on to whoever links it.
add_library(args_parser INTERFACE)
# Namespaced alias, so target_link_libraries(... args_parser::args_parser) looks the same
# whether the library is used via add_subdirectory/FetchContent or via find_package
# (the installed targets file exports the same name).
add_library(args_parser::args_parser ALIAS args_parser)

# The header needs C23 (nullptr, constexpr, bool, [[maybe_unused]]); INTERFACE applies
# this to consumers, not to the library itself.
# target_compile_features(args_parser INTERFACE c_std_23)

# Variables
option(ARGS_PARSER_BUILD_EXAMPLE "Whether to compile the example" OFF)
# Defaults to ON only when this is the top-level project, so embedding the library in
# another project doesn't add our headers to that project's `cmake --install`.
option(ARGS_PARSER_INSTALL "Generate install rules" ${PROJECT_IS_TOP_LEVEL})

# Declares the headers as a file set. INTERFACE scope: consumers receive it.
#   FILES     - the headers in the set
#   BASE_DIRS - root the paths are relative to; automatically added to consumers' include
#               path, so `#include "args_parser.h"` works without target_include_directories.
#               Also determines the header layout under <prefix>/include when installed.
target_sources(args_parser INTERFACE
        FILE_SET HEADERS
        BASE_DIRS include
        FILES include/args_parser/args_parser.h
)

if (ARGS_PARSER_BUILD_EXAMPLE)
    enable_language(C)
    add_executable(args_parser_example example/args_parser_example.c)
    # PRIVATE: an executable has no consumers of its own to pass requirements on to.
    target_link_libraries(args_parser_example PRIVATE args_parser::args_parser)
    # Per-target (not the global CMAKE_RUNTIME_OUTPUT_DIRECTORY) so a parent project's
    # other binaries are unaffected. PROJECT_BINARY_DIR is this project's build dir:
    # same as the top-level one when standalone, a subfolder when embedded.
    set_target_properties(args_parser_example PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin
    )
endif ()

if (ARGS_PARSER_INSTALL)
    include(GNUInstallDirs)                # CMAKE_INSTALL_LIBDIR etc.
    include(CMakePackageConfigHelpers)     # write_basic_package_version_file

    # Where the files consumed by find_package(args_parser) are installed;
    # CMake searches <prefix>/lib/cmake/<name>/ automatically.
    set(ARGS_PARSER_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/args_parser)

    # Installs the headers of the FILE_SET to <prefix>/include and records the target
    # in the export set "args_parserTargets".
    install(TARGETS args_parser
            EXPORT args_parserTargets
            FILE_SET HEADERS
    )
    # Generates args_parserTargets.cmake from the export set. It recreates the imported
    # target args_parser::args_parser (include path pointing at the install prefix,
    # c_std_23 requirement) for consumers.
    install(EXPORT args_parserTargets
            NAMESPACE args_parser::
            DESTINATION ${ARGS_PARSER_CMAKE_DIR}
    )

    # The Config file is what find_package(args_parser) loads; here it just pulls in the
    # targets file. Add find_dependency(...) calls here if the library gains dependencies.
    file(WRITE ${PROJECT_BINARY_DIR}/args_parserConfig.cmake
            "include(\"\${CMAKE_CURRENT_LIST_DIR}/args_parserTargets.cmake\")\n")
    # The version file is checked first by find_package(args_parser <version>).
    #   SameMajorVersion - accepts any version with the same major number (>= requested)
    #   ARCH_INDEPENDENT - header-only, so skip the pointer-size compatibility check
    write_basic_package_version_file(${PROJECT_BINARY_DIR}/args_parserConfigVersion.cmake
            COMPATIBILITY SameMajorVersion
            ARCH_INDEPENDENT
    )
    install(FILES
            ${PROJECT_BINARY_DIR}/args_parserConfig.cmake
            ${PROJECT_BINARY_DIR}/args_parserConfigVersion.cmake
            DESTINATION ${ARGS_PARSER_CMAKE_DIR}
    )
endif ()
