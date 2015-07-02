cmake_minimum_required( VERSION 2.8 )
project ( BOS )
include( ExternalProject )

option( TESTING_ENABLED "Enables the unit tests" ON )

if (UNIX)
  list( APPEND CMAKE_CXX_FLAGS "-std=c++11 -Wall" )
endif()

include_directories( "${CMAKE_SOURCE_DIR}" )

#==============================================================================
#======================[ What do we link against? ]============================
#==============================================================================
       
    if(UNIX)
        if (APPLE)
            set( UI_LIBS )
        else()
            set( pthread )
        endif()
   else()
        message( ERROR "You seem to be using Windows. Ain't nobody got time fo that!" )
   endif()

#   if (NOT ${FIRST_BUILD})
      add_executable( bos bos.cpp )
      target_link_libraries( bos ${UI_LIBS} )
#   endif()

#==============================================================================
#================[ And code for awesome tests here ]=======================
#==============================================================================

if (TESTING_ENABLED)
  enable_testing()
  add_subdirectory( tests )
endif()

