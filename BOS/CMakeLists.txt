cmake_minimum_required( VERSION 2.8 )
project ( BOS )

option( GRAPHICAL_TOOLS "Enable the graphical debug tools (requires bullshit)" ON )
if (GRAPHICAL_TOOLS)
    include( ExternalProject )
    option( FIRST_BUILD "Download + compile the glfw sources only" OFF )

    if(UNIX)
        list( APPEND CMAKE_CXX_FLAGS "-std=c++11" )
        if (APPLE)
            find_package( glew REQUIRED)
            find_library( COCOA_LIBRARY cocoa )
            find_library( IOKIT_LIBRARY iokit )
            find_library( COREVIDEO_LIBRARY corevideo )
        endif()
    endif()

    ExternalProject_Add(GLFW
                        PREFIX "${CMAKE_BINARY_DIR}/lib"
                        BINARY_DIR "${CMAKE_BINARY_DIR}/lib"
                        CMAKE_ARGS
                        "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/lib"
                        #INSTALL_COMMAND ""
                        GIT_REPOSITORY "https://github.com/glfw/glfw.git")

    find_library( libglfw
              NAMES glfw3 
              PATHS "${CMAKE_BINARY_DIR}/lib"
              PATH_SUFFIXES lib src)

    include_directories( "${CMAKE_BINARY_DIR}/lib/src/GLFW/include" )
    include_directories( "${CMAKE_SOURCE_DIR}" )


    if (NOT ${FIRST_BUILD})
      add_executable( edrun main.cpp renderer/Shader.cpp )
      if (UNIX AND (NOT APPLE))
        target_link_libraries( edrun GL GLU GLEW pthread ${libglfw} ${glfw_LIBARIRES} X11 Xrandr Xinerama Xi Xxf86vm Xcursor )
      elseif (UNIX AND APPLE)
        find_package(OpenGL REQUIRED)
        target_link_libraries( edrun "${GLEW_LIBRARIES}" "${IOKIT_LIBRARY}" "${COREVIDEO_LIBRARY}" "${COCOA_LIBRARY}" "${OPENGL_LIBRARIES}" "${libglfw}" )
      endif()
    endif()
endif()