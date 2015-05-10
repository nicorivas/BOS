cmake_minimum_required( VERSION 2.8 )
project ( BOS )
include( ExternalProject )

option( GRAPHICAL_TOOLS "Enable the graphical debug tools (requires bullshit)" ON )
option( TESTING_ENABLED "Enables the unit tests" ON )

if (UNIX)
 list( APPEND CMAKE_CXX_FLAGS "-std=c++11" )
endif()

include_directories( "${CMAKE_SOURCE_DIR}" )

#Congratulations! HDF5 will now be added.
set( HDF5_VERSION 1.8.14 )

ExternalProject_Add( hdf5
   PREFIX "${CMAKE_CURRENT_BINARY_DIR}/lib/hdf5"
   SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/hdf5-${HDF5_VERSION}
   INSTALL_COMMAND ""
   CMAKE_ARGS "-Wno-dev" "-DHDF5_BUILD_TOOLS=ON" "-DHDF5_BUILD_CPP_LIB=ON" "-DHDF5_BUILD_EXAMPLES=ON" "-DCMAKE_BUILD_TYPE=Release" )
  
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/deps/hdf5-${HDF5_VERSION}/src" )
include_directories( "${CMAKE_CURRENT_BINARY_DIR}/lib/hdf5/src/hdf5-build" )

add_executable( HDF5UnitTest HDF5UnitTest.cpp hdf5/HDF5.cpp )
target_link_libraries( HDF5UnitTest ${CMAKE_CURRENT_BINARY_DIR}/lib/hdf5/src/hdf5-build/bin/libhdf5.a dl)
# target_compile_options( HDF5UnitTest PUBLIC -fno-rtti )
add_test( HDF5UnitTest HDF5UnitTest )
  
find_library( DIALOG_LIBRARIES dialog)
find_library( CURSES_LIBRARIES ncursesw)

if (GRAPHICAL_TOOLS)
    option( FIRST_BUILD "Download + compile the glfw sources only" OFF )

    if(UNIX)
       
        if (APPLE)
            find_package( glew REQUIRED)
            find_library( COCOA_LIBRARY cocoa )
            find_library( IOKIT_LIBRARY iokit )
            find_library( COREVIDEO_LIBRARY corevideo )
        endif()
    endif()

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



    if (NOT ${FIRST_BUILD})
      add_executable( edrun main.cpp renderer/Shader.cpp  hdf5/HDF5.cpp )

      add_executable( edgui edgui.cpp renderer/Shader.cpp )


      if (UNIX AND (NOT APPLE))
        target_link_libraries( edrun GL GLU GLEW ${libglfw} ${glfw_LIBARIRES} X11 Xrandr Xinerama Xi Xxf86vm Xcursor pthread 
                                     ${CMAKE_CURRENT_BINARY_DIR}/lib/hdf5/src/hdf5-build/bin/libhdf5.a dl)
        target_link_libraries( edgui GL GLU GLEW ${libglfw} ${glfw_LIBARIRES} X11 Xrandr Xinerama Xi Xxf86vm Xcursor pthread )
      elseif (UNIX AND APPLE)
        find_package(OpenGL REQUIRED)
        target_link_libraries( edrun "${GLEW_LIBRARIES}" "${IOKIT_LIBRARY}" "${COREVIDEO_LIBRARY}" "${COCOA_LIBRARY}" "${OPENGL_LIBRARIES}" "${libglfw}"
                                     ${CMAKE_CURRENT_BINARY_DIR}/lib/hdf5/src/hdf5-build/bin/libhdf5.a dl )
        target_link_libraries( edgui "${GLEW_LIBRARIES}" "${IOKIT_LIBRARY}" "${COREVIDEO_LIBRARY}" "${COCOA_LIBRARY}" "${OPENGL_LIBRARIES}" "${libglfw}" )
      endif()
    endif()
endif()

if (TESTING_ENABLED)
  enable_testing()
  add_subdirectory( tests )
endif()

