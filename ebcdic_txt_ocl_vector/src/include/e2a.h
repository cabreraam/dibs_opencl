//
//e2a.h
//
//Author:			Anthony Cabrera
//Date:				8/03/16
//Description:		This file takes EBCDIC files and converts them into
//                  plain text (ASCII) files. This is the header file
//                  for e2a.c

#ifndef E2A_H
#define E2A_H

#include <stddef.h>

extern unsigned char e2a_lut[256];

char EBCDICtoASCII(const unsigned char c);

void EBCDICtoASCII_all(unsigned char* source, unsigned int numElts);

void EBCDICtoASCII_extra(unsigned char* source, size_t start, size_t end);

void EBCDICtoASCII_extra_MWI(unsigned char* src, unsigned char* dst,  
	size_t start, size_t end);

#endif
