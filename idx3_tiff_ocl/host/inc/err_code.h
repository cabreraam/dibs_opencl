#ifndef Error_H_included
#define Error_H_included

/*----------------------------------------------------------------------------
 *
 * Name:     err_code()
 *
 * Purpose:  Function to output descriptions of errors for an input error code
 *           and quit a program on an error with a user message
 *
 *
 * RETURN:   echoes the input error code / echos user message and exits
 *
 * HISTORY:  Written by Tim Mattson, June 2010
 *           This version automatically produced by genErrCode.py
 *           script written by Tom Deakin, August 2013
 *           Modified by Bruce Merry, March 2014
 *           Updated by Tom Deakin, October 2014
 *               Included the checkError function written by
 *               James Price and Simon McIntosh-Smith
 *
 *----------------------------------------------------------------------------
 */
#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/opencl.h>
#endif

#ifdef FPGA
#include "CL/opencl.h"
#else
#include <CL/cl.h>
#endif

#ifdef __cplusplus
 #include <cstdio>
 #include <string.h>
#endif

const char *err_code_for_user (cl_int err_in);
void check_error_user(cl_int err, const char* operation, const char* filename, int line);

#define usrCheckError(E,S) check_error_user (E,S,__FILE__,__LINE__)

#endif
