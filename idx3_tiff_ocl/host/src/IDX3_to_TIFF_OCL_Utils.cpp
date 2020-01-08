#include "IDX3_to_TIFF_OCL.h"


int ocl_get_device(cl_platform_id* plat, cl_device_id* dev)
{
  cl_uint num_platforms = 0;
  cl_uint num_devices;
  cl_int status;

  status = clGetPlatformIDs(1, plat, &num_platforms);
  if(status != CL_SUCCESS){
      usrCheckError(status, "getting platforms");
      return EXIT_FAILURE;
  }
  // get the device ID
  status = clGetDeviceIDs(*plat, CL_DEVICE_TYPE_ALL, 1, dev, &num_devices);
  if(status != CL_SUCCESS){
      usrCheckError(status, "getting devices");
      return EXIT_FAILURE;
  }
  char dev_name[50];
  status = clGetDeviceInfo(*dev, CL_DEVICE_NAME, 50, dev_name, NULL);
  if(status != CL_SUCCESS){
      usrCheckError(status, "getting devices");
      return EXIT_FAILURE;
  }
  printf("Using Device: %s\n", dev_name);
  return EXIT_SUCCESS;
}


#if defined(CPU) || defined(GPU)
//For CPU/GPU
cl_int buildFromSource( cl_context* context, cl_device_id* dev,
                        cl_program* program, const char* filepath)
{
  	cl_int status = 0;
	FILE *file = fopen("device/idx3-to-tiff.cl", "r");
	if (!file)
	{
		fprintf(stderr, "Error: Could not open kernel source file\n");
		exit(EXIT_FAILURE);
	}
	fseek(file, 0, SEEK_END);
	int len = ftell(file) + 1;
	rewind(file);

	char *source = (char *)calloc(len, sizeof(char));
	if (!source)
	{
		fprintf(stderr, "Error: Could not allocate memory for source string\n");
		exit(EXIT_FAILURE);
	}
	size_t ret = fread(source, sizeof(char), len, file);
	fclose(file);
  	*program = clCreateProgramWithSource(*context, 1, (const char **)&source, NULL, &status);
  	usrCheckError(status, "Creating program");
  	status = clBuildProgram(*program, 0, NULL, NULL, NULL, NULL);
	
	if (status != CL_SUCCESS)
	{
		size_t len = 0;
		status = clGetProgramBuildInfo(*program, *dev, CL_PROGRAM_BUILD_LOG, 0, NULL, &len);
		char *buffer = (char *)calloc(len, sizeof(char));
		status = clGetProgramBuildInfo(*program, *dev, CL_PROGRAM_BUILD_LOG, len, buffer, NULL);
		printf("buffer: %s\n", buffer);
		free(buffer);
		return EXIT_FAILURE;
	}
	return status;
}
#endif

//For FPGA
#ifdef FPGA
cl_int buildFromBinary(cl_context* context, cl_device_id* dev, cl_program* prog, const char* source)
{
  cl_int kernel_status = 0;
  cl_int status = 0;
  size_t binsize = 0;
  unsigned char *binary_file = loadBinaryFile((const char*)source, &binsize);
  if(!binary_file) {
    usrCheckError(status, "Failed loadBinaryFile.");
    return 1;
  }
  *prog = clCreateProgramWithBinary(*context, 1, dev, &binsize, (const unsigned char**)&binary_file, &kernel_status, &status);
  if(kernel_status != CL_SUCCESS) {
    usrCheckError(kernel_status, "Failed clCreateProgramWithBinary.");
    return 1;
  }
  delete [] binary_file;
  status = clBuildProgram(*prog, 0, NULL, "", NULL, NULL);
  if(status != CL_SUCCESS) {
    usrCheckError(status, "Failed clBuildProgram.");
    return 1;
  }
  return CL_SUCCESS;

}
#endif

unsigned int read_2byte_int(unsigned char *imageDataBuffer, unsigned char offset)
{
  unsigned int numHighBit = *(imageDataBuffer + offset);
  unsigned int numLowBit = *(imageDataBuffer + offset + 1);
  return (numHighBit << 8) | (numLowBit);
}

// Loads the byte data of the .idx3-ubyte at the given filepath into the
// given buffer and returns the length of the file(buffer)
size_t idx_load_to_buffer(const char *filepath, unsigned char **buffer)
{
  FILE *file = fopen(filepath, "rb");
  if (file == NULL)
  {
    printf("Cannot open file \n");
  }
  // find the length of the file
  fseek(file, 0, SEEK_END);
  size_t fileLength = ftell(file);
  rewind(file);
  // allocate the correct amount of memory
  *buffer = (unsigned char *)calloc(fileLength, sizeof(unsigned char));
  // read the file into the buffer
  fread(*buffer, fileLength, 1, file);
  fclose(file);
  return fileLength;
}

unsigned int idx1_read_label(const unsigned char* labelDataBuffer, const unsigned char imageNumber)
{
	return (unsigned int)labelDataBuffer[IDX1_OFFSET_LABEL + imageNumber];
}

cl_uint idx1_read_label(const unsigned char *labelDataBuffer, const unsigned int imageNumber)
{
	return (unsigned char)labelDataBuffer[IDX1_OFFSET_LABEL + imageNumber];
}

struct header generate_tiff_header(unsigned int pixelCount)
{
  struct header tiffHeader;
  tiffHeader.endian = HEADER_ENDIAN_LITTLE;
  tiffHeader.magicNumber = HEADER_MAGIC_NUMBER;
  tiffHeader.ifdOffset = pixelCount + 8;
  return tiffHeader;
}

void write_file(unsigned char *buffer, unsigned int imageNumber,
				         unsigned int label, size_t fileLength)
{
  char fileName[50];

	// check if the folder "TIFF" exists, create it if it doesn't
	struct stat st = {0};

	if (stat("TIFF", &st) == -1)
	{
		mkdir("TIFF", 0700);
	}
	sprintf(fileName, "TIFF/image%d(%d).tiff", imageNumber, label);
	FILE *file = fopen(fileName, "wb");
	fwrite(buffer, sizeof(unsigned char), fileLength, file);
	fclose(file);
}
