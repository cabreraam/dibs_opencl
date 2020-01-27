# DIBS Application: idx3->tiff

## Building 

### Building Host Code Instructions:

There are two `Makefile`s in this directory. The default `Makefile` compiles
for the Intel HARPv2 platform. `Makefile.cpu` compiles for OpenCL compliant
CPUs. From this point forward, the build instructions are specifically for the
Intel HARPv2 system.

For building the *MWI* version, run the command:
```
make FPGA=1 HARP=1 MWI=1
```
This will create the binary file `idx3_to_tiff_cl_mwi`


For building the *SWI* version, run the command:
```
make FPGA=1 HARP=1 
```
This will create the binary file `idx3_to_tiff_cl`

Both binaries, by default, are created in the current directory.


### Building FPGA Design Instructions 

The following build scripts can be used to sweep coarse-grained design knobs
for the `idx3_to_tiff` application. 

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

To execute any version of the application, navigate to the 
[top level directory](./)
directory.

To execute the MWI binary, run the command in the form::
```
./idx3_to_tiff_cl_mwi [path/to/binary.aocx] [wgsize]
```
where `wgsize` is the local work group size that the provided kernel binary was
built for.


To execute the SWI binary, run the command in the form::
```
./idx3_to_tiff_cl [path/to/binary.aocx] 
```

There are two scripts provided for running the MWI and SWI 
versions--[run_script_mwi.sh](./src/run_script_mwi.sh) and  
[run_script_swi.sh](./src/run_script_mwi.sh), respectively--multiple
times. The number of times the application is run is set by the script variable
`NUM_RUNS`. The default value is 100 times. The `stdout` outputs are directed
to a data directory (`DATA_DIR`) defined in the run scripts. 
