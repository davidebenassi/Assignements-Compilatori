# Third Assignment

This folder contains some files to implement the Loop Invariant Code Motion optimization pass, tested for LLVM 17.0.6.

## Loop Invariant Code Motion
The aim of this optimization pass is to identify **Loop Invariant** Instructions and move them outside the Loop, to the Loop Preheader Block. \
\
**Loop Invariant Instruction:** a Loop Invariant Instruction is an instruction within a loop that produces the same result on each iteration of the loop and can therefore be moved outside the loop without affecting the program's correctness.

## Build the Optimization Pass

### Files Structure 
Refering to [this](https://github.com/davidebenassi/llvm17.0.6-Installation?tab=readme-ov-file#folder-structure) folder structure, files for optimization must be placed in the following locations:

- **LoopWalk.cpp** 	- _SRC/llvm/lib/Transforms/Utils/_
- **LoopWalk.h** 	- _SRC/llvm/include/llvm/Transforms/Utils/_
- **CMakeLists.txt** 	- _SRC/llvm/lib/Transforms/Utils/_
- **PassRegistry.def** 	- _SRC/llvm/lib/Passes/_
- **PassBuilder.cpp** 	- _SRC/llvm/lib/Passes/_

Inside ```$(ROOT/BUILD)``` folder run : ```make opt```

## Run the Optimization Pass
```bash
(ROOT/BUILD) $ ./bin/opt -p loop-opts <src_file.ll> -S -o <optimized_file.ll>
```
As source file you can use the ```test/licm.ll``` file.

### Generate .ll file
The ```test/licm.ll``` is generated  from ```test/licm.c```:

 1. ```(ROOT/TEST) $ clang -O0 -emit-llvm -S -c licm.c -o licm.ll``` 

 2. ```(ROOT/BUILD) $ ./bin/opt -p mem2reg ../TEST/licm.ll -o ../TEST/licm.bc```

 3. ```(ROOT/TEST) $ llvm-dis licm.bc -o licm.ll```

 4. **Optional** ```(ROOT/TEST) $ rm licm.bc```


## Contributors
 - Davide Benassi [[@davidebenassi]](https://github.com/davidebenassi)
 - Ivan Giove [[@giove29]](https://github.com/giove29)
