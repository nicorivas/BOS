# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.1

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.1.2/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.1.2/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/nicorivas/Software/BOS

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/nicorivas/Software/BOS/build

# Include any dependencies generated for this target.
include tests/CMakeFiles/WallCollisionsUnitTest.dir/depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/WallCollisionsUnitTest.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/WallCollisionsUnitTest.dir/flags.make

tests/CMakeFiles/WallCollisionsUnitTest.dir/WallCollisionsUnitTest.cpp.o: tests/CMakeFiles/WallCollisionsUnitTest.dir/flags.make
tests/CMakeFiles/WallCollisionsUnitTest.dir/WallCollisionsUnitTest.cpp.o: ../tests/WallCollisionsUnitTest.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /Users/nicorivas/Software/BOS/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object tests/CMakeFiles/WallCollisionsUnitTest.dir/WallCollisionsUnitTest.cpp.o"
	cd /Users/nicorivas/Software/BOS/build/tests && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/WallCollisionsUnitTest.dir/WallCollisionsUnitTest.cpp.o -c /Users/nicorivas/Software/BOS/tests/WallCollisionsUnitTest.cpp

tests/CMakeFiles/WallCollisionsUnitTest.dir/WallCollisionsUnitTest.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WallCollisionsUnitTest.dir/WallCollisionsUnitTest.cpp.i"
	cd /Users/nicorivas/Software/BOS/build/tests && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /Users/nicorivas/Software/BOS/tests/WallCollisionsUnitTest.cpp > CMakeFiles/WallCollisionsUnitTest.dir/WallCollisionsUnitTest.cpp.i

tests/CMakeFiles/WallCollisionsUnitTest.dir/WallCollisionsUnitTest.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WallCollisionsUnitTest.dir/WallCollisionsUnitTest.cpp.s"
	cd /Users/nicorivas/Software/BOS/build/tests && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /Users/nicorivas/Software/BOS/tests/WallCollisionsUnitTest.cpp -o CMakeFiles/WallCollisionsUnitTest.dir/WallCollisionsUnitTest.cpp.s

tests/CMakeFiles/WallCollisionsUnitTest.dir/WallCollisionsUnitTest.cpp.o.requires:
.PHONY : tests/CMakeFiles/WallCollisionsUnitTest.dir/WallCollisionsUnitTest.cpp.o.requires

tests/CMakeFiles/WallCollisionsUnitTest.dir/WallCollisionsUnitTest.cpp.o.provides: tests/CMakeFiles/WallCollisionsUnitTest.dir/WallCollisionsUnitTest.cpp.o.requires
	$(MAKE) -f tests/CMakeFiles/WallCollisionsUnitTest.dir/build.make tests/CMakeFiles/WallCollisionsUnitTest.dir/WallCollisionsUnitTest.cpp.o.provides.build
.PHONY : tests/CMakeFiles/WallCollisionsUnitTest.dir/WallCollisionsUnitTest.cpp.o.provides

tests/CMakeFiles/WallCollisionsUnitTest.dir/WallCollisionsUnitTest.cpp.o.provides.build: tests/CMakeFiles/WallCollisionsUnitTest.dir/WallCollisionsUnitTest.cpp.o

# Object files for target WallCollisionsUnitTest
WallCollisionsUnitTest_OBJECTS = \
"CMakeFiles/WallCollisionsUnitTest.dir/WallCollisionsUnitTest.cpp.o"

# External object files for target WallCollisionsUnitTest
WallCollisionsUnitTest_EXTERNAL_OBJECTS =

tests/WallCollisionsUnitTest: tests/CMakeFiles/WallCollisionsUnitTest.dir/WallCollisionsUnitTest.cpp.o
tests/WallCollisionsUnitTest: tests/CMakeFiles/WallCollisionsUnitTest.dir/build.make
tests/WallCollisionsUnitTest: tests/CMakeFiles/WallCollisionsUnitTest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable WallCollisionsUnitTest"
	cd /Users/nicorivas/Software/BOS/build/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/WallCollisionsUnitTest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/WallCollisionsUnitTest.dir/build: tests/WallCollisionsUnitTest
.PHONY : tests/CMakeFiles/WallCollisionsUnitTest.dir/build

tests/CMakeFiles/WallCollisionsUnitTest.dir/requires: tests/CMakeFiles/WallCollisionsUnitTest.dir/WallCollisionsUnitTest.cpp.o.requires
.PHONY : tests/CMakeFiles/WallCollisionsUnitTest.dir/requires

tests/CMakeFiles/WallCollisionsUnitTest.dir/clean:
	cd /Users/nicorivas/Software/BOS/build/tests && $(CMAKE_COMMAND) -P CMakeFiles/WallCollisionsUnitTest.dir/cmake_clean.cmake
.PHONY : tests/CMakeFiles/WallCollisionsUnitTest.dir/clean

tests/CMakeFiles/WallCollisionsUnitTest.dir/depend:
	cd /Users/nicorivas/Software/BOS/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/nicorivas/Software/BOS /Users/nicorivas/Software/BOS/tests /Users/nicorivas/Software/BOS/build /Users/nicorivas/Software/BOS/build/tests /Users/nicorivas/Software/BOS/build/tests/CMakeFiles/WallCollisionsUnitTest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/CMakeFiles/WallCollisionsUnitTest.dir/depend

