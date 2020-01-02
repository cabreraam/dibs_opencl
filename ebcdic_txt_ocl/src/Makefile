
CXX = g++

CXXFLAGS=-O2 

INC = -I./include  

LIBS = -lm

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
endif


BIN = conv_cl

AOCL_UTILS_DIR = ../../common_harp

SRC = conv.cpp a2e.c e2a.c conv_routines.c 


$(BIN): $(SRC) ../../common_harp/src/AOCLUtils/opencl.cpp
	$(CXX) $^ $(CXXFLAGS) $(INC) -I$(AOCL_UTILS_DIR)/inc/ -o $@ $(LIBS) -Wall

clean:
	rm -f $(BIN) *.o
