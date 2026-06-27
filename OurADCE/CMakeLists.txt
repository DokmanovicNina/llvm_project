add_llvm_library( LLVMOurADCE MODULE BUILDTREE_ONLY
        OurADCE.cpp
        DEPENDS
        intrinsics_gen
        PLUGIN_TOOL
        opt
)