# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/xbj/projects/RPC

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/xbj/projects/RPC/build

# Include any dependencies generated for this target.
include CMakeFiles/app.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/app.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/app.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/app.dir/flags.make

CMakeFiles/app.dir/rocket/common/config.cpp.o: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/rocket/common/config.cpp.o: /home/xbj/projects/RPC/rocket/common/config.cpp
CMakeFiles/app.dir/rocket/common/config.cpp.o: CMakeFiles/app.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/xbj/projects/RPC/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/app.dir/rocket/common/config.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/app.dir/rocket/common/config.cpp.o -MF CMakeFiles/app.dir/rocket/common/config.cpp.o.d -o CMakeFiles/app.dir/rocket/common/config.cpp.o -c /home/xbj/projects/RPC/rocket/common/config.cpp

CMakeFiles/app.dir/rocket/common/config.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/app.dir/rocket/common/config.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/xbj/projects/RPC/rocket/common/config.cpp > CMakeFiles/app.dir/rocket/common/config.cpp.i

CMakeFiles/app.dir/rocket/common/config.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/app.dir/rocket/common/config.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/xbj/projects/RPC/rocket/common/config.cpp -o CMakeFiles/app.dir/rocket/common/config.cpp.s

CMakeFiles/app.dir/rocket/common/log.cpp.o: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/rocket/common/log.cpp.o: /home/xbj/projects/RPC/rocket/common/log.cpp
CMakeFiles/app.dir/rocket/common/log.cpp.o: CMakeFiles/app.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/xbj/projects/RPC/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/app.dir/rocket/common/log.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/app.dir/rocket/common/log.cpp.o -MF CMakeFiles/app.dir/rocket/common/log.cpp.o.d -o CMakeFiles/app.dir/rocket/common/log.cpp.o -c /home/xbj/projects/RPC/rocket/common/log.cpp

CMakeFiles/app.dir/rocket/common/log.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/app.dir/rocket/common/log.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/xbj/projects/RPC/rocket/common/log.cpp > CMakeFiles/app.dir/rocket/common/log.cpp.i

CMakeFiles/app.dir/rocket/common/log.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/app.dir/rocket/common/log.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/xbj/projects/RPC/rocket/common/log.cpp -o CMakeFiles/app.dir/rocket/common/log.cpp.s

CMakeFiles/app.dir/rocket/common/util.cpp.o: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/rocket/common/util.cpp.o: /home/xbj/projects/RPC/rocket/common/util.cpp
CMakeFiles/app.dir/rocket/common/util.cpp.o: CMakeFiles/app.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/xbj/projects/RPC/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/app.dir/rocket/common/util.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/app.dir/rocket/common/util.cpp.o -MF CMakeFiles/app.dir/rocket/common/util.cpp.o.d -o CMakeFiles/app.dir/rocket/common/util.cpp.o -c /home/xbj/projects/RPC/rocket/common/util.cpp

CMakeFiles/app.dir/rocket/common/util.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/app.dir/rocket/common/util.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/xbj/projects/RPC/rocket/common/util.cpp > CMakeFiles/app.dir/rocket/common/util.cpp.i

CMakeFiles/app.dir/rocket/common/util.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/app.dir/rocket/common/util.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/xbj/projects/RPC/rocket/common/util.cpp -o CMakeFiles/app.dir/rocket/common/util.cpp.s

CMakeFiles/app.dir/rocket/net/eventloop.cpp.o: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/rocket/net/eventloop.cpp.o: /home/xbj/projects/RPC/rocket/net/eventloop.cpp
CMakeFiles/app.dir/rocket/net/eventloop.cpp.o: CMakeFiles/app.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/xbj/projects/RPC/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/app.dir/rocket/net/eventloop.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/app.dir/rocket/net/eventloop.cpp.o -MF CMakeFiles/app.dir/rocket/net/eventloop.cpp.o.d -o CMakeFiles/app.dir/rocket/net/eventloop.cpp.o -c /home/xbj/projects/RPC/rocket/net/eventloop.cpp

CMakeFiles/app.dir/rocket/net/eventloop.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/app.dir/rocket/net/eventloop.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/xbj/projects/RPC/rocket/net/eventloop.cpp > CMakeFiles/app.dir/rocket/net/eventloop.cpp.i

CMakeFiles/app.dir/rocket/net/eventloop.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/app.dir/rocket/net/eventloop.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/xbj/projects/RPC/rocket/net/eventloop.cpp -o CMakeFiles/app.dir/rocket/net/eventloop.cpp.s

CMakeFiles/app.dir/rocket/net/fd_event.cpp.o: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/rocket/net/fd_event.cpp.o: /home/xbj/projects/RPC/rocket/net/fd_event.cpp
CMakeFiles/app.dir/rocket/net/fd_event.cpp.o: CMakeFiles/app.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/xbj/projects/RPC/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/app.dir/rocket/net/fd_event.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/app.dir/rocket/net/fd_event.cpp.o -MF CMakeFiles/app.dir/rocket/net/fd_event.cpp.o.d -o CMakeFiles/app.dir/rocket/net/fd_event.cpp.o -c /home/xbj/projects/RPC/rocket/net/fd_event.cpp

CMakeFiles/app.dir/rocket/net/fd_event.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/app.dir/rocket/net/fd_event.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/xbj/projects/RPC/rocket/net/fd_event.cpp > CMakeFiles/app.dir/rocket/net/fd_event.cpp.i

CMakeFiles/app.dir/rocket/net/fd_event.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/app.dir/rocket/net/fd_event.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/xbj/projects/RPC/rocket/net/fd_event.cpp -o CMakeFiles/app.dir/rocket/net/fd_event.cpp.s

CMakeFiles/app.dir/rocket/net/io_thread.cpp.o: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/rocket/net/io_thread.cpp.o: /home/xbj/projects/RPC/rocket/net/io_thread.cpp
CMakeFiles/app.dir/rocket/net/io_thread.cpp.o: CMakeFiles/app.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/xbj/projects/RPC/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/app.dir/rocket/net/io_thread.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/app.dir/rocket/net/io_thread.cpp.o -MF CMakeFiles/app.dir/rocket/net/io_thread.cpp.o.d -o CMakeFiles/app.dir/rocket/net/io_thread.cpp.o -c /home/xbj/projects/RPC/rocket/net/io_thread.cpp

CMakeFiles/app.dir/rocket/net/io_thread.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/app.dir/rocket/net/io_thread.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/xbj/projects/RPC/rocket/net/io_thread.cpp > CMakeFiles/app.dir/rocket/net/io_thread.cpp.i

CMakeFiles/app.dir/rocket/net/io_thread.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/app.dir/rocket/net/io_thread.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/xbj/projects/RPC/rocket/net/io_thread.cpp -o CMakeFiles/app.dir/rocket/net/io_thread.cpp.s

CMakeFiles/app.dir/rocket/net/io_thread_group.cpp.o: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/rocket/net/io_thread_group.cpp.o: /home/xbj/projects/RPC/rocket/net/io_thread_group.cpp
CMakeFiles/app.dir/rocket/net/io_thread_group.cpp.o: CMakeFiles/app.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/xbj/projects/RPC/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/app.dir/rocket/net/io_thread_group.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/app.dir/rocket/net/io_thread_group.cpp.o -MF CMakeFiles/app.dir/rocket/net/io_thread_group.cpp.o.d -o CMakeFiles/app.dir/rocket/net/io_thread_group.cpp.o -c /home/xbj/projects/RPC/rocket/net/io_thread_group.cpp

CMakeFiles/app.dir/rocket/net/io_thread_group.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/app.dir/rocket/net/io_thread_group.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/xbj/projects/RPC/rocket/net/io_thread_group.cpp > CMakeFiles/app.dir/rocket/net/io_thread_group.cpp.i

CMakeFiles/app.dir/rocket/net/io_thread_group.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/app.dir/rocket/net/io_thread_group.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/xbj/projects/RPC/rocket/net/io_thread_group.cpp -o CMakeFiles/app.dir/rocket/net/io_thread_group.cpp.s

CMakeFiles/app.dir/rocket/net/timer.cpp.o: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/rocket/net/timer.cpp.o: /home/xbj/projects/RPC/rocket/net/timer.cpp
CMakeFiles/app.dir/rocket/net/timer.cpp.o: CMakeFiles/app.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/xbj/projects/RPC/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/app.dir/rocket/net/timer.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/app.dir/rocket/net/timer.cpp.o -MF CMakeFiles/app.dir/rocket/net/timer.cpp.o.d -o CMakeFiles/app.dir/rocket/net/timer.cpp.o -c /home/xbj/projects/RPC/rocket/net/timer.cpp

CMakeFiles/app.dir/rocket/net/timer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/app.dir/rocket/net/timer.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/xbj/projects/RPC/rocket/net/timer.cpp > CMakeFiles/app.dir/rocket/net/timer.cpp.i

CMakeFiles/app.dir/rocket/net/timer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/app.dir/rocket/net/timer.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/xbj/projects/RPC/rocket/net/timer.cpp -o CMakeFiles/app.dir/rocket/net/timer.cpp.s

CMakeFiles/app.dir/rocket/net/timer_event.cpp.o: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/rocket/net/timer_event.cpp.o: /home/xbj/projects/RPC/rocket/net/timer_event.cpp
CMakeFiles/app.dir/rocket/net/timer_event.cpp.o: CMakeFiles/app.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/xbj/projects/RPC/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/app.dir/rocket/net/timer_event.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/app.dir/rocket/net/timer_event.cpp.o -MF CMakeFiles/app.dir/rocket/net/timer_event.cpp.o.d -o CMakeFiles/app.dir/rocket/net/timer_event.cpp.o -c /home/xbj/projects/RPC/rocket/net/timer_event.cpp

CMakeFiles/app.dir/rocket/net/timer_event.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/app.dir/rocket/net/timer_event.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/xbj/projects/RPC/rocket/net/timer_event.cpp > CMakeFiles/app.dir/rocket/net/timer_event.cpp.i

CMakeFiles/app.dir/rocket/net/timer_event.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/app.dir/rocket/net/timer_event.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/xbj/projects/RPC/rocket/net/timer_event.cpp -o CMakeFiles/app.dir/rocket/net/timer_event.cpp.s

CMakeFiles/app.dir/rocket/net/wakeup_fd_event.cpp.o: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/rocket/net/wakeup_fd_event.cpp.o: /home/xbj/projects/RPC/rocket/net/wakeup_fd_event.cpp
CMakeFiles/app.dir/rocket/net/wakeup_fd_event.cpp.o: CMakeFiles/app.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/xbj/projects/RPC/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object CMakeFiles/app.dir/rocket/net/wakeup_fd_event.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/app.dir/rocket/net/wakeup_fd_event.cpp.o -MF CMakeFiles/app.dir/rocket/net/wakeup_fd_event.cpp.o.d -o CMakeFiles/app.dir/rocket/net/wakeup_fd_event.cpp.o -c /home/xbj/projects/RPC/rocket/net/wakeup_fd_event.cpp

CMakeFiles/app.dir/rocket/net/wakeup_fd_event.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/app.dir/rocket/net/wakeup_fd_event.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/xbj/projects/RPC/rocket/net/wakeup_fd_event.cpp > CMakeFiles/app.dir/rocket/net/wakeup_fd_event.cpp.i

CMakeFiles/app.dir/rocket/net/wakeup_fd_event.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/app.dir/rocket/net/wakeup_fd_event.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/xbj/projects/RPC/rocket/net/wakeup_fd_event.cpp -o CMakeFiles/app.dir/rocket/net/wakeup_fd_event.cpp.s

CMakeFiles/app.dir/testcases/test_eventloop.cpp.o: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/testcases/test_eventloop.cpp.o: /home/xbj/projects/RPC/testcases/test_eventloop.cpp
CMakeFiles/app.dir/testcases/test_eventloop.cpp.o: CMakeFiles/app.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/xbj/projects/RPC/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object CMakeFiles/app.dir/testcases/test_eventloop.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/app.dir/testcases/test_eventloop.cpp.o -MF CMakeFiles/app.dir/testcases/test_eventloop.cpp.o.d -o CMakeFiles/app.dir/testcases/test_eventloop.cpp.o -c /home/xbj/projects/RPC/testcases/test_eventloop.cpp

CMakeFiles/app.dir/testcases/test_eventloop.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/app.dir/testcases/test_eventloop.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/xbj/projects/RPC/testcases/test_eventloop.cpp > CMakeFiles/app.dir/testcases/test_eventloop.cpp.i

CMakeFiles/app.dir/testcases/test_eventloop.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/app.dir/testcases/test_eventloop.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/xbj/projects/RPC/testcases/test_eventloop.cpp -o CMakeFiles/app.dir/testcases/test_eventloop.cpp.s

CMakeFiles/app.dir/testcases/test_log.cpp.o: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/testcases/test_log.cpp.o: /home/xbj/projects/RPC/testcases/test_log.cpp
CMakeFiles/app.dir/testcases/test_log.cpp.o: CMakeFiles/app.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/xbj/projects/RPC/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building CXX object CMakeFiles/app.dir/testcases/test_log.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/app.dir/testcases/test_log.cpp.o -MF CMakeFiles/app.dir/testcases/test_log.cpp.o.d -o CMakeFiles/app.dir/testcases/test_log.cpp.o -c /home/xbj/projects/RPC/testcases/test_log.cpp

CMakeFiles/app.dir/testcases/test_log.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/app.dir/testcases/test_log.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/xbj/projects/RPC/testcases/test_log.cpp > CMakeFiles/app.dir/testcases/test_log.cpp.i

CMakeFiles/app.dir/testcases/test_log.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/app.dir/testcases/test_log.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/xbj/projects/RPC/testcases/test_log.cpp -o CMakeFiles/app.dir/testcases/test_log.cpp.s

# Object files for target app
app_OBJECTS = \
"CMakeFiles/app.dir/rocket/common/config.cpp.o" \
"CMakeFiles/app.dir/rocket/common/log.cpp.o" \
"CMakeFiles/app.dir/rocket/common/util.cpp.o" \
"CMakeFiles/app.dir/rocket/net/eventloop.cpp.o" \
"CMakeFiles/app.dir/rocket/net/fd_event.cpp.o" \
"CMakeFiles/app.dir/rocket/net/io_thread.cpp.o" \
"CMakeFiles/app.dir/rocket/net/io_thread_group.cpp.o" \
"CMakeFiles/app.dir/rocket/net/timer.cpp.o" \
"CMakeFiles/app.dir/rocket/net/timer_event.cpp.o" \
"CMakeFiles/app.dir/rocket/net/wakeup_fd_event.cpp.o" \
"CMakeFiles/app.dir/testcases/test_eventloop.cpp.o" \
"CMakeFiles/app.dir/testcases/test_log.cpp.o"

# External object files for target app
app_EXTERNAL_OBJECTS =

/home/xbj/projects/RPC/bin/app: CMakeFiles/app.dir/rocket/common/config.cpp.o
/home/xbj/projects/RPC/bin/app: CMakeFiles/app.dir/rocket/common/log.cpp.o
/home/xbj/projects/RPC/bin/app: CMakeFiles/app.dir/rocket/common/util.cpp.o
/home/xbj/projects/RPC/bin/app: CMakeFiles/app.dir/rocket/net/eventloop.cpp.o
/home/xbj/projects/RPC/bin/app: CMakeFiles/app.dir/rocket/net/fd_event.cpp.o
/home/xbj/projects/RPC/bin/app: CMakeFiles/app.dir/rocket/net/io_thread.cpp.o
/home/xbj/projects/RPC/bin/app: CMakeFiles/app.dir/rocket/net/io_thread_group.cpp.o
/home/xbj/projects/RPC/bin/app: CMakeFiles/app.dir/rocket/net/timer.cpp.o
/home/xbj/projects/RPC/bin/app: CMakeFiles/app.dir/rocket/net/timer_event.cpp.o
/home/xbj/projects/RPC/bin/app: CMakeFiles/app.dir/rocket/net/wakeup_fd_event.cpp.o
/home/xbj/projects/RPC/bin/app: CMakeFiles/app.dir/testcases/test_eventloop.cpp.o
/home/xbj/projects/RPC/bin/app: CMakeFiles/app.dir/testcases/test_log.cpp.o
/home/xbj/projects/RPC/bin/app: CMakeFiles/app.dir/build.make
/home/xbj/projects/RPC/bin/app: CMakeFiles/app.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/xbj/projects/RPC/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Linking CXX executable /home/xbj/projects/RPC/bin/app"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/app.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/app.dir/build: /home/xbj/projects/RPC/bin/app
.PHONY : CMakeFiles/app.dir/build

CMakeFiles/app.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/app.dir/cmake_clean.cmake
.PHONY : CMakeFiles/app.dir/clean

CMakeFiles/app.dir/depend:
	cd /home/xbj/projects/RPC/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/xbj/projects/RPC /home/xbj/projects/RPC /home/xbj/projects/RPC/build /home/xbj/projects/RPC/build /home/xbj/projects/RPC/build/CMakeFiles/app.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/app.dir/depend

