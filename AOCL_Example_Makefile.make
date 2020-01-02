
The following are example Makefile fragments for compiling and linking
a host program against the host runtime libraries included with the 
Altera SDK for OpenCL.


Example GNU makefile on Linux, with GCC toolchain:

AOCL_COMPILE_CONFIG=$(shell aocl compile-config)
AOCL_LINK_CONFIG=$(shell aocl link-config)

host_prog : host_prog.o
	g++ -o host_prog host_prog.o $(AOCL_LINK_CONFIG)

host_prog.o : host_prog.cpp
	g++ -c host_prog.cpp $(AOCL_COMPILE_CONFIG)

