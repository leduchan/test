# Technical Exercise 
The technical exercies is given by GreeWaves Technologies. It consists of 5 taks, which are contained 
in the different directories named task1, task2, task3, task4 and task5.

## Setup OS Environment:
In this framework, we use a Ubuntu 20.04 running on virtual machine from window 10.  
* Step 1: Download and install Oracle VM VirtualBox from https://www.virtualbox.org/wiki/Downloads
* Step 2: Download .iso ubuntu image https://ubuntu.com/download/desktop
* Step 4: Create, Configure and setup linux Ubuntu 20.04 running on VM VirtualBox.
    
## Setup GAP SDK Environment
Goto the link: https://github.com/GreenWaves-Technologies/gap_sdk and follow the instructions to 
install all packages for OS, download and install the toolchain, configure the SDK, install SDK, etc.

## How to start compiling and running the programs on GVSOC:

Before compiling and running the applications, please make sure:
* The toolchain has been installed successfully.
* The SDK has been built without error.
* The shell environment has been configured correctly for the GAP8 SDK.

## How to compile and run the programs on GVSOC:
The tasks are executed and contained in the director gap_sdk/techexercise/.

```bash
cd .../gap_sdk/techexercise/task1/
```
Then execute the command for compiling and running the program on GVSOC:

```bash
make clean all run PMSIS_OS=freertos platform=gvsoc
```
or 

```bash
./launch.sh
```     
