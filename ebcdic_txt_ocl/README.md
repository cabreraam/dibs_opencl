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

TODO: Fill this out.
Figure out what directory you want the kernel bitstream to live.
Show what a build command might look like.

```
Build command
```

Use the build script. One for MWI (`mwi_path/build_vlab_all.sh`) and 
SWI (`swi_path/build_vlab_all.sh`).


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

