.. _sdk_examples:

Technical Exercies
========

The technical exercie is to write the programs that should
compile and run on GAP8 GVSOC

Before compiling and running the applications, please make sure:
- The toolchain has been installed successfully.\
- The SDK has been built without error.\
* The shell environment has been configured correctly for the GAP8 SDK.

#How to compile and run the programms on GVSOC:
On the terminal: cd ../task1/, and then execute the following commands: 

 .. code-block:: bash

    # Run on GVSoC
    make clean all run platform=gvsoc

    # Run on real board
    make clean all run platform=board

    # You can run and compile using a bash script
    ./launch.sh
