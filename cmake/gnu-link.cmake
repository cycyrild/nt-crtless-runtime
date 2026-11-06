function(add_gnu_final_link target)
  cmake_parse_arguments(
    PARSE_ARGV
    1
    ARG
    ""
    "SUBSYSTEM;OUTPUT_SUFFIX;EXPORTS_FILE"
    "LIBRARIES"
  )

  cmake_path(
    ABSOLUTE_PATH ARG_EXPORTS_FILE
    BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    NORMALIZE
    OUTPUT_VARIABLE _exports_file
  )

  get_target_property(_target_type "${target}" TYPE)
  if(_target_type STREQUAL "SHARED_LIBRARY")
    set(_pe_options --dll)
  elseif(_target_type STREQUAL "EXECUTABLE")
    set(_pe_options)
  else()
    message(FATAL_ERROR "add_gnu_final_link: target '${target}' must be "
                        "an EXECUTABLE or SHARED_LIBRARY, got '${_target_type}'"
    )
  endif()

  set(_linker_script "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/gnu-final.ld")

  set(_output "${CMAKE_CURRENT_BINARY_DIR}/${target}.ld${ARG_OUTPUT_SUFFIX}")

  set(_map "${CMAKE_CURRENT_BINARY_DIR}/${target}.ld.map")

  # Keep LLD's PE internal
  set_target_properties(
    ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                         "${CMAKE_CURRENT_BINARY_DIR}/intermediate/$<CONFIG>"
  )
  target_link_options(
    ${target}
    PRIVATE
    -nostdlib
    -nodefaultlibs
    -nostartfiles
    -fno-rtlib-defaultlib
    -flto=full
    "LINKER:/base:${PREFERRED_IMAGE_BASE}"
    "LINKER:/filealign:${PE_ALIGNMENT}"
    "LINKER:/align:${PE_ALIGNMENT}"
    LINKER:/entry:start
    LINKER:/map
    LINKER:/opt:ref
    LINKER:/opt:icf
    LINKER:/timestamp:0
    LINKER:/lldsavetemps
    "LINKER:/def:${_exports_file}"
  )

  set(_libraries)
  foreach(_library IN LISTS ARG_LIBRARIES)
    list(APPEND _libraries "-l${_library}")
  endforeach()

  # cmake-format: off
  add_custom_command(
    TARGET ${target}
    POST_BUILD

    BYPRODUCTS
      "${_output}"
      "${_map}"

    COMMAND
      "${MINGW_LD}"
      -T "${_linker_script}"
      ${_pe_options}
      --subsystem "${ARG_SUBSYSTEM}"
      -e start
      --gc-sections
      --no-insert-timestamp
      "--image-base=${PREFERRED_IMAGE_BASE}"
      "--file-alignment=${PE_ALIGNMENT}"
      "--section-alignment=${PE_ALIGNMENT}"
      --strip-all
      --nostdlib
      "-Map=${_map}"
      "$<TARGET_FILE:${target}>.lto.obj"
      ${_exports_file}
      ${_libraries}
      -o "${_output}"

    COMMAND
      "${CMAKE_COMMAND}"
      -E env
      "PYTHONPATH=${CMAKE_CURRENT_FUNCTION_LIST_DIR}/pe_post_process"
      "${PYTHON3}"
      -m src
      "${_map}"

    COMMENT
      "GNU final link and PE post-processing ${target}"

    VERBATIM
    COMMAND_EXPAND_LISTS
  )
  # cmake-format: on

endfunction()
