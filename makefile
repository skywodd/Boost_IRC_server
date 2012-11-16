# User friendly makefile
# 
# I really don't known why I've finally make a "good" makefile 
# for this fucking school project but ... I do ...

# ----- Compilation options -----
# Output executable file name
EXECUTABLE := Boost_IRC_server

# Build directory
BUILD_DIR := release

# Boost library root path 
# (if not using the default boost installation of your system)
# Remarks: always use slash (/) and not backslash !
#BOOST_PATH := C:/boost_1_51_0

# Boost libraries suffix 
# (if your using hand compiled Boost libraries)
#LIB_SUFFIX := -mgw47-mt-1_51

# Build options 
# (if you want to compile with custom flags add them here)
# -DDEBUG enable debug output
# -Os enable optimizations, -O0 disable them
# -g enable debug symbols
# -Wall enable all warnings, you should never compile without this one
# -D_WIN32_WINNT=0x0501 allow compilation on windows
# EXAMPLE : Simple release for linux 
BUILD_OPS := -DDEBUG -O0 -g -Wall
# EXAMPLE : Simple release for windows
#BUILD_OPS := -DDEBUG -O0 -g -Wall -D_WIN32_WINNT=0x0501

# Library dependencies 
# (if your Boost port need extra libraries to link properly)
# EXAMPLE : Simple release for windows
#LIB_ADDON := -lMswsock -lWs2_32

# DO NOT EDIT AFTER THIS LINE -------------------------
# Excepted if you known that you make ...
# 
# Trust me, I a jedi

# Libraries dependencies
LIBS := -lboost_system$(LIB_SUFFIX) -lboost_date_time$(LIB_SUFFIX) -lboost_regex$(LIB_SUFFIX) $(LIB_ADDON)

# Boost paths
BOOST_INCLUDE_PATH := -I$(BOOST_PATH)
BOOST_LIB_PATH := -L$(BOOST_PATH)/stage/lib

# Source files
SRCS := $(wildcard *.cpp)
# Object files
OBJS := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRCS))
# Executable
EXECUTABLE := $(addprefix $(BUILD_DIR)/, $(EXECUTABLE))

# Compile c++ files to object files
$(BUILD_DIR)/%.o: %.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ $(BUILD_OPS) $(BOOST_INCLUDE_PATH) -c -fmessage-length=0 -ffunction-sections -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

# Incremential build
all: Boost_IRC_server

# Build all
Boost_IRC_server: $(OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross G++ Linker'
	g++ $(BOOST_LIB_PATH) -Wl,-gc-sections -o "$(EXECUTABLE)" $(OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Clean build directory
clean: 
	rm -rf $(OBJS) $(EXECUTABLE)
	@echo ' '
