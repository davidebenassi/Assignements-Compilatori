# First Assignment

This folder contains some files to implement the following optimization passes, tested for LLVM 17.0.1.

## Passes

**Local optimization passes**
 
 1. *Algebraic Identity*
    - $x + 0 = 0 + x \Rightarrow x$
    - $x \times 1 = 1 \times x \Rightarrow x$

 2. *Advanced Strength Reduction*
    - $15 \times x = x \times 15 \Rightarrow (x << 4) – x$
    - $y = x / 8 ⇒ y = x >> 3$

 3. *Multi-Instruction Optimization*	
    - $a = b + 1, c = a − 1 ⇒ a = b + 1, c = b$


<!-- TODO: write this section
## Run the Optimization Pass

### Files Structure 

### Command Line - Linux
-->

## Contributors
 - Davide Benassi [@davidebenassi]
 - Ivan Giove [@giove29]
