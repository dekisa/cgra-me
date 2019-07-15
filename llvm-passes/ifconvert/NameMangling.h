//===- OCL/NameMangling.h ---------------------------------------------===//
//
// This file provides a class that stores the (mangled) names of the OpenCL
// builtin functions.
//
//===----------------------------------------------------------------------===//

#ifndef NAMEMANGLING_H
#define NAMEMANGLING_H

#include <string>

namespace ocl {
namespace NameMangling {

struct BuiltInFunctionName {
  enum FunctionCategory {
    FC_Others         = 0     ,
    FC_Workitem       = 1 << 0,
    FC_Printf         = 1 << 1,
    FC_Sync           = 1 << 2,
    FC_Math           = 1 << 3,
    FC_IntrinsicMath  = 1 << 4
  };

  BuiltInFunctionName(const std::string MangledName, const std::string UnmangledName, unsigned NumParams, FunctionCategory FuncCat);

  std::string MangledName;
  std::string UnmangledName;
  unsigned NumParams;
  FunctionCategory FuncCat;
};

void add(const std::string &MangledName, const std::string &UnmangledName,
  unsigned Params, BuiltInFunctionName::FunctionCategory FunctionCat);

bool isKnownName(const std::string &name);
bool isKnownUnmangledName(const std::string &name);
bool isArithmeticFunction(const std::string &name);
bool isWorkItemFunction(const std::string &name);
bool isPrintfFunction(const std::string &name);
bool isSynchronizationFunction(const std::string &name);
std::string unmangleName(const std::string &name);
std::string mangleName(const std::string &name);

} // end ns NameMangling

} // end ns ocl

#endif /* NAMEMANGLING_H */

