////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
//
// This program is free software; you can redistribute it and/or modify 
// it under the terms of the GNU General Public License (version 2) as 
// published by the Free Software Foundation.
// 
// This program is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
// General Public License for more details.
// 
// You should have received a copy of the GNU General Public License 
// along with this program;if not, write to the 
// 
// Free Software Foundation, Inc., 
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
////////////////////////////////////////////////////////////////

/**
 * @file   Util.cpp
 * @author Chuck Sugnet
 * @date   Mon May 16 16:04:48 2005
 * 
 * @brief   General Utilities.
 * 
 * 
 */

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

#ifdef WIN32
#include <windows.h>
#include <io.h>
#include <stdio.h>
#include <direct.h>
#include <stdlib.h>
#define 	S_ISDIR(m)   (((m) & S_IFMT) == S_IFDIR)

#ifndef _DLL /* _MSC_VER < 1400 */
#ifndef __MINGW32__
#define		open	_open
#define		close	_close
#define		mkdir	_mkdir
#endif
#endif

#else
#include <unistd.h>
#endif

// OS stuff for memInfo
#ifdef __APPLE__
#include <mach/mach.h>
#endif
#ifdef __linux__
#include <sys/sysinfo.h>
#endif

//
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <map>

#include "Verbose.h"
#include "Err.h"
#include "Util.h"
#include "Convert.h"
#include "RowFile.h"
#include "TableFile.h"

using namespace std;

/** 
 * Create a copy of a string. Free this with delete [] (or freezArray()) when
 * done.
 * @param s - c-string to be copied.
 * @return char * newly allocated c-string.
 */
char *Util::cloneString(const char *s) {
  char *copy = NULL;
  int length = -1;
  if(s == NULL)
    return NULL;

  length = strlen(s);
  //  copy = new ( sizeof(char) * (length+1) );
  copy = new char[length + 1];
  strncpy(copy, s, length+1);
  return copy;
}

/** 
 * Open an ofstream for writing to. Abort if can't open
 * for some reason.
 * @param out - stream to be opened.
 * @param fileName - name of file to be opened.
 */
void Util::mustOpenToWrite(std::ofstream &out, const char *fileName) {
  assert(fileName);
  out.open(fileName);
  if(!out.is_open() || !out.good()) {
    Err::errAbort("Couldn't open file: " + string(fileName) + " to write.");
  }
  // Set to throw an exception if something bad happens rather than silently fail.
  out.exceptions(ofstream::eofbit | ofstream::failbit | ofstream::badbit );
}

/** 
 * Close an output stream making sure that it is ok before doing so.
 * @param out - stream to be closed.
 */
void Util::carefulClose(std::ofstream &out) {
  // If file is open, do some checks to make sure that it was successful...
  if(out.is_open()) {
    if(out.bad()) {
      Err::errAbort("Util::carefulClose() - Error ofstream bad.");
    }
  }
  out.close();
}

/** 
 * Close an output stream making sure that it is ok before doing so.
 * @param out - stream to be closed.
 */
void Util::carefulClose(std::fstream &out) {
  // If file is open, do some checks to make sure that it was successful...
  if(out.is_open()) {
    if(!out.bad()) {
      //      Err::errAbort("Util::carefulClose() - Error ofstream bad.");
    }
  }
  out.close();
}

/** 
 * Return true if file is readable, false otherwise.
 * @param fileName 
 */
bool Util::fileReadable(const char *fileName) {
  int f;
  f = open(fileName, O_RDONLY);
  if(f < 0) 
    return false;
  close(f);
  return true;
}

/** 
* @brief Chop off the last character if it is a path separator.
* windows stat() can't handle having it there.
* @param s - string to have '/' or '\' chopped off if last.
*/
void Util::chompLastIfSep(std::string &s) {
	string::size_type i = s.rfind(PATH_SEPARATOR);
	if(i != string::npos && i == s.length() -1)
		s.erase(i);
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
/** 
 * Return true if directory exists and is readable, false otherwise.
 * @param dirName 
 */
bool Util::directoryReadable(const char *dirName) {
  string dirString(dirName);
  chompLastIfSep(dirString); // Windows doesn't like the trailing '\'
  struct stat s;
  int val = 0;
#ifdef WIN32
  int group = 0;
  int user = 0;
#else
  gid_t group = getgid();
  uid_t user = getuid();
#endif
  bool readable = false;
  val = stat( dirString.c_str() , &s);
  /* if it doesn't exist or isn't readable, can't read it. */
  if(!S_ISDIR(s.st_mode) || val != 0) {
    return false;
  }
  /* If user owns directory and user readable then we can read it. */
  if(user == s.st_uid) {
    readable = (S_IRUSR & s.st_mode) && (S_IXUSR & s.st_mode);
  }
  /* if group owns directory and it is user readable then we can read it. */
  else if(group == s.st_gid) {
    readable = (S_IRGRP & s.st_mode) && (S_IXGRP & s.st_mode);
  }
  /* if anybody can read directory then we can too. */
  else if((S_IROTH & s.st_mode) && (S_IXOTH & s.st_mode)) {
    readable = true;
  }
  return readable;
}

/** 
 * return true if directory exists and is readable, false otherwise.
 * @param dirname 
 */
bool Util::directoryWritable(const char *dirname) {
  string dirString(dirname);
  chompLastIfSep(dirString); // Windows doesn't like the trailing '\'
  struct stat s;
  int val = 0;
#ifdef WIN32
  int group = 0;
  int user = 0;
#else
  gid_t group = getgid();
  uid_t user = getuid();
#endif
  bool writable = false;
  val = stat( dirString.c_str(), &s);
  /* If it isn't a directory we can't write to it... */
  if(!S_ISDIR(s.st_mode) || val != 0){
    return false;
  }
  /* If user owns directory and user writeable then we can write it. */
  if(user == s.st_uid) {  
	  writable = (S_IWUSR & s.st_mode) && (S_IXUSR & s.st_mode);
  }
  /* if group owns directory and it is user writeable then we can write it. */
  else if(group == s.st_gid) {
    writable = (S_IWGRP & s.st_mode) && (S_IXGRP & s.st_mode);
  }
  /* if anybody can write directory then we can too. */
  else if((S_IWOTH & s.st_mode) && (S_IXOTH & s.st_mode)) {
	  writable = true;
  }
  return writable;
}

/** 
 * Make a directory. Returns true if directory is created
 * successfully, false if directory exists and aborts if any other
 * error is encountered.
 * @param dirName - Directory name to be made.
 * @return - true if created sucessfully, false if already exists.
 */
bool Util::makeDir(const char *dirName) {
  int error;
#ifdef WIN32
  error = mkdir(dirName);
#else
  error = mkdir(dirName, 0777);
#endif
  if(error != 0 && errno == EEXIST) 
    return false;
  else if(error != 0) 
    Err::errAbort("Error: Util::makeDir() - Error making directory " + ToStr(dirName));
  return true;
}

/**
 * Create a directory. Returns a pointer to error message, else 0.
 * An error is reported if no directory currently exists and a
 * new directory could not be created, or if a file, not a
 * directory, already exists, with the requested name.
 * No error is reported if the directory already exists.
 *
 * @param dirName - Directory name to be made.
 * @return - Pointer to error message, if any, else zero.
 */
std::string* Util::createDir(const char *dirName) {
#ifdef WIN32
  // Windows: use ISO C++ conformants; POSIX functions are deprecated.
  struct _stat outDirStat;
  const int statError = _stat (dirName, &outDirStat);
#else
  struct stat outDirStat;
  const int statError = stat (dirName, &outDirStat);
#endif

  // Error return from fstat - requested output directory
  // doesn't exist, so we create one.
  string* msg = 0;
  if (statError != 0) {
  #ifdef WIN32
    // Windows: use ISO C++ _mkdir; umask is deprecated.
    const int mkdirError = _mkdir (dirName);
  #else
    const mode_t Umask = umask (0);

    // Set directory execute permissions based on current umask.
    mode_t Mode = S_IRWXU;	// User (this code) can read, write, execute.
    Mode |= (~Umask & (S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH));
    if (Mode & (S_IRGRP | S_IWGRP)) // Group, other access is optional.
      Mode |= S_IXGRP;
    if (Mode & (S_IROTH | S_IWOTH))
      Mode |= S_IXOTH;
    const int mkdirError = mkdir (dirName, Mode);

    // Restore caller's umask.
    umask (Umask);
  #endif
    if (mkdirError != 0) {
      // Report error.
      msg = new string ("Unable to create directory " + string (dirName));
      return msg;
    }
  } // end if (statError != 0)

#ifndef WIN32
  // Requested file exists - just check if it's a directory.
  else if (! S_ISDIR (outDirStat.st_mode) ) {
    // Report error.
    msg = new string ("Requested output directory " + string (dirName) + " is not a directory");
    return msg;
  }
#endif
  // No error found.
  return 0;
}

/** 
 * Chop up a string into a vector of words.
 * 
 * @param s - string of interest.
 * @param delim - delimiter to split on.
 * @param words - vector to put words into, will be cleared then filled.
 */
void Util::chopString(const std::string &s, char delim, std::vector<std::string> &words) {
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
 * @brief Get the root of a filename. Chops on '/' for unix and
 * '\' or '/' windows.
 * @param s - File name to find root of.
 * @return - Root of string.
 */
string Util::fileRoot(const std::string &s) {
  string::size_type pos = 0;
  string name;
  pos = s.rfind(PATH_SEPARATOR);
#ifdef WIN32
  // accept the unix path separator on windows for cygwin and sanity.
  string::size_type unixPos = s.rfind("/");
  if(unixPos != string::npos) {
    pos = unixPos;
  }
#endif
  if(pos != string::npos)
    name = s.substr(pos+1);
  else 
    name = s;
  assert(name.length() > 0);
  return name;
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
 * 
 * @return - Number of differences >= epsilon found.
 */
int Util::matrixDifferences(const char *targetFile, const char *queryFile, 
                      int colSkip, int rowSkip, double epsilon, bool printMismatch,
                      bool matchRows) {
  vector< vector<double> > qMatrix, tMatrix;
  unsigned int rowIx = 0, colIx = 0;
  unsigned int numCol = 0, numRow = 0;
  bool same = true;
  map<string, int> qMatrixMap;
  vector<string> qMatrixRows, tMatrixRows;
  unsigned int diffCount = 0;
  unsigned int rowDiffCount = 0;
  double maxDiff = 0;
  Verbose::out(2, "Reading in file: " + ToStr(targetFile));
  RowFile::matrixFromFile(targetFile, qMatrix, rowSkip, colSkip);
  Verbose::out(2, "Reading in file: " + ToStr(queryFile));
  RowFile::matrixFromFile(queryFile, tMatrix, rowSkip, colSkip);

  /* If we are matching by rows grab the first column from each file
     and assume it is the unique row names. Create a map from the 
     names to the indexes for qMatrix */
  if(matchRows) {
    colIx = 0;
    Verbose::out(2, "Reading in rownames.");
    TableFile::columnFromFile(targetFile, qMatrixRows, colIx, rowSkip, true);
    TableFile::columnFromFile(queryFile, tMatrixRows, colIx, rowSkip, true);
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
      maxDiff = max(maxDiff, fabs(val));
      if(!(fabs(val) <= epsilon)) {
        same = false;
        rowDiff = true;
        diffCount++;
        if(printMismatch) {
          Verbose::out(1, "row: " + ToStr(rowIx) + " col: " + ToStr(colIx) + " (" +
                       ToStr(qMatrix[qMatRowIx][colIx]) + " vs. " + ToStr( tMatrix[rowIx][colIx]) + ")" + " Diff: " + ToStr(val));
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
    Verbose::out(2, "Different in " + ToStr(rowDiffCount) + " of " + ToStr(numRow) + " ( " + ToStr(percent) + "% )  entries.");
    percent = (float) diffCount / total * 100;
    Verbose::out(2, "Different at " + ToStr(diffCount) + " of " + ToStr(total) + " ( " + ToStr(percent) + "% )  entries.");
  }
  return diffCount;
}


/** 
 * Simple minded function for converting unix paths to windows and
 * vice-versa. Will fail if any drives are specified or any escaping of 
 * characters are going on...
 * 
 * @param path - filename/dirname to be converted
 * 
 * @return converted filename for that platform.
 */
std::string Util::getPathName(const char *path) {
  std::string s(path);
  if(s.find(':') != std::string::npos)
    Err::errAbort("Can't convert " + ToStr(path) + " as it contains a ':' character");
  // 92 is the ascii code for '\'
#if defined (WIN32)
  subChar(s, '/', 92);
#else
  subChar(s, 92, '/');
#endif
  return s;
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
  

/** 
 * Determine the free and total amount of memory in bytes on this
 * machine.
 * 
 * @param free -  Bytes available currently.
 * @param total - Total bytes installed on machine.
 * @param swapAvail - Amount of swap available on machine.
 * @param memAvail  - Amount of space we should consider available.
 * 
 * @return true if successful, false otherwise.
 */
bool Util::memInfo(uint64_t &free, uint64_t &total, uint64_t &swapAvail, uint64_t& memAvail) {
  bool success = false;

#ifdef WIN32 
#define meminfo_defined

  MEMORYSTATUSEX statex;
  statex.dwLength = sizeof(statex);
  if(GlobalMemoryStatusEx(&statex) == 0) {
    Err::errAbort("Util::memInfo() - Could not determine memory usage with: GlobalMemoryStatusEx().");
  }
  free = statex.ullAvailPhys;
  total = statex.ullTotalPhys;
  swapAvail = statex.ullAvailVirtual - statex.ullAvailPhys;
  memAvail = (uint64_t)(free * 0.90);
  success = true;
#endif

// 386 systems cant map more than 2GB of user memory.
// (unless you patch...)
#define MEMINFO_2GB_MAX (2UL*1024*1024*1024)


#ifdef __linux__
#define meminfo_defined
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

#ifdef __i386__
  // we test for __i386__ as these systems cant map more than 2GB of space
  // even if they have more RAM in the system.
  if (memAvail>MEMINFO_2GB_MAX) {
    memAvail=MEMINFO_2GB_MAX;
  }
#endif
  success = true;
#endif

  /// @todo Cant we test for __DARWIN__?
#ifdef __APPLE__
#define meminfo_defined

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

  // clamp it to 2GB
  if (memAvail>MEMINFO_2GB_MAX) {
    memAvail=MEMINFO_2GB_MAX;
  }

  success = true;
#endif

  // The catch all clause
#ifndef meminfo_defined
  success = false;
#endif

  return success;
}
