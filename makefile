# User friendly makefile

# Boost library path (if not using default boost installation)
#BOOST_PATH = C:/boost_1_51_0

# Boost libraries suffix (if any)
#LIB_SUFFIX = -mgw47-mt-1_51

# Build options (if any)
BUILD_OPS = -DDEBUG -Wall
#BUILD_OPS = -DDEBUG -O0 -g -Wall -D_WIN32_WINNT=0x0501

# Library dependencies (if any)
#LIB_ADDON = -lMswsock -lWs2_32

# DO NOT EDIT AFTER THIS LINE -------------------------
ifdef BOOST_PATH
    export BOOST_INCLUDE_PATH := -I$(BOOST_PATH)
	export BOOST_LIB_PATH := -L$(BOOST_PATH)/stage/lib
endif
ifdef LIB_SUFFIX
	export LIB_SUFFIX
endif
ifdef BUILD_OPS
	export BUILD_OPS
endif
ifdef LIB_ADDON
	export LIB_ADDON
endif

all: 
	$(MAKE) -C release all

clean: 
	$(MAKE) -C release clean

.PHONY: release