cmake_minimum_required( VERSION 2.8 )
project ( BOS )
include( ExternalProject )

option( GRAPHICAL_TOOLS "Enable the graphical debug tools (requires bullshit)" ON )
option( TESTING_ENABLED "Enables the unit tests" ON )

if (UNIX)
 list( APPEND CMAKE_CXX_FLAGS "-std=c++11" )
endif()

include_directories( "${CMAKE_SOURCE_DIR}" )

#==============================================================================
#=========================[  HDF5 Lib  ]=======================================
#==============================================================================

#Congratulations! HDF5 will now be added.
set( HDF5_VERSION 1.8.14 )

# HDF5 install from repos is broken currently.
# So we just include it for our and your convenience!
ExternalProject_Add( hdf5
   PREFIX "${CMAKE_CURRENT_BINARY_DIR}/lib/hdf5"
   SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/hdf5-${HDF5_VERSION}
   INSTALL_COMMAND ""
   CMAKE_ARGS "-Wno-dev" "-DHDF5_BUILD_TOOLS=ON" "-DHDF5_BUILD_CPP_LIB=ON" "-DHDF5_BUILD_EXAMPLES=ON" "-DCMAKE_BUILD_TYPE=Release" )
  
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/deps/hdf5-${HDF5_VERSION}/src" )
include_directories( "${CMAKE_CURRENT_BINARY_DIR}/lib/hdf5/src/hdf5-build" )

#==============================================================================
#========================[  Graphical bullshit ]===============================
#==============================================================================

if (GRAPHICAL_TOOLS)
    option( FIRST_BUILD "Download + compile the glfw sources only" OFF )
    option( VIDEO_OUTPUT "Enable video output" OFF)
    option( MERCURY_TOOLS "Enable the awesome MercuryDPM tools which are too cool for the official MercuryDPM repo." ON )

#==============================================================================
#========================[ Video lib bullshit ]================================
#==============================================================================

    if(VIDEO_OUTPUT)
      find_library( AVCODEC_LIBRARIES avcodec)
      find_library( AVUTIL_LIBRARIES avutil)
      add_definitions( -DENABLE_VIDEO )
    endif()

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
	set( HDF5_LIBS ${CMAKE_CURRENT_BINARY_DIR}/lib/hdf5/src/hdf5-build/bin/libhdf5.a dl )
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
      add_executable( edrun main.cpp renderer/Shader.cpp  hdf5/HDF5.cpp )
      add_executable( edgui edgui.cpp renderer/Shader.cpp )
      add_dependencies( edrun GLFW hdf5 )

      if ( MERCURY_TOOLS )
        add_subdirectory( hg )
      endif()

      target_link_libraries( edrun ${UI_LIBS} ${HDF5_LIBS} )
      target_link_libraries( edgui ${UI_LIBS} ${HDF5_LIBS} )
    endif()
endif()

#==============================================================================
#================[ And bad code for awesome tests here ]=======================
#==============================================================================

if (TESTING_ENABLED)
  enable_testing()
  add_subdirectory( tests )
endif()

