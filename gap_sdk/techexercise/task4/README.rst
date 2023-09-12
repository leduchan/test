Task 4 
===========

Requirements
------------

No specific requirement. This example should run without issue on all chips/boards/OSes.

Description
-----------

The program is to implement:
        - Create and initialize two square matrices in L2 memory (minimum size 64x64).
        - Copy these two matrices from L2 memory to L1 memory.
        - Addition of the two copied matrices.
        - Multiplication of the two copied matrices.
        - The two result matrices allocated in L1 memory. 
        - Copy the two result matrices from L1 memory to L2 memory.

Extend the previous `launch.sh` script to accept a matrix size parameter:
   For example, entering `./launch.sh -s 128` will select a matrix size of
   128x128. Use a default value of 64 when size is not specified.   
        
You can compile and run this program on GAP8 GVSoC or your board:

.. code-block:: bash

    # Run on GVSoC
    make clean all run platform=gvsoc

    # Run on real board
    make clean all run platform=board

    # You can run and compile using a bash script
    ./launch.sh

When compile and run this program using './launch.sh -s 128', there is the following error
that needs time to fix it:
        hanle@hanle:~/gap_sdk/testHan/task4$ ./launch.sh -s 128
        Select the target : 
        	1 - GAPOC_B_SPI_V2
        	2 - GAPOC_B_V2
        	3 - GAPUINO_V3
        	4 - GAPUINO_V3_SPI
        
        The target board you have chosen is : gapuino, GAP8_V3.
            CC test3.c
        In file included from /home/hanle/gap_sdk/rtos/freeRTOS/vendors/gwt/pmsis/include/pmsis.h:52:0,
                         from test3.c:18:
        /home/hanle/gap_sdk/rtos/freeRTOS/vendors/gwt/pmsis/include/pmsis/backend/implementation_specific_defines.h:13:10: fatal error: device/system_gap9.h: No such file or directory
         #include "device/system_gap9.h"
                  ^~~~~~~~~~~~~~~~~~~~~~
        compilation terminated.
        make: *** [/home/hanle/gap_sdk/rtos/freeRTOS/vendors/gwt/rules/freeRTOS_rules.mk:411: /home/hanle/gap_sdk/testHan/task4/BUILD/GAP8_V3/GCC_RISCV_FREERTOS/test3.o] Error 1

You should have an output looking like this (order may vary):

.. code-block::

     *** Matrix Copy on GAP8 GVSoC ***
    
    Perf : 20951 cycles Timer : 28173 cycles
    Copied Matrix (matrix1) in L1 memory:
    	...
    Copied Matrix (matrix2) in L1 memory:
    	...
    
    Matrix (matrix1) copy operation is successful.
    Matrix (matrix2) copy operation is successful.
    
    The addition result matrix is: 
    	...
    The multiplication result matrix is:
    	...
    
    The addition result matrix copy operaion is successful.
    The multiplication result matrix copy operation is successful.
    
    Test success !
