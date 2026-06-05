add_llvm_library( LLVMOurSimplifyCFG MODULE BUILDTREE_ONLY
  OurSimplifyCFG.cpp
  DEPENDS
  intrinsics_gen
  PLUGIN_TOOL
  opt
  )
