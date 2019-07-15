SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fno-rtti" )

add_library(ifconvert MODULE ifconvert.cpp HardwareModel.cpp LoopusUtils.cpp NameMangling.cpp)

target_link_libraries(
  ifconvert
  PRIVATE cgra-me
  )
