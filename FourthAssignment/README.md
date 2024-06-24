# Fourth Assignment

This folder contains some files to implement the Loop Fusion optimization pass, tested for LLVM 17.0.6.

## Loop Fusion

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

