//
//conversions.c
//
//Author:			Anthony Cabrera
//Date:				8/03/16
//Description:		This is the entry point for using a2e.c and e2a.c for conv-
//					erting ASCII to EBCDIC and vice versa.
//
//Compile:			make -f makefile2
//

/**************************************************************************//**
 * conv.h
 * @file
 *
 * @author Anthony Cabrera
 * @date: 8/03/16
 *
 * @brief	This is the entry point for using a2e.c and e2a.c for converting
 * ASCII to EBCDIC and vice versa.
 *
 * Compile:			make -f makefile2
 */

#include <stdio.h> // printf
#include <stdlib.h> // size_t, malloc, atoi
#include <time.h> // clock()
#include <string.h> // strcat
#include <unistd.h> // getopt
#include <getopt.h> // getopt
#include "a2e.h"
#include "e2a.h"
#include "conv_routines.h"

