#define R_AFFX_VERBOSE 1
#define R_AFFX_REALLY_VERBOSE 2

/*
 * Using R's test of endianness
 */

#include <Rconfig.h>

#ifdef WORDS_BIGENDIAN
  #define IS_BIG_ENDIAN 1
#endif

