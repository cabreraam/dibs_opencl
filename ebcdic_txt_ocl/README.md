# DIBS Application: ebcdic->txt

## Building 

### Building Host Code Instructions:

Navigate to the [src](./src) directory. 

For building the *MWI* version, run the command:
```
make FPGA=1 HARP=1 MWI=1
```
This will create the binary file `conv_cl_mwi`


For building the *SWI* version, run the command:
```
make FPGA=1 HARP=1 
```
This will create the binary file `conv_cl`


### Building FPGA Design Instructions 

The following build scripts can be used to sweep coarse-grained design knobs
for the `ebcdic_txt` application. 

For MWI: 
([build_vlab_all_mwi.sh](build_scripts/build_vlab_all_mwi.sh))

and SWI: 
([build_vlab_all_swi.sh](build_scripts/build_vlab_all_swi.sh))

*NOTE:* Before doing this, you will need to update the `KERNEL_DIR_PREFIX`
variable in each script to reflect where you want the built kernel bitstreams to
live. By default, they will be built in the [kernels](./kernels) directory.

Paths in the scripts are relative to the top level directory (i.e., by 
default, they need to be issued from the directory where this README.md lives).


## Execution Instructions:

To execute any version of the application, navigate to the [src](./src)
directory.

This code takes the following inputs from the command line:
```
-i 	'i'nput file (in its entirety, e.g., bigsonnets000.txt)
-o 	'o'utput file (.txt will be appended to this name, 
			so outfile --> outfile.txt)
-s 	kernel 's'ource file; for FPGA kernels, use path+aocx file
-w 	local 'w'orkgroup size. Must always be specified. For SWI kernels, the
			parameter will not affect runtime but still must be specified. 
```

To execute the MWI binary, run the command in the form::
```
./conv_cl_mwi [-i inputfile] [-o outputfile] [-s path/to/binary.aocx] [-w IntVal]
```

To execute the SWI binary, run the command in the form::
```
./conv_cl [-i inputfile] [-o outputfile] [-s path/to/binary.aocx] [-w 1]
```

There are two scripts provided for running the MWI and SWI 
versions--[run_script_mwi.sh](./src/run_script_mwi.sh) and  
[run_script_swi.sh](./src/run_script_mwi.sh), respectively--multiple
times. The number of times the application is run is set by the script variable
`NUM_RUNS`. The default value is 100 times. 
