/*header file for idx to tiff*/

#ifdef FPGA
#include "CL/opencl.h"
#include "AOCLUtils/aocl_utils.h"
#else
#include <CL/cl.h>
#endif

#include "err_code.h"
//#include "device_picker.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define LOCAL_SIZE 64

// IDX1-UBYTE SPECIFIED BYTE OFFSETS
#define IDX1_OFFSET_LABEL 8

// IDX3-UBYTE SPECIFIED BYTE OFFSETS
#define IDX3_OFFSET_IMAGE_NUM 6
#define IDX3_OFFSET_ARRAY_START 16
#define IDX3_OFFSET_ROW_NUM 11
#define IDX3_OFFSET_COL_NUM 15

// LOCAL FILEPATHS
#define FILEPATH_IMG "t10k-images.idx3-ubyte"
#define FILEPATH_LBL "t10k-labels.idx1-ubyte"
#define FILEPATH_CL_SRC "Idx3toTiff.cl"
#define FILEPATH_CL_BIN "Idx3toTiff.aocx"
// TIFF HEADER CODES
#define HEADER_ENDIAN_LITTLE 0x4949
#define HEADER_ENDIAN_BIG 0x4D4D
#define HEADER_MAGIC_NUMBER 0x002A

// TIFF TAG ID CODES
#define TAG_WIDTH 0x0100
#define TAG_HEIGHT 0x0101
#define TAG_BITS_PER_SAMPLE 0x0102
#define TAG_COMPRESSION 0x0103
#define TAG_INTERPRETATION 0x0106
#define TAG_STRIP_OFFSETS 0x0111
#define TAG_ROWS_PER_STRIP 0x0116
#define TAG_STRIP_BYTE_COUNTS 0x0117
#define TAG_X_RESOLUTION 0x011a
#define TAG_Y_RESOLUTION 0x011b
#define TAG_RESOLUTION_UNIT 0x0153

#define NUM_TAGS 11
#define TIFF_END 0x00000000

#ifdef FPGA
using namespace aocl_utils;
#endif

// Structure for TIFF Header
struct header {
	uint16_t endian;
	uint16_t magicNumber;
	uint32_t ifdOffset;
};

// Structure for TIFF Tags
struct tag {
	uint16_t tagHeader;
	uint16_t dataType;
	uint32_t numValues;
	uint32_t value;
};

//Returns a pointer to the kernel source code

cl_int buildFromSource(cl_context* context, cl_device_id* dev,
                        cl_program* prog, const char* source);

cl_int buildFromBinary(cl_context* context, cl_device_id* dev,
                         cl_program* prog, const char* source);

int ocl_get_device(cl_platform_id* plat, cl_device_id* dev);


/////////////////////////////////////
//Specific to this data tranformation
////////////////////////////////////

// Reads a 2-byte int from 2 unsigned chars, given the offset from the high bit
unsigned int read_2byte_int(unsigned char *imageDataBuffer, unsigned char offset);

// Loads the byte data of the .idx3-ubyte at the given filepath into the
// given buffer and returns the length of the file(buffer)
// READ FILE
size_t idx_load_to_buffer(const char *filepath, unsigned char **buffer);

// Finds the corresponding label value given the image number
unsigned int idx1_read_label(const unsigned char *labelDataBuffer,
                              const unsigned char imageNumber);

// Generates a tiff header with constants and the given pixel count
struct header generate_tiff_header(unsigned int pixelCount);

// Writes TIFF file with given labels using the buffer of data
void write_file(unsigned char *buffer, unsigned int imageNumber,
				          unsigned int label, size_t fileLength);

