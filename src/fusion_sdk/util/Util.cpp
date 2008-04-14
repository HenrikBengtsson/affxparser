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

//
#include <errno.h>
#include <fcntl.h>
#include <map>
#include <math.h>
#include <sstream>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>
//
#include "util/Convert.h"
#include "util/Err.h"
#include "util/RowFile.h"
#include "util/TableFile.h"
#include "util/Util.h"
#include "util/Verbose.h"

#define	POSIX_OPEN open
#define POSIX_CLOSE close
#define	POSIX_MKDIR mkdir

#ifdef WIN32
#include <direct.h>
#include <io.h>
#include <windows.h>
#include <float.h>
#define 	S_ISDIR(m)   (((m) & S_IFMT) == S_IFDIR)

#ifndef __MINGW32__
#define	POSIX_OPEN _open
#define POSIX_CLOSE _close
#define	POSIX_MKDIR _mkdir
#endif /* __MINGW32__ */

#else
#include <unistd.h>
#endif /* WIN32 */

// OS stuff for memInfo
#ifdef __APPLE__
#include <mach/mach.h>
#endif
#ifdef __linux__
#include <sys/sysinfo.h>
#endif
#ifdef __sun__
#include <unistd.h>
#include <ieeefp.h>
#endif


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

bool
Util::stringEndsWith(const std::string& str,const std::string& end)
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
      Err::errAbort("Util::carefulClose() - ofstream bad.");
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
    if(out.bad()) {
      Err::errAbort("Util::carefulClose() - ofstream bad.");
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
  f = POSIX_OPEN(fileName, O_RDONLY);
  if(f < 0) 
    return false;
  POSIX_CLOSE(f);
  return true;
}

/**
 * Return true on success. False otherwise
 * @param in - file to copy
 * @param out - name of the new file
 */
bool Util::fileCopy(const char *in, const char *out) {
  ///@todo there is probably a better way to copy files and check for errors
  bool result = true;

  std::ifstream is (in,ios::binary);
  std::ofstream os (out,ios::binary);
  if(!is.good() || !os.good())
      result = false;

  os << is.rdbuf();
  if(!is.good() || !os.good())
      result = false;

  is.close();
  os.close();
  if(!is.good() || !os.good())
      result = false;

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
 * @brief return true if directory exists and is readable, false otherwise
 * @param dirname The dir to test for write-ability
 */
bool Util::directoryWritable(const char* dirname) {
  std::string dirname_tmp;
  struct stat s;
  bool writable = false;

#ifdef WIN32
  int group = 0;
  int user = 0;
#else
  gid_t group = getgid();
  uid_t user = getuid();
#endif

  // we dont want the trailing slash.
  dirname_tmp=dirname;
  chompLastIfSep(dirname_tmp);

  int rv = stat(dirname_tmp.c_str(), &s);

  /* Cant write if not found. */
  if (rv!=0) {
    return false;
  }
  /* If it isn't a directory we can't write to it... */
  if (!S_ISDIR(s.st_mode)) {
    return false;
  }
  /* If user owns directory and user writeable then we can write it. */
  if (user == s.st_uid) {  
	  writable = (S_IWUSR & s.st_mode) && (S_IXUSR & s.st_mode);
  }
  /* if group owns directory and it is user writeable then we can write it. */
  else if (group == s.st_gid) {
    writable = (S_IWGRP & s.st_mode) && (S_IXGRP & s.st_mode);
  }
  /* if anybody can write directory then we can too. */
  else if((S_IWOTH & s.st_mode) && (S_IXOTH & s.st_mode)) {
	  writable = true;
  }
  //
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
  error = POSIX_MKDIR(dirName);
#else
  error = POSIX_MKDIR(dirName, 0777);
#endif
  if(error != 0 && errno == EEXIST) 
    return false;
  else if(error != 0) 
    Err::errAbort("Error: Util::makeDir() - failed to make directory " + ToStr(dirName));
  return true;
}

/// @todo remove this function -- redundant with makeDir
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
    const int mkdirError = POSIX_MKDIR (dirName, Mode);

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
  if(s.empty()) 
    return s;
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
      bool failed = (fabs(val) > epsilon);
      bool okFinite = true;
      okFinite &= isFinite(qMatrix[qMatRowIx][colIx]);
      okFinite &= isFinite(tMatrix[rowIx][colIx]);
      if(!okFinite) 
        Verbose::out(1, "Non-finite floating point numbers at row: " + ToStr(rowIx) + " column: " + ToStr(colIx));
      if(failed || !okFinite) {
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

  proc_meminfo.open(proc_meminfo_filename.c_str());
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
/// @param     swapAvail The amount of swap avail. (bytes)
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

/** 
 * Determine the free and total amount of memory in bytes on this machine.
 * 
 * @param free      - Bytes available currently.
 * @param total     - Total bytes installed on machine.
 * @param swapAvail - Amount of swap available on machine.
 * @param memAvail  - Amount of space we should consider available.
 * @param cap32bit  - Cap the memory at 4GB.
 * 
 * @return true if successful, false otherwise.
 */
bool Util::memInfo(uint64_t &free, uint64_t &total, 
                   uint64_t &swapAvail, uint64_t& memAvail, 
                   bool cap32bit) {
  bool success = false;
  bool is32bit = false;
  free=total=swapAvail=memAvail=0;

  // One of these should be defined.
#ifdef WIN32
  is32bit = true;
  success=memInfo_win32(free,total,swapAvail,memAvail);
#endif
#ifdef __linux__
  success=memInfo_linux("/proc/meminfo",free,total,swapAvail,memAvail);
#endif
#ifdef __APPLE__
  is32bit = true; // Until 10.5 OS X can't address big mem even on G5, what a bummer
  success=memInfo_darwin(free,total,swapAvail,memAvail);
#endif
#ifdef __sun__
  success=memInfo_solaris(free,total,swapAvail,memAvail);
#endif
#ifdef __i386__
  is32bit = true;
#endif

  // The catch all clause
  //#ifndef memInfo_defined
  //#error memInfo is not defined for this platform
  //#endif

  //#ifdef __i386__ ||
  // we test for __i386__ as these systems cant map more than 2GB of space
  // even if they have more RAM in the system.
  if (memAvail>MEMINFO_2GB_MAX && is32bit && cap32bit) {
    memAvail=MEMINFO_2GB_MAX;
  }
  //#endif

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
void Util::printStringWidth(std::ostream &out, const char *str, int prefix,
                            int currentPos, int maxWidth ) {
  const char *wStart = NULL, *wEnd = NULL; /* Start and end of word pointers. */
  int position = currentPos;
  int nextSize = 0;
  int i = 0;
  wStart = str;

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
      *wStart++;
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
    if(wEnd != '\0' &&
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
  char *timeStr = NULL;
  struct tm *tp;
  time_t t = time(NULL);
  tp = localtime(&t);
  timeStr = asctime(tp); // timestamp prefix for log entries
  if(timeStr == NULL) {
    timeStr = "unknown";
  } else if(strlen(timeStr) < 24) {
    timeStr[strlen(timeStr) - 1] = '\0'; // knock off trailing '\n'
  } else {
    // we knock of to a fixed size as we've seen
    // trailing garbage if using strlen() suggesting
    // that there are times when the null term is not
    // where we expect it.
    timeStr[24] = '\0'; // knock off trailing '\n'
  }
  std::string timess = timeStr;
  return timess;
}
