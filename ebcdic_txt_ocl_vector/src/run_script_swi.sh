#!/bin/bash

# Anthony Cabrera
# Jan 2019
# This script sweeps across all of the coarse-grained OpenCL SWI parameters.
# It is specific to ebcdic_txt but it is extensible to other apps.

MWI_OR_SWI=swi
KERNEL_PATH=/homes/cabreraam/dibs/ebcdic_txt
DATA_DIR=${MWI_OR_SWI}_data
APP_NAME=e2a
APP_BIN=./conv_cl

mkdir -p ${DATA_DIR}

KERNEL_PREFIX=${APP_NAME}_${MWI_OR_SWI}

# coarse-grained knobs
#UNROLL=( 1 2 4 8 16 32 64 128 256 512 1024 )
UNROLL=( 1024 512 256 128 64 32 16 8 4 2 1 )

NUM_RUNS=2

echo "Running data for ${APP_NAME}"
echo "UNROLL = { ${UNROLL[*]} }"

#NOTE -w (workgroup) option isn't necessary for the kernel but some value is 
# required by the app even though it does not affect the SWI implementation 
# at all

for unroll in "${UNROLL[@]}"
do
	echo -e "On unroll = ${unroll}"
	for i in $(seq 1 ${NUM_RUNS})
	do
		echo -e "\tRun ${i}"

		KERNEL=${KERNEL_PATH}
		KERNEL+=/e2a_unroll_${unroll}
		KERNEL+=/e2a_unroll_${unroll}.aocx

		# Run the application!
		echo "${APP_BIN} \
			-i ../input/big_sonnetsE001.txt \
			-o ${MWI_OR_SWI} \
			-s ${KERNEL} \
			-w 1 \
			>> ${DATA_DIR}/${APP}_${MWI_OR_SWI}_${unroll}.out"

			done
done

echo "Done!"
