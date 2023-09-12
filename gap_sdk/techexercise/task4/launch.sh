#!/bin/bash

# Default matrix size
MATRIX_SIZE=64

# Parse command line arguments
while getopts ":s:" opt; do
  case $opt in
    s)
      MATRIX_SIZE=$OPTARG
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      ;;
  esac
done

# Set the MATRIX_SIZE environment variable
export CFLAGS="-DMATRIX_SIZE=$MATRIX_SIZE"

# Configure SDK
# Configure the shell environment correctly for the GAP SDK 
source ~hanle/gap_sdk/sourceme.sh

# Directly source the board config.
# The SDK supports 2 boards (gapuino and gapoc).
# Each of them can use version 1 or version 2 of the GAP8 chip.
source ~hanle/gap_sdk/configs/gapuino_v3.sh
source ~hanle/gap_sdk/configs/gap8_v3.sh 

# Compiles and launches the code.
# make clean all run PMSIS_OS=freertos platform=gvsoc 
#make clean all run PMSIS_OS=freertos platform=gvsoc   
#make clean all run platform=gvsoc CFLAGS="-DMATRIX_SIZE=$MATRIX_SIZE"  
make clean all run PMSIS_OS=freertos platform=gvsoc CFLAGS="-DMATRIX_SIZE=$MATRIX_SIZE"  

