cmake .. -DSWIG_EXECUTABLE=swigwin-3.0.7/swig.exe -DSWIG_DIR=swigwin-3.0.7/ -DSDL2_LIBRARY=../../build/lib/SDL2-2.0.3/lib/x86/SDL2.lib -DSDL2_INCLUDE_DIR=../../build/lib/SDL2-2.0.3/include/ -DCOFFEE_CSHARP=TRUE -Dassimp_loc=../../build/lib/assimp-vc130-mt.lib -Dglbinding_loc=../../build/lib/glbinding.lib
MSBuild.exe Coffee.vcxproj
