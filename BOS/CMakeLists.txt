cmake_minimum_required( VERSION 2.8 )
project ( BOS )
include( ExternalProject )

option( GRAPHICAL_TOOLS "Enable the graphical debug tools (requires bullshit)" ON )
option( TESTING_ENABLED "Enables the unit tests" ON )

if (UNIX)
  list( APPEND CMAKE_CXX_FLAGS "-std=c++11 -Wall" )
endif()

include_directories( "${CMAKE_SOURCE_DIR}" )

#==============================================================================
#========================[  Graphical bullshit ]===============================
#==============================================================================

if (GRAPHICAL_TOOLS)
    option( FIRST_BUILD "Download + compile the glfw sources only" OFF )

#==============================================================================
#========================[ Graphics lib bullshit ]=============================
#==============================================================================

    ExternalProject_Add(GLFW
                        PREFIX "${CMAKE_BINARY_DIR}/lib/glfw"
                        BINARY_DIR "${CMAKE_BINARY_DIR}/lib/glfw"
                        CMAKE_ARGS
                        "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/lib"
                        #INSTALL_COMMAND ""
                        GIT_REPOSITORY "https://github.com/glfw/glfw.git")

    find_library( libglfw
              NAMES glfw3 
              PATHS "${CMAKE_BINARY_DIR}/lib/glfw"
              PATH_SUFFIXES lib src)

    include_directories( "${CMAKE_BINARY_DIR}/lib/glfw/src/GLFW/include" )
    include_directories( "${CMAKE_BINARY_DIR}" )

#==============================================================================
#======================[ What do we link against? ]============================
#==============================================================================
       
    if(UNIX)
        if (APPLE)
            find_package( OpenGL REQUIRED )
            find_package( glew REQUIRED )
            find_library( COCOA_LIBRARY cocoa )
            find_library( IOKIT_LIBRARY iokit )
            find_library( COREVIDEO_LIBRARY corevideo )
	    set( UI_LIBS "${GLEW_LIBRARIES}" "${IOKIT_LIBRARY}" "${COREVIDEO_LIBRARY}" "${COCOA_LIBRARY}" "${OPENGL_LIBRARIES}" "${libglfw}" )
        else()
            set( UI_LIBS GL GLU GLEW ${libglfw} ${glfw_LIBARIRES} X11 Xrandr Xinerama Xi Xxf86vm Xcursor pthread )
        endif()
        if (VIDEO_OUTPUT)
          LIST( APPEND UI_LIBS ${AVUTIL_LIBRARIES} ${AVCODEC_LIBRARIES} )
        endif()
    else()
        message( ERROR "You seem to be using Windows. Ain't nobody got time fo that!" )
    endif()

    if (NOT ${FIRST_BUILD})
      add_subdirectory( data )

#==============================================================================
#=============[ Put bad code for executables right here ]======================
#==============================================================================
      add_executable( bos bos.cpp )
      add_dependencies( bos GLFW )

      target_link_libraries( bos ${UI_LIBS} )
    endif()
endif()

#==============================================================================
#================[ And bad code for awesome tests here ]=======================
#==============================================================================

if (TESTING_ENABLED)
  enable_testing()
  add_subdirectory( tests )
endif()

