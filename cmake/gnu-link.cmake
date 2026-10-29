function(add_gnu_final_link target)
    find_package(Python3 REQUIRED COMPONENTS Interpreter)

    cmake_parse_arguments(
        PARSE_ARGV 1
        ARG
        ""
        "SUBSYSTEM;OUTPUT_SUFFIX;EXPORTS_FILE"
        "LIBRARIES"
    )

    if(NOT TARGET "${target}")
        message(FATAL_ERROR
            "add_gnu_final_link: unknown target '${target}'"
        )
    endif()

    if(NOT DEFINED ARG_SUBSYSTEM)
        message(FATAL_ERROR
            "add_gnu_final_link: SUBSYSTEM is required"
        )
    endif()

    if(NOT DEFINED ARG_OUTPUT_SUFFIX)
        message(FATAL_ERROR
            "add_gnu_final_link: OUTPUT_SUFFIX is required"
        )
    endif()

    if(NOT DEFINED ARG_EXPORTS_FILE)
        message(FATAL_ERROR
            "add_gnu_final_link: EXPORTS_FILE is required"
        )
    endif()

    get_filename_component(_exports_file "${ARG_EXPORTS_FILE}" ABSOLUTE
        BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
    )
    if(NOT EXISTS "${_exports_file}" OR IS_DIRECTORY "${_exports_file}")
        message(FATAL_ERROR
            "add_gnu_final_link: exports file not found: '${_exports_file}'"
        )
    endif()

    get_target_property(_target_type "${target}" TYPE)
    if(_target_type STREQUAL "SHARED_LIBRARY")
        set(_pe_options --dll)
    elseif(_target_type STREQUAL "EXECUTABLE")
        set(_pe_options)
    else()
        message(FATAL_ERROR
            "add_gnu_final_link: target '${target}' must be "
            "an EXECUTABLE or SHARED_LIBRARY, got '${_target_type}'"
        )
    endif()

    # Shared PE settings for both linkers. The preferred base must also match
    # the bootstrap's preferred_image_base in src/start.cc.
    set(_image_base 0x180000000)
    set(_pe_alignment 0x1000)

    set(_linker_script
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/gnu-final.ld"
    )

    set(_output
        "${CMAKE_CURRENT_BINARY_DIR}/${target}.ld${ARG_OUTPUT_SUFFIX}"
    )

    set(_map
        "${CMAKE_CURRENT_BINARY_DIR}/${target}.ld.map"
    )

    # Keep LLD's PE internal
    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/intermediate/$<CONFIG>"
    )
    target_link_options(${target} PRIVATE
        -nostdlib
        -nodefaultlibs
        -nostartfiles
        -fno-rtlib-defaultlib
        "LINKER:/base:${_image_base}"
        "LINKER:/filealign:${_pe_alignment}"
        "LINKER:/entry:start"
        "LINKER:/map"
        "LINKER:/opt:ref"
        "LINKER:/opt:icf"
        "LINKER:/timestamp:0"
        "LINKER:/lldsavetemps"
        "LINKER:/def:${_exports_file}"
    )

    set(_libraries)
    foreach(_library IN LISTS ARG_LIBRARIES)
        list(APPEND _libraries "-l${_library}")
    endforeach()

    file(GLOB_RECURSE _pe_post_process_scripts
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/pe_post_process/*.py"
    )

    # Changes to either final stage must rerun the coupled link pipeline
    set_property(TARGET ${target} APPEND PROPERTY LINK_DEPENDS
        "${_linker_script}"
        ${_pe_post_process_scripts}
        ${_exports_file}
    )

    add_custom_command(
        TARGET ${target} POST_BUILD
        BYPRODUCTS
        "${_output}"
        "${_map}"

        COMMAND
        x86_64-w64-mingw32-ld
        -T "${_linker_script}"
        ${_pe_options}
        --subsystem "${ARG_SUBSYSTEM}"
        -e start
        --gc-sections
        --no-insert-timestamp
        "--section-alignment=${_pe_alignment}"
        "--image-base=${_image_base}"
        "--file-alignment=${_pe_alignment}"
        --strip-all
        --nostdlib
        "-Map=${_map}"
        "$<TARGET_FILE:${target}>.lto.obj"
        ${_exports_file}
        -L "/usr/x86_64-w64-mingw32/lib"
        ${_libraries}
        -o "${_output}"

        COMMAND
        "${CMAKE_COMMAND}"
        -E env
        "PYTHONPATH=${CMAKE_CURRENT_FUNCTION_LIST_DIR}"
        "${Python3_EXECUTABLE}"
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/pe_post_process/main.py"
        "${_map}"

        COMMENT
        "GNU final link and PE post-processing ${target}"

        VERBATIM
        COMMAND_EXPAND_LISTS
    )

endfunction()
