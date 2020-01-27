#include "IDX3_to_TIFF_OCL.h"
#include "timer.h"

#if defined(FPGA)
size_t local = 1;
size_t global = 1;
//const char* source_path = "bin/Idx3toTiff.aocx";
const char* source_path = "/homes/cabreraam/dibs_opencl/idx3_tiff_ocl/device/with_ivdep.aocx";
#elif defined(CPU)
size_t local = 1; //REPLACE
size_t global = 1;
const char* source_path = "device/Idx3toTiff.cl";
#else //GPU
size_t local = 64;
size_t global = 1;
const char* source_path = "device/Idx3toTiff.cl";
#endif
const char* kernel_name = "generate_tiff_file";

//Globals
static cl_device_id device;	          // compute device id
static cl_platform_id platforms;      // compute platform ids
static cl_context context;		        // compute context
static cl_command_queue queue;        // compute command queue
static cl_program program;		        // compute program
static cl_kernel kernel;		          // compute kernel
static cl_int status;                 // status int for errors
cl_event event;

#if defined(FPGA) || defined(CPU)
unsigned char* svm_headerBuffer;
unsigned char* svm_footerBuffer;
unsigned char* svm_pixelDataBuffer;
unsigned char* svm_tiffFileOutputBuffer;
#else //GPU
cl_mem cl_headerBuffer;
cl_mem cl_footerBuffer;
cl_mem cl_pixelDataBuffer;
cl_mem cl_tiffFileOutputBuffer;
#endif

#ifdef FPGA
void cleanup() {
    //It DOES NOTHING!
    //Have to declare it due to altera Opencl
}
#endif
void freeResources();



int main(int argc, char *argv[]) {
	unsigned char *imageDataBuffer;
	unsigned char *labelDataBuffer;

	//timing stuff
	TimeStamp compute_start, compute_end;
	double compute_time;

	// load the files into buffers and store the length
	size_t imageFileLength = idx_load_to_buffer(FILEPATH_IMG, &imageDataBuffer);
	size_t labelFileLength = idx_load_to_buffer(FILEPATH_LBL, &labelDataBuffer);

	// store image specifications
	unsigned int numImages = read_2byte_int(imageDataBuffer, IDX3_OFFSET_IMAGE_NUM);
	unsigned int height = *(imageDataBuffer + IDX3_OFFSET_ROW_NUM);
	unsigned int width = *(imageDataBuffer + IDX3_OFFSET_COL_NUM);
	unsigned int pixelCount = width * height;
	printf("height: %u, width: %u, pixelCount: %u\n", height, width, pixelCount);
	// generate header

  // generate tags / headers
	struct header tiffHeader = generate_tiff_header(pixelCount);
	enum dataTypes{
		TYPE_BYTE = 1,
		TYPE_ASCII,
		TYPE_SHORT,
		TYPE_LONG,
		TYPE_RATIONAL
	};
	struct tag tiffTags[NUM_TAGS] = {
		{TAG_WIDTH, TYPE_LONG, 1, width},
		{TAG_HEIGHT, TYPE_LONG, 1, height},
		{TAG_BITS_PER_SAMPLE, TYPE_SHORT, 1, 8},
		{TAG_COMPRESSION, TYPE_SHORT, 1, 1},
		{TAG_INTERPRETATION, TYPE_SHORT, 1, 0},
		{TAG_STRIP_OFFSETS, TYPE_LONG, 1, 8},
		{TAG_ROWS_PER_STRIP, TYPE_SHORT, 1, height},
		{TAG_STRIP_BYTE_COUNTS, TYPE_LONG, 1, height * width},
		{TAG_X_RESOLUTION, TYPE_LONG, 1, 1},
		{TAG_Y_RESOLUTION, TYPE_LONG, 1, 1},
		{TAG_RESOLUTION_UNIT, TYPE_SHORT, 1, 1}};
  uint16_t numTags = NUM_TAGS;
  uint32_t tiffEnd = TIFF_END;
  /////////////////////////////////////////////////////////////////////////////
  //Create OpenCL program componets
  //Init platforms and devices
  int check = ocl_get_device(&platforms, &device);
  if (check != EXIT_SUCCESS) {
		printf("Cannot open resources\n");
    freeResources();
		return -1;
	}
  //Initialize OpenCL program componets
  //Init context
  context = clCreateContext(0, 1, &device, NULL, NULL, &status);
  usrCheckError(status, "Creating context");
  //Init command Queue
	queue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &status);
	usrCheckError(status, "Creating command queue");


  /////////////////////////////////////////////////////////////////////////////
  //Initalize buffers and build program

  size_t headerBufferLength = sizeof(struct header);
  size_t footerBufferLength = (sizeof(struct tag) * NUM_TAGS) * sizeof(uint16_t) + sizeof(uint32_t);
  const unsigned int fileLength = headerBufferLength + pixelCount + footerBufferLength;

  #if defined FPGA
  //Header buffer
    svm_headerBuffer = (unsigned char*)clSVMAllocAltera(context, CL_MEM_READ_ONLY, headerBufferLength, 0);
    status = clEnqueueSVMMemcpy(queue, CL_TRUE, (void *)svm_headerBuffer, &tiffHeader, headerBufferLength, 0, NULL, NULL);
    usrCheckError(status, "Memory Copy header buffer");
  //Footer buffer
    svm_footerBuffer = (unsigned char*)clSVMAllocAltera(context, CL_MEM_READ_ONLY,
                                                          footerBufferLength, 0);
    status = clEnqueueSVMMap(queue, CL_TRUE, CL_MAP_WRITE, (void *)svm_footerBuffer, footerBufferLength, 0, NULL, NULL);
      usrCheckError(status, "map footerbuffer");
      //Create a pointer for iterating
      unsigned char *footerBufferPtr = svm_footerBuffer;
      //Copy in the number of tags

      memcpy(footerBufferPtr, &numTags, sizeof(numTags));
      footerBufferPtr += sizeof(numTags);
      //Copy in all tags
      for(uint16_t i = 0; i < numTags; ++i){
        memcpy(footerBufferPtr, &(tiffTags[i]), sizeof(struct tag));
        footerBufferPtr += sizeof(struct tag);
      }
      //Copy in end tag
      memcpy(footerBufferPtr, &tiffEnd, sizeof(tiffEnd));
    status = clEnqueueSVMUnmap(queue, (void *)svm_footerBuffer, 0, NULL, NULL);
    usrCheckError(status, "unmap footerbuffer");
  //Input Data (pixel data buffer)
    svm_pixelDataBuffer = (unsigned char*)clSVMAllocAltera(context, CL_MEM_READ_ONLY,
                                                          imageFileLength, 0);

    //memcpy(svm_pixelDataBuffer, imageDataBuffer, imageFileLength);
    status = clEnqueueSVMMemcpy(queue, CL_TRUE, (void *)svm_pixelDataBuffer, (void *)imageDataBuffer, imageFileLength, 0, NULL, NULL);
    usrCheckError(status, "Memory Copy PixelBuffer");
    svm_tiffFileOutputBuffer = (unsigned char*)clSVMAllocAltera(context, CL_MEM_READ_WRITE,
                                                          (sizeof(unsigned char) * numImages * fileLength),
                                                          0);
  //Build from binary
    //status = buildFromBinary(&context, &device, &program, source_path);
    status = buildFromBinary(&context, &device, &program, argv[1]);

  #elif defined CPU

  //Header buffer
    svm_headerBuffer = (unsigned char*)clSVMAlloc(context, CL_MEM_READ_ONLY, headerBufferLength, 0);

    status = clEnqueueSVMMemcpy(queue, CL_TRUE, (void *)svm_headerBuffer, &tiffHeader, headerBufferLength, 0, NULL, NULL);

    usrCheckError(status, "Memory Copy header buffer");


  //Footer buffer
    svm_footerBuffer = (unsigned char*)clSVMAlloc(context, CL_MEM_READ_ONLY, footerBufferLength, 0);

    status = clEnqueueSVMMap(queue, CL_TRUE, CL_MAP_WRITE, (void *)svm_footerBuffer, footerBufferLength, 0, NULL, NULL);

    usrCheckError(status, "map footerbuffer");
        //Create a pointer for iterating
        unsigned char *footerBufferPtr = svm_footerBuffer;
        //Copy in the number of tags
        memcpy(footerBufferPtr, &numTags, sizeof(numTags));
        footerBufferPtr += sizeof(numTags);
        //Copy in all tags
        for(uint16_t i = 0; i < numTags; ++i){
          memcpy(footerBufferPtr, &(tiffTags)[i], sizeof(struct tag));
          footerBufferPtr += sizeof(struct tag);
        }
        //Copy in end tag
        memcpy(footerBufferPtr, &tiffEnd, sizeof(tiffEnd));
    status = clEnqueueSVMUnmap(queue, (void *)svm_footerBuffer, 0, NULL, NULL);
    usrCheckError(status, "unmap footerbuffer");


  //Input Data (pixel data buffer)
    svm_pixelDataBuffer = (unsigned char*)clSVMAlloc(context, CL_MEM_READ_ONLY, imageFileLength, 0);

    status = clEnqueueSVMMemcpy(queue, CL_TRUE, (void *)svm_pixelDataBuffer, (void *)imageDataBuffer, imageFileLength, 0, NULL, NULL);

    usrCheckError(status, "Memory Copy PixelBuffer");

  //Output Data Buffer
    svm_tiffFileOutputBuffer = (unsigned char*)clSVMAlloc(context, CL_MEM_READ_WRITE,
                                                          (sizeof(unsigned char) * numImages * fileLength),
                                                          0);

  //Build from source
    status =  buildFromSource(&context, &device, &program, source_path );


  #else //GPU
  //Fuck it
    cl_uchar *headerBuffer, *footerBuffer;
    load_buffer(&headerBuffer, tiffHeader, (cl_uint)headerBufferLength,
                &footerBuffer, tiffTags, (cl_uint)footerBufferLength);
    cl_headerBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                     headerBufferLength, headerBuffer, &status);

    usrCheckError(status, "Creating buffer cl_headerBuffer");

    cl_footerBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                     footerBufferLength, footerBuffer, &status);

    usrCheckError(status, "Creating buffer cl_footerBuffer");

    cl_pixelDataBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                      imageFileLength, imageDataBuffer, &status);

    usrCheckError(status, "Creating buffer cl_pixelDataBuffer");

    cl_tiffFileOutputBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
  											 sizeof(cl_uchar) * numImages * fileLength, NULL, &status);

  	usrCheckError(status, "Creating buffer cl_tiffFileOutputBuffer");

    status =  buildFromSource(&context, &device, &program, source_path);
  #endif


  //Create KERNEL
  kernel = clCreateKernel(program, kernel_name, &status);
  usrCheckError(status, "creating kernel");


  //Set arguments for kernel

  #if defined FPGA

    status = clSetKernelArg(kernel, 0, sizeof(cl_uint), &numImages);
    status |= clSetKernelArg(kernel, 1, sizeof(cl_uint), &fileLength);
    status |= clSetKernelArg(kernel, 2, sizeof(cl_uint), &pixelCount);
    status |= clSetKernelArg(kernel, 3, sizeof(cl_uint), &headerBufferLength);
    status |= clSetKernelArg(kernel, 4, sizeof(cl_uint), &footerBufferLength);
    status |=  clSetKernelArgSVMPointerAltera(kernel, 5, (void*)svm_headerBuffer);
    status |=  clSetKernelArgSVMPointerAltera(kernel, 6, (void*)svm_footerBuffer);
    status |=  clSetKernelArgSVMPointerAltera(kernel, 7, (void*)svm_pixelDataBuffer);
    status |=  clSetKernelArgSVMPointerAltera(kernel, 8, (void*)svm_tiffFileOutputBuffer);
    usrCheckError(status, "Setting kernel arguments");

  #elif defined CPU

    status = clSetKernelArg(kernel, 0, sizeof(cl_uint), &numImages);
    status |= clSetKernelArg(kernel, 1, sizeof(cl_uint), &fileLength);
    status |= clSetKernelArg(kernel, 2, sizeof(cl_uint), &pixelCount);
    status |= clSetKernelArg(kernel, 3, sizeof(cl_uint), &headerBufferLength);
    status |= clSetKernelArg(kernel, 4, sizeof(cl_uint), &footerBufferLength);
    status |=  clSetKernelArgSVMPointer(kernel, 5, (void*)svm_headerBuffer);
    status |=  clSetKernelArgSVMPointer(kernel, 6, (void*)svm_footerBuffer);
    status |=  clSetKernelArgSVMPointer(kernel, 7, (void*)svm_pixelDataBuffer);
    status |=  clSetKernelArgSVMPointer(kernel, 8, (void*)svm_tiffFileOutputBuffer);
    usrCheckError(status, "Setting kernel arguments");

  #else //GPU

    status = clSetKernelArg(kernel, 0, sizeof(cl_uint), &numImages);
    status |= clSetKernelArg(kernel, 1, sizeof(cl_uint), &fileLength);
    status |= clSetKernelArg(kernel, 2, sizeof(cl_uint), &pixelCount);
    status |= clSetKernelArg(kernel, 3, sizeof(cl_uint), &headerBufferLength);
    status |= clSetKernelArg(kernel, 4, sizeof(cl_uint), &footerBufferLength);
    status |= clSetKernelArg(kernel, 5, sizeof(cl_mem), &cl_headerBuffer);
    status |= clSetKernelArg(kernel, 6, sizeof(cl_mem), &cl_footerBuffer);
    status |= clSetKernelArg(kernel, 7, sizeof(cl_mem), &cl_pixelDataBuffer);
    status |= clSetKernelArg(kernel, 8, sizeof(cl_mem), &cl_tiffFileOutputBuffer);
    usrCheckError(status, "Setting kernel arguments");
  #endif

  //////////////////////////////////////////////////////////////////
  // END SETUP - START OCL KERNEL
  //////////////////////////////////////////////////////////////////

#if defined FPGA
		GetTime(compute_start);
#ifdef MWI
		local = atoi(argv[2]);
    global = (size_t)numImages * local;
		printf("global = %zu, local = %zu\n", global, local);
    status = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global, &local,
  		                                0, NULL, &event);
#else
    status = clEnqueueTask(queue, kernel, 0, NULL, NULL);
#endif
    usrCheckError(status, "Enqueuing kernel");
    status = clFinish(queue);
		GetTime(compute_end);
    usrCheckError(status, "Waiting for finish from device");
  	compute_time = TimeDiff(compute_start, compute_end);
  	printf("\nComputation done in %0.3lf ms.\n", compute_time);


  #elif defined CPU

    global = (size_t)numImages * local;
    status = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global, &local,
  		                                0, NULL, &event);
    status = clFinish(queue);
    usrCheckError(status, "Waiting for finish from device");

  #else //GPU

    global = (size_t)numImages * local;
    status = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global, &local,
  		                                0, NULL, NULL);

  	usrCheckError(status, "Enqueuing kernel");
    status = clFinish(queue);
    usrCheckError(status, "Waiting for finish from device");

  #endif
  printf("Returning from kernel\n");


  ////////////////////////////////////////////////////////////////////
  // END KERNEL - READ BACK DATA
  ////////////////////////////////////////////////////////////////////
/*  size_t tiffFileOutputBufferSize = sizeof(unsigned char) * numImages * fileLength;
  //Read buffer SVM cl_mem
  #if defined FPGA
    status = clEnqueueSVMMap(queue, CL_TRUE, CL_MAP_READ, (void *)svm_tiffFileOutputBuffer, tiffFileOutputBufferSize, 0, NULL, NULL);
    usrCheckError(status, "Enqueue SVM Map Output");

  #elif defined CPU
    status = clEnqueueSVMMap(queue, CL_TRUE, CL_MAP_READ, (void *)svm_tiffFileOutputBuffer, tiffFileOutputBufferSize, 0, NULL, NULL);
    usrCheckError(status, "Enqueue SVM Map Output");

  #else //GPU
    unsigned char *tiffFileOutputBuffer = (cl_uchar *)calloc(1, tiffFileOutputBufferSize);
    status = clEnqueueReadBuffer( queue, cl_tiffFileOutputBuffer, CL_TRUE, 0,
  		                            tiffFileOutputBufferSize, tiffFileOutputBuffer,
  		                            0, NULL, NULL);
    usrCheckError(status, "Reading back buffer cl_tiffFileOutputBuffer");
  #endif

  #ifdef GPU
    cl_uchar *pointerTiffFileOutputBuffer = tiffFileOutputBuffer;
  	for (unsigned int imageNumber = 0; imageNumber < numImages; ++imageNumber)
  	{
  		cl_uchar *buffer = (cl_uchar *)calloc(1, fileLength);
  		memcpy(buffer, pointerTiffFileOutputBuffer, fileLength);
  		pointerTiffFileOutputBuffer += fileLength;
  		const cl_uint label = idx1_read_label(labelDataBuffer, imageNumber);
  		write_file(buffer, imageNumber, label, fileLength);
  		free(buffer);
  	}
  #else
    unsigned char *pointerTiffFileOutputBuffer = svm_tiffFileOutputBuffer;
    for (unsigned int imageNumber = 0; imageNumber < numImages; ++imageNumber)
    {
      unsigned char *buffer = (unsigned char *)calloc(1, fileLength);
      memcpy(buffer, pointerTiffFileOutputBuffer, fileLength);
      pointerTiffFileOutputBuffer += fileLength;
      const cl_uint label = idx1_read_label(labelDataBuffer, imageNumber);
      write_file(buffer, imageNumber, label, fileLength);
      free(buffer);
    }
  #endif

	cl_ulong time_start;
    cl_ulong time_end;
  	clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
  	clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
  	double nanoSeconds = time_end-time_start;
  	printf("OpenCl Execution time is: %0.3f milliseconds \n",nanoSeconds / 1000000.0);
*/
	// Free allocated memory

	free(imageDataBuffer);
	free(labelDataBuffer);
  #ifdef FPGA
    clSVMFreeAltera(context,svm_headerBuffer);
    clSVMFreeAltera(context,svm_footerBuffer);
  	clSVMFreeAltera(context,svm_pixelDataBuffer);
    clSVMFreeAltera(context,svm_tiffFileOutputBuffer);
  #elif defined(CPU)
    clSVMFree(context,svm_headerBuffer);
    clSVMFree(context,svm_footerBuffer);
    clSVMFree(context,svm_pixelDataBuffer);
    clSVMFree(context,svm_tiffFileOutputBuffer);
  #else //GPU 
	free(headerBuffer);
	free(footerBuffer);
	status = clReleaseMemObject(cl_headerBuffer);
    status |= clReleaseMemObject(cl_footerBuffer);
    status |= clReleaseMemObject(cl_pixelDataBuffer);
    status |= clReleaseMemObject(cl_tiffFileOutputBuffer);
  #endif
  status |= clReleaseProgram(program);
  status |= clReleaseCommandQueue(queue);
  status |= clReleaseContext(context);
  status |= clReleaseKernel(kernel);
  //status |= clReleaseDevice(device);

	usrCheckError(status, "Releasing OpenCL resources");
	return EXIT_SUCCESS;

}

void load_buffer(cl_uchar **headerBuffer, struct header header, cl_uint *headerBufferLength,
				 cl_uchar **footerBuffer, struct tag tags[NUM_TAGS], cl_uint *footerBufferLength)
{
	// copy header
	*headerBuffer = (cl_uchar *)calloc(1, *headerBufferLength);
	memcpy(*headerBuffer, &header, *headerBufferLength);
	//printf("headerBufferLength: %u\n", *headerBufferLength);

	const uint16_t numTags = NUM_TAGS;
	const uint32_t tiffEnd = TIFF_END;

	// copy footer
	*footerBuffer = (cl_uchar *)calloc(*footerBufferLength, sizeof(cl_uchar));
	cl_uchar *pointerFooterBuffer = *footerBuffer;
	memcpy(pointerFooterBuffer, &numTags, sizeof(numTags));
	pointerFooterBuffer += sizeof(numTags);

	cl_uint i;
	for (i = 0; i < numTags; ++i)
	{
		memcpy(pointerFooterBuffer, &tags[i], sizeof(struct tag));
		pointerFooterBuffer += sizeof(struct tag);
	}
	memcpy(pointerFooterBuffer, &tiffEnd, sizeof(tiffEnd));
	//printf("footerBufferLength: %u\n", *footerBufferLength);
}


void freeResources() {


}
