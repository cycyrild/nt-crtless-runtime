cmake_minimum_required(VERSION 3.20)

project(nt-crtless-runtime LANGUAGES CXX)

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
    SYSTEM PUBLIC
        phnt
    PUBLIC
        include
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        printf
)

target_compile_options(printf PRIVATE
    -Wno-everything
)
