/*****************************************************************
 This header file is used when compiling with MinGW on Windows.
 It is agile to the gcc toolchain currently used; it uses
 different setups for:

  - gcc (>= 4.9.3): introduced in R (>= 3.3.0)
  - gcc (>= 4.6.3): R (< 3.3.0) and some R (>= 3.3.0) installs

 Henrik Bengtsson, 2016-04-05
*****************************************************************/
#define GCC_VERSION (__GNUC__ * 10000 \
                              + __GNUC_MINOR__ * 100 \
                              + __GNUC_PATCHLEVEL__)

#if GCC_VERSION >= 40903
#include "_mingw_gcc493.h"
#elif GCC_VERSION >= 40603
#include "_mingw_gcc463.h"
#else
/* Fall back assuming gcc 4.6.3 toolchain */
#include "_mingw_gcc463.h"
#endif
