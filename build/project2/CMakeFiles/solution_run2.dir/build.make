# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/huxinyu/Compiler-Group13

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/huxinyu/Compiler-Group13/build

# Utility rule file for solution_run2.

# Include the progress variables for this target.
include project2/CMakeFiles/solution_run2.dir/progress.make

project2/CMakeFiles/solution_run2:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/huxinyu/Compiler-Group13/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "run generated solution in /home/huxinyu/Compiler-Group13/project2"
	cd /home/huxinyu/Compiler-Group13/project2 && /home/huxinyu/Compiler-Group13/build/project2/solution2

solution_run2: project2/CMakeFiles/solution_run2
solution_run2: project2/CMakeFiles/solution_run2.dir/build.make

.PHONY : solution_run2

# Rule to build all files generated by this target.
project2/CMakeFiles/solution_run2.dir/build: solution_run2

.PHONY : project2/CMakeFiles/solution_run2.dir/build

project2/CMakeFiles/solution_run2.dir/clean:
	cd /home/huxinyu/Compiler-Group13/build/project2 && $(CMAKE_COMMAND) -P CMakeFiles/solution_run2.dir/cmake_clean.cmake
.PHONY : project2/CMakeFiles/solution_run2.dir/clean

project2/CMakeFiles/solution_run2.dir/depend:
	cd /home/huxinyu/Compiler-Group13/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/huxinyu/Compiler-Group13 /home/huxinyu/Compiler-Group13/project2 /home/huxinyu/Compiler-Group13/build /home/huxinyu/Compiler-Group13/build/project2 /home/huxinyu/Compiler-Group13/build/project2/CMakeFiles/solution_run2.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : project2/CMakeFiles/solution_run2.dir/depend

