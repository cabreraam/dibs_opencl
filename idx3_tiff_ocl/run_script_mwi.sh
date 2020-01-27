#!/bin/bash

# Anthony Cabrera
# Jan 2019
# This script sweeps across all of the coarse-grained OpenCL MWI parameters.
# It is specific to idx3 to tiff, but it is extensible to other apps.

MWI_OR_SWI=mwi
KERNEL_PATH=./device_no_barrier
DATA_DIR=${MWI_OR_SWI}_data_no_bar
APP_NAME=idx3_to_tiff
APP_BIN=./idx3_to_tiff_cl_mwi_no_bar

KERNEL_PREFIX=${APP_NAME}_${MWI_OR_SWI}

mkdir -p ${DATA_DIR}

# coarse-grained knobs
WGSIZE=( 64 128 256 512 ) 
NUMCOMPUTEUNITS=( 1 2 4 8 )
NUMSIMD=( 1 2 4 8 16 )

NUM_RUNS=100

echo "Running data for ${APP_NAME}"
echo "WGSIZE = { ${WGSIZE[*]} }"
echo "NUMCOMPUTEUNITS { ${NUMCOMPUTEUNITS[*]} }"
echo "NUMSIMD = { ${NUMSIMD[*]} }"

for wg in "${WGSIZE[@]}"
do
	for ncu in "${NUMCOMPUTEUNITS[@]}"
	do
		for ns in "${NUMSIMD[@]}"
		do
			for i in $(seq 1 ${NUM_RUNS})
			do
				echo -e "\tRun ${i}"

				KERNEL=${KERNEL_PATH}
				KERNEL+=/${KERNEL_PREFIX}_${wg}_${ncu}_${ns}_no_barrier
				KERNEL+=/${KERNEL_PREFIX}_${wg}_${ncu}_${ns}.aocx

				# Run the application!
				echo "${APP_BIN} \
					${KERNEL} \
					${wg} \
					>> ${DATA_DIR}/${KERNEL_PREFIX}_${wg}_${ncu}_${ns}.out"
			done
		done
	done
done
