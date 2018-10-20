# fastfillarray
Constant time initialization array

## Overview

This is the implementation of constant time intiialization data structure with O(1) read and write
complexity.

The goal of this project is to implement an array with single bit of memory redundancy, as described
in [1]. Since the solution uses underlying, so called, folks implementation, which is also constant
time initialization/read/write, but redundant on the mememory, the data structure invented by
Navarro has been chosen. See [2] for details.


[1] https://people.seas.harvard.edu/~minilek/publications/papers/fillable.pdf
[2] https://www.dcc.uchile.cl/~gnavarro/ps/sccc12.pdf

##Implementation

The implementation is a C++ template. It uses operator[] as a read, but needs to define explicit
write and init calls.
