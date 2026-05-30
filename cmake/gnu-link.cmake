function(add_gnu_final_link target dependency)
    set(_gnu_linker_script
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/gnu-final.ld"
    )

    set(_gnu_executable
        "${CMAKE_CURRENT_BINARY_DIR}/${target}.ld.exe"
    )

    set(_map_file
        "${CMAKE_CURRENT_BINARY_DIR}/${target}.ld.map"
    )

    add_custom_command(
        OUTPUT
        "${_gnu_executable}"
        "${_map_file}"

        COMMAND
        x86_64-w64-mingw32-ld
        -T "${_gnu_linker_script}"
        --subsystem console
        -e start
        --gc-sections
        --no-insert-timestamp
        --section-alignment=0x1000
        --file-alignment=0x1000
        --strip-all
        --nostdlib
        "-Map=${_map_file}"
        "$<TARGET_FILE:${target}>.lto.obj"
        "$<TARGET_FILE:${dependency}>"
        -L "${XWIN_ROOT}/sdk/lib/um/x86_64"
        -lntdll
        -o "${_gnu_executable}"

        DEPENDS
        ${target}
        ${dependency}
        "${_gnu_linker_script}"

        COMMENT
        "GNU final link ${target}"

        VERBATIM
        COMMAND_EXPAND_LISTS
    )

    add_custom_target(
        ${target}-gnu-link
        ALL
        DEPENDS
        "${_gnu_executable}"
        "${_map_file}"
    )
endfunction()
