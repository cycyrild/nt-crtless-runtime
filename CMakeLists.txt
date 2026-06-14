cmake_minimum_required(VERSION 3.20)

project(nt-crtless-runtime LANGUAGES CXX)

add_subdirectory(phnt)

get_target_property(PHNT_INCLUDE_DIRS phnt INTERFACE_INCLUDE_DIRECTORIES)

set_property(TARGET phnt APPEND PROPERTY
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES
    "${PHNT_INCLUDE_DIRS}"
)

block()
    set(BUILD_SHARED_LIBS OFF)
    set(ALIAS_STANDARD_FUNCTION_NAMES HARD)

    add_subdirectory(printf)
endblock()

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    src/*.cc
)

add_library(${PROJECT_NAME} OBJECT
    ${SRC_FILES}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    include
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    phnt::phnt
    printf
)

target_compile_options(printf PRIVATE
    -Wno-everything
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)
