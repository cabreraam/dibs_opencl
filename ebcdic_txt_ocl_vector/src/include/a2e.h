//
//a2e.h
//
//Author:			Anthony Cabrera
//Date:				8/03/16
//Description:		This file takes plain text (ASCII) files and converts them
//                into EBCDIC.
//					      This is the header file for a2e.c

#ifndef A2E_H
#define A2E_H


extern unsigned char a2e_lut[256];

char ASCIItoEBCDIC(const unsigned char c);

void ASCIItoEBCDIC_all(unsigned char* source, unsigned int numElts);

#endif
