# DIBS Application: ebcdic->txt

## Building 

### Building Host Code Instructions:

Navigate to the src directory. 

For building the *MWI* version, run the command:
```
make HARP=1 MWI=1
```
This will create the binary file `conv_cl_mwi`


For building the *SWI* version, run the command:
```
make HARP=1 
```
This will create the binary file `conv_cl`


### Building FPGA Design Instructions 

The following build scripts can be used to sweep coarse-grained design knobs
for the `ebcdic_txt` application. 

For MWI: 
([build_vlab_all_mwi.sh](build_scripts/build_vlab_all_mwi.sh)

and SWI: 
([build_vlab_all_swi.sh](build_scripts/build_vlab_all_swi.sh)

*NOTE:* Before doing this, you will need to update the `KERNEL_DIR_PREFIX`
variable in each script to reflect where you want the built kernel bitstreams to
live.

## Execution Instructions:

The name of the application binary is:
conv

This code takes the following inputs from the command line:
```
-i 	'i'nput file (in its entirety, e.g., bigsonnets000.txt)
-o 	'o'utput file (.txt will be appended to this name, 
			so outfile --> outfile.txt)
-s 	kernel 's'ource file; for FPGA kernels, use path+aocx file
-w 	local 'w'orkgroup size. Must always be specified. For SWI kernels, the
			parameter will not affect runtime but still must be specified. 
```

To execute the binary, run the command in the form::
```
./conv [-i inputfile] [-o outputfile] [-s path/to/binary.aocx] [-w IntVal]
```

