#!/bin/bash

# Anthony Cabrera
# Jan 2019
# This script sweeps across all of the coarse-grained OpenCL SWI parameters.
# It is specific to ebcdic_txt but it is extensible to other apps.

MWI_OR_SWI=swi
KERNEL_PATH=./device
DATA_DIR=${MWI_OR_SWI}_data
APP_NAME=idx3_to_tiff
APP_BIN=./idx3_to_tiff_cl

mkdir -p ${DATA_DIR}

KERNEL_PREFIX=idx3_to_tiff_${MWI_OR_SWI}

# coarse-grained knobs
#UNROLL=( 1 2 4 8 16 32 64 128 256 512 1024 )
#UNROLL=( 8 16 32 64 128 )
UNROLL=( 1 2 4 )
# 256, 512, and 1024 not built yet, and not sure if it will actually build

NUM_RUNS=2

echo "Running data for ${APP_NAME}"
echo "UNROLL = { ${UNROLL[*]} }"

for unroll in "${UNROLL[@]}"
do
	echo -e "On unroll = ${unroll}"
	for i in $(seq 1 ${NUM_RUNS})
	do
		echo -e "\tRun ${i}"

		KERNEL=${KERNEL_PATH}
		KERNEL+=/${KERNEL_PREFIX}_${unroll}
		KERNEL+=/${KERNEL_PREFIX}_${unroll}.aocx

		echo "${APP_BIN} \
			${KERNEL} \
			>> ${DATA_DIR}/${KERNEL_PREFIX}_${unroll}.out"
	done
done

echo "Done!"
