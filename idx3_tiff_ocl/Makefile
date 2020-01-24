
CXX = g++

CXXFLAGS=-O2 

INC = -I./host/inc

#LIBS = -lm
LIBS =  

BIN = idx3_to_tiff_cl

ifdef HARP
	AOCL_COMPILE_CONFIG=$(shell aocl compile-config)
	INC += $(AOCL_COMPILE_CONFIG)
	AOCL_LINK_CONFIG=$(shell aocl link-config)
	LIBS += $(AOCL_LINK_CONFIG)
	#LIBS+= -lOpenCL
else
	LIBS += -lOpenCL
endif

ifdef FPGA
	CXXFLAGS+=-DFPGA=1
endif

ifdef MWI 
	CXXFLAGS+=-DMWI=1
	BIN = idx3_to_tiff_cl_mwi
endif



AOCL_UTILS_DIR = ../common_harp

#SRC = conv.cpp a2e.c e2a.c conv_routines.c 
SRC := $(wildcard host/src/*.cpp)


$(BIN): $(SRC) ../common_harp/src/AOCLUtils/opencl.cpp
	$(CXX) $^ $(CXXFLAGS) $(INC) -I$(AOCL_UTILS_DIR)/inc/ -o $@ $(LIBS) -Wall

clean:
	rm -f $(BIN) *.o
