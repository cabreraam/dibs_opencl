#!/bin/bash

# Anthony Cabrera
# 1/3/2020
# This script is for automating the kernel building process. It is similar to my
# IWOCL work. If using the Intel vLab servers, this can be issued in the head
# node of those servers.

#To echo commands as they execute, uncomment the line below 
#set -x

APP_NAME="e2a"
MWI_OR_SWI=mwi
KERNEL_DIR_PREFIX=${APP_NAME}_${MWI_OR_SWI}

OCL_SRC_FILE=e2a.cl

# coarse-grained knobs
UNROLL=( 1 2 4 8 16 32 64 128 256 512 1024 )

echo "Start script to build some kernels"
echo ""

#naming convention: unroll_<unroll>
for unroll in "${UNROLL[@]}"
do

	echo "Enqueueing build for ${APP_NAME} kernel with UNROLL=${unroll}"

	KERNEL_DIR=${KERNEL_DIR_PREFIX}_unroll_${unroll}
	KERNEL=${KERNEL_DIR}

	mkdir -p ${KERNEL_DIR}
	qsub-aoc ${OCL_SRC_FILE} \
		-DUNROLL=${unroll} \
		-o ${KERNEL_DIR}/${KERNEL}.aocx \
		--board bdw_fpga_v1.0 -v --report -g

done

