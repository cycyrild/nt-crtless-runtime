include_guard(GLOBAL)

set(CMAKE_TRY_COMPILE_PLATFORM_VARIABLES XWIN_ROOT)

if(NOT
   CMAKE_HOST_SYSTEM_NAME
   STREQUAL
   "Linux"
)
  message(FATAL_ERROR "This project must be configured from Linux with LLVM. "
                      "Current host is '${CMAKE_HOST_SYSTEM_NAME}'."
  )
endif()

set(CMAKE_SYSTEM_NAME Windows)

set(CMAKE_C_COMPILER_TARGET x86_64-pc-windows-msvc)
set(CMAKE_CXX_COMPILER_TARGET x86_64-pc-windows-msvc)

set(ASM_LTO_COMPILER "${CMAKE_CXX_COMPILER}") # ASM sources are wrapped in a C++ translation unit
                                              # for LTO compatibility
set(CMAKE_INCLUDE_FLAG_ASM "-I")
set(CMAKE_ASM_COMPILE_OBJECT
    "${CMAKE_CURRENT_LIST_DIR}/clang-asm-lto-wrapper.sh ${ASM_LTO_COMPILER} --target=${CMAKE_CXX_COMPILER_TARGET} <DEFINES> <INCLUDES> <FLAGS> -o <OBJECT> -c <SOURCE>"
)

set(CMAKE_MSVC_RUNTIME_LIBRARY "")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES
    "${XWIN_ROOT}/crt/include"
    "${XWIN_ROOT}/sdk/include/ucrt"
    "${XWIN_ROOT}/sdk/include/shared"
    "${XWIN_ROOT}/sdk/include/um"
)
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_C_STANDARD_INCLUDE_DIRECTORIES})

link_directories("${XWIN_ROOT}/crt/lib/x86_64" "${XWIN_ROOT}/sdk/lib/um/x86_64")

add_compile_definitions(
  PHNT_MODE=PHNT_MODE_USER
  PHNT_VERSION=${PHNT_VERSION}
  _WIN32_WINNT=${_WIN32_WINNT}
  _AMD64_=1
)

if(NOT
   CMAKE_PROJECT_NAME
   STREQUAL
   "CMAKE_TRY_COMPILE"
)
  add_compile_options(
    -march=${TARGET_CPU}
    -ffunction-sections
    -fdata-sections
    -fvisibility=hidden
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
    -fno-ident
    -Wall
    -Wextra
    -Wpedantic
    -fno-stack-protector
    -fno-stack-check
    -mno-stack-arg-probe
    -DPREFERRED_IMAGE_BASE=${PREFERRED_IMAGE_BASE}
    -DPE_ALIGNMENT=${PE_ALIGNMENT}
    # The GNU final link consumes LLD's combined LTO object, so every configuration must compile the
    # application and its dependencies with LTO.
    -flto=full
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
  )
endif()
