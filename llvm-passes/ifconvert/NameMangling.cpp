//===- NameMangling.h --------- -------------------------------------------===//
//===----------------------------------------------------------------------===//

#include <map>

#include "NameMangling.h"

#define DEF_BUILTIN(Mangled, Unmangled, NumParams, FuncCat) \
  { Mangled, BuiltInFunctionName(Mangled, Unmangled, NumParams, FuncCat) }

using namespace ocl::NameMangling;

// TODO: Might be that the mangled name of get_local_linear_id and
// get_global_linear_id is wrong
std::map<std::string, BuiltInFunctionName> Mappings = {
  // Workitem
  DEF_BUILTIN("_Z12get_work_dim"              , "get_work_dim"            , 0, BuiltInFunctionName::FunctionCategory::FC_Workitem),
  DEF_BUILTIN("_Z15get_global_sizej"          , "get_global_size"         , 1, BuiltInFunctionName::FunctionCategory::FC_Workitem),
  DEF_BUILTIN("_Z13get_global_idj"            , "get_global_id"           , 1, BuiltInFunctionName::FunctionCategory::FC_Workitem),
  DEF_BUILTIN("_Z14get_local_sizej"           , "get_local_size"          , 1, BuiltInFunctionName::FunctionCategory::FC_Workitem),
  DEF_BUILTIN("_Z23get_enqueued_local_sizej"  , "get_enqueued_local_size" , 1, BuiltInFunctionName::FunctionCategory::FC_Workitem),
  DEF_BUILTIN("_Z12get_local_idj"             , "get_local_id"            , 1, BuiltInFunctionName::FunctionCategory::FC_Workitem),
  DEF_BUILTIN("_Z14get_num_groupsj"           , "get_num_groups"          , 1, BuiltInFunctionName::FunctionCategory::FC_Workitem),
  DEF_BUILTIN("_Z12get_group_idj"             , "get_group_id"            , 1, BuiltInFunctionName::FunctionCategory::FC_Workitem),
  DEF_BUILTIN("_Z17get_global_offsetj"        , "get_global_offset"       , 1, BuiltInFunctionName::FunctionCategory::FC_Workitem),
  DEF_BUILTIN("_Z20get_global_linear_idv"     , "get_global_linear_id"    , 0, BuiltInFunctionName::FunctionCategory::FC_Workitem),
  DEF_BUILTIN("_Z19get_local_linear_idv"      , "get_local_linear_id"     , 0, BuiltInFunctionName::FunctionCategory::FC_Workitem),

  // synchronization
  DEF_BUILTIN("_Z18work_group_barrierj"       , "work_group_barrier"      , 1, BuiltInFunctionName::FunctionCategory::FC_Sync),
  DEF_BUILTIN("_Z18work_group_barrierjj"      , "work_group_barrier"      , 2, BuiltInFunctionName::FunctionCategory::FC_Sync),
  DEF_BUILTIN("_Z7barrierj"                   , "barrier"                 , 1, BuiltInFunctionName::FunctionCategory::FC_Sync),
  // printf
  DEF_BUILTIN("_Z6printfPrU3AS2cz"            , "printf"                  , 1, BuiltInFunctionName::FunctionCategory::FC_Printf),

  // math
  DEF_BUILTIN("_Z4acosf"                      , "acos"                    , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z4acosDv2_f"                  , "acos"                    , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z4acosDv3_f"                  , "acos"                    , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z4acosDv4_f"                  , "acos"                    , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z4acosDv8_f"                  , "acos"                    , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z4acosDv16_f"                 , "acos"                    , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z4acosd"                      , "acos"                    , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z4acosDv2_d"                  , "acos"                    , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z4acosDv3_d"                  , "acos"                    , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z4acosDv4_d"                  , "acos"                    , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z4acosDv8_d"                  , "acos"                    , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z4acosDv16_d"                 , "acos"                    , 1, BuiltInFunctionName::FunctionCategory::FC_Math),

  DEF_BUILTIN("_Z4asinf"                      , "asin"                    , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z4asinDv2_f"                  , "asin"                    , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z4asinDv3_f"                  , "asin"                    , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z4asinDv4_f"                  , "asin"                    , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z4asinDv8_f"                  , "asin"                    , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z4asinDv16_f"                 , "asin"                    , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z4asind"                      , "acos"                    , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z4asinDv2_d"                  , "acos"                    , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z4asinDv3_d"                  , "acos"                    , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z4asinDv4_d"                  , "acos"                    , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z4asinDv8_d"                  , "acos"                    , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z4asinDv16_d"                 , "acos"                    , 1, BuiltInFunctionName::FunctionCategory::FC_Math),

  DEF_BUILTIN("_Z3cosf"                       , "cos"                     , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z3cosDv2_f"                   , "cos"                     , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z3cosDv3_f"                   , "cos"                     , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z3cosDv4_f"                   , "cos"                     , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z3cosDv8_f"                   , "cos"                     , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z3cosDv16_f"                  , "cos"                     , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z3cosd"                       , "cos"                     , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z3cosDv2_d"                   , "cos"                     , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z3cosDv3_d"                   , "cos"                     , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z3cosDv4_d"                   , "cos"                     , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z3cosDv8_d"                   , "cos"                     , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z3cosDv16_d"                  , "cos"                     , 1, BuiltInFunctionName::FunctionCategory::FC_Math),

  DEF_BUILTIN("_Z3sinf"                       , "sin"                     , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z3sinDv2_f"                   , "sin"                     , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z3sinDv3_f"                   , "sin"                     , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z3sinDv4_f"                   , "sin"                     , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z3sinDv8_f"                   , "sin"                     , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z3sinDv16_f"                  , "sin"                     , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z3sind"                       , "sin"                     , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z3sinDv2_d"                   , "sin"                     , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z3sinDv3_d"                   , "sin"                     , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z3sinDv4_d"                   , "sin"                     , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z3sinDv8_d"                   , "sin"                     , 1, BuiltInFunctionName::FunctionCategory::FC_Math),
  DEF_BUILTIN("_Z3sinDv16_d"                  , "sin"                     , 1, BuiltInFunctionName::FunctionCategory::FC_Math),

  DEF_BUILTIN("llvm.fmuladd.f32"              , "fmuladd.f32"             , 3, BuiltInFunctionName::FunctionCategory::FC_IntrinsicMath),
  DEF_BUILTIN("llvm.fmuladd.f64"              , "fmuladd.f64"             , 3, BuiltInFunctionName::FunctionCategory::FC_IntrinsicMath)
};

ocl::NameMangling::BuiltInFunctionName::BuiltInFunctionName(
    const std::string MangledName, const std::string UnmangledName,
    unsigned NumParams, FunctionCategory FuncCat)
   : MangledName(MangledName), UnmangledName(UnmangledName), NumParams(NumParams), FuncCat(FuncCat) {
}


/// \brief Determines if the (mangled) function name is known.
/// \param name The name to look up.
bool ocl::NameMangling::isKnownName(const std::string &name) {
  const auto result = Mappings.find(name);
  if (result == Mappings.end()) {
    return false;
  } else {
    return true;
  }
}

bool ocl::NameMangling::isKnownUnmangledName(const std::string &name) {
  for (auto NIT = Mappings.cbegin(), NEND = Mappings.cend();
      NIT != NEND; ++NIT) {
    if (NIT->second.UnmangledName.compare(name) == true) {
      return true;
    }
  }
  return false;
}

/// \brief Determines if the given name exists and if its category is Math.
/// \param name The name of the function to test.
bool ocl::NameMangling::isArithmeticFunction(const std::string &name) {
  const auto result = Mappings.find(name);
  if (result == Mappings.end()) {
    return false;
  }
  if (result->second.FuncCat == BuiltInFunctionName::FunctionCategory::FC_Math) {
    return true;
  } else {
    return false;
  }
}

/// \brief Determines if the given name exists and if its category is WorkItem.
/// \param name The name of the function to test.
bool ocl::NameMangling::isWorkItemFunction(const std::string &name) {
  const auto result = Mappings.find(name);
  if (result == Mappings.end()) {
    return false;
  }
  if (result->second.FuncCat == BuiltInFunctionName::FunctionCategory::FC_Workitem) {
    return true;
  } else {
    return false;
  }
}

/// \brief Determines if the given name exists and if its category is Printf.
/// \param name The name of the function to test.
bool ocl::NameMangling::isPrintfFunction(const std::string &name) {
  const auto result = Mappings.find(name);
  if (result == Mappings.end()) {
    return false;
  }
  if (result->second.FuncCat == BuiltInFunctionName::FunctionCategory::FC_Printf) {
    return true;
  } else {
    return false;
  }
}

/// \brief Determines if the given name exists and if its category is Synchronization.
/// \param name The name of the function to test.
bool ocl::NameMangling::isSynchronizationFunction(const std::string &name) {
  const auto result = Mappings.find(name);
  if (result == Mappings.end()) {
    return false;
  }
  if (result->second.FuncCat == BuiltInFunctionName::FunctionCategory::FC_Sync) {
    return true;
  } else {
    return false;
  }
}

/// \brief Returns the unmangled name of the given mangled name if it exists.
std::string ocl::NameMangling::unmangleName(const std::string &name) {
  const auto result = Mappings.find(name);
  if (result == Mappings.end()) {
    return name;
  }
  return result->second.UnmangledName;
}

/// \brief Returns the mangled name of the given unmangled name if it exists.
std::string ocl::NameMangling::mangleName(const std::string &name) {
  for (auto NIT = Mappings.cbegin(), NEND = Mappings.cend();
      NIT != NEND; ++NIT) {
    if (NIT->second.UnmangledName.compare(name) == 0) {
      return NIT->second.MangledName;
    }
  }
  return name;
}
