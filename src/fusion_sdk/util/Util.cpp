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

/**
 * @file   Util.cpp
 * @author Chuck Sugnet
 * @date   Mon May 16 16:04:48 2005
 *
 * @brief   General Utilities.
 */

//
#include "util/Util.h"
//
#include "calvin_files/utils/src/StringUtils.h"

#include "util/Convert.h"
#include "util/Err.h"
#include "util/Fs.h"
#include "util/RowFile.h"
#include "util/TableFile.h"
#include "util/Verbose.h"
//
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

// silences some warnings.
#include "portability/apt-no-warnings.h"

#define POSIX_OPEN open
#define POSIX_CLOSE close
#define POSIX_MKDIR mkdir

#ifdef WIN32
#include <windows.h>
#include <direct.h>
#include <float.h>
#include <io.h>
#include <malloc.h>
#include <tchar.h>
#include <stdio.h>
#define S_ISDIR(m)   (((m) & S_IFMT) == S_IFDIR)
#define BUFFSIZE 10000

//
#ifndef POSIX_OPEN
#define POSIX_OPEN _open
#endif
#ifndef POSIX_CLOSE
#define POSIX_CLOSE _close
#endif
#ifndef POSIX_MKDIR
#define POSIX_MKDIR _mkdir
#endif

#else
#include <unistd.h>
#endif /* WIN32 */

// OS stuff for memInfo, getAvailableDiskSpace, isSameVolume
#ifdef __APPLE__
#include <mach/mach.h>
#include <sys/stat.h>
#include <sys/mount.h>
#endif

#ifdef __linux__
#include <sys/sysinfo.h>
#include <sys/statfs.h>
#endif

#ifdef __sun__
#include <unistd.h>
#include <ieeefp.h>
#endif

#ifdef WIN32
#include <time.h>
void sleep(unsigned int ms) {
   clock_t target = ms + clock();
   while (target > clock());
}
#endif /*WIN32*/



using namespace std;

/**
 * Create a copy of a string. Free this with delete [] (or freezArray()) when
 * done.
 * @param s - c-string to be copied.
 * @return char * newly allocated c-string.
 */

char *Util::cloneString(const char *s) {
  char *copy = NULL;
  if(s == NULL)
    return NULL;

  size_t length = strlen(s);
  //  copy = new ( sizeof(char) * (length+1) );
  copy = new char[length + 1];
  // strncpy is deprecated
  // strncpy(copy, s, length+1);
  memcpy(copy,s,length+1);
  return copy;
}
/*
//todo vliber version
char *Util::cloneString(const string &s) {
  char *copy = NULL;
  if(s.c_str() == NULL)
    return NULL;
  size_t length = strlen(s.c_str());
  copy = new char[length + 1];
  //  strcpy(copy,length+1,s.c_str());
  strcpy(copy,s.c_str());
  //malloc(s.length()+1);
  //strcpy(copy,s.length()+1,s.c_str());
  return copy;
}
*/

bool Util::stringEndsWith(const std::string& str,const std::string& end)
{
  std::string::const_reverse_iterator s_i=str.rbegin();
  std::string::const_reverse_iterator s_end=str.rend();
  std::string::const_reverse_iterator e_i=end.rbegin();
  std::string::const_reverse_iterator e_end=end.rend();

  // see if the whole thing end is there.
  while (e_i!=e_end) {
    if ((s_i==s_end) ||   // ran out of input string
        ((*e_i)!=(*s_i)) // mismatch
        ) {
      return false; // didnt end with 'end'.
    }
    // advance (backwards!)
    e_i++;
    s_i++;
  }
  // 'end' was at the end of the string.
  return true;
}

bool Util::endsWithStr(const std::string& str,const std::string& ending,int charsAtEnd)
{
  if (ending.size()>str.size()) {
    return false;
  }
  std::string tmpstr=str.substr(str.size()-(ending.size()+charsAtEnd),ending.size());
  return tmpstr==ending;
}

bool Util::endsWithStr(const std::string& str,const std::string& ending)
{
  return endsWithStr(str,ending,0);
}

/**
* @brief Chop off the last character if it is a path separator.
* windows stat() can't handle having it there.
* @param s - string to have '/' or '\' chopped off if last.
*/
void Util::chompLastIfSep(std::string &s) {
  string::size_type i = s.rfind(Fs::osPathSep());
    if(i != string::npos && i == s.length() -1)
        s.erase(i);
}

/**
 * Chop the last suffix (as defined by '.') from a string
 * @param - string to chop
 * @param - delimiter, default '.'
 */
std::string Util::chopSuffix(const std::string& s, char d) {
  string::size_type pos = s.rfind(d);
  if(pos != string::npos) {
    return s.substr(0,pos);
  }
  return s;
}

#ifdef WIN32
// Windows doesn't seem to have these defined, so we'll define them
// here for utility. Note that with the windows _stat() function all
// users have id 0 and group 0.
#ifndef __MINGW32__
#define S_IRUSR 00400 //owner has read permission
#define  S_IWUSR 00200 //owner has write permission
#define S_IXUSR 00100 ///owner has execute permission
#endif
#define S_IRWXG 00070 //mask for group permissions
#define  S_IRGRP 00040 //group has read permission
#define  S_IWGRP 00020 //group has write permission
#define S_IXGRP 00010 //group has execute permission
#define  S_IRWXO 00007 //mask for permissions for others (not in group)
#define  S_IROTH 00004 //others have read permission
#define S_IWOTH 00002 //others have write permisson
#define S_IXOTH 00001 // others have execute permission
#endif


void Util::breakByString(const std::string &s, const std::string &delim, std::vector<std::string> &words) {
    words.clear();
    int curPos = 0;
    APT_ERR_ASSERT(delim.length() > 0, "delim must be non-empty.");
    while(1) {
        size_t pos = s.find(delim,curPos);
        if (pos == string::npos) {
            words.push_back(s.substr(curPos, s.size()));
            break;
        }
        string sub = s.substr(curPos, pos - curPos);
        words.push_back(sub);
        curPos = pos + delim.size();
        if ( curPos >= s.length() ) {
            break;
        }
    }
}

/**
 * Chop up a string into a vector of words.
 *
 * @param s - string of interest.
 * @param delim - delimiter to split on.
 * @param words - vector to put words into, will be cleared then filled.
 */
/// @todo shouldnt this be "split"?
void Util::chopString(const std::string& s,const char delim,std::vector<std::string>& words) {
  string::size_type len = 0, start = 0, next = 0;
  words.clear();
  len = s.length();

  while(start < len) {
    next = s.find(delim, start);
    if(next == string::npos) {
      next = s.size(); // entire string.
    }
    words.push_back(s.substr(start, next - start));
    start = next+1;
  }
}

/**
 * Chop up a string into a vector of words.
 *
 * @param s - string of interest.
 * @param delims - delimiters to split on. The split will occur at any character
 *               - among those present in the string.
 * @param words - vector to put words into, will be cleared then filled.
 */
void Util::chopString(const std::string& s, const char* delims, std::vector<std::string>& words) {
  string::size_type len = 0, start = 0, next = 0;
  words.clear();
  len = s.length();

  while(start < len) {
    next = s.find_first_of(delims, start);
    if(next == string::npos) {
      next = s.size(); // entire string.
    }
    words.push_back(s.substr(start, next - start));
    start = next+1;
  }
}


/**
 * Check each entry in two matrices to see if they are the same.  If
 * doing 'match-rows' we will attempt to find the matching row by the
 * row name.
 *
 * @param targetFile - File to read target matrix from.
 * @param queryFile - File to read the query matrix from.
 * @param colSkip - How many of initial columns to ignore (i.e. row names)
 * @param rowSkip - How many of initial rows to ignore (i.e. column header)
 * @param epsilon - What is the tolerance of difference.
 *                  i.e. if q[i][j] - t[i][j] >= epsilon then there is a difference.
 * @param printMismatch - Should we print out the cases where difference is >= epsilon
 * @param matchRows - Should we try to match the rows based on the row identifiers.
 * @param fraction - What fractional difference is tolerated, test for value equivalence (not used by default).
 *                  i.e. if q[i][j] - t[i][j] < fraction*max( |q[i][j]|, |t[i][j]| )
 *
 * @return - Number of differences >= epsilon found.
 */
int Util::matrixDifferences(const std::string& targetFile,
                            const std::string& queryFile,
                            int colSkip, int rowSkip,
                            double epsilon,
                            bool printMismatch,
                            bool matchRows,
                            double fraction,
                            int printMismatchMax ) {
  vector< vector<double> > qMatrix, tMatrix;
  unsigned int rowIx = 0, colIx = 0;
  unsigned int numCol = 0, numRow = 0;
  bool same = true;
  map<string, int> qMatrixMap;
  vector<string> qMatrixRows, tMatrixRows;
  unsigned int diffCount = 0;
  unsigned int rowDiffCount = 0;
  double maxDiff = 0;

  std::string tmp_target_name=Fs::convertToUncPath(targetFile);
  Verbose::out(2, "Reading in file: "+tmp_target_name);
  RowFile::matrixFromFile(tmp_target_name, qMatrix, rowSkip, colSkip);

  std::string tmp_query_name=Fs::convertToUncPath(queryFile);
  Verbose::out(2, "Reading in file: " +tmp_query_name);
  RowFile::matrixFromFile(tmp_query_name, tMatrix, rowSkip, colSkip);

  /* If we are matching by rows grab the first column from each file
     and assume it is the unique row names. Create a map from the
     names to the indexes for qMatrix */
  if (matchRows) {
    colIx = 0;
    Verbose::out(2, "Reading in rownames.");
    // @todo these are named backwards. (q/t)
    TableFile::columnFromFile(tmp_target_name, qMatrixRows, colIx, rowSkip, true);
    TableFile::columnFromFile(tmp_query_name,  tMatrixRows, colIx, rowSkip, true);
    //
    for(rowIx = 0; rowIx < qMatrixRows.size(); rowIx++) {
      if(qMatrixMap.find(qMatrixRows[rowIx]) != qMatrixMap.end()) {
        Err::errAbort("Duplicate row names: " + qMatrixRows[rowIx] + " in matrix 1");
      }
      qMatrixMap[qMatrixRows[rowIx]] = rowIx;
    }
  }

  Verbose::out(2,"Looking for differences.");
  if((!matchRows && qMatrix.size() != tMatrix.size()) || qMatrix[0].size() != tMatrix[0].size()) {
    Err::errAbort("Matrices are different sizes, not comparable.");
  }
  numRow = tMatrix.size();
  numCol = tMatrix[0].size();

  for(rowIx = 0; rowIx < numRow; rowIx++) {
    bool rowDiff = false;
    for(colIx = 0; colIx < numCol; colIx++) {
      int qMatRowIx = rowIx;
      /* If doing match rows, look up the correct row based on row name. */
      if(matchRows) {
        if(qMatrixMap.find(tMatrixRows[rowIx]) == qMatrixMap.end() )
          Err::errAbort("Can't find rowname: " + tMatrixRows[rowIx] + " in matrix 1");
        else
          qMatRowIx = qMatrixMap[tMatrixRows[rowIx]];
      }
      double val = qMatrix[qMatRowIx][colIx] - tMatrix[rowIx][colIx];
      maxDiff = Max(maxDiff, fabs(val));
      // allowed absolute difference from fractional tolerance (zero by default)
      double epsilon2 = fraction*Max( fabs(qMatrix[qMatRowIx][colIx]), fabs(tMatrix[rowIx][colIx]) );
      // absolute difference is acceptable if it satisfies either (least restrictive) tolerance
      bool failed = (fabs(val) > Max(epsilon,epsilon2));
      bool okFinite = true;
      okFinite &= isFinite(qMatrix[qMatRowIx][colIx]);
      okFinite &= isFinite(tMatrix[rowIx][colIx]);
      if(!okFinite)
        Verbose::out(2, "Non-finite floating point numbers at row: " + ToStr(rowIx) + " column: " + ToStr(colIx));
      if(failed || !okFinite) {
        same = false;
        rowDiff = true;
        diffCount++;
        // report mismatches up to maximum count, if set, with warning if maximum count exceeded.
        if( printMismatch && ((int)diffCount<=printMismatchMax || printMismatchMax<0) ) {
          Verbose::out(2, "row: " + ToStr(rowIx) + " col: " + ToStr(colIx) + " (" +
                       ToStr(qMatrix[qMatRowIx][colIx]) + " vs. " + ToStr( tMatrix[rowIx][colIx]) + ")" + " Diff: " + ToStr(val));
        }
        if( printMismatch && diffCount==printMismatchMax+1 && printMismatchMax>0 ) {
          Verbose::out(2, "Number of differences exceeds maximum number (" + ToStr(printMismatchMax) + ") to report.");
        }
      }
    }
    if(rowDiff)
      rowDiffCount++;
  }
  if(maxDiff > 0) {
    Verbose::out(2, "Max difference is: " + ToStr(maxDiff));
  }
  if(same)
    Verbose::out(2,"Same.");
  else {
    unsigned int total = numRow * numCol;
    float percent = (float) rowDiffCount / numRow * 100;
    Verbose::out(2, "Different in " + ToStr(rowDiffCount) + " of " + ToStr(numRow) + " ( " + ToStr(percent) + "% )  rows.");
    percent = (float) diffCount / total * 100;
    Verbose::out(2, "Different at " + ToStr(diffCount) + " of " + ToStr(total) + " ( " + ToStr(percent) + "% )  values.");
  }
  return diffCount;
}

void Util::replaceString(std::string &s, const std::string &from, const std::string &to) {
    vector<string> words;
    breakByString(s, from.c_str(), words);
    std::ostringstream ss;
    ss << words[0];
    for (int i = 1; i < words.size(); i++) {
        ss << to;
        ss << words[i];
    }
    s = ss.str();
}



/**
 * Schrage's algorithm for generating random numbers in 32 bits.
 * @param ix - pointer to integer seed, cannot be zero.
 */
int32_t Util::schrageRandom(int32_t *ix) {
  int32_t a = 16807, p = 2147483647, xhi = 0, xlo = 0;
  int32_t rand = 0;
  int32_t r = p % a;
  int32_t q = p / a;
  Err::check(*ix > 0, "Error: Util::schrageRandom() - Cannot seed with 0");
  /* Break into high and low bits to avoid overflow. */
  xhi = (*ix) / q;
  xlo = (*ix)  - xhi*q;
  /* Combine high and low. */
  (*ix) = a * xlo - r * xhi;
  if((*ix) < 0) {
    (*ix) = (*ix) + p;
  }
  rand = (*ix);
  return rand;
}

std::string Util::asMB(uint64_t x) {
  return ToStr(x/MEGABYTE) + "MB";
}

#ifdef WIN32
#define memInfo_defined 1
/// @brief     Query windows for its memInfo.
/// @param     free      The amount of free memory. (bytes)
/// @param     total     The physical memory installed (bytes)
/// @param     swapAvail The amount of swap avail. (bytes)
/// @param     memAvail  Suggested amount of memory to use.
/// @return    true on success
bool memInfo_win32(uint64_t &free, uint64_t &total,
                   uint64_t &swapAvail, uint64_t& memAvail)
{
  MEMORYSTATUSEX statex;
  statex.dwLength = sizeof(statex);
  if(GlobalMemoryStatusEx(&statex) == 0) {
    Err::errAbort("Util::memInfo() - Could not determine memory usage with: GlobalMemoryStatusEx().");
  }
  free = statex.ullAvailPhys;
  total = statex.ullTotalPhys;
  swapAvail = statex.ullAvailVirtual - statex.ullAvailPhys;
  memAvail = (uint64_t)(free * 0.90);

  return true;
}
#endif

#ifdef __linux__
#define memInfo_defined 1

/// @brief     Read and parse the contents of /proc/meminfo on 2.4 and 2.6 systems.
/// @param     proc_meminfo_filename File to parse (changeable for testing)
/// @param     free      The amount of free memory. (bytes)
/// @param     total     The physical memory installed (bytes)
/// @param     swapAvail The amount of swap avail. (bytes)
/// @param     memAvail  Suggested amount of memory to use.
/// @return    true on success
bool memInfo_linux(std::string proc_meminfo_filename,
                   uint64_t &free, uint64_t &total,
                   uint64_t &swapAvail, uint64_t& memAvail)
{
  std::ifstream proc_meminfo;
  std::string key;
  std::string line;
  //char line_buf[1024];
  //std::istringstream line_stm(line_buf,sizeof(line_buf));
  uint64_t val;
  std::string units;
  uint64_t buffersUsed=0, cached=0;

  free=total=swapAvail=memAvail=0;

  // 2.4
  // total: used: free: shared: buffers: cached:
  // Mem:  4097478656 2765283328 1332195328 0 219951104 2328576000
  // Swap: 3224301568        0 3224301568

  // 2.6
  // MemTotal:      4059404 kB
  // MemFree:       2169240 kB
  // Buffers:         33776 kB
  // Cached:        1618328 kB

  Fs::aptOpen(proc_meminfo, proc_meminfo_filename);
  while (!proc_meminfo.eof()) {
    getline(proc_meminfo,line);
    //cout<<"L: "<<line<<"\n";
    std::istringstream line_stm(line);

    //proc_meminfo.getline(line_buf,sizeof(line_buf));
    //line_stm.seekg(0,ios_base::beg);
    //cout << line_stm;
    key="";
    val=0;
    units="";
    // MemTotal:      4059404 kB
    line_stm >> key;
    line_stm >> val;
    line_stm >> units;

    // Handle the linux-2.4 case.
    if (key=="total:") {
      std::string ignore;
      std::string line_mem;
      getline(proc_meminfo,line_mem);
      std::istringstream line_stm_24_mem(line_mem);
      line_stm_24_mem >> key;
      line_stm_24_mem >> total;       // total:
      line_stm_24_mem >> ignore;      // used:
      line_stm_24_mem >> free;        // free:
      line_stm_24_mem >> ignore;      // shared:
      line_stm_24_mem >> buffersUsed; // buffers:
      //
      std::string line_swap;
      getline(proc_meminfo,line_swap);
      std::istringstream line_stm_24_swap(line_swap);
      line_stm_24_swap >> ignore;      // Swap:
      line_stm_24_swap >> swapAvail;
      // we dont need any more data
      break;
    }
    //
    //printf("%-20s=%10lu %4s\n",key.c_str(),val,units.c_str());
    //
    if (units=="kB") {
      val=val*1024;
    }
    else if (units=="MB") {
      val=val*1024*1024;
    } else {
      // just give up if the units arent there.
      break;
      // assert(0);
    }
    //
    if (key=="MemTotal:") {
      total=val;
    }
    else if (key=="MemFree:") {
      free=val;
    }
    else if (key=="SwapFree:") {
      swapAvail=val;
    }
    else if (key=="Buffers:") {
      buffersUsed=val;
    }
    else if (key=="Cached:") {
      cached=val;
    }
  }
  proc_meminfo.close();

  memAvail=(uint64_t)(free+0.90*buffersUsed+cached);

  return true;
}
#endif

#ifdef __APPLE__
#define memInfo_defined 1
/// @brief     Query the mach kernel for memInfo.
/// @param     free      The amount of free memory. (bytes)
/// @param     total     The physical memory installed (bytes)
/// @param     swapAvail Darwin can use all the disk space, return 0
/// @param     memAvail  Suggested amount of memory to use.
/// @return    true on success
bool memInfo_darwin(uint64_t &free, uint64_t &total,
                    uint64_t &swapAvail, uint64_t& memAvail)
{
  // ask mach
  struct vm_statistics vm_stat;
  vm_size_t   my_pagesize;
  mach_port_t my_host=mach_host_self();
  mach_msg_type_number_t count = HOST_VM_INFO_COUNT;

  if (host_page_size(my_host,&my_pagesize)!=KERN_SUCCESS) {
    Err::errAbort("Util::memInfo() - Didnt succeed with 'host_page_size'.");
  }
  if (host_statistics(my_host, HOST_VM_INFO,(integer_t*)&vm_stat,&count) != KERN_SUCCESS) {
    Err::errAbort("Util::memInfo() - Didnt succeed with 'host_statistics'.");
  }

  // there isnt a total in the struct; sum it up.
  total=(vm_stat.free_count+vm_stat.active_count+vm_stat.inactive_count+vm_stat.wire_count);
  // THEN multiply by the page size.  (a 64b multiply)
  total=total*my_pagesize;

//#define PRINT_IT(x) printf("%-30s: %8d\n",#x,x)
//  PRINT_IT(vm_stat.free_count);
//  PRINT_IT(vm_stat.active_count);
//  PRINT_IT(vm_stat.inactive_count);
//  PRINT_IT(vm_stat.wire_count);

  // should we add in vm_stat.inactive_count?
  free=vm_stat.free_count;
  free=free*my_pagesize;
  // darwin can use all the disk space -- report zero
  swapAvail=0;
  // our guess as to what we can allocate
  memAvail=(uint64_t)(0.90*(vm_stat.free_count+vm_stat.inactive_count)*my_pagesize);

  return true;
}
#endif

#ifdef __sun__
#define memInfo_defined 1
/// @brief     Query a solaris kernel for memInfo
/// @param     free      The amount of free memory. (bytes)
/// @param     total     The physical memory installed (bytes)
/// @param     swapAvail The amount of swap avail. (always zero.)
/// @param     memAvail  Suggested amount of memory to use.
/// @return    true on success
bool memInfo_solaris(uint64_t &free, uint64_t &total,
                     uint64_t &swapAvail, uint64_t& memAvail)
{
  uint64_t page_size=sysconf(_SC_PAGESIZE);

  free  =sysconf(_SC_AVPHYS_PAGES)*page_size;
  total =sysconf(_SC_PHYS_PAGES  )*page_size;
  // figure we can use a quarter of the allocated memory
  memAvail=free+((total-free)/4);
  // dont want to call swapctl...
  swapAvail=0;

  return true;
}
#endif

#ifdef __which_system_was_this_for__
#define memInfo_defined 1
/// @brief     Use linux sysinfo to get linux memInfo. (the old interface)
/// @param     free      The amount of free memory. (bytes)
/// @param     total     The physical memory installed (bytes)
/// @param     swapAvail The amount of swap avail. (bytes)
/// @param     memAvail  Suggested amount of memory to use.
/// @return    true on success
bool memInfo_sysinfo(uint64_t &free, uint64_t &total,
                     uint64_t &swapAvail, uint64_t& memAvail)
{
  struct sysinfo info;
  if(sysinfo(&info) != 0) {
    Err::errAbort("Util::memInfo() - Could not determine memory usage with: sysinfo().");
  }
  free = info.freeram * info.mem_unit;
  total = info.totalram * info.mem_unit;
  swapAvail = info.freeswap * info.mem_unit;
  // We use this as a guesstimate of how much ram we can safely allocate on the system.
  // The factor of 0.90 is to leave a bit of memory for other users.
  memAvail=(uint64_t)(free+0.90*(info.bufferram*info.mem_unit));

  return true;
}
#endif


bool Util::memInfo(uint64_t &free, uint64_t &total,
                   uint64_t &swapAvail, uint64_t& memAvail,
                   bool cap32bit) {
  bool success = false;
  bool is32bit = true;
  free=total=swapAvail=memAvail=0;

  // One of these should be defined.
  ///@todo is there a more robust way to determine 32 vs 64 bitness?
#ifdef WIN32
#ifdef _WIN64
  is32bit = false;
#else
  is32bit = true;
#endif
  success=memInfo_win32(free,total,swapAvail,memAvail);
#endif //WIN32

#ifdef __linux__
#ifdef __LP64__
  is32bit = false;
#else
  is32bit = true;
#endif
  success=memInfo_linux("/proc/meminfo",free,total,swapAvail,memAvail);
#endif //__linux__

#ifdef __APPLE__
#ifdef __LP64__
  is32bit = false;
#else
  is32bit = true;
#endif
  success=memInfo_darwin(free,total,swapAvail,memAvail);
#endif //__APPLE__

#ifdef __sun__
  success=memInfo_solaris(free,total,swapAvail,memAvail);
#endif // __sun__

  if (memAvail>MEMINFO_2GB_MAX && (is32bit || cap32bit)) {
    memAvail=MEMINFO_2GB_MAX;
  }

  return success;
}


/**
 * Return a pointer to the next character that is white space
 * or NULL if none found.
 * @param s - cstring to find white space in.
 * @return - Pointer to next whitespace character or NULL if none
 *   found.
 */
const char *Util::nextWhiteSpace(const char *s) {
  while(s[0] != '\0' && !isspace(s[0])) {
    s++;
  }
  return s;
}

/**
 * Print a string wrapping at max width from the current
 * position.
 * @param out - stream to output string to.
 * @param str - The cstring to be printed.
 * @param prefix - How many spaces to put on begining of newline.
 * @param maxWidth - Where to wrap text at.
 * @param currentPos - What position in the line is
 *                      cursor currently at.
 */
void Util::printStringWidth(std::ostream &out,const std::string& str,
                            int prefix,
                            int currentPos, int maxWidth ) {
  const char *wStart = NULL, *wEnd = NULL; /* Start and end of word pointers. */
  int position = currentPos;
  int nextSize = 0;
  int i = 0;
  wStart = str.c_str();

  /* While there are still characters to be printed. */
  while(*wStart != '\0') {

    /* Clean out any whitespace. */
    while(isspace(*wStart) && *wStart != '\0') {
      if(*wStart == '\n') {
        out.put('\n');
        for(i = 0; i < prefix; i++)
          out.put(' ');
        fflush(stdout);
        position = prefix;
      }
      // WAS '*wStart++' but that is unnecessary and gives compile
      // warning on "expression result unused".
      wStart++;
    }

    if(*wStart == '\0')
      break;

    /* Find the end of current word. */
    wEnd = wStart;
    while(!isspace(*wEnd) && *wEnd != '\0')
      wEnd++;

    /* Time for a newline? */
    if((wEnd - wStart) + position >= maxWidth) {
      out.put('\n');
      for(i = 0; i < prefix; i++)
         out.put(' ');
      position = prefix;
    }

    /* Print out the word. */
    while(wStart < wEnd) {
      out.put(*wStart);
      fflush(stdout);
      wStart++;
      position++;
    }

    /* Look to see where next word is. */
    while(isspace(*wEnd)) {
      if(*wEnd == '\n') {
        out.put('\n');
        for(i = 0; i < prefix; i++)
            out.put(' ');
        position = prefix;
      }
      wEnd++;
    }

    /* Figure out the size of the next word. */
    wStart = nextWhiteSpace(wEnd);
    if(wStart != NULL)
      nextSize = wStart - wEnd;
    else
      nextSize = 0;

    /* Print a space if we're not going to
       print a newline. */
    if(*wEnd != '\0' &&
       nextSize + position < maxWidth &&
       position != 0) {
      out.put(' ');
      position++;
    }

    wStart = wEnd;
  }
}

/**
 * Wrapper for different version of isnan() on different systems.
 * @param x - number to be checked for NaN or INF
 * @return - true if x is finite (-INF < x && x < +INF && x != nan), false otherwise
 */
bool Util::isFinite(double x) {
  bool isOk = false;
#ifdef WIN32
  isOk = _finite(x);
#else
  isOk = finite(x);
#endif
  return isOk;
}

std::string Util::getTimeStamp()
{
  time_t now=time(NULL);
  const char* ctime_ptr=ctime(&now);
  APT_ERR_ASSERT(ctime_ptr != NULL, "Got null value from ctime()");
  std::string ctime_str=ctime_ptr;
  trimString(ctime_str);
  return ctime_str;
}


void Util::changeEnd(std::string& str,const std::string& from,const std::string& to)
{
  size_t pos=str.rfind(from);
  if (pos!=string::npos) {
    str=str.substr(0,pos)+to;
  }
}
void Util::changeEnd(std::vector<std::string>& str_vec,const std::string& from,const std::string& to)
{
  for (int i=0;i<str_vec.size();i++) {
    changeEnd(str_vec[i],from,to);
  }
}


//Assumes sentinel character (equal to NULL) exists at end of array
std::vector<std::string> Util::listToVector(const char* in[])
{
  std::vector<string> fullName;

  for (int i = 0; in[i] != NULL; i++)
  {
    fullName.push_back(in[i]);
  }

  return fullName;
}

std::vector<std::string> Util::listToVector(const char* in[], int size)
{
  std::vector<std::string> fullName;

  for (int i = 0; i < size; i++)
    {
      fullName.push_back(in[i]);
    }

  return fullName;
}

std::vector<std::string> Util::addPrefixSuffix(std::vector<std::string> middle,
                           const std::string &prefix, const std::string &suffix)
{
  std::vector<std::string> fullName;

  for (std::vector<std::string>::iterator i = middle.begin(); i < middle.end();i++)
  {
    fullName.push_back(Fs::Unc(prefix + (*i) + suffix));
  }

  return fullName;
}

std::vector<std::string> Util::addPrefixSuffix(const char* middle[],
                           const std::string &prefix, const std::string &suffix)
{
  std::vector<std::string> fullName = addPrefixSuffix(listToVector(middle), prefix, suffix);

  return fullName;
}

std::vector<std::string> Util::addPrefixSuffix(const char* middle[], int size,
                           const std::string &prefix, const std::string &suffix)
{
  std::vector<std::string> fullName = addPrefixSuffix(listToVector(middle, size), prefix, suffix);

  return fullName;
}

std::vector<std::string> Util::addPrefixSuffix(const char* middle[],
                           const std::string &prefix)
{
  std::vector<std::string> fullName = addPrefixSuffix(listToVector(middle), prefix, "");

  return fullName;
}

//String Stuff
//Assumes a sentintel exists (equal to "") as the last element in the array.
std::vector<std::string> Util::listToVector(std::string in[])
{
  std::vector<std::string> fullName;

  for (int i = 0; in[i] != ""; i++)
  {
    fullName.push_back(in[i]);
  }

  return fullName;
}

std::vector<std::string> Util::listToVector(std::string in[], int size)
{
  std::vector<std::string> fullName;

  for(int i = 0; i < size; i++)
  {
    fullName.push_back(in[i]);
  }

  return fullName;
}

std::vector<std::string> Util::addPrefixSuffix(std::string middle[],
                           const std::string &prefix, const std::string &suffix)
{
  std::vector<std::string> fullName = addPrefixSuffix(listToVector(middle), prefix, suffix);

  return fullName;
}

std::vector<std::string> Util::addPrefixSuffix(std::string middle[], int size,
                           const std::string &prefix, const std::string &suffix)
{
  std::vector<std::string> fullName = addPrefixSuffix(listToVector(middle,size), prefix, suffix);

  return fullName;
}

std::string Util::joinVectorString(std::vector<std::string> toJoin, const std::string &sep)
{
  std::string fullString="";

  for (std::vector<std::string>::iterator i = toJoin.begin(); i < toJoin.end();i++)
  {
    fullString += (*i)+sep;
  }
  return fullString;
}

Util::StaticMem &Util::getStaticMem() {
    static Util::StaticMem mem;
    return mem;
}

uint64_t Util::getMemFreeAtStart() {
    Util::StaticMem &mem = getStaticMem();
    return  mem.getMemFreeAtStart();
}

uint64_t Util::getMemFreeAtBlock() {
    Util::StaticMem &mem = getStaticMem();
    return  mem.getMemFreeAtBlock();
}

void Util::popMemFreeAtStart() {
    Util::StaticMem &mem = getStaticMem();
    mem.popMemFreeAtStart();
}

void Util::pushMemFreeAtStart() {
    Util::StaticMem &mem = getStaticMem();
    uint64_t freeRam = 0, totalRam = 0, swapAvail = 0, memAvail = 0;
    Util::memInfo(freeRam, totalRam, swapAvail, memAvail, false);
    mem.pushMemFreeAtStart(memAvail);
}

std::string Util::toString( const std::wstring &src )  {
  char* szSource = new char[ src.length()+1 ];
  wcstombs( szSource, src.c_str(), src.length()+1 );
  std::string result(szSource);
  delete [] szSource;
  return result;
}
