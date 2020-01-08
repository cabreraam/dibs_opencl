// Copies 64 byte lines from src to dst

#define IDX3_OFFSET_ARRAY_START 16

__kernel void generate_tiff_file(
    const uint numImages,
    const uint fileLength,
    const uint pixelCount,
    const uint headerBufferLength,
    const uint footerBufferLength,
    global const uchar * restrict headerBuffer,
    global const uchar * restrict footerBuffer,
    global const uchar * restrict pixelDataBuffer,
    global uchar * restrict tiffFileOutputBuffer)
{

	uint tiffOutputIndex_loop1 = 0;
	uint tiffOutputIndex_loop2 = 0;
	uint tiffOutputIndex_loop3 = 0;

	for(uint i = 0; i < numImages; ++i){
		tiffOutputIndex_loop1 = (i * fileLength);
		
		//CopyHeader
		#pragma unroll 16
		for(uint j = 0; j < headerBufferLength; ++j){
			tiffFileOutputBuffer[tiffOutputIndex_loop1 + j] = headerBuffer[j];
		}	
		tiffOutputIndex_loop2 = tiffOutputIndex_loop1 + headerBufferLength;
		//CopyPixelData
		#pragma unroll 64
		for (uint k = 0; k < pixelCount; ++k)
        {
            tiffFileOutputBuffer[tiffOutputIndex_loop2 + k] = pixelDataBuffer[k + (i * pixelCount) + IDX3_OFFSET_ARRAY_START];
        }
		tiffOutputIndex_loop3 = tiffOutputIndex_loop2 + pixelCount;
		//Copy footer
		#pragma unroll 64
		for (uint l = 0; l < footerBufferLength; ++l)
		{
			tiffFileOutputBuffer[tiffOutputIndex_loop3 + l] = footerBuffer[l];
		}
		//Return to start the next image
	}			

}

