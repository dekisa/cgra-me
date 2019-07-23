## List of modifications

### Arhcitecture

description of architecture fu\
[src/modules/NESWArchFU.cpp](src/modules/NESWArchFU.cpp)

description of architecutre\
[src/archs/NESWArch.cpp](src/archs/NESWArch.cpp)

adding new fu\
[inc/CGRA/user-inc/UserModules.h](inc/CGRA/user-inc/UserModules.h)

adding new architecture\
[inc/CGRA/user-inc/UserArchs.h](inc/CGRA/user-inc/UserArchs.h)

fixing the io problem\
[src/core/Module.cpp](src/core/Module.cpp)

### Bitstream
generating bitstream\`
[src/core/BitStream.cpp](src/core/BitStream.cpp)

hardcoded data for bitstream\
[inc/CGRA/user-inc/NESWbitstream.h](inc/CGRA/user-inc/NESWbitstream.h)

handling configuration bits\
[src/core/BitSetting.cpp](src/core/BitSetting.cpp)

calling bitstream generator\
[src/run/cgrame.cpp](src/run/cgrame.cpp)

### Dataflow graph
parsing dfg to get constant configuration value when mapping dfg\
[src/core/ModuleFuncUnit.cpp](src/core/ModuleFuncUnit.cpp)

changing dfg node label data when generating dfg\
[src/core/OpGraph.cpp](src/core/OpGraph.cpp)

Dataflow graph\
[llvm-passes/DFG/DFGGeneration.cpp](llvm-passes/DFG/DFGGeneration.cpp)

## List of limitations
- C file containing functions to be converted to DFG should be in separate folder inside `benchmarks/userbench`. Name of the C file should be the same as the folder name.
- `Makefile` should containg the line including `rules.mk` ([example](https://github.com/dekisa/cgra-me/blob/DFG_runonfunction_const_arg_data/benchmarks/userbench/functions_test/Makefile#L2))
- Covert C functions to DFG using `make` command
- Conert DFGs to bitstream using `mappit.sh`, see [example](https://github.com/dekisa/cgra-me/blob/DFG_runonfunction_const_arg_data/benchmarks/userbench/functions_test/mappit.sh)
- Functions to be converted to DFG should be marked with a tag ([example](https://github.com/dekisa/cgra-me/blob/DFG_runonfunction_const_arg_data/benchmarks/userbench/functions_test/functions_test.c#L4)) and that tag should appear in `LOOP_TAGS` variable ([example](https://github.com/dekisa/cgra-me/blob/DFG_runonfunction_const_arg_data/benchmarks/rules_functions_test.mk#L14))
- Function to be converted to DFG should contain only one basic block. If conversion is implemented in two ways: using LLVM default optimization -O3 and using Oclacc if-conversion ported to LLVM 9.0.0, the pass is [here](https://github.com/dekisa/cgra-me/blob/DFG_runonfunction_const_arg_data/llvm-passes/ifconvert/ifconvert.cpp) and rules.mk using the pass is [here](https://github.com/dekisa/cgra-me/blob/DFG_runonfunction_const_arg_data/benchmarks/rules_mod.mk#L30). Both solutions work, but the second one can be easily modified
- If conversion generates `select` instruction which is not supported by mapper for now. Current FUs only have two inputs and select instruction has a third input, so we should implement a new FU having 3 inputs, change the bitstream format. Work on this is started on `experiment_select` branch.
- Function arguments are converted to inputs when they are read and to outputs when they are writen to. For inputs, reference by value is supported. For inputs and outputs, pointers and arrays are supported with limitations:
  - Pointers can be dereferenced only without any address ofset
  - Arrays can be multidimensional. Indexes can be constants and values passed as function arguments, other possibilities are not supported. Also, when using values passed as function arguments as array index, these must be `long int`.
  - Return instruction is converted to output
  - See [example](https://github.com/dekisa/cgra-me/blob/DFG_runonfunction_const_arg_data/benchmarks/userbench/load_store_tests/load_store_tests.c) function demonstrating all input and output possibilities
- For generating bitstream you should provide the filename using `--bitstream_output <filename>` option, see [example](https://github.com/dekisa/cgra-me/blob/DFG_runonfunction_const_arg_data/benchmarks/rules_bitstream.mk#L48). You should only use this option with NEWS Artchitecture
