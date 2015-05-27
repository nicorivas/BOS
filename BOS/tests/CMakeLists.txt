cmake_minimum_required( VERSION 2.8 )

add_executable( PriorityQueue PriorityQueue.cpp )
add_test( PriorityQueue PriorityQueue )

add_executable( MathUnitTest MathUnitTest.cpp )
add_test( MathUnitTest MathUnitTest )

add_executable( WallCollisionsUnitTest WallCollisionsUnitTest.cpp )
add_test( WallCollisionsUnitTest WallCollisionsUnitTest )

add_executable( SimulationUnitTest SimulationUnitTest.cpp )
add_test( SimulationUnitTest SimulationUnitTest )

add_executable( GridUnitTest GridUnitTest.cpp )
add_test( GridUnitTest GridUnitTest )

add_executable( IntersectionUnitTest IntersectionUnitTest.cpp )
add_test( IntersectionUnitTest IntersectionUnitTest )
