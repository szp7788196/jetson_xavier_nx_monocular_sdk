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
CMAKE_SOURCE_DIR = /home/szp/work/nvidia/applications/monocular_sdk

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/szp/work/nvidia/applications/monocular_sdk/build

# Include any dependencies generated for this target.
include CMakeFiles/monocular.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/monocular.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/monocular.dir/flags.make

CMakeFiles/monocular.dir/cmd_parse/cmd_parse.c.o: CMakeFiles/monocular.dir/flags.make
CMakeFiles/monocular.dir/cmd_parse/cmd_parse.c.o: ../cmd_parse/cmd_parse.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/szp/work/nvidia/applications/monocular_sdk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/monocular.dir/cmd_parse/cmd_parse.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/monocular.dir/cmd_parse/cmd_parse.c.o   -c /home/szp/work/nvidia/applications/monocular_sdk/cmd_parse/cmd_parse.c

CMakeFiles/monocular.dir/cmd_parse/cmd_parse.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/monocular.dir/cmd_parse/cmd_parse.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/szp/work/nvidia/applications/monocular_sdk/cmd_parse/cmd_parse.c > CMakeFiles/monocular.dir/cmd_parse/cmd_parse.c.i

CMakeFiles/monocular.dir/cmd_parse/cmd_parse.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/monocular.dir/cmd_parse/cmd_parse.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/szp/work/nvidia/applications/monocular_sdk/cmd_parse/cmd_parse.c -o CMakeFiles/monocular.dir/cmd_parse/cmd_parse.c.s

CMakeFiles/monocular.dir/cmd_parse/cmd_parse.c.o.requires:

.PHONY : CMakeFiles/monocular.dir/cmd_parse/cmd_parse.c.o.requires

CMakeFiles/monocular.dir/cmd_parse/cmd_parse.c.o.provides: CMakeFiles/monocular.dir/cmd_parse/cmd_parse.c.o.requires
	$(MAKE) -f CMakeFiles/monocular.dir/build.make CMakeFiles/monocular.dir/cmd_parse/cmd_parse.c.o.provides.build
.PHONY : CMakeFiles/monocular.dir/cmd_parse/cmd_parse.c.o.provides

CMakeFiles/monocular.dir/cmd_parse/cmd_parse.c.o.provides.build: CMakeFiles/monocular.dir/cmd_parse/cmd_parse.c.o


CMakeFiles/monocular.dir/cssc132/cssc132.c.o: CMakeFiles/monocular.dir/flags.make
CMakeFiles/monocular.dir/cssc132/cssc132.c.o: ../cssc132/cssc132.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/szp/work/nvidia/applications/monocular_sdk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/monocular.dir/cssc132/cssc132.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/monocular.dir/cssc132/cssc132.c.o   -c /home/szp/work/nvidia/applications/monocular_sdk/cssc132/cssc132.c

CMakeFiles/monocular.dir/cssc132/cssc132.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/monocular.dir/cssc132/cssc132.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/szp/work/nvidia/applications/monocular_sdk/cssc132/cssc132.c > CMakeFiles/monocular.dir/cssc132/cssc132.c.i

CMakeFiles/monocular.dir/cssc132/cssc132.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/monocular.dir/cssc132/cssc132.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/szp/work/nvidia/applications/monocular_sdk/cssc132/cssc132.c -o CMakeFiles/monocular.dir/cssc132/cssc132.c.s

CMakeFiles/monocular.dir/cssc132/cssc132.c.o.requires:

.PHONY : CMakeFiles/monocular.dir/cssc132/cssc132.c.o.requires

CMakeFiles/monocular.dir/cssc132/cssc132.c.o.provides: CMakeFiles/monocular.dir/cssc132/cssc132.c.o.requires
	$(MAKE) -f CMakeFiles/monocular.dir/build.make CMakeFiles/monocular.dir/cssc132/cssc132.c.o.provides.build
.PHONY : CMakeFiles/monocular.dir/cssc132/cssc132.c.o.provides

CMakeFiles/monocular.dir/cssc132/cssc132.c.o.provides.build: CMakeFiles/monocular.dir/cssc132/cssc132.c.o


CMakeFiles/monocular.dir/handler/handler.c.o: CMakeFiles/monocular.dir/flags.make
CMakeFiles/monocular.dir/handler/handler.c.o: ../handler/handler.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/szp/work/nvidia/applications/monocular_sdk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/monocular.dir/handler/handler.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/monocular.dir/handler/handler.c.o   -c /home/szp/work/nvidia/applications/monocular_sdk/handler/handler.c

CMakeFiles/monocular.dir/handler/handler.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/monocular.dir/handler/handler.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/szp/work/nvidia/applications/monocular_sdk/handler/handler.c > CMakeFiles/monocular.dir/handler/handler.c.i

CMakeFiles/monocular.dir/handler/handler.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/monocular.dir/handler/handler.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/szp/work/nvidia/applications/monocular_sdk/handler/handler.c -o CMakeFiles/monocular.dir/handler/handler.c.s

CMakeFiles/monocular.dir/handler/handler.c.o.requires:

.PHONY : CMakeFiles/monocular.dir/handler/handler.c.o.requires

CMakeFiles/monocular.dir/handler/handler.c.o.provides: CMakeFiles/monocular.dir/handler/handler.c.o.requires
	$(MAKE) -f CMakeFiles/monocular.dir/build.make CMakeFiles/monocular.dir/handler/handler.c.o.provides.build
.PHONY : CMakeFiles/monocular.dir/handler/handler.c.o.provides

CMakeFiles/monocular.dir/handler/handler.c.o.provides.build: CMakeFiles/monocular.dir/handler/handler.c.o


CMakeFiles/monocular.dir/led/led.c.o: CMakeFiles/monocular.dir/flags.make
CMakeFiles/monocular.dir/led/led.c.o: ../led/led.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/szp/work/nvidia/applications/monocular_sdk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/monocular.dir/led/led.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/monocular.dir/led/led.c.o   -c /home/szp/work/nvidia/applications/monocular_sdk/led/led.c

CMakeFiles/monocular.dir/led/led.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/monocular.dir/led/led.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/szp/work/nvidia/applications/monocular_sdk/led/led.c > CMakeFiles/monocular.dir/led/led.c.i

CMakeFiles/monocular.dir/led/led.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/monocular.dir/led/led.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/szp/work/nvidia/applications/monocular_sdk/led/led.c -o CMakeFiles/monocular.dir/led/led.c.s

CMakeFiles/monocular.dir/led/led.c.o.requires:

.PHONY : CMakeFiles/monocular.dir/led/led.c.o.requires

CMakeFiles/monocular.dir/led/led.c.o.provides: CMakeFiles/monocular.dir/led/led.c.o.requires
	$(MAKE) -f CMakeFiles/monocular.dir/build.make CMakeFiles/monocular.dir/led/led.c.o.provides.build
.PHONY : CMakeFiles/monocular.dir/led/led.c.o.provides

CMakeFiles/monocular.dir/led/led.c.o.provides.build: CMakeFiles/monocular.dir/led/led.c.o


CMakeFiles/monocular.dir/monocular/monocular.c.o: CMakeFiles/monocular.dir/flags.make
CMakeFiles/monocular.dir/monocular/monocular.c.o: ../monocular/monocular.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/szp/work/nvidia/applications/monocular_sdk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/monocular.dir/monocular/monocular.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/monocular.dir/monocular/monocular.c.o   -c /home/szp/work/nvidia/applications/monocular_sdk/monocular/monocular.c

CMakeFiles/monocular.dir/monocular/monocular.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/monocular.dir/monocular/monocular.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/szp/work/nvidia/applications/monocular_sdk/monocular/monocular.c > CMakeFiles/monocular.dir/monocular/monocular.c.i

CMakeFiles/monocular.dir/monocular/monocular.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/monocular.dir/monocular/monocular.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/szp/work/nvidia/applications/monocular_sdk/monocular/monocular.c -o CMakeFiles/monocular.dir/monocular/monocular.c.s

CMakeFiles/monocular.dir/monocular/monocular.c.o.requires:

.PHONY : CMakeFiles/monocular.dir/monocular/monocular.c.o.requires

CMakeFiles/monocular.dir/monocular/monocular.c.o.provides: CMakeFiles/monocular.dir/monocular/monocular.c.o.requires
	$(MAKE) -f CMakeFiles/monocular.dir/build.make CMakeFiles/monocular.dir/monocular/monocular.c.o.provides.build
.PHONY : CMakeFiles/monocular.dir/monocular/monocular.c.o.provides

CMakeFiles/monocular.dir/monocular/monocular.c.o.provides.build: CMakeFiles/monocular.dir/monocular/monocular.c.o


CMakeFiles/monocular.dir/mpu9250/mpu9250.c.o: CMakeFiles/monocular.dir/flags.make
CMakeFiles/monocular.dir/mpu9250/mpu9250.c.o: ../mpu9250/mpu9250.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/szp/work/nvidia/applications/monocular_sdk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object CMakeFiles/monocular.dir/mpu9250/mpu9250.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/monocular.dir/mpu9250/mpu9250.c.o   -c /home/szp/work/nvidia/applications/monocular_sdk/mpu9250/mpu9250.c

CMakeFiles/monocular.dir/mpu9250/mpu9250.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/monocular.dir/mpu9250/mpu9250.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/szp/work/nvidia/applications/monocular_sdk/mpu9250/mpu9250.c > CMakeFiles/monocular.dir/mpu9250/mpu9250.c.i

CMakeFiles/monocular.dir/mpu9250/mpu9250.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/monocular.dir/mpu9250/mpu9250.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/szp/work/nvidia/applications/monocular_sdk/mpu9250/mpu9250.c -o CMakeFiles/monocular.dir/mpu9250/mpu9250.c.s

CMakeFiles/monocular.dir/mpu9250/mpu9250.c.o.requires:

.PHONY : CMakeFiles/monocular.dir/mpu9250/mpu9250.c.o.requires

CMakeFiles/monocular.dir/mpu9250/mpu9250.c.o.provides: CMakeFiles/monocular.dir/mpu9250/mpu9250.c.o.requires
	$(MAKE) -f CMakeFiles/monocular.dir/build.make CMakeFiles/monocular.dir/mpu9250/mpu9250.c.o.provides.build
.PHONY : CMakeFiles/monocular.dir/mpu9250/mpu9250.c.o.provides

CMakeFiles/monocular.dir/mpu9250/mpu9250.c.o.provides.build: CMakeFiles/monocular.dir/mpu9250/mpu9250.c.o


CMakeFiles/monocular.dir/net/net.c.o: CMakeFiles/monocular.dir/flags.make
CMakeFiles/monocular.dir/net/net.c.o: ../net/net.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/szp/work/nvidia/applications/monocular_sdk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object CMakeFiles/monocular.dir/net/net.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/monocular.dir/net/net.c.o   -c /home/szp/work/nvidia/applications/monocular_sdk/net/net.c

CMakeFiles/monocular.dir/net/net.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/monocular.dir/net/net.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/szp/work/nvidia/applications/monocular_sdk/net/net.c > CMakeFiles/monocular.dir/net/net.c.i

CMakeFiles/monocular.dir/net/net.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/monocular.dir/net/net.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/szp/work/nvidia/applications/monocular_sdk/net/net.c -o CMakeFiles/monocular.dir/net/net.c.s

CMakeFiles/monocular.dir/net/net.c.o.requires:

.PHONY : CMakeFiles/monocular.dir/net/net.c.o.requires

CMakeFiles/monocular.dir/net/net.c.o.provides: CMakeFiles/monocular.dir/net/net.c.o.requires
	$(MAKE) -f CMakeFiles/monocular.dir/build.make CMakeFiles/monocular.dir/net/net.c.o.provides.build
.PHONY : CMakeFiles/monocular.dir/net/net.c.o.provides

CMakeFiles/monocular.dir/net/net.c.o.provides.build: CMakeFiles/monocular.dir/net/net.c.o


CMakeFiles/monocular.dir/serial/serial.c.o: CMakeFiles/monocular.dir/flags.make
CMakeFiles/monocular.dir/serial/serial.c.o: ../serial/serial.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/szp/work/nvidia/applications/monocular_sdk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building C object CMakeFiles/monocular.dir/serial/serial.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/monocular.dir/serial/serial.c.o   -c /home/szp/work/nvidia/applications/monocular_sdk/serial/serial.c

CMakeFiles/monocular.dir/serial/serial.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/monocular.dir/serial/serial.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/szp/work/nvidia/applications/monocular_sdk/serial/serial.c > CMakeFiles/monocular.dir/serial/serial.c.i

CMakeFiles/monocular.dir/serial/serial.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/monocular.dir/serial/serial.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/szp/work/nvidia/applications/monocular_sdk/serial/serial.c -o CMakeFiles/monocular.dir/serial/serial.c.s

CMakeFiles/monocular.dir/serial/serial.c.o.requires:

.PHONY : CMakeFiles/monocular.dir/serial/serial.c.o.requires

CMakeFiles/monocular.dir/serial/serial.c.o.provides: CMakeFiles/monocular.dir/serial/serial.c.o.requires
	$(MAKE) -f CMakeFiles/monocular.dir/build.make CMakeFiles/monocular.dir/serial/serial.c.o.provides.build
.PHONY : CMakeFiles/monocular.dir/serial/serial.c.o.provides

CMakeFiles/monocular.dir/serial/serial.c.o.provides.build: CMakeFiles/monocular.dir/serial/serial.c.o


CMakeFiles/monocular.dir/sync/sync.c.o: CMakeFiles/monocular.dir/flags.make
CMakeFiles/monocular.dir/sync/sync.c.o: ../sync/sync.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/szp/work/nvidia/applications/monocular_sdk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building C object CMakeFiles/monocular.dir/sync/sync.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/monocular.dir/sync/sync.c.o   -c /home/szp/work/nvidia/applications/monocular_sdk/sync/sync.c

CMakeFiles/monocular.dir/sync/sync.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/monocular.dir/sync/sync.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/szp/work/nvidia/applications/monocular_sdk/sync/sync.c > CMakeFiles/monocular.dir/sync/sync.c.i

CMakeFiles/monocular.dir/sync/sync.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/monocular.dir/sync/sync.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/szp/work/nvidia/applications/monocular_sdk/sync/sync.c -o CMakeFiles/monocular.dir/sync/sync.c.s

CMakeFiles/monocular.dir/sync/sync.c.o.requires:

.PHONY : CMakeFiles/monocular.dir/sync/sync.c.o.requires

CMakeFiles/monocular.dir/sync/sync.c.o.provides: CMakeFiles/monocular.dir/sync/sync.c.o.requires
	$(MAKE) -f CMakeFiles/monocular.dir/build.make CMakeFiles/monocular.dir/sync/sync.c.o.provides.build
.PHONY : CMakeFiles/monocular.dir/sync/sync.c.o.provides

CMakeFiles/monocular.dir/sync/sync.c.o.provides.build: CMakeFiles/monocular.dir/sync/sync.c.o


CMakeFiles/monocular.dir/sync_module/sync_module.c.o: CMakeFiles/monocular.dir/flags.make
CMakeFiles/monocular.dir/sync_module/sync_module.c.o: ../sync_module/sync_module.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/szp/work/nvidia/applications/monocular_sdk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building C object CMakeFiles/monocular.dir/sync_module/sync_module.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/monocular.dir/sync_module/sync_module.c.o   -c /home/szp/work/nvidia/applications/monocular_sdk/sync_module/sync_module.c

CMakeFiles/monocular.dir/sync_module/sync_module.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/monocular.dir/sync_module/sync_module.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/szp/work/nvidia/applications/monocular_sdk/sync_module/sync_module.c > CMakeFiles/monocular.dir/sync_module/sync_module.c.i

CMakeFiles/monocular.dir/sync_module/sync_module.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/monocular.dir/sync_module/sync_module.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/szp/work/nvidia/applications/monocular_sdk/sync_module/sync_module.c -o CMakeFiles/monocular.dir/sync_module/sync_module.c.s

CMakeFiles/monocular.dir/sync_module/sync_module.c.o.requires:

.PHONY : CMakeFiles/monocular.dir/sync_module/sync_module.c.o.requires

CMakeFiles/monocular.dir/sync_module/sync_module.c.o.provides: CMakeFiles/monocular.dir/sync_module/sync_module.c.o.requires
	$(MAKE) -f CMakeFiles/monocular.dir/build.make CMakeFiles/monocular.dir/sync_module/sync_module.c.o.provides.build
.PHONY : CMakeFiles/monocular.dir/sync_module/sync_module.c.o.provides

CMakeFiles/monocular.dir/sync_module/sync_module.c.o.provides.build: CMakeFiles/monocular.dir/sync_module/sync_module.c.o


CMakeFiles/monocular.dir/ub482/ub482.c.o: CMakeFiles/monocular.dir/flags.make
CMakeFiles/monocular.dir/ub482/ub482.c.o: ../ub482/ub482.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/szp/work/nvidia/applications/monocular_sdk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building C object CMakeFiles/monocular.dir/ub482/ub482.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/monocular.dir/ub482/ub482.c.o   -c /home/szp/work/nvidia/applications/monocular_sdk/ub482/ub482.c

CMakeFiles/monocular.dir/ub482/ub482.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/monocular.dir/ub482/ub482.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/szp/work/nvidia/applications/monocular_sdk/ub482/ub482.c > CMakeFiles/monocular.dir/ub482/ub482.c.i

CMakeFiles/monocular.dir/ub482/ub482.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/monocular.dir/ub482/ub482.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/szp/work/nvidia/applications/monocular_sdk/ub482/ub482.c -o CMakeFiles/monocular.dir/ub482/ub482.c.s

CMakeFiles/monocular.dir/ub482/ub482.c.o.requires:

.PHONY : CMakeFiles/monocular.dir/ub482/ub482.c.o.requires

CMakeFiles/monocular.dir/ub482/ub482.c.o.provides: CMakeFiles/monocular.dir/ub482/ub482.c.o.requires
	$(MAKE) -f CMakeFiles/monocular.dir/build.make CMakeFiles/monocular.dir/ub482/ub482.c.o.provides.build
.PHONY : CMakeFiles/monocular.dir/ub482/ub482.c.o.provides

CMakeFiles/monocular.dir/ub482/ub482.c.o.provides.build: CMakeFiles/monocular.dir/ub482/ub482.c.o


CMakeFiles/monocular.dir/ui3240/ui3240.c.o: CMakeFiles/monocular.dir/flags.make
CMakeFiles/monocular.dir/ui3240/ui3240.c.o: ../ui3240/ui3240.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/szp/work/nvidia/applications/monocular_sdk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building C object CMakeFiles/monocular.dir/ui3240/ui3240.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/monocular.dir/ui3240/ui3240.c.o   -c /home/szp/work/nvidia/applications/monocular_sdk/ui3240/ui3240.c

CMakeFiles/monocular.dir/ui3240/ui3240.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/monocular.dir/ui3240/ui3240.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/szp/work/nvidia/applications/monocular_sdk/ui3240/ui3240.c > CMakeFiles/monocular.dir/ui3240/ui3240.c.i

CMakeFiles/monocular.dir/ui3240/ui3240.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/monocular.dir/ui3240/ui3240.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/szp/work/nvidia/applications/monocular_sdk/ui3240/ui3240.c -o CMakeFiles/monocular.dir/ui3240/ui3240.c.s

CMakeFiles/monocular.dir/ui3240/ui3240.c.o.requires:

.PHONY : CMakeFiles/monocular.dir/ui3240/ui3240.c.o.requires

CMakeFiles/monocular.dir/ui3240/ui3240.c.o.provides: CMakeFiles/monocular.dir/ui3240/ui3240.c.o.requires
	$(MAKE) -f CMakeFiles/monocular.dir/build.make CMakeFiles/monocular.dir/ui3240/ui3240.c.o.provides.build
.PHONY : CMakeFiles/monocular.dir/ui3240/ui3240.c.o.provides

CMakeFiles/monocular.dir/ui3240/ui3240.c.o.provides.build: CMakeFiles/monocular.dir/ui3240/ui3240.c.o


CMakeFiles/monocular.dir/fifo/ringbuf.c.o: CMakeFiles/monocular.dir/flags.make
CMakeFiles/monocular.dir/fifo/ringbuf.c.o: ../fifo/ringbuf.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/szp/work/nvidia/applications/monocular_sdk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building C object CMakeFiles/monocular.dir/fifo/ringbuf.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/monocular.dir/fifo/ringbuf.c.o   -c /home/szp/work/nvidia/applications/monocular_sdk/fifo/ringbuf.c

CMakeFiles/monocular.dir/fifo/ringbuf.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/monocular.dir/fifo/ringbuf.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/szp/work/nvidia/applications/monocular_sdk/fifo/ringbuf.c > CMakeFiles/monocular.dir/fifo/ringbuf.c.i

CMakeFiles/monocular.dir/fifo/ringbuf.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/monocular.dir/fifo/ringbuf.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/szp/work/nvidia/applications/monocular_sdk/fifo/ringbuf.c -o CMakeFiles/monocular.dir/fifo/ringbuf.c.s

CMakeFiles/monocular.dir/fifo/ringbuf.c.o.requires:

.PHONY : CMakeFiles/monocular.dir/fifo/ringbuf.c.o.requires

CMakeFiles/monocular.dir/fifo/ringbuf.c.o.provides: CMakeFiles/monocular.dir/fifo/ringbuf.c.o.requires
	$(MAKE) -f CMakeFiles/monocular.dir/build.make CMakeFiles/monocular.dir/fifo/ringbuf.c.o.provides.build
.PHONY : CMakeFiles/monocular.dir/fifo/ringbuf.c.o.provides

CMakeFiles/monocular.dir/fifo/ringbuf.c.o.provides.build: CMakeFiles/monocular.dir/fifo/ringbuf.c.o


# Object files for target monocular
monocular_OBJECTS = \
"CMakeFiles/monocular.dir/cmd_parse/cmd_parse.c.o" \
"CMakeFiles/monocular.dir/cssc132/cssc132.c.o" \
"CMakeFiles/monocular.dir/handler/handler.c.o" \
"CMakeFiles/monocular.dir/led/led.c.o" \
"CMakeFiles/monocular.dir/monocular/monocular.c.o" \
"CMakeFiles/monocular.dir/mpu9250/mpu9250.c.o" \
"CMakeFiles/monocular.dir/net/net.c.o" \
"CMakeFiles/monocular.dir/serial/serial.c.o" \
"CMakeFiles/monocular.dir/sync/sync.c.o" \
"CMakeFiles/monocular.dir/sync_module/sync_module.c.o" \
"CMakeFiles/monocular.dir/ub482/ub482.c.o" \
"CMakeFiles/monocular.dir/ui3240/ui3240.c.o" \
"CMakeFiles/monocular.dir/fifo/ringbuf.c.o"

# External object files for target monocular
monocular_EXTERNAL_OBJECTS =

libmonocular.so: CMakeFiles/monocular.dir/cmd_parse/cmd_parse.c.o
libmonocular.so: CMakeFiles/monocular.dir/cssc132/cssc132.c.o
libmonocular.so: CMakeFiles/monocular.dir/handler/handler.c.o
libmonocular.so: CMakeFiles/monocular.dir/led/led.c.o
libmonocular.so: CMakeFiles/monocular.dir/monocular/monocular.c.o
libmonocular.so: CMakeFiles/monocular.dir/mpu9250/mpu9250.c.o
libmonocular.so: CMakeFiles/monocular.dir/net/net.c.o
libmonocular.so: CMakeFiles/monocular.dir/serial/serial.c.o
libmonocular.so: CMakeFiles/monocular.dir/sync/sync.c.o
libmonocular.so: CMakeFiles/monocular.dir/sync_module/sync_module.c.o
libmonocular.so: CMakeFiles/monocular.dir/ub482/ub482.c.o
libmonocular.so: CMakeFiles/monocular.dir/ui3240/ui3240.c.o
libmonocular.so: CMakeFiles/monocular.dir/fifo/ringbuf.c.o
libmonocular.so: CMakeFiles/monocular.dir/build.make
libmonocular.so: CMakeFiles/monocular.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/szp/work/nvidia/applications/monocular_sdk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Linking C shared library libmonocular.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/monocular.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/monocular.dir/build: libmonocular.so

.PHONY : CMakeFiles/monocular.dir/build

CMakeFiles/monocular.dir/requires: CMakeFiles/monocular.dir/cmd_parse/cmd_parse.c.o.requires
CMakeFiles/monocular.dir/requires: CMakeFiles/monocular.dir/cssc132/cssc132.c.o.requires
CMakeFiles/monocular.dir/requires: CMakeFiles/monocular.dir/handler/handler.c.o.requires
CMakeFiles/monocular.dir/requires: CMakeFiles/monocular.dir/led/led.c.o.requires
CMakeFiles/monocular.dir/requires: CMakeFiles/monocular.dir/monocular/monocular.c.o.requires
CMakeFiles/monocular.dir/requires: CMakeFiles/monocular.dir/mpu9250/mpu9250.c.o.requires
CMakeFiles/monocular.dir/requires: CMakeFiles/monocular.dir/net/net.c.o.requires
CMakeFiles/monocular.dir/requires: CMakeFiles/monocular.dir/serial/serial.c.o.requires
CMakeFiles/monocular.dir/requires: CMakeFiles/monocular.dir/sync/sync.c.o.requires
CMakeFiles/monocular.dir/requires: CMakeFiles/monocular.dir/sync_module/sync_module.c.o.requires
CMakeFiles/monocular.dir/requires: CMakeFiles/monocular.dir/ub482/ub482.c.o.requires
CMakeFiles/monocular.dir/requires: CMakeFiles/monocular.dir/ui3240/ui3240.c.o.requires
CMakeFiles/monocular.dir/requires: CMakeFiles/monocular.dir/fifo/ringbuf.c.o.requires

.PHONY : CMakeFiles/monocular.dir/requires

CMakeFiles/monocular.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/monocular.dir/cmake_clean.cmake
.PHONY : CMakeFiles/monocular.dir/clean

CMakeFiles/monocular.dir/depend:
	cd /home/szp/work/nvidia/applications/monocular_sdk/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/szp/work/nvidia/applications/monocular_sdk /home/szp/work/nvidia/applications/monocular_sdk /home/szp/work/nvidia/applications/monocular_sdk/build /home/szp/work/nvidia/applications/monocular_sdk/build /home/szp/work/nvidia/applications/monocular_sdk/build/CMakeFiles/monocular.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/monocular.dir/depend

