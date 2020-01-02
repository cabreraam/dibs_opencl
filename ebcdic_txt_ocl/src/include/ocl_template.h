//initialize()
//	display_device_info
//		this also gets platformid 
//

//Read in kernel source somehow. Maybe with fread. In nw, we use a util func
	//"read_kernel" 

//Create context with clCreateProgramWithBinary 


//1) Define the platform
//		Grab a platform with clGetPlatformIDs
//		Get device id with clGetDeviceID
//		Create a context with clCreateContext
//		Create a command queue
//2) Create and build the program 
//		Build the program object with clCreateProgramWithBinary
//		Compile the program to create a dynmaic library from which specific
//			kernels can be pulled with clBuildProgram
//3) Setup memory objects	
//

#ifndef OCL_TEMPLATE_H
#define OCL_TEMPLATE_H

#include <CL/cl.h>
typedef struct {
	cl_platform_id*		platform_id;
	cl_device_id			device_id;
	cl_device_type		device_type;
	cl_context				context;
	cl_command_queue	cmd_queue;
	cl_program				program;
	cl_kernel					kernel;
} opencl_info;


#endif
