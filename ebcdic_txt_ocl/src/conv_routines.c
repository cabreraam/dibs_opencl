// conv_fileio.c
//
// Author:			Anthony Cabrera
// Date:			8/10/16
// Last Updated:	8/10/16
// Description:		These are generic fileio functions;
//

#include "conv_routines.h"
#include <stdio.h>
#include <stdlib.h>
//#include <time.h>
#include <string.h>
#include "ocl_template.h"

#ifdef FPGA
	#include "AOCLUtils/aocl_utils.h"
#endif

void usage(char *prog_name)
{
	printf("TODO: this usage does not reflect OpenCL inputs\n\n");
	printf("USAGE:\n\t %s [-i inputfile] [-o outputfile] [-a] [-n]\n", prog_name);
	printf("\n");
	printf("DESCRIPTION:\n\tConvert the input file into either ASCII or EBCDIC,");
	printf("\n");
	printf("\t-i\n\t\tinput filename");
	printf("\n");
	printf("\t-o\n\t\toutput filename.\n");
	printf("\t\tNOTE:do not use file extension in name. This program will ");
	printf("append .txt to it\n");
	printf("\t-a\n\t\tflag that specifies ASCII to EBCDIC conversion. If not");
	printf("\n\t\tdefined, then conversion is from EBCDIC to ASCII\n");
	printf("\t-n\n\t\tnumber of runs. If not specified, default is 1\n");
	printf("\t\tNOTE:do not exceed 500 runs.");
	printf("\nEXAMPLE:\n\t ./conv -i input.in -o output \n");
	printf("\t\tHere, the conversion is from EBCDIC to ASCII and only 1 run is");
	printf("performed.\n");
	printf("\t ./conv -i input.in -o output -a -n 50");
	printf("\t\tHere, the conversion is from ASCII to EBCDIC with 50 runs\n\n");
	printf("performed.\n\n");
}

/* figure out how big the file is and read it into a buffer */
int readCharFile(FILE* ifp, long int *buf_size, unsigned char** source, size_t* newLen)
{

	if (fseek(ifp, 0L, SEEK_END) != 0)
	{
		printf("Something has gone wrong.\n");
		return -1;
	}

	else
	{
		/* return current file position after fseek to effectively get
		 * # of bytes */
		*buf_size = ftell(ifp);
		//printf("buf_size = %ld.\n", *buf_size);
		if ((*buf_size) == -1)
		{
			printf("Something has gone wrong.\n");
			return *buf_size;
		}
		else
		{
			/* set input file pointer back to beginning */
			if (fseek(ifp, 0L, SEEK_SET) != 0)
			{
				printf("Something has gone wrong.\n");
				return -1;
			}
			else
			{
				*source = (unsigned char*) malloc( 
					sizeof(unsigned char) * ((*buf_size) + 1) 
				);
				*newLen = fread(*source, sizeof(char), *buf_size, ifp);

				if (ferror(ifp))
				{
					printf("Something has gone wrong.\n");
					return -1;
				}
			}

		}
	}
	return 0;
}


int get_file_size(FILE* ifp, unsigned long int *buf_size)
{
	if (fseek(ifp, 0L, SEEK_END) != 0)
	{
		printf("Problem with call to fseek.\n");
		return -1;
	}

	else
	{
		*buf_size = ftell(ifp);
		if ((*buf_size) == -1)
		{
			printf("Problem with resetting file ptr to beginning with ftell.\n");
			return -1;
		}
		else
		{
			/* set input file pointer back to beginning */
			if (fseek(ifp, 0L, SEEK_SET) != 0)
			{
				printf("Something has gone wrong.\n");
				return -1;
			}
		}
	}

	return 0;
}

int readCharFile_ocl(FILE* ifp, unsigned long int buf_size, unsigned char** source, 
	size_t* newLen, opencl_info* ocl_info)
{

	/*cl_int is_context_valid = clGetContextInfo (	ocl_info->context,
	 	CL_CONTEXT_REFERENCE_COUNT,
	 	128,
	 	NULL,
	 	NULL);
	if (is_context_valid != CL_SUCCESS)
	{
		printf("context not valid; abort\n");
		return -1;
	}*/

	unsigned char* temp_src = (unsigned char*) malloc(sizeof(unsigned char) * (buf_size + 1)); 
	printf("reg malloc ok\n");

#ifdef FPGA
	*source = (unsigned char*) clSVMAllocAltera(
		ocl_info->context, CL_MEM_READ_WRITE, sizeof(unsigned char) * (buf_size + 1), 1024 
	);
#else
	*source = (unsigned char*) clSVMAlloc(
		ocl_info->context, CL_MEM_READ_WRITE, sizeof(unsigned char) * (buf_size + 1), 64 
	);
#endif
	if (*source == NULL)
	{
		printf("error in clSVMAlloc\n");
		return -1;
	}

	cl_int status;
	status = clEnqueueSVMMap(ocl_info->cmd_queue, CL_TRUE, CL_MAP_WRITE, (void *)(*source), 
		sizeof(unsigned char)*(buf_size+1), 0, NULL, NULL);
	if (status != CL_SUCCESS)
	{
		printf("abort\n");
		return -1;
	}
	printf("enqueuesvmmap ok\n");
	/*clEnqueueSVMMemcpy(ocl_info->cmd_queue, CL_TRUE, (void *)(*source), (void *)temp_src, sizeof(unsigned char) * buf_size, 0, NULL, NULL);
	printf("enqueuesvmmemcpy ok\n");*/

	*newLen = fread(*source, sizeof(char), buf_size, ifp);
	if (ferror(ifp))
	{
		printf("Something has gone wrong.\n");
		return -1;
	}
	(*source)[(*newLen)++] = '\0'; /* Just to be safe. */
	clEnqueueSVMUnmap(ocl_info->cmd_queue, (void *)(*source), 0, NULL, NULL);
	printf("enqueuesvmunmap ok\n");

	free(temp_src);
	return 0;

}


