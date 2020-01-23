# DIBS OpenCL FPGA Implementations

This repository contains OpenCL FPGA host and device code for the DIBS 
benchmarking suite targeting the Intel HARPv2 CPU+FPGA platform. This should be
portable to other FPGA devices compliant with the OpenCL standard, with the
exception of changing SVM allocated memory to traditional cl_mem objects.

## Build Information
Building kernels for each application will use the Intel FPGA OpenCL SDK
offline compiler `aoc`. Because of the parameterizibility of the kernels, each
application directory has a build script (usually `build_vlab_all.sh`) that
will sweep a pre-determined design space for each kernel. That design space is
usually limited to constraints made by the tools (e.g., a compute unit cannot
be replicated more than 8 times) or ones encountered during the build process
(e.g., a particular parameter configuration is not synthesizable due to how
much of the FPGA's chip resources are required or how difficult it is to route
a resulting design). The build scripts, at their core, will feature a build
command that will resemble the following:

```
aoc kernel_file.cl -o desired_bitsream_path/your_bitstream_name.aocx \
	-DPARAM1=Int_Value -DPARAM2=Int_Value -DPARAM3=Int_Value \
	--board bdw_fpga_v1.0 -v --report -g 
```

## Running the Applications
Each application has their own specific running commands. Each application also
has scripts to run the different versions, e.g., `run_script_mwi.sh`. The
number of times to run each application is a variable `NUM_RUNS` that can be
configured by the user. 

## Publications

This work is currently under submission.

## Contact

Issues and pull requests are welcome!
The current maintainers of this project are listed below.

Anthony Cabrera and Roger Chamberlain
Department of Computer Science and Engineering
James McKelvey School of Engineering
{firstinitiallastname, firstname} at wustl dot edu
