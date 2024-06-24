# Fourth Assignment

This folder contains some files to implement the Loop Fusion optimization pass, tested for LLVM 17.0.6.

## Loop Fusion
Loop fusion is a compiler optimization technique that combines two or more loops into a single loop to improve code efficiency.

### Fusion Conditions

 1. **Loop1** and **Loop2** must be "adjacent" : there cannot be any statements that execute between the end of Loop1 and the beginning of Loop2.
 2. **Loop1** and **Loop2** must iterate the same number of times.
 3. **Loop1** and **Loop2** must be "Control Flow Equivalent" : when one loop executes, the other loop also executes.
 4. There cannot be any negative distance dependencies between **Loop1** and **Loop2**.

**NOTE**: The fourth features is commented in the code beacause, in LLVM, the Dependence Analisys is very conservative, which makes it very complicated to find two completely independent instructions.
 

## Build the Optimization Pass

### Files Structure 
Refering to [this](https://github.com/davidebenassi/llvm17.0.6-Installation?tab=readme-ov-file#folder-structure) folder structure, files for optimization must be placed in the following locations:

- **LoopFusion.cpp** 	- _SRC/llvm/lib/Transforms/Utils/_
- **LoopFusion.h** 	- _SRC/llvm/include/llvm/Transforms/Utils/_
- **CMakeLists.txt** 	- _SRC/llvm/lib/Transforms/Utils/_
- **PassRegistry.def** 	- _SRC/llvm/lib/Passes/_
- **PassBuilder.cpp** 	- _SRC/llvm/lib/Passes/_

Inside ```$(ROOT/BUILD)``` folder run : ```make opt```

## Run the Optimization Pass
```bash
(ROOT/BUILD) $ ./bin/opt -p loop-fusion-pass <src_file.ll> -S -o <optimized_file.ll>
```
As source file you can use the ```test/loop.ll``` file.

### Generate .ll file
The ```test/loop.ll``` is generated  from ```test/loop.c```:

 1. ```(ROOT/TEST) $ clang -O0 -emit-llvm -S -c loop.c -o loop.ll``` 

 2. ```(ROOT/BUILD) $ ./bin/opt -p mem2reg ../TEST/loop.ll -S -o ../TEST/loop.ll```


## Contributors
 - Davide Benassi [[@davidebenassi]](https://github.com/davidebenassi)
 - Ivan Giove [[@giove29]](https://github.com/giove29)

