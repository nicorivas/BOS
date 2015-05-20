cmake_minimum_required( VERSION 2.8 )

#==============================================================================
#=============[ The superduperawesome hgui video tool!!! ]=====================
#==============================================================================

add_executable( hggui hggui.cpp ../renderer/Shader.cpp )
add_executable( hgpostproc hgpostproc.cpp )

add_dependencies( hggui build_shaders )
    
target_link_libraries( hggui ${UI_LIBS} ${HDF5_LIBS} )


