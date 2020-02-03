// conv_routines.h
//
// Author:			Anthony Cabrera
// Date:			8/10/14
// Last Updated:	8/10/14
// Description:		These are generic fileio functions;
//


#ifndef CONV_ROUTINES_H
#define CONV_ROUTINES_H

#include "ocl_template.h"
#include <stdio.h>

void usage(char * prog_name);

int readCharFile(FILE* ifp, long int *buf_size, unsigned char** source,
	size_t* newLen);

int get_file_size(FILE* ifp, unsigned long int* buf_size);

int readCharFile_ocl(FILE* ifp, unsigned long int buf_size, unsigned char** source, 
	size_t* newLen, opencl_info* ocl_info);

#endif
