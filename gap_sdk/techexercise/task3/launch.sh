#!/bin/bash

# Configure SDK
# Configure the shell environment correctly for the GAP SDK 
source ~hanle/gap_sdk/sourceme.sh

# Compiles and launches the code.
make clean all run PMSIS_OS=freertos platform=gvsoc 

