add_llvm_library( LLVMMyAggressiveInstcombinePass MODULE BUILDTREE_ONLY
  MyAggressiveInstcombinePass.cpp
  DEPENDS
  intrinsics_gen
  PLUGIN_TOOL
  opt
  ) 