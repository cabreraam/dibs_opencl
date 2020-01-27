// Generates the TIFF file at a given image number, writes its contents
// into an output buffer.

// IDX3-UBYTE SPECIFIED BYTE OFFSETS
#define IDX3_OFFSET_ARRAY_START 16

#ifndef WGSIZE
	#define WGSIZE 64 
#endif

#ifndef NUMCOMPUNITS
	#define NUMCOMPUNITS 1 
#endif

#ifndef NUMSIMD
	#define NUMSIMD 1
#endif

__kernel void generate_tiff_file(
    const uint numImages,
    const uint fileLength,
    const uint pixelCount,
    const uint headerBufferLength,
    const uint footerBufferLength,
    global const uchar *headerBuffer,
    global const uchar *footerBuffer,
    global const uchar *pixelDataBuffer,
    global uchar *tiffFileOutputBuffer)
{

    uint imageNumber = get_group_id(0);
    uint localId = get_local_id(0);
    uint localSize = get_local_size(0);
    if (imageNumber < numImages)
    {
        uint i, tiffOutputIndex = (imageNumber * fileLength);

        // copy header
        barrier(CLK_LOCAL_MEM_FENCE);
        for (i = localId; i < headerBufferLength; i += localSize)
        {
            tiffFileOutputBuffer[tiffOutputIndex + i] = headerBuffer[i];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
        tiffOutputIndex += headerBufferLength;

        // copy pixel data
        barrier(CLK_LOCAL_MEM_FENCE);
        for (i = localId; i < pixelCount; i += localSize)
        {
            tiffFileOutputBuffer[tiffOutputIndex + i] = pixelDataBuffer[i + (imageNumber * pixelCount) + IDX3_OFFSET_ARRAY_START];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
        tiffOutputIndex += pixelCount;

        // copy TIFF footer
        barrier(CLK_LOCAL_MEM_FENCE);
        for (i = localId; i < footerBufferLength; i += localSize)
        {
            tiffFileOutputBuffer[tiffOutputIndex + i] = footerBuffer[i];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
        tiffOutputIndex += footerBufferLength;
    }
}
