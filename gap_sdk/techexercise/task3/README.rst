Task 3
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

You can compile and run this program on GAP8 GVSoC or your board:

.. code-block:: bash

    # Run on GVSoC
    make clean all run platform=gvsoc

    # Run on real board
    make clean all run platform=board

    # You can run and compile using a bash script
    ./launch.sh

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
