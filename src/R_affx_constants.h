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
