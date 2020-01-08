#General Makefile for OpenCL CPU / FPGA

ifeq ($(VERBOSE),1)
ECHO := 
else
ECHO := @
endif

TARGET := idx3_to_tiff

# Directories.
HOST_DIR := host
TARGET_DIR := bin
INC_DIRS := host/inc ../common/inc ../extlibs/inc 

# All include files.
INC_FILES := $(foreach D,$(INC_DIRS),$(wildcard $D/*.h))

# Source files.
SRCS := $(wildcard host/src/*.cpp)

# Libraries.
LIBS := OpenCL m 

# Compiler.
CXX := g++

# Compilation flags
ifeq ($(DEBUG),1)
CXXFLAGS += -g 
else
CXXFLAGS += -O2 
endif
CXXFLAGS += -Wall -I/usr/local/include -L/usr/local/lib
#CXXFLAGS += -DGPU
CXXFLAGS += -DCPU -fopenmp

# Make it all!
all : $(TARGET_DIR)/$(TARGET)

# Host executable target.
$(TARGET_DIR)/$(TARGET) : Makefile $(SRCS) $(INC_FILES)
	@[ -d $(TARGET_DIR) ] || mkdir $(TARGET_DIR)
	$(ECHO)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -fPIC $(foreach D,$(INC_DIRS),-I$D) \
			$(SRCS) \
			$(foreach D,$(LIB_DIRS),-L$D) \
			$(foreach L,$(LIBS),-l$L) \
			-o $(TARGET_DIR)/$(TARGET) 

# Standard make targets
clean :
	$(ECHO)rm -f $(TARGET_DIR)/$(TARGET)

.PHONY : all clean
