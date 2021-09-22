////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License 
// (version 2.1) as published by the Free Software Foundation.
// 
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
// for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
//
////////////////////////////////////////////////////////////////

/// @file  util-meminfo.cpp
/// @brief prints the results of our meminfo functions for testing.

#include "util/Util.h"

#define B_FMT "%12llu  "
#define B_CVT(x) ((long long unsigned int)x)
#define M_FMT "%10.3fMB  "
#define M_CVT(x) (x/(1024.0*1024))

int
main(int argc,char* argv[])
{
  uint64_t free;
  uint64_t total;
  uint64_t swapAvail;
  uint64_t memAvail;

#ifdef __linux__
  if (argc==2) {
    printf("Reading linux /proc/meminfo from '%s' for testing:\n",argv[1]);
    memInfo_linux(argv[1],free,total,swapAvail,memAvail);
  } else {
    Util::memInfo(free,total,swapAvail,memAvail,false);
  }
#else
  Util::memInfo(free,total,swapAvail,memAvail,false);
#endif

  printf("meminfo: free=" B_FMT "total=" B_FMT "swapAvail=" B_FMT "memAvail=" B_FMT "\n",
         B_CVT(free),B_CVT(total),B_CVT(swapAvail),B_CVT(memAvail));
  printf("meminfo: free=" M_FMT "total=" M_FMT "swapAvail=" M_FMT "memAvail=" M_FMT "\n",
         M_CVT(free),M_CVT(total),M_CVT(swapAvail),M_CVT(memAvail));

// and again with a cap

#ifdef __linux__
  if (argc==2) {
    printf("Reading linux /proc/meminfo from '%s' for testing:\n",argv[1]);
    memInfo_linux(argv[1],free,total,swapAvail,memAvail);
  } else {
    Util::memInfo(free,total,swapAvail,memAvail,true);
  }
#else
  Util::memInfo(free,total,swapAvail,memAvail,true);
#endif

  printf("meminfo (cap): free=" B_FMT "total=" B_FMT "swapAvail=" B_FMT "memAvail=" B_FMT "\n",
         B_CVT(free),B_CVT(total),B_CVT(swapAvail),B_CVT(memAvail));
  printf("meminfo (cap): free=" M_FMT "total=" M_FMT "swapAvail=" M_FMT "memAvail=" M_FMT "\n",
         M_CVT(free),M_CVT(total),M_CVT(swapAvail),M_CVT(memAvail));

  return 0;
}
