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
 * @file   Util.h
 * @author Chuck Sugnet
 * @date   Mon May 16 15:52:31 2005
 * 
 * @brief  General Utilities.
 */

#ifndef _UTIL_H_
#define _UTIL_H_

//
#include <assert.h>
#include <fstream>
#include <string.h>
#include <string>
#include <vector>
#include <ctype.h>
#include <time.h>
//
#include "util/Convert.h"
#include "util/Err.h"
#include "portability/affy-base-types.h"

/// The string which seperates path components on windows
#define PATH_SEPARATOR_WIN32      "\\"
/// The character which seperates path components on windows
#define PATH_SEPARATOR_WIN32_CHAR '\\'
/// The string which seperates path components on unix
#define PATH_SEPARATOR_UNIX       "/"
/// The character which seperates path components on unix
#define PATH_SEPARATOR_UNIX_CHAR  '/'

/// Path separator depends on OS...
#if defined (WIN32)
/// The perferred string for path separators on this system
#define PATH_SEPARATOR      PATH_SEPARATOR_WIN32
/// The perferred char for path separators on this system
#define PATH_SEPARATOR_CHAR PATH_SEPARATOR_WIN32_CHAR
#else
#define PATH_SEPARATOR      PATH_SEPARATOR_UNIX
#define PATH_SEPARATOR_CHAR PATH_SEPARATOR_UNIX_CHAR
#endif

/// 386 systems cant map more than 2GB of user memory.
/// (unless you have a patched kernel...)
#ifdef WIN32
// cap at 1.3G on Windows due to expected memory fragmentation
/// @todo This was 1.7G on head before merge with python branch. Further work on genotype engine may allow for bumping this back up some.
#define MEMINFO_2GB_MAX floor(1.3f*1024*1024*1024)
#else
#define MEMINFO_2GB_MAX (2UL*1024*1024*1024)
#endif

/// Log base two
#define log2(x) ( log((x))/log((2.0)) )
/// Square of the two numbers
/// (Note: this is bad style due to X being evaled twice.)
#define sqr(x) ((x)*(x))

/// Returns the number of elements in the array.
#define ArraySize(a) (sizeof(a)/sizeof((a)[0]))

/** delete function that deletes the pointer and sets it to NULL. */
template<class T> inline void Freez(T*& p) { delete p; p = 0; }
/** delete function that deletes an array and sets the pointer to NULL. */
template<class T> inline void FreezArray(T*& p) { delete[] p; p = 0; }

/** 
 * @name InstanceOf 
 * 
 * Check at run time if an object is an instance of a class.  This differs from
 * comparing the result of type_id, as it takes inheritance into account.  Note
 * that base class must be polymorphic, that is it must have a virtual function
 * or inherit from another class.
 * 
 * @param objPtr A pointer to the object to check.  A pointer must be used, not
 * an object.  If necessary, generate an address.
 * @param The class name to test against.
 * @return true if the object is an instance of the class.
 */
#define InstanceOf(objPtr, className) \
    ((bool)(dynamic_cast<const className*>(objPtr) != NULL))

/**
 *  Util
 * @brief Utility functions for memory, etc.
 */
class Util {
public:

  /** 
   * Make the string all lowercase.
   * @param s - string to be modified
   */
  static void downcaseString(std::string &s) {
    for(unsigned int i = 0; i < s.size(); i++) {
      s[i] = tolower(s[i]);
    }
  }

  /** Comparison object for use in map for char * */
   struct ltstr {
     /// Is one string less than another?
     bool operator()(const char* s1, const char* s2) const {
       return strcmp(s1, s2) < 0;
     }
   };

  /** 
   * Get a timestamp string.
   * @return - pointer to statically allocated buffer with time.
   */
  static std::string getTimeStamp();

   /**
    * Some older compilers (solaris) don't have a round function.
    */
   static int round(double x) { return ((int)((x)+0.5)); }

  /** 
   * Create a copy of a string. Free this with delete [] (or freezArray()) when
   * done.
   * @param s - c-string to be copied.
   * @return char * newly allocated c-string.
   */
  static char *cloneString(const char *s);

  /**
   * @brief     Does the string end with the other string?
   * @param     str       string to check  
   * @param     endstr    ending string.
   * @return    true if str ends with endstr.
   */
  static bool stringEndsWith(const std::string& str,const std::string& end);

  /** 
   * Open an ofstream for writing to. Abort if can't open
   * for some reason.
   * @param out - stream to be opened.
   * @param fileName - name of file to be opened.
   */
  static void mustOpenToWrite(std::ofstream &out, const char *fileName);
  static void mustOpenToWrite(std::ofstream &out, const std::string &fileName) {
    return mustOpenToWrite(out, fileName.c_str());
  }

  /** 
   * Close an output stream making sure that it is ok before doing so.
   * @param out - stream to be closed.
   */
  static void carefulClose(std::ofstream &out);

  /** 
   * Close an output stream making sure that it is ok before doing so.
   * @param out - stream to be closed.
   */
  static void carefulClose(std::fstream &out);

  /** 
   * Return true if file exists, false otherwise.
   * @param fileName 
   */
  static bool fileReadable(const char *fileName);
  static bool fileReadable(const std::string &fileName) {
      return fileReadable(fileName.c_str());
  }

  /**
   * Return true on success. False otherwise
   * @param in - file to copy
   * @param out - name of the new file
   */
  static bool fileCopy(const char *in, const char *out);
  static bool fileCopy(const std::string &in, const std::string &out) {
      return fileCopy(in.c_str(),out.c_str());
  }

  /** 
   * @brief Chop off the last character if it is a path separator.
   * windows stat() can't handle having it there.
   * @param s - string to have '/' or '\' chopped off if last.
   */
  static void chompLastIfSep(std::string &s);
  /** 
   * Return true if directory exists and is readable, false otherwise.
   * @param dirName 
   */
  static bool directoryReadable(const char *dirName);
  static bool directoryReadable(const std::string &dirName) {
      return directoryReadable(dirName.c_str());
  }
  
  /** 
   * Return true if directory exists and is readable, false otherwise.
   * @param dirName 
   */
  static bool directoryWritable(const char *dirName);
  static bool directoryWritable(const std::string &dirName) {
      return directoryWritable(dirName.c_str());
  }

  /** 
   * Make a directory. Returns true if directory is created
   * successfully, false if directory exists and aborts if any other
   * error is encountered.
   * @param dirName - Directory name to be made.
   * @return - true if created sucessfully, false if already exists.
   */
  static bool makeDir(const char *dirName);
  static bool makeDir(const std::string &dirName) {
      return makeDir(dirName.c_str());
  }

  /**
   * Create a directory. Returns a pointer to error message, else 0.
   * An error is reported if no directory currently exists and a
   * new directory could not be created, or if a file, not a
   * directory, already exists, with the requested name.
   * No error is returned if a directory already exists.
   * @param dirName - Directory name to be made.
   * @return - Pointer to error message if any, else zero.
   */
  static std::string* createDir(const char *dirName);

  /** 
   * Chop up a string into a vector of words.
   * 
   * @param s - string of interest.
   * @param delim - delimiter to split on.
   * @param words - vector to put words into, will be cleared then filled.
   */
  static void chopString(const std::string &s, char delim, 
                         std::vector<std::string> &words);

  /** 
   * @brief Cut off any preceding and trailing white space.
   * @param s - String to be trimmed.
   * @param whitespace - String with characters to be trimmed.
   */
  static void trimString(std::string& s, const char *whitespace  = " \r\n\t") {
    s = s.erase(s.find_last_not_of(whitespace)+1);
    s = s.erase(0,s.find_first_not_of(whitespace));
  }

  /** 
   * @brief Get the root of a filename. Chops on '/' for unix and
   * '\' or '/' windows.
   * @param s - File name to find root of.
   * @return - Root of string.
   */
  static std::string fileRoot(const std::string &s);

  /** 
   * @brief Check to see if two strings are the same.
   * 
   * @param s1 - string 1.
   * @param s2 - string 2.
   * 
   * @return true if strcmp() considers them the same.
   */
  static bool sameString(const char *s1, const char *s2) {
    return strcmp(s1,s2) == 0;
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
  static int matrixDifferences(const char *targetFile, const char *queryFile, 
                               int colSkip, int rowSkip, double epsilon, bool printMismatch,
                               bool matchRows);

  /** 
   * Replace the from character with the to character for
   * all instances in string supplied.
   * 
   * @param s - string to do the replacement in.
   * @param from - original character to be replaced.
   * @param to - character to do the replacing with.
   */  
  static void subChar(std::string &s, char from, char to) {
    std::string::size_type pos = 0;
    for(pos = 0; pos < s.size(); pos++) {
      if(s[pos] == from) {
        s[pos] = to;
      }
    }
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
  static std::string getPathName(const char *path);


  /**
   * Schrage's algorithm for generating random numbers in 32 bits.  
   * @param ix - pointer to integer seed, cannot be zero.
   */
  static int32_t schrageRandom(int32_t *ix);

  static bool memInfo(uint64_t &free, uint64_t &total, uint64_t &swapAvail,uint64_t& memAvail, bool cap32bit=true);

  /** 
   * Return a pointer to the next character that is white space
   * or NULL if none found. 
   * @param s - cstring to find white space in.
   * @return - Pointer to next whitespace character or NULL if none
   *   found. 
   */
  static const char *nextWhiteSpace(const char *s);

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
  static void printStringWidth(std::ostream &out, const char *str, int prefix,
                               int currentPos, int maxWidth=70 );

  /** 
   * Wrapper for different version of isnan() on different systems.
   * @param x - number to be checked for NaN or INF
   * @return - true if x is finite (-INF < x && x < +INF && x != nan), false otherwise
   */
  static bool isFinite(double x);
  
};

#ifdef __linux__
bool memInfo_linux(std::string proc_meminfo_filename,
                   uint64_t &free, uint64_t &total,
                   uint64_t &swapAvail, uint64_t& memAvail);
#endif



#endif /* _UTIL_H */
