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
CMAKE_BINARY_DIR = /Users/nicorivas/Software/BOS

# Include any dependencies generated for this target.
include tests/CMakeFiles/GridUnitTest.dir/depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/GridUnitTest.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/GridUnitTest.dir/flags.make

tests/CMakeFiles/GridUnitTest.dir/GridUnitTest.cpp.o: tests/CMakeFiles/GridUnitTest.dir/flags.make
tests/CMakeFiles/GridUnitTest.dir/GridUnitTest.cpp.o: tests/GridUnitTest.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /Users/nicorivas/Software/BOS/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object tests/CMakeFiles/GridUnitTest.dir/GridUnitTest.cpp.o"
	cd /Users/nicorivas/Software/BOS/tests && /usr/bin/g++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/GridUnitTest.dir/GridUnitTest.cpp.o -c /Users/nicorivas/Software/BOS/tests/GridUnitTest.cpp

tests/CMakeFiles/GridUnitTest.dir/GridUnitTest.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/GridUnitTest.dir/GridUnitTest.cpp.i"
	cd /Users/nicorivas/Software/BOS/tests && /usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -E /Users/nicorivas/Software/BOS/tests/GridUnitTest.cpp > CMakeFiles/GridUnitTest.dir/GridUnitTest.cpp.i

tests/CMakeFiles/GridUnitTest.dir/GridUnitTest.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/GridUnitTest.dir/GridUnitTest.cpp.s"
	cd /Users/nicorivas/Software/BOS/tests && /usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -S /Users/nicorivas/Software/BOS/tests/GridUnitTest.cpp -o CMakeFiles/GridUnitTest.dir/GridUnitTest.cpp.s

tests/CMakeFiles/GridUnitTest.dir/GridUnitTest.cpp.o.requires:
.PHONY : tests/CMakeFiles/GridUnitTest.dir/GridUnitTest.cpp.o.requires

tests/CMakeFiles/GridUnitTest.dir/GridUnitTest.cpp.o.provides: tests/CMakeFiles/GridUnitTest.dir/GridUnitTest.cpp.o.requires
	$(MAKE) -f tests/CMakeFiles/GridUnitTest.dir/build.make tests/CMakeFiles/GridUnitTest.dir/GridUnitTest.cpp.o.provides.build
.PHONY : tests/CMakeFiles/GridUnitTest.dir/GridUnitTest.cpp.o.provides

tests/CMakeFiles/GridUnitTest.dir/GridUnitTest.cpp.o.provides.build: tests/CMakeFiles/GridUnitTest.dir/GridUnitTest.cpp.o

# Object files for target GridUnitTest
GridUnitTest_OBJECTS = \
"CMakeFiles/GridUnitTest.dir/GridUnitTest.cpp.o"

# External object files for target GridUnitTest
GridUnitTest_EXTERNAL_OBJECTS =

tests/GridUnitTest: tests/CMakeFiles/GridUnitTest.dir/GridUnitTest.cpp.o
tests/GridUnitTest: tests/CMakeFiles/GridUnitTest.dir/build.make
tests/GridUnitTest: tests/CMakeFiles/GridUnitTest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable GridUnitTest"
	cd /Users/nicorivas/Software/BOS/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/GridUnitTest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/GridUnitTest.dir/build: tests/GridUnitTest
.PHONY : tests/CMakeFiles/GridUnitTest.dir/build

tests/CMakeFiles/GridUnitTest.dir/requires: tests/CMakeFiles/GridUnitTest.dir/GridUnitTest.cpp.o.requires
.PHONY : tests/CMakeFiles/GridUnitTest.dir/requires

tests/CMakeFiles/GridUnitTest.dir/clean:
	cd /Users/nicorivas/Software/BOS/tests && $(CMAKE_COMMAND) -P CMakeFiles/GridUnitTest.dir/cmake_clean.cmake
.PHONY : tests/CMakeFiles/GridUnitTest.dir/clean

tests/CMakeFiles/GridUnitTest.dir/depend:
	cd /Users/nicorivas/Software/BOS && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/nicorivas/Software/BOS /Users/nicorivas/Software/BOS/tests /Users/nicorivas/Software/BOS /Users/nicorivas/Software/BOS/tests /Users/nicorivas/Software/BOS/tests/CMakeFiles/GridUnitTest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/CMakeFiles/GridUnitTest.dir/depend

