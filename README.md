# Game-of-Life-in-C-Parallel-

This is an implementation of John Conway's Game of Life in C using the OpenMP library to parallelize the execution.

The compiler flag -DPRINT_GEN is used to indicate that the generations are required to be printed. 
By default, they are not printed as they can result in huge output sizes if the dimensions and/or max generations are high.
