cmake_minimum_required(VERSION 3.20)

project(nt-crtless-runtime LANGUAGES CXX ASM)

add_subdirectory(phnt)

get_target_property(PHNT_INCLUDE_DIRS phnt INTERFACE_INCLUDE_DIRECTORIES)

set_property(
  TARGET phnt
  APPEND
  PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${PHNT_INCLUDE_DIRS}"
)

block()
set(BUILD_SHARED_LIBS OFF)
set(FMT_OS OFF)
set(FMT_TEST OFF)
set(FMT_DOC OFF)
set(FMT_INSTALL OFF)
add_subdirectory(fmt EXCLUDE_FROM_ALL)
endblock()

block()
set(CTRE_BUILD_TESTS OFF)
add_subdirectory(ctre EXCLUDE_FROM_ALL)
endblock()

file(
  GLOB_RECURSE
  SRC_FILES
  CONFIGURE_DEPENDS
  src/*.cc
)

list(
  FILTER
  SRC_FILES
  EXCLUDE
  REGEX
  "/start_(process|dll)\\.cc$"
)

add_library(${PROJECT_NAME} OBJECT ${SRC_FILES})

target_sources(
  ${PROJECT_NAME}
  INTERFACE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${CMAKE_CURRENT_SOURCE_DIR}/src/start_process.cc>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${CMAKE_CURRENT_SOURCE_DIR}/src/start_dll.cc>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${CMAKE_CURRENT_SOURCE_DIR}/src/start_dll.cc>"
)

target_include_directories(${PROJECT_NAME} PRIVATE include_private)
target_include_directories(${PROJECT_NAME} PUBLIC include_public)

target_link_libraries(${PROJECT_NAME} PUBLIC phnt::phnt fmt::fmt-header-only ctre::ctre)

target_compile_definitions(
  ${PROJECT_NAME} PUBLIC FMT_USE_LOCALE=0 FMT_USE_EXCEPTIONS=0 FMT_OPTIMIZE_SIZE=2
)

target_compile_options(${PROJECT_NAME} PUBLIC "-DFMT_THROW(x)=abort()")
