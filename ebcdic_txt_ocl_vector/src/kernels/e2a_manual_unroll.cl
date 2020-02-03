/* 
 * Filename: e2a.cl 
 * Author: Anthony Cabrera
 * Date: 2/26/19
 * Description: OpenCL Kernel for EBCDIC to TEXT format 
 *
 */

#ifndef UNROLL
	#define UNROLL 1 
#endif

#ifndef WGSIZE
	#define WGSIZE 64 
#endif

#ifndef NUMCOMPUNITS
	#define NUMCOMPUNITS 1 
#endif

#ifndef NUMSIMD
	#define NUMSIMD 1 
#endif


//__attribute__((num_compute_units(NUMCOMPUNITS))) //only works for FPGA
__attribute__((reqd_work_group_size(WGSIZE,1,1)))
//__attribute__((num_simd_work_items(NUMSIMD)))//only works for fpga
__kernel void 
k_EBCDIC_to_ASCII(	__global uchar8* const restrict src,
										__global uchar8* restrict dst)
{

	unsigned char e2a_lut[256] = {
		0,  1,  2,  3,156,  9,134,127,151,141,142, 11, 12, 13, 14, 15,//0-15
		16, 17, 18, 19,157,133,  8,135, 24, 25,146,143, 28, 29, 30, 31,//16-31
		128,129,130,131,132, 10, 23, 27,136,137,138,139,140,  5,  6,  7,//32-47
		144,145, 22,147,148,149,150,  4,152,153,154,155, 20, 21,158, 26,//48-63
		32,160,161,162,163,164,165,166,167,168, 91, 46, 60, 40, 43, 33,//64-80
		38,169,170,171,172,173,174,175,176,177, 93, 36, 42, 41, 59, 94,//80-95
		45, 47,178,179,180,181,182,183,184,185,124, 44, 37, 95, 62, 63,//96-111
		186,187,188,189,190,191,192,193,194, 96, 58, 35, 64, 39, 61, 34,//112-127
		195, 97, 98, 99,100,101,102,103,104,105,196,197,198,199,200,201,//128-143
		202,106,107,108,109,110,111,112,113,114,203,204,205,206,207,208,//144-159
		209,126,115,116,117,118,119,120,121,122,210,211,212,213,214,215,//160-175
		216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,//176-191
		123, 65, 66, 67, 68, 69, 70, 71, 72, 73,232,233,234,235,236,237,//192-207
		125, 74, 75, 76, 77, 78, 79, 80, 81, 82,238,239,240,241,242,243,//208-223
		92,159, 83, 84, 85, 86, 87, 88, 89, 90,244,245,246,247,248,249,//224-239
		48, 49, 50, 51, 52, 53, 54, 55, 56, 57,250,251,252,253,254,255 //240-255
	};
	unsigned int i = get_global_id(0);
	uchar8 orig_char;
	uchar8 xformd_char;

	orig_char = src[i];

	/*xformd_char.s0 = e2a_lut[(int) orig_char.s0]; 
	xformd_char.s1 = e2a_lut[(int) orig_char.s1]; 
	xformd_char.s2 = e2a_lut[(int) orig_char.s2]; 
	xformd_char.s3 = e2a_lut[(int) orig_char.s3]; 
	xformd_char.s4 = e2a_lut[(int) orig_char.s4]; 
	xformd_char.s5 = e2a_lut[(int) orig_char.s5]; 
	xformd_char.s6 = e2a_lut[(int) orig_char.s6]; 
	xformd_char.s7 = e2a_lut[(int) orig_char.s7];*/
	xformd_char.s0 = e2a_lut[orig_char.s0]; 
	xformd_char.s1 = e2a_lut[orig_char.s1]; 
	xformd_char.s2 = e2a_lut[orig_char.s2]; 
	xformd_char.s3 = e2a_lut[orig_char.s3]; 
	xformd_char.s4 = e2a_lut[orig_char.s4]; 
	xformd_char.s5 = e2a_lut[orig_char.s5]; 
	xformd_char.s6 = e2a_lut[orig_char.s6]; 
	xformd_char.s7 = e2a_lut[orig_char.s7];
	//printf("i = %d, src[%d] = %s\n", i, i, &xformd_char);

	dst[i] = xformd_char;
	/*dst[i].s0 = (char) xformd_char.s0;
	dst[i].s1 = (char) xformd_char.s1;
	dst[i].s2 = (char) xformd_char.s2;
	dst[i].s3 = (char) xformd_char.s3;
	dst[i].s4 = (char) xformd_char.s4;
	dst[i].s5 = (char) xformd_char.s5;
	dst[i].s6 = (char) xformd_char.s6;
	dst[i].s7 = (char) xformd_char.s7;*/

}	 

