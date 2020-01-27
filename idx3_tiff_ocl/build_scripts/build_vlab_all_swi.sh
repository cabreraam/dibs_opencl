#!/bin/bash

# Anthony Cabrera
# 1/3/2020
# This script is for automating the kernel building process. It is similar to my
# IWOCL work. If using the Intel vLab servers, this can be issued in the head
# node of those servers.

#To echo commands as they execute, uncomment the line below 
#set -x


APP_NAME="idx3_to_tiff"
MWI_OR_SWI=swi
KERNEL_DIR_PREFIX=kernels/${APP_NAME}_${MWI_OR_SWI}

OCL_SRC_FILE=kernels/idx3-to-tiff-swi.cl

# coarse-grained knobs
#UNROLL=( 1 2 4 8 16 32 64 128 256 512 1024 ) 
#UNROLL=( 8 16 32 64 128 256 512 1024 ) 
UNROLL=( 1 2 4 ) 

echo "Start script to build some kernels"
echo ""

#naming convention: swi_<unroll>
for unroll in "${UNROLL[@]}"
do

	echo "Enqueueing build for ${APP_NAME} kernel with UNROLL=${unroll}"

	KERNEL_DIR=${KERNEL_DIR_PREFIX}_${unroll}
	KERNEL=${KERNEL_DIR}

	mkdir -p ${APP_KERNEL_DIR_PREFIX}_swi_${unroll}

	qsub-aoc ${OCL_SRC_FILE} \
		-DUNROLL=${unroll} \
		-o ${KERNEL_DIR}/${KERNEL}.aocx \
		--board bdw_fpga_v1.0 -v --report -g

done

