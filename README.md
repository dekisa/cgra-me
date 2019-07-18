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
[src/core/Module.cp](src/core/Module.cp)

### Bitstream
generating bitstream\
[src/core/BitStream.cpp](src/core/BitStream.cpp)

hardcoded data for bitstream\
[inc/CGRA/user-inc/NESWbitstream](inc/CGRA/user-inc/NESWbitstream)

handling configuration bits\
[src/core/BitSetting.cpp](src/core/BitSetting.cpp)

calling bitstream generator\
[src/run/cgrame.c](src/run/cgrame.c)

### Dataflow graph
parsing dfg to get constant configuration value when mapping dfg\
[src/core/ModuleFuncUnit.cpp](src/core/ModuleFuncUnit.cpp)

changing dfg node label data when generating dfg\
[src/core/OpGraph.cp](src/core/OpGraph.cp)

Dataflow graph\
[llvm-passes/DFG/DFGGeneration.cpp](llvm-passes/DFG/DFGGeneration.cpp)
