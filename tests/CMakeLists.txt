cmake_minimum_required( VERSION 2.8 )

add_executable( PriorityQueue PriorityQueue.cpp )
add_test( PriorityQueue PriorityQueue )

add_executable( MathUnitTest MathUnitTest.cpp )
add_test( MathUnitTest MathUnitTest )

add_executable( WallCollisionsUnitTest WallCollisionsUnitTest.cpp )
add_test( WallCollisionsUnitTest WallCollisionsUnitTest )
