#!/bin/bash

# Anthony Cabrera
# Jan 2019
# This script sweeps across all of the coarse-grained OpenCL MWI parameters.
# It is specific to ebcdic_txt but it is extensible to other apps.

MWI_OR_SWI=mwi
KERNEL_PATH=/homes/cabreraam/dibs/ebcdic_txt_mwi
DATA_DIR=mwi_data
APP_NAME=e2a
APP_BIN=./conv_cl_mwi

mkdir -p ${DATA_DIR}

KERNEL_PREFIX=${APP_NAME}_${MWI_OR_SWI}


# coarse-grained knobs
#WGSIZE=( 64 256 512 ) 
WGSIZE=( 128 ) 
NUMCOMPUTEUNITS=( 1 2 4 8 )
NUMSIMD=( 1 2 4 8 16 )

NUM_RUNS=2

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
			touch e2a_mwi_${wg}_${ncu}_${ns}.out
			for i in $(seq 1 ${NUM_RUNS})
			do
				echo -e "\tRun ${i}"

				KERNEL=${KERNEL_PATH}
				KERNEL+=/${KERNEL_PREFIX}_${wg}_${ncu}_${ns}
				KERNEL+=/${KERNEL_PREFIX}_${wg}_${ncu}_${ns}.aocx

				# Run the application!
				echo "${APP_BIN} \
					-i ../input/big_sonnetsE001.txt \
					-o mwi \
					-s ${KERNEL} \
					-w ${wg} >> ${DATA_DIR}/${KERNEL_PREFIX}_${wg}_${ncu}_${ns}.out"
			done
		done
	done
done
