Hello World
===========

Requirements
------------

No specific requirement. This example should run without issue on all chips/boards/OSes.

Description
-----------

The program is a classic Hello World.
It prints an hello world string on all available cores.

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

     *** PMSIS HelloWorld ***
    
    Entering main controller
    [32 0] Hello World!
    Cluster master core entry
    Hello world [Cluster 0, Core 0]
    Hello world [Cluster 0, Core 4]
    Hello world [Cluster 0, Core 5]
    Hello world [Cluster 0, Core 6]
    Hello world [Cluster 0, Core 3]
    Hello world [Cluster 0, Core 7]
    Hello world [Cluster 0, Core 1]
    Hello world [Cluster 0, Core 2]
    Cluster master core exit
    Test success!

