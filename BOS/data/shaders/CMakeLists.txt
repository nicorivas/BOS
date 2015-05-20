cmake_minimum_required( VERSION 2.8 )

FILE(GLOB ALL_SHADERS *.glsl)
FOREACH(SHADER ${ALL_SHADERS})
  get_filename_component(SHADERNAME ${SHADER} NAME_WE)
  add_custom_command(OUTPUT shader_${SHADERNAME}.h
    COMMAND echo " const static char ${SHADERNAME}SRC[] = {"        >  ${CMAKE_CURRENT_BINARY_DIR}/shader_${SHADERNAME}.h
         && xxd -i < ${CMAKE_CURRENT_SOURCE_DIR}/${SHADERNAME}.glsl >> ${CMAKE_CURRENT_BINARY_DIR}/shader_${SHADERNAME}.h
         && echo "};"                                               >> ${CMAKE_CURRENT_BINARY_DIR}/shader_${SHADERNAME}.h
  DEPENDS ${SHADERNAME}.glsl
  VERBATIM )
  LIST( APPEND SHADERLIST shader_${SHADERNAME}.h )
ENDFOREACH()

add_custom_target( build_shaders DEPENDS ${SHADERLIST} )



