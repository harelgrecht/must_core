# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_SOURCE_DIR = /home/user/Desktop/Projects/must_core

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/user/Desktop/Projects/must_core/build/build

# Include any dependencies generated for this target.
include CMakeFiles/MustV2Software.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/MustV2Software.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/MustV2Software.dir/flags.make

CMakeFiles/MustV2Software.dir/EthDevice/EthDevice.cpp.o: CMakeFiles/MustV2Software.dir/flags.make
CMakeFiles/MustV2Software.dir/EthDevice/EthDevice.cpp.o: ../../EthDevice/EthDevice.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/user/Desktop/Projects/must_core/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/MustV2Software.dir/EthDevice/EthDevice.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/MustV2Software.dir/EthDevice/EthDevice.cpp.o -c /home/user/Desktop/Projects/must_core/EthDevice/EthDevice.cpp

CMakeFiles/MustV2Software.dir/EthDevice/EthDevice.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MustV2Software.dir/EthDevice/EthDevice.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/user/Desktop/Projects/must_core/EthDevice/EthDevice.cpp > CMakeFiles/MustV2Software.dir/EthDevice/EthDevice.cpp.i

CMakeFiles/MustV2Software.dir/EthDevice/EthDevice.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MustV2Software.dir/EthDevice/EthDevice.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/user/Desktop/Projects/must_core/EthDevice/EthDevice.cpp -o CMakeFiles/MustV2Software.dir/EthDevice/EthDevice.cpp.s

CMakeFiles/MustV2Software.dir/GPIOHandler/gpioHandler.cpp.o: CMakeFiles/MustV2Software.dir/flags.make
CMakeFiles/MustV2Software.dir/GPIOHandler/gpioHandler.cpp.o: ../../GPIOHandler/gpioHandler.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/user/Desktop/Projects/must_core/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/MustV2Software.dir/GPIOHandler/gpioHandler.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/MustV2Software.dir/GPIOHandler/gpioHandler.cpp.o -c /home/user/Desktop/Projects/must_core/GPIOHandler/gpioHandler.cpp

CMakeFiles/MustV2Software.dir/GPIOHandler/gpioHandler.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MustV2Software.dir/GPIOHandler/gpioHandler.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/user/Desktop/Projects/must_core/GPIOHandler/gpioHandler.cpp > CMakeFiles/MustV2Software.dir/GPIOHandler/gpioHandler.cpp.i

CMakeFiles/MustV2Software.dir/GPIOHandler/gpioHandler.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MustV2Software.dir/GPIOHandler/gpioHandler.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/user/Desktop/Projects/must_core/GPIOHandler/gpioHandler.cpp -o CMakeFiles/MustV2Software.dir/GPIOHandler/gpioHandler.cpp.s

CMakeFiles/MustV2Software.dir/LogHandler/logger.cpp.o: CMakeFiles/MustV2Software.dir/flags.make
CMakeFiles/MustV2Software.dir/LogHandler/logger.cpp.o: ../../LogHandler/logger.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/user/Desktop/Projects/must_core/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/MustV2Software.dir/LogHandler/logger.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/MustV2Software.dir/LogHandler/logger.cpp.o -c /home/user/Desktop/Projects/must_core/LogHandler/logger.cpp

CMakeFiles/MustV2Software.dir/LogHandler/logger.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MustV2Software.dir/LogHandler/logger.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/user/Desktop/Projects/must_core/LogHandler/logger.cpp > CMakeFiles/MustV2Software.dir/LogHandler/logger.cpp.i

CMakeFiles/MustV2Software.dir/LogHandler/logger.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MustV2Software.dir/LogHandler/logger.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/user/Desktop/Projects/must_core/LogHandler/logger.cpp -o CMakeFiles/MustV2Software.dir/LogHandler/logger.cpp.s

CMakeFiles/MustV2Software.dir/NetworkManager/NetworkManager.cpp.o: CMakeFiles/MustV2Software.dir/flags.make
CMakeFiles/MustV2Software.dir/NetworkManager/NetworkManager.cpp.o: ../../NetworkManager/NetworkManager.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/user/Desktop/Projects/must_core/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/MustV2Software.dir/NetworkManager/NetworkManager.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/MustV2Software.dir/NetworkManager/NetworkManager.cpp.o -c /home/user/Desktop/Projects/must_core/NetworkManager/NetworkManager.cpp

CMakeFiles/MustV2Software.dir/NetworkManager/NetworkManager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MustV2Software.dir/NetworkManager/NetworkManager.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/user/Desktop/Projects/must_core/NetworkManager/NetworkManager.cpp > CMakeFiles/MustV2Software.dir/NetworkManager/NetworkManager.cpp.i

CMakeFiles/MustV2Software.dir/NetworkManager/NetworkManager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MustV2Software.dir/NetworkManager/NetworkManager.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/user/Desktop/Projects/must_core/NetworkManager/NetworkManager.cpp -o CMakeFiles/MustV2Software.dir/NetworkManager/NetworkManager.cpp.s

CMakeFiles/MustV2Software.dir/PacketAuthentication/packet_authenticator.cpp.o: CMakeFiles/MustV2Software.dir/flags.make
CMakeFiles/MustV2Software.dir/PacketAuthentication/packet_authenticator.cpp.o: ../../PacketAuthentication/packet_authenticator.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/user/Desktop/Projects/must_core/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/MustV2Software.dir/PacketAuthentication/packet_authenticator.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/MustV2Software.dir/PacketAuthentication/packet_authenticator.cpp.o -c /home/user/Desktop/Projects/must_core/PacketAuthentication/packet_authenticator.cpp

CMakeFiles/MustV2Software.dir/PacketAuthentication/packet_authenticator.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MustV2Software.dir/PacketAuthentication/packet_authenticator.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/user/Desktop/Projects/must_core/PacketAuthentication/packet_authenticator.cpp > CMakeFiles/MustV2Software.dir/PacketAuthentication/packet_authenticator.cpp.i

CMakeFiles/MustV2Software.dir/PacketAuthentication/packet_authenticator.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MustV2Software.dir/PacketAuthentication/packet_authenticator.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/user/Desktop/Projects/must_core/PacketAuthentication/packet_authenticator.cpp -o CMakeFiles/MustV2Software.dir/PacketAuthentication/packet_authenticator.cpp.s

CMakeFiles/MustV2Software.dir/RealTimeThread/real_time_thread.cpp.o: CMakeFiles/MustV2Software.dir/flags.make
CMakeFiles/MustV2Software.dir/RealTimeThread/real_time_thread.cpp.o: ../../RealTimeThread/real_time_thread.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/user/Desktop/Projects/must_core/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/MustV2Software.dir/RealTimeThread/real_time_thread.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/MustV2Software.dir/RealTimeThread/real_time_thread.cpp.o -c /home/user/Desktop/Projects/must_core/RealTimeThread/real_time_thread.cpp

CMakeFiles/MustV2Software.dir/RealTimeThread/real_time_thread.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MustV2Software.dir/RealTimeThread/real_time_thread.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/user/Desktop/Projects/must_core/RealTimeThread/real_time_thread.cpp > CMakeFiles/MustV2Software.dir/RealTimeThread/real_time_thread.cpp.i

CMakeFiles/MustV2Software.dir/RealTimeThread/real_time_thread.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MustV2Software.dir/RealTimeThread/real_time_thread.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/user/Desktop/Projects/must_core/RealTimeThread/real_time_thread.cpp -o CMakeFiles/MustV2Software.dir/RealTimeThread/real_time_thread.cpp.s

CMakeFiles/MustV2Software.dir/SelfSearchConfig/SelfSearchConfig.cpp.o: CMakeFiles/MustV2Software.dir/flags.make
CMakeFiles/MustV2Software.dir/SelfSearchConfig/SelfSearchConfig.cpp.o: ../../SelfSearchConfig/SelfSearchConfig.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/user/Desktop/Projects/must_core/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/MustV2Software.dir/SelfSearchConfig/SelfSearchConfig.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/MustV2Software.dir/SelfSearchConfig/SelfSearchConfig.cpp.o -c /home/user/Desktop/Projects/must_core/SelfSearchConfig/SelfSearchConfig.cpp

CMakeFiles/MustV2Software.dir/SelfSearchConfig/SelfSearchConfig.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MustV2Software.dir/SelfSearchConfig/SelfSearchConfig.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/user/Desktop/Projects/must_core/SelfSearchConfig/SelfSearchConfig.cpp > CMakeFiles/MustV2Software.dir/SelfSearchConfig/SelfSearchConfig.cpp.i

CMakeFiles/MustV2Software.dir/SelfSearchConfig/SelfSearchConfig.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MustV2Software.dir/SelfSearchConfig/SelfSearchConfig.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/user/Desktop/Projects/must_core/SelfSearchConfig/SelfSearchConfig.cpp -o CMakeFiles/MustV2Software.dir/SelfSearchConfig/SelfSearchConfig.cpp.s

CMakeFiles/MustV2Software.dir/UART/UARTHandler.cpp.o: CMakeFiles/MustV2Software.dir/flags.make
CMakeFiles/MustV2Software.dir/UART/UARTHandler.cpp.o: ../../UART/UARTHandler.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/user/Desktop/Projects/must_core/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/MustV2Software.dir/UART/UARTHandler.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/MustV2Software.dir/UART/UARTHandler.cpp.o -c /home/user/Desktop/Projects/must_core/UART/UARTHandler.cpp

CMakeFiles/MustV2Software.dir/UART/UARTHandler.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MustV2Software.dir/UART/UARTHandler.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/user/Desktop/Projects/must_core/UART/UARTHandler.cpp > CMakeFiles/MustV2Software.dir/UART/UARTHandler.cpp.i

CMakeFiles/MustV2Software.dir/UART/UARTHandler.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MustV2Software.dir/UART/UARTHandler.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/user/Desktop/Projects/must_core/UART/UARTHandler.cpp -o CMakeFiles/MustV2Software.dir/UART/UARTHandler.cpp.s

CMakeFiles/MustV2Software.dir/main.cpp.o: CMakeFiles/MustV2Software.dir/flags.make
CMakeFiles/MustV2Software.dir/main.cpp.o: ../../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/user/Desktop/Projects/must_core/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/MustV2Software.dir/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/MustV2Software.dir/main.cpp.o -c /home/user/Desktop/Projects/must_core/main.cpp

CMakeFiles/MustV2Software.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MustV2Software.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/user/Desktop/Projects/must_core/main.cpp > CMakeFiles/MustV2Software.dir/main.cpp.i

CMakeFiles/MustV2Software.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MustV2Software.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/user/Desktop/Projects/must_core/main.cpp -o CMakeFiles/MustV2Software.dir/main.cpp.s

# Object files for target MustV2Software
MustV2Software_OBJECTS = \
"CMakeFiles/MustV2Software.dir/EthDevice/EthDevice.cpp.o" \
"CMakeFiles/MustV2Software.dir/GPIOHandler/gpioHandler.cpp.o" \
"CMakeFiles/MustV2Software.dir/LogHandler/logger.cpp.o" \
"CMakeFiles/MustV2Software.dir/NetworkManager/NetworkManager.cpp.o" \
"CMakeFiles/MustV2Software.dir/PacketAuthentication/packet_authenticator.cpp.o" \
"CMakeFiles/MustV2Software.dir/RealTimeThread/real_time_thread.cpp.o" \
"CMakeFiles/MustV2Software.dir/SelfSearchConfig/SelfSearchConfig.cpp.o" \
"CMakeFiles/MustV2Software.dir/UART/UARTHandler.cpp.o" \
"CMakeFiles/MustV2Software.dir/main.cpp.o"

# External object files for target MustV2Software
MustV2Software_EXTERNAL_OBJECTS =

bin/MustV2Software: CMakeFiles/MustV2Software.dir/EthDevice/EthDevice.cpp.o
bin/MustV2Software: CMakeFiles/MustV2Software.dir/GPIOHandler/gpioHandler.cpp.o
bin/MustV2Software: CMakeFiles/MustV2Software.dir/LogHandler/logger.cpp.o
bin/MustV2Software: CMakeFiles/MustV2Software.dir/NetworkManager/NetworkManager.cpp.o
bin/MustV2Software: CMakeFiles/MustV2Software.dir/PacketAuthentication/packet_authenticator.cpp.o
bin/MustV2Software: CMakeFiles/MustV2Software.dir/RealTimeThread/real_time_thread.cpp.o
bin/MustV2Software: CMakeFiles/MustV2Software.dir/SelfSearchConfig/SelfSearchConfig.cpp.o
bin/MustV2Software: CMakeFiles/MustV2Software.dir/UART/UARTHandler.cpp.o
bin/MustV2Software: CMakeFiles/MustV2Software.dir/main.cpp.o
bin/MustV2Software: CMakeFiles/MustV2Software.dir/build.make
bin/MustV2Software: CMakeFiles/MustV2Software.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/user/Desktop/Projects/must_core/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Linking CXX executable bin/MustV2Software"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/MustV2Software.dir/link.txt --verbose=$(VERBOSE)
	/usr/bin/cmake -E copy_if_different /home/user/Desktop/Projects/must_core/user_setting.json /home/user/Desktop/Projects/must_core/build/build/bin
	/usr/bin/cmake -E copy_if_different /home/user/Desktop/Projects/must_core/self_search_ips.json /home/user/Desktop/Projects/must_core/build/build/bin

# Rule to build all files generated by this target.
CMakeFiles/MustV2Software.dir/build: bin/MustV2Software

.PHONY : CMakeFiles/MustV2Software.dir/build

CMakeFiles/MustV2Software.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/MustV2Software.dir/cmake_clean.cmake
.PHONY : CMakeFiles/MustV2Software.dir/clean

CMakeFiles/MustV2Software.dir/depend:
	cd /home/user/Desktop/Projects/must_core/build/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/user/Desktop/Projects/must_core /home/user/Desktop/Projects/must_core /home/user/Desktop/Projects/must_core/build/build /home/user/Desktop/Projects/must_core/build/build /home/user/Desktop/Projects/must_core/build/build/CMakeFiles/MustV2Software.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/MustV2Software.dir/depend

