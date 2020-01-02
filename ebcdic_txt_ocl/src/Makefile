
CXX = g++

CXXFLAGS=-O2 

ifdef FPGA
	CXXFLAGS+=-DFPGA=1
endif

ifdef MWI 
	CXXFLAGS+=-DMWI=1
endif

LIBS = -lOpenCL -lm

BIN = conv_cl

AOCL_UTILS_DIR = ../../common_harp

SRC = conv.cpp a2e.c e2a.c conv_routines.c 


$(BIN): $(SRC) ../../common_harp/src/AOCLUtils/opencl.cpp
	$(CXX) $^ $(CXXFLAGS) -I./include -I$(AOCL_UTILS_DIR)/inc/ -o $@ $(LIBS) -Wall

clean:
	rm -f $(BIN) *.o
