file(REMOVE_RECURSE
  "../lib/libcgra-me.pdb"
  "../lib/libcgra-me.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/cgra-me.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
