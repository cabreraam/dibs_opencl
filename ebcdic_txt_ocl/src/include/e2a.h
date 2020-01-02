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

extern unsigned char e2a_lut[256];

char EBCDICtoASCII(const unsigned char c);

void EBCDICtoASCII_all(unsigned char* source, unsigned int numElts);

#endif
