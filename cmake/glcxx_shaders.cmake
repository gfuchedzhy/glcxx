# generate single cpp file
function(glcxx_generate_shader shader cpp)
  get_filename_component(name ${shader} NAME_WE)
  # string to char sequence
  string(REGEX REPLACE "(.)" "'\\1'," name ${name})
  string(REGEX REPLACE ",$" "" name ${name})

  file(READ ${shader} shader_content)
  file(WRITE ${cpp}
    "// generated from ${shader}; do not edit\n"
    "namespace glcxx {\n"
    "\tnamespace ct {\n"
    "\t\ttemplate<char...c> struct string;\n\t}\n\n"
    "\ttemplate<typename TName>\n"
    "\tconst char* get_program_src();\n\n"
    "\ttemplate<>\n"
    "\tconst char* get_program_src<ct::string<${name}>>()\n"
    "\t{\n\t\tstatic char retval[] = R\"("
    "${shader_content}"
    ")\";\n\t\treturn retval;\n\t}\n}")
endfunction()

# in command mode, execute generation
if (CMAKE_ARGC)
  glcxx_generate_shader("${IN}" "${OUT}")
endif()

# store current script path
set(GLCXX_GENERATE_SHADERS_PATH ${CMAKE_CURRENT_LIST_DIR}/glcxx_shaders.cmake
  CACHE INTERNAL "")

# schedule generation for given files, store result to out_cpp
function(glcxx_generate_shaders out_cpp)
  set(src)
  foreach (shader ${ARGN})
    if(IS_ABSOLUTE ${shader})
      file(RELATIVE_PATH shader ${CMAKE_CURRENT_SOURCE_DIR} ${shader})
    endif()
    set(in  "${CMAKE_CURRENT_SOURCE_DIR}/${shader}")
    set(out "${CMAKE_CURRENT_BINARY_DIR}/${shader}.cpp")
    add_custom_command(
      OUTPUT ${out}
      COMMAND ${CMAKE_COMMAND} -DIN=${in} -DOUT=${out} -P ${GLCXX_GENERATE_SHADERS_PATH}
      DEPENDS ${in}
      COMMENT "Generating shader CXX source ${shader}.cpp"
      VERBATIM)
    list(APPEND src ${out})
  endforeach ()
  set(${out_cpp} "${src}" PARENT_SCOPE)
endfunction()

# compile given shaders into a static library
function(glcxx_compile_shaders lib)
  glcxx_generate_shaders(out_cpp ${ARGN})
  add_library("${lib}" "${out_cpp}")
endfunction()
