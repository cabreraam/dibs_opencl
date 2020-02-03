//
//conv.cpp
//
//Author:			Anthony Cabrera
//Date:				Originally, 8/03/16, now sometime in December `19 for OpenCL
//Description:		This is the entry point for using a2e.c and e2a.c for conv-
//					erting ASCII to EBCDIC and vice versa.
//
//	Default setting will be SVM and CPU
//		e.g., for FPGA, need to compile host code with -DFPGA=1
//		e.g., for FPGA and MWI, need -DFPGA=1 and -DMWI=1
//	For SWI, don't need to play with enqueuing kernel options at all
//
#include <stdio.h> // printf
#include <stdlib.h> // size_t, malloc, atoi
#include <string.h> // strcat
#include <unistd.h> // getopt
#include <getopt.h> // getopt
#include "a2e.h"
#include "e2a.h"
#include "conv_routines.h"
#include "timer.h"

#ifdef FPGA
#include "AOCLUtils/opencl.h"
#endif //FPGA
#include <CL/opencl.h>

void dump_error(const char *str, cl_int status);
char *getKernelSource(const char *filename);
void freeResources(opencl_info* ocl_info, unsigned char* source, 
	unsigned char* dst);

int main(int argc, char* argv[])
{
	/* file io variables */
	FILE *ifp = NULL, *ofp = NULL;
	char of_name[128] = "../output/";

	/* original variables */
	unsigned char *source = 0;
	int ch;
	unsigned long int buf_size;	//size of input file in bytes
	size_t newLen; 							//number of bytes read into source

	/* opencl stuff */
	opencl_info ocl_info;			//opencl platform/device variables
	cl_int status;	
	cl_uint num_platforms;
	cl_uint num_devices;
#ifdef FPGA
	char kernel_source[256];
#endif
	size_t local_wi_size; //local workitem size

	/* timing stuff */
	//TimeStamp input_start, input_end;
	//TimeStamp output_start, output_end;
	TimeStamp compute_start, compute_end;
	//double input_time, output_time, compute_time;
	double compute_time;

	/* Usage */
	if (argc <= 4 || argc >= 10)
	{
			printf("Incorrect number of arguments.\n");
		  usage(argv[0]);
    	return(-1);
	}
	else
	{
		while ((ch = getopt(argc, argv, "i:o:s:w:")) != -1)
		{
			switch (ch)
			{
				case 'i': // input file name
					ifp = fopen(optarg, "r");
					if (ifp == NULL)
					{
						perror("Error opening input file");
						return -1;
					}
					break;

				case 'o': // 'o'utput file name
					strcat(of_name, optarg);
					break;

#ifdef FPGA
				case 's': // kernel 's'ource file
					sprintf(kernel_source, "%s", optarg);
					printf("%s\n",kernel_source);
					break;
#endif

				case 'w': // local 'w'ork item size
					local_wi_size = atoi(optarg);	
					break;

				default:
					usage(argv[0]);
					return -1;

			}
		}
	}

	/* initialize opencl stuff */
	// get the platform ID
	status = clGetPlatformIDs(0, NULL, &num_platforms); 
	ocl_info.platform_id = (cl_platform_id*) malloc(sizeof(cl_platform_id) * num_platforms);
	status = clGetPlatformIDs(num_platforms, ocl_info.platform_id, &num_platforms);
	if (status != CL_SUCCESS)
	{
		dump_error("Failed clGetPlatformIDs.", status);
		freeResources(&ocl_info, NULL, NULL);
		return 1;
	}

	// get the device ID
#ifdef FPGA
	status = clGetDeviceIDs(ocl_info.platform_id[0], 
		CL_DEVICE_TYPE_ALL, 1, &ocl_info.device_id, &num_devices);
#else
	status = clGetDeviceIDs(ocl_info.platform_id[0], 
		CL_DEVICE_TYPE_CPU, 1, &ocl_info.device_id, &num_devices);
#endif //FPGA
	if (status != CL_SUCCESS) 
	{
		dump_error("Failed clGetDeviceIDs.", status);
		freeResources(&ocl_info, NULL, NULL);
		return 1;
	}
  /*if (num_devices != 1) 
	{
    printf("Found %d devices! Exiting now\n", num_devices);
		freeResources(&ocl_info, NULL);
    return 1;
  }*/

	// create a context
	// https://www.khronos.org/registry/OpenCL/sdk/1.0/docs/man/xhtml/clCreateContext.html
	ocl_info.context = clCreateContext(0, 1, &ocl_info.device_id, NULL, NULL, &status);
  if(status != CL_SUCCESS) 
	{
    dump_error("Failed clCreateContext.", status);
		freeResources(&ocl_info, NULL, NULL);
    return 1;
  }

  // create a command queue
  ocl_info.cmd_queue = clCreateCommandQueue(ocl_info.context, ocl_info.device_id, 0, &status);
  if(status != CL_SUCCESS) {
    dump_error("Failed clCreateCommandQueue.", status);
		freeResources(&ocl_info, NULL, NULL);
    return 1;
  }

  cl_int kernel_status;

#ifdef FPGA
//TODO: Anthony start here
  // create program with binary
  size_t binsize = 0;
  unsigned char* binary_file = aocl_utils::loadBinaryFile((const char *)kernel_source, &binsize);
  if(!binary_file) 
	{
    dump_error("Failed loadBinaryFile.", status);
		freeResources(&ocl_info, NULL, NULL);
    return 1;
  }
  ocl_info.program = clCreateProgramWithBinary(ocl_info.context, 1, 
		&ocl_info.device_id, &binsize, (const unsigned char**)&binary_file, 
		&kernel_status, &status);
  if(status != CL_SUCCESS) 
	{
    dump_error("Failed clCreateProgramWithBinary.", status);
		freeResources(&ocl_info, NULL, NULL);
    return 1;
  }
  delete [] binary_file;

  const char options[] = "";
#else
	//create program with source
#ifdef MWI
	//char *kernel_source = getKernelSource("./kernels/e2a_mwi.cl");
	char *kernel_source = getKernelSource("./kernels/e2a_manual_unroll.cl");
	printf("using e2a_manual_unroll\n");
#else
	char *kernel_source = getKernelSource("./kernels/e2a.cl");
#endif //MWI
	ocl_info.program = clCreateProgramWithSource(ocl_info.context, 1, 
		(const char **) &kernel_source, NULL, &kernel_status);

#ifdef MWI
  const char options[] = "-DUNROLL=1";
#else
  const char options[] = "-DUNROLL=2";
#endif //MWI

#endif //FPGA

  // build the program
  status = clBuildProgram(ocl_info.program, 0, NULL, options, NULL, NULL);
  if(status != CL_SUCCESS) 
	{
    dump_error("Failed clBuildProgram.", status);
		freeResources(&ocl_info, NULL, NULL);
    return 1;
  }

	// create the kernel
	ocl_info.kernel = clCreateKernel(ocl_info.program, "k_EBCDIC_to_ASCII", &status);

	if(status != CL_SUCCESS) 
	{
		dump_error("Failed clCreateKernel.", status);
		freeResources(&ocl_info, NULL, NULL);
		return 1;
	}

	strcat(of_name, "000.txt");

	//create host buffer through readCharFile_ocl
	// figure out how big the file is and read it into a buffer 
	if (get_file_size(ifp, &buf_size))
	{
		printf("get_file_size error\n");
		return -1;
	}
	printf("buf_size = %d\n", buf_size);
	if (readCharFile_ocl(ifp, buf_size, &source, &newLen, &ocl_info))
	{
		printf("readCharFile_ocl error\n");
		return -1;
	}


	// set the arguments
#ifdef FPGA
	status = clSetKernelArgSVMPointerAltera(ocl_info.kernel, 0, (void*) source);
#else
	status = clSetKernelArgSVMPointer(ocl_info.kernel, 0, (void*) source);
#endif //FPGA
	if(status != CL_SUCCESS) 
	{
		dump_error("Failed set arg 0.", status);
		return 1;
	}
#ifdef MWI 
	//If MWI, we don't need second kernel argument, 
	//just define global/local WI sizes
	//for some reason, 1024 alignment for CPU does not work!!!
	unsigned char *dst = (unsigned char *) 
		clSVMAlloc(ocl_info.context, CL_MEM_READ_WRITE, sizeof(unsigned char) * newLen,
			64);
	if (!dst)
	{
		printf("could not allocate dst buffer\n");
		freeResources(&ocl_info, NULL, NULL);
		return 1;  
	}

		//TODO: ANTHONY START HERE
	//init dst vector
	/*status = clEnqueueSVMMap(ocl_info.cmd_queue, CL_TRUE, CL_MAP_WRITE, (void*)(dst), 
		sizeof(unsigned char)*(newLen), 0, NULL, NULL);
	for (int idx = 0; idx < newLen; ++idx)
		dst[idx] = 0;
	printf("%s\n", dst);
	clEnqueueSVMUnmap(ocl_info.cmd_queue, (void *)(dst), 0, NULL, NULL);*/
			
	//clEnqueueSVMUnmap(ocl_info.cmd_queue, (void *)(dst), 0, NULL, NULL);
	status = clSetKernelArgSVMPointer(ocl_info.kernel, 1, (void*) dst);
	if(status != CL_SUCCESS) 
	{
		dump_error("Failed Set arg 1.", status);
		freeResources(&ocl_info, source, dst);
		return 1;
	}
	printf("argument 1 set\n");
#else 
	printf("setting len arg\n");
	cl_ulong my_size = (cl_ulong) newLen;
	status = clSetKernelArg(ocl_info.kernel, 1, sizeof(cl_ulong), &my_size);
	if(status != CL_SUCCESS) 
	{
		dump_error("Failed Set arg 1.", status);
		freeResources(&ocl_info, source, dst);
		return 1;
	}
#endif //MWI

	printf("Launching the kernel...\n");
	/* remaining stuff for enqueueing kernel */
	printf("right before enqueueing kernel\n");
	GetTime(compute_start);
	// Enqueue the EBCDIC to ASCII conversion!
#ifdef MWI
	size_t total = newLen; 
	size_t local = local_wi_size; 
	size_t modulo = total % local;
	size_t global = (total - modulo) / 8;
	printf("total = %d\n", total);
	printf("local = %d\n", local);
	printf("modulo = %d\n", modulo);
	printf("global = %d\n", global);
	status = clEnqueueNDRangeKernel(ocl_info.cmd_queue, ocl_info.kernel, 1, NULL,
		&global, &local, 0, NULL, NULL);
#else
	status = clEnqueueTask(ocl_info.cmd_queue, ocl_info.kernel, 0, NULL, NULL);
#endif //MWI
	if (status != CL_SUCCESS) {
		dump_error("Failed to launch kernel.", status);
		freeResources(&ocl_info, source, dst);
		return 1;
	}

	//TODO: include below line?
	clFinish(ocl_info.cmd_queue);

// NOTE: This will be different from the FCCM submission version because of the
// vectorized types. We will need to address toggling between the versions!!!
#ifdef MWI
	// map the source buffer
	status = clEnqueueSVMMap(ocl_info.cmd_queue, CL_TRUE, CL_MAP_READ, 
		(void *)source, newLen, 0, NULL, NULL); 
	if(status != CL_SUCCESS) 
	{
		dump_error("Failed clEnqueueSVMMap", status);
		freeResources(&ocl_info, source, dst);
		return 1;
	}
	// map the dst buffer
	status = clEnqueueSVMMap(ocl_info.cmd_queue, CL_TRUE, CL_MAP_WRITE, 
		(void *)dst, newLen, 0, NULL, NULL); 
	if(status != CL_SUCCESS) 
	{
		dump_error("Failed clEnqueueSVMMap", status);
		freeResources(&ocl_info, source, dst);
		return 1;
	}
	EBCDICtoASCII_extra_MWI(source, dst, global*8, total); 

	//unmap src buffer
	status = clEnqueueSVMUnmap(ocl_info.cmd_queue, (void *)source, 0, NULL, NULL); 
	if(status != CL_SUCCESS) {
		dump_error("Failed clEnqueueSVMUnmap", status);
		freeResources(&ocl_info, source, dst);
		return 1;
	}
	//unmap dst buffer
	status = clEnqueueSVMUnmap(ocl_info.cmd_queue, (void *)dst, 0, NULL, NULL); 
	if(status != CL_SUCCESS) {
		dump_error("Failed clEnqueueSVMUnmap", status);
		freeResources(&ocl_info, source, dst);
		return 1;
	}
#endif //MWI
	clFinish(ocl_info.cmd_queue);
	GetTime(compute_end);
  compute_time = TimeDiff(compute_start, compute_end);
  printf("\nComputation done in %0.3lf ms.\n", compute_time);

	//printf("Output filename: %s\n", of_name);
	/* make the conversions and write to output file */


// NOTE: This will be different from the FCCM submission version because of the
// vectorized types. We will need to address toggling between the versions!!!
// Here, I'm mapping dst for read to output buffer, NOT the source buffer
	status = clEnqueueSVMMap(ocl_info.cmd_queue, CL_TRUE, CL_MAP_READ, 
		(void *)dst, newLen, 0, NULL, NULL); 
	if(status != CL_SUCCESS) 
	{
		dump_error("Failed clEnqueueSVMMap", status);
		freeResources(&ocl_info, source, dst);
		return 1;
	}
	ofp = fopen(of_name, "w");
	if (ofp == NULL)
	{
		printf("Output directory doesn't exist. Has it been created?\n");
		printf("Terminating prematurely\n");
		free(source);
		return -1;
	}
	fwrite(dst, sizeof(char), buf_size, ofp);
	// Unmap
	status = clEnqueueSVMUnmap(ocl_info.cmd_queue, (void *)dst, 0, NULL, NULL); 
	if(status != CL_SUCCESS) {
		dump_error("Failed clEnqueueSVMUnmap", status);
		freeResources(&ocl_info, source, dst);
		return 1;
	}
	fclose(ofp);
	//TODO: Verify output


	/* free dynamically allocated memory */
    //free(source);
	/* close input/output files */
	fclose(ifp);
	freeResources(&ocl_info, source, dst);
    //fclose(ofp);
	return 0;
}

void dump_error(const char *str, cl_int status) {
  printf("%s\n", str);
  printf("Error code: %d\n", status);
}

/* Taken from Deakin and Mcintosh-Smith */
// Added the null character
char *getKernelSource(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "Error: Could not open kernel source file\n");
        exit(EXIT_FAILURE);
    }
    fseek(file, 0, SEEK_END);
    int len = ftell(file) + 1;
    rewind(file);

    char *source = (char *)calloc(sizeof(char), len);
    if (!source)
    {
        fprintf(stderr, "Error: Could not allocate memory for source string\n");
        exit(EXIT_FAILURE);
    }
    fread(source, sizeof(char), len, file);
    fclose(file);
		source[len] = '\0';
    return source;
}

// free the resources allocated during initialization
void freeResources(opencl_info* ocl_info, unsigned char* source, 
	unsigned char* dst) 
{
	opencl_info handle = *ocl_info;

  if(handle.kernel) 
    clReleaseKernel(handle.kernel);  
  if(handle.program) 
    clReleaseProgram(handle.program);
  if(handle.cmd_queue) 
    clReleaseCommandQueue(handle.cmd_queue);
  if(source) 
#ifdef FPGA
   clSVMFreeAltera(handle.context,source);
#else
   clSVMFree(handle.context,source);
#endif
   //clSVMFreeAltera(handle.context,source);
	if(dst)
#ifdef FPGA
   clSVMFreeAltera(handle.context,dst);
#else
   clSVMFree(handle.context,dst);
#endif
  if(handle.context) 
    clReleaseContext(handle.context);
	if(handle.platform_id)
		free(handle.platform_id);

}

void cleanup() {};