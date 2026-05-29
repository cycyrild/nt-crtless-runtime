include_guard(GLOBAL)

if(NOT CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    message(FATAL_ERROR
        "This project must be configured from Linux with LLVM. "
        "Current host is '${CMAKE_HOST_SYSTEM_NAME}'."
    )
endif()

set(CMAKE_USER_MAKE_RULES_OVERRIDE "${CMAKE_CURRENT_LIST_DIR}/override-link-rules.cmake")

set(CMAKE_SYSTEM_NAME Windows)

set(XWIN_ROOT "$ENV{HOME}/.local/share/xwin" CACHE PATH "Path to the xwin installation root")

set(CMAKE_C_COMPILER clang-20)
set(CMAKE_CXX_COMPILER clang++-20)
set(CMAKE_ASM_COMPILER clang-20)

set(CMAKE_MSVC_RUNTIME_LIBRARY "")

set(CMAKE_C_COMPILER_TARGET x86_64-pc-windows-msvc)
set(CMAKE_CXX_COMPILER_TARGET x86_64-pc-windows-msvc)
set(CMAKE_ASM_COMPILER_TARGET x86_64-pc-windows-msvc)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES
    "${XWIN_ROOT}/crt/include"
    "${XWIN_ROOT}/sdk/include/ucrt"
    "${XWIN_ROOT}/sdk/include/shared"
    "${XWIN_ROOT}/sdk/include/um"
)
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_C_STANDARD_INCLUDE_DIRECTORIES})

link_directories(
    "${XWIN_ROOT}/crt/lib/x86_64"
    "${XWIN_ROOT}/sdk/lib/um/x86_64"
)

set(NT_CALL_PHNT_VERSION "PHNT_WINDOWS_10" CACHE STRING "PHNT target Windows version")

set(NT_CALL_WIN32_WINNT "0x0A00" CACHE STRING "_WIN32_WINNT target version")

add_compile_definitions(
    PHNT_MODE=PHNT_MODE_USER
    PHNT_VERSION=${NT_CALL_PHNT_VERSION}
    _WIN32_WINNT=${NT_CALL_WIN32_WINNT}
    _AMD64_=1
    NOMINMAX
)

add_compile_options(
    -march=skylake
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
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
)

add_link_options(
    -nostdlib
    -nodefaultlibs
    -nostartfiles
    -fno-rtlib-defaultlib
)
