#!/bin/bash

# Anthony Cabrera
# 1/3/2020
# This is a script I wrote in my IWOCL paper that I'm reusing for DIBS stuff!
# A little different though; we're not using screen terminals, because we don't
# have direct access to the build server. Here, we just use qsub. 

echo "Start script to build some kernels"
echo ""

APP_NAME="ebcdic_to_ascii"
APP_KERNEL_DIR="e2a"

#UNROLL=( 1 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768)
UNROLL=( 2048 4096 8192 16384 32768 )


for unroll in "${UNROLL[@]}"
do


	echo "Enqueueing build for ${APP_NAME} kernel with UNROLL=${unroll}"

	mkdir ${APP_KERNEL_DIR}_unroll_${unroll}
	qsub-aoc e2a.cl -DUNROLL=${unroll} -o \
		e2a_unroll_${unroll}/e2a_unroll_${unroll}.aocx \
		--board bdw_fpga_v1.0 -v --report -g


done

