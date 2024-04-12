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

### Dead Code Elimination
The code performs a primordial implementation of a **Dead Code Elimination** after the three *Local Optimization Passes* described above.

### Known Issue
This **Dead Code Elimination** only run once. This results in some *Dead Code Instructions* that remains after running the optimization passes.

**Example: Code that shows the highlighted problem**

```
define dso_local i32 @foo(i32 noundef %0, i32 noundef %1) {
  %3 = sub nsw i32 %1, 2
  %4 = add nsw i32 %3, 2
  %5 = mul nsw i32 %4, 2
  %6 = shl i32 %0, 1
  %7 = sdiv i32 %5, 4
  %8 = mul nsw i32 1, %4 
  ret i32 %7
}
```
**Code After Optimization Passes**
```
define dso_local i32 @foo(i32 noundef %0, i32 noundef %1) {
  %3 = sub nsw i32 %1, 2            ; Dead Code Instruction not eliminated
  %4 = shl i32 %1, 1
  %5 = lshr i32 %4, 2
  ret i32 %5
}

```

## Run the Optimization Pass

### Files Structure 
Refering to [this](https://github.com/davidebenassi/llvm17.0.6-Installation?tab=readme-ov-file#folder-structure) folder structure, files for optimization must be placed in the following locations:
- **LocalOpts.cpp** 	- _SRC/llvm/lib/Transforms/Utils/_
- **LocalOpts.h** 	- _SRC/llvm/include/llvm/Transforms/Utils/_
- **CMakeLists.txt** 	- _SRC/llvm/lib/Transforms/Utils/_
- **PassRegistry.def** 	- _SRC/llvm/lib/Passes/_
- **PassBuilder.cpp** 	- _SRC/llvm/lib/Passes/_

**NOTE:** substitute the already present files.
### Command Line - Linux
1. Once you have placed all files correctly move into ```$ROOT/BUILD``` folder.
2. Compile the optimization tool - ```make opt```
3. Run your pass - ```./bin/opt -p localopts <src_file.ll> -S -o <optimized_file.ll>```
4. (Optional) Install your pass - ```make install```
## Contributors
 - Davide Benassi [[@davidebenassi]](https://github.com/davidebenassi)
 - Ivan Giove [[@giove29]](https://github.com/giove29)
