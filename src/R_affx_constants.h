#define R_AFFX_VERBOSE 1
#define R_AFFX_REALLY_VERBOSE 2

/*
 * Using R's test of endianness
 */

#include <Rconfig.h>

#ifdef WORDS_BIGENDIAN
  #define IS_BIG_ENDIAN 1
#endif

#ifdef _MSC_VER
#define _strtoui64 strtoull	/* for TsvFile.cpp */
#include <w32api.h>
#define WINVER WindowsXP	/* for Util.cpp, via TsvFile.cpp */
#endif

/* Patch for compilation errors ("'int32_t' does not name a type")
   on Windows using the Rtools 2.15.0.1915-1919 toolchain, which
   is used by R (> 2.14.1), cf. private email on 'affxparser (devel)
   on Windows' on Jan 25 - Mar 5, 2012. */
#ifdef __MINGW32__
#include <stdint.h>
#endif

