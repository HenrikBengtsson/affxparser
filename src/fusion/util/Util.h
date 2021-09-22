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
 * @file   Util.h
 * @author Chuck Sugnet
 * @date   Mon May 16 15:52:31 2005
 *
 * @brief  General Utilities.
 */

#ifndef _UTIL_H_
#define _UTIL_H_

/// uncomment this to remove functions which are 
//#define APT_OBSOLETE_FUNCS 1

//
#include "portability/affy-base-types.h"
#include "portability/apt-win-dll.h"
#include "util/Convert.h"
#include "util/Err.h"
//
#include <cassert>
#include <cctype>
#include <cstring>
#include <ctime>
#include <fstream>
#include <string>
#include <vector>


#define MEGABYTE 1048576

/// 386 systems cant map more than 2GB of user memory.
/// (unless you have a patched kernel...)
#ifdef WIN32
#ifdef _WIN64
#define MEMINFO_2GB_MAX (2UL*1024*1024*1024)
#else
// cap at 1.3G on Windows due to expected memory fragmentation
/// @todo This was 1.7G on head before merge with python branch. Further work on genotype engine may allow for bumping this back up some.
#define MEMINFO_2GB_MAX floor(1.3f*1024*1024*1024)
#endif
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
template<class T> inline void Freez(T*& p) { delete p; p = NULL; }
/** delete function that deletes an array and sets the pointer to NULL. */
template<class T> inline void FreezArray(T*& p) { delete[] p; p = NULL; }

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
class APTLIB_API Util {
public:

  /**
   * Make the string all lowercase.
   * @param s - string to be modified
   */
  static void downcaseString_inplace(std::string& str) {
    for(unsigned int i = 0; i < str.size(); i++) {
      str[i] = (char)tolower(str[i]);
    }
  }
  /// @brief     Copy and downcase a 8bit string.
  /// @param     str_orig
  /// @return
  static std::string downcaseString(const std::string& str_orig) {
    std::string str=str_orig;
    downcaseString_inplace(str);
    return str;
  }


  /**
   * Make the string all uppercase.
   * @param s - string to be modified
   */
  static void upcaseString_inplace(std::string& str) {
    for(unsigned int i = 0; i < str.size(); i++) {
      str[i] = (char)toupper(str[i]);
    }
  }
  /// @brief     Copy and upcase a 8bit string.
  /// @param     str_orig
  /// @return
  static std::string upcaseString(const std::string& str_orig) {
    std::string str=str_orig;
    upcaseString_inplace(str);
    return str;
  }

  /** Comparison object for use in map for char * */
   struct ltstr {
     /// Is one string less than another?
     bool operator()(const char* s1, const char* s2) const {
      return strcmp(s1, s2) < 0;
     }
   };

   struct ltstring {
     /// Is one string less than another?
     bool operator()(const std::string& s1, const std::string& s2) const {
       return strcmp(s1.c_str(), s2.c_str()) < 0;
       //return s1.compare(s2)<0; //vliber
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
  static char *cloneString(const char *);
  // one shouldnt need this. -jhg
  // static char *cloneString(const std::string &);

  /**
   * @brief     Does the string end with the other string?
   * @param     str       string to check
   * @param     endstr    ending string.
   * @return    true if str ends with endstr.
   */
  static bool stringEndsWith(const std::string& str,const std::string& end);

  // like AffxString but uses std::string.

  /// @brief     Check to see if STR ends with ENDING.
  /// @param     str
  /// @param     ending
  /// @return    true if it does.
  static bool endsWithStr(const std::string& str,const std::string& ending);
  /// @brief     Checks to see if STR ends with ENDING ignoring the last POSFROMEND chars.
  ///            Basicly the POSFROMEND acts as a wildcard "." at the end of the string.
  /// @param     str
  /// @param     ending
  /// @param     posFromEnd number
  /// @return    true if it does
  static bool endsWithStr(const std::string& str,const std::string& ending,int posFromEnd);

  //#ifndef APT_OBSOLETE_FUNCS
  /**
   * @brief Chop off the last character if it is a path separator.
   * windows stat() can't handle having it there.
   * @param s - string to have '/' or '\' chopped off if last.
   */
  static void chompLastIfSep(std::string &s);

  /**
   * Chop the last suffix (as defined by '.') from a string
   * @param - string to chop
   * @param - delimiter, default '.'
   */
  static std::string chopSuffix(const std::string& s, char d = '.');


  /**
   * Chop up a string into a vector of words.
   *
   * @param s - string of interest.
   * @param delim - delimiter to split on.
   * @param words - vector to put words into, will be cleared then filled.
   */
  static void chopString(const std::string& s,const char delim,std::vector<std::string>& words);

  /**
   * Chop up a string into a vector of words.
   *
   * @param s - string of interest.
   * @param delims - delimiters to split on. The split will occur at any character
   *               - among those present in the string.
   * @param words - vector to put words into, will be cleared then filled.
   */
  static void chopString(const std::string& s, const char* delims, std::vector<std::string>& words);

  /**
   * @brief Cut off any preceding and trailing white space.
   * @param s - String to be trimmed.
   * @param whitespace - String with characters to be trimmed.
   */
  static void trimString(std::string& s, const char *whitespace  = " \r\n\t") {
    s = s.erase(s.find_last_not_of(whitespace)+1);
    s = s.erase(0,s.find_first_not_of(whitespace));
  }

  /// @brief     Change the end of a string
  /// @param     str       string to change
  /// @param     from      string to change from
  /// @param     to        string to change to
  static void changeEnd(std::string& str,const std::string& from,const std::string& to);

  /// @brief     Change the endings of a vector of strings.
  /// @param     str_vec   vector to change
  /// @param     from      string to change from
  /// @param     to        string to change to
  static void changeEnd(std::vector<std::string>& str_vec,const std::string& from,const std::string& to);

  /**
   * @brief Check to see if two strings are the same.
   *
   * @param s1 - string 1.
   * @param s2 - string 2.
   *
   * @return true if compare() considers them the same.
   */
  static bool sameString(const std::string &s1, const std::string &s2) {
    return s1.compare(s2) == 0;
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
   * @param fraction - Maximum fractional difference considered equivalent (default: not used).
   * @param printMismatchMax - Maximum number of mismatches to print (default: no limit).
   *
   * @return - Number of differences >= epsilon found.
   */
  static int matrixDifferences(const std::string& targetFile,
                               const std::string& queryFile,
                               int colSkip, int rowSkip,
                               double epsilon,
                               bool printMismatch,
                               bool matchRows,
                               double fraction = 0.0,
                               int printMismatchMax = -1);

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
   * Replace the from string with the to string for
   * all instances in string supplied.
   *
   * @param s - string to do the replacement in.
   * @param from - original string to be replaced.
   * @param to - string to do the replacing with.
   */
  static void replaceString(std::string &s, const std::string &from, const std::string &to);

  static void breakByString(const std::string &s, const std::string &delim, std::vector<std::string> &words);

  static void removeChar(std::string &s, char character) {
    std::ostringstream buf;
    for (size_t i = 0; i < s.length(); i++) {
      if (s.at(i) != character) {
        buf << s.at(i);
      }
    }
    s = buf.str();
  }

  static bool is32Bit() {
    bool is32 = true;
    int nBytes = sizeof(size_t);
    if (nBytes == 4) {
      is32 = true;
    }
    else if (nBytes == 8) {
      is32 = false;
    }
#ifdef _WIN64
    is32 = false;
#endif
#ifdef __LP64__
    is32 = false;
#endif
    return is32;
  }


  /**
   * Schrage's algorithm for generating random numbers in 32 bits.
   * @param ix - pointer to integer seed, cannot be zero.
   */
  static int32_t schrageRandom(int32_t *ix);

  static std::string asMB(uint64_t x);

  /**
   * Determine the free and total amount of memory in bytes on this machine.
   *
   * @param free      - Bytes available currently.
   * @param total     - Total bytes installed on machine.
   * @param swapAvail - Amount of swap available on machine. OSX can use all disk space so reports 0 on OSX
   * @param memAvail  - Amount of space we should consider available.
   * @param cap32bit  - Cap the memory at 4GB.
   *
   * @return true if successful, false otherwise.
   */
  static bool memInfo(uint64_t &free, uint64_t &total, uint64_t &swapAvail,uint64_t& memAvail, bool cap32bit=true);

  static uint64_t getMemFreeAtStart();
  static uint64_t getMemFreeAtBlock();
  static void pushMemFreeAtStart();
  static void popMemFreeAtStart();

  /**
   * Return a pointer to the next character that is white space
   * or NULL if none found.
   * @param s - cstring to find white space in.
   * @return - Pointer to next whitespace character or NULL if none
   *   found.
   */
  static const char *nextWhiteSpace(const char *);

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
  static void printStringWidth(std::ostream &out,const std::string& str, int prefix,int currentPos, int maxWidth=70);

  /**
   * Wrapper for different version of isnan() on different systems.
   * @param x - number to be checked for NaN or INF
   * @return - true if x is finite (-INF < x && x < +INF && x != nan), false otherwise
   */
  static bool isFinite(double x);

   inline static void PrintTextClassTitle(const std::string &className){
    printf ("****%s****\n",className.c_str());
  }

  static void PrintTextFunctionTitle(const std::string &className, const std::string &functionName){
    printf ("****%s::%s****\n",className.c_str(),functionName.c_str());
  }

  static std::vector<std::string> addPrefixSuffix(std::vector<std::string> middle,
                          const std::string &prefix, const std::string &suffix);
  static std::vector<std::string> addPrefixSuffix(const char* middle[],
                          const std::string &prefix, const std::string &suffix);
  static std::vector<std::string> addPrefixSuffix(const char* middle[], int size,
                          const std::string &prefix, const std::string &suffix);
  static std::vector<std::string> listToVector(const char* in[], int size);
  static std::vector<std::string> listToVector(const char* in[]);

  static std::vector<std::string> addPrefixSuffix(const char* middle[],
                          const std::string &prefix);

  static std::vector<std::string> addPrefixSuffix(std::string middle[],
                          const std::string &prefix, const std::string &suffix);
  static std::vector<std::string> addPrefixSuffix(std::string middle[], int size,
                          const std::string &prefix, const std::string &suffix);
  static std::vector<std::string> listToVector(std::string in[]);
  static std::vector<std::string> listToVector(std::string in[], int size);

  static std::string joinVectorString(std::vector<std::string> toJoin, const std::string &sep);

  static std::string escapeString(const std::string &s, char c, char escape='\\') {
    std::string e;
    e.reserve(s.size());
    for (size_t i = 0; i < s.size(); i++) {
      if (s[i] == c || s[i] == escape) {
        e.push_back(escape);
      }
      e.push_back(s[i]);
    }
    return e;
  }

  static std::string deEscapeString(std::string &s, char escape='\\') {
    std::string e;
    e.reserve(s.size());
    for (size_t i = 0; i < s.size(); i++) {
      if (s[i] == escape) {
        i++;
      }
      e.push_back(s[i]);
    }
    return e;
  }

  static std::wstring toWString( const std::string &src ) {
    std::wstring dst( src.length(), L' ');
    std::copy( src.begin(), src.end(), dst.begin());
    return dst;
  }
  static std::string toString( const std::wstring &src );

private:
    class StaticMem {
        public:
            StaticMem() {
                uint64_t freeRam = 0, totalRam = 0, swapAvail = 0, memAvail = 0;
                Util::memInfo(freeRam, totalRam, swapAvail, memAvail, false);
                m_MemFreeAtStart.push_back(memAvail);
            }
            uint64_t getMemFreeAtStart() {
                return m_MemFreeAtStart[0];
            }
            uint64_t getMemFreeAtBlock() {
                return m_MemFreeAtStart[m_MemFreeAtStart.size()-1];
            }
            void pushMemFreeAtStart(uint64_t mem) {
                m_MemFreeAtStart.push_back(mem);
            }
            uint64_t popMemFreeAtStart() {
                uint64_t mem = m_MemFreeAtStart[m_MemFreeAtStart.size()-1];
                m_MemFreeAtStart.pop_back();
                return mem;
            }

        private:
            std::vector<uint64_t> m_MemFreeAtStart;
    };

    static StaticMem &getStaticMem();

};



#ifdef __linux__
bool memInfo_linux(std::string proc_meminfo_filename,
                   uint64_t &free, uint64_t &total,
                   uint64_t &swapAvail, uint64_t& memAvail);
#endif



#endif /* _UTIL_H */
