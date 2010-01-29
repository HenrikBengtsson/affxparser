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
#include "util/Convert.h"
#include "util/Err.h"
//
#include "apt/apt.h"
#include "portability/affy-base-types.h"
//
#include <cassert>
#include <cctype>
#include <cstring>
#include <ctime>
#include <fstream>
#include <string.h>
#include <string>
#include <vector>
//




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
      str[i] = tolower(str[i]);
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

  /**
   * Search the affy library file path for the actual file name to open
   * @param fileName - the name of the file to find
   * @param searchPath - alternative search path
   */
  static std::string findLibFile(const std::string &fileName, const std::string &searchPath = "");

  /** 
   * Open an ofstream for writing to. Abort if can't open for some reason.
   * @param out - stream to be opened.
   * @param fileName - name of file to be opened.
   */
  static void mustOpenToWrite(std::ofstream &out, const std::string& fileName);
 
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
  static bool fileReadable(const std::string &fileName);

  /** 
   * Return true if file exists, false otherwise.
   * @param fileName 
   */
  static bool fileExists(const std::string &fileName);

  /**
   * Return true on success. False otherwise
   * @param in - file to copy
   * @param out - name of the new file
   */
  static bool fileCopy(const std::string &in, const std::string &out, bool throwOnError = true); 

  /**
   * Return true on success. False otherwise
   * @param in - file to copy
   * @param out - name of the new file
   */
  static bool fileRename(const std::string &in, const std::string &out, bool throwOnError = true);

  /*! Deletes a file.
   * @param fileName The name of the file to delete.
   * @return True if the file was deleted.
   */
  static bool fileRemove(const std::string &fileName, bool throwOnError = true);

  static void fileRemove(std::vector<std::string> &filesToRemove);

  /// @brief     removes the directory
  /// @param     dirName       name of the dir to remove.
  /// @param     throwOnError  throw an error if it fails.
  /// @return    true if removed.
  static bool dirRemove(const std::string &dirName, bool throwOnError = true);

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
   * Return true if directory exists and is readable, false otherwise.
   * @param dirName 
   */
  static bool directoryReadable(const std::string &dirName); 
  
  /** 
   * Return true if directory exists and is readable, false otherwise.
   * @param dirName 
   */
  static bool directoryWritable(const std::string &dirName); 

  /** 
   * Make a directory. Returns true if directory is created
   * successfully, false if directory exists and aborts if any other
   * error is encountered.
   * @param dirName - Directory name to be made.
   * @return - true if created sucessfully, false if already exists.
   */
  static bool makeDir(const std::string &dirName); 

  /// @brief     Creates a directory path
  /// @param     pathName   
  /// @return    true if a writeable directory.
  static bool makeDirPath(const std::string &pathName);

  /** 
   * Chop up a string into a vector of words.
   * 
   * @param s - string of interest.
   * @param delim - delimiter to split on.
   * @param words - vector to put words into, will be cleared then filled.
   */
  static void chopString(const std::string& s,const char delim,std::vector<std::string>& words);

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
   * @brief Get the root of a filename. Chops on '/' for unix and
   * '\' or '/' windows.
   * @param s - File name to find root of.
   * @return - Root of string.
   */
  static std::string fileRoot(const std::string& filename);
  static std::vector<std::string> fileRoot(const std::vector<std::string>& filename_vec);

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
   * 
   * @return - Number of differences >= epsilon found.
   */
  static int matrixDifferences(const std::string& targetFile, 
                               const std::string& queryFile, 
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
   * characters are going on. On windows this method will attempt to handle
   * long paths/filenames by converting the specified path to a UNC absolute
   * path with a leading "\\?\".
   * 
   * @param path -       filename/dirname to be converted
   * @param singleFile - boolean to indicate if the path/string is a single file 
   *                     or if it has multiple files (eg a whole command line). 
   *                     If it is a whole command line then to attempt to handle 
   *                     long paths/filenames on windows will be attempted
   * 
   * @return converted filename for that platform.
   */
  static std::string convertPathName(const std::string &path, bool singleFile = true);


  /**
   * Schrage's algorithm for generating random numbers in 32 bits.  
   * @param ix - pointer to integer seed, cannot be zero.
   */
  static int32_t schrageRandom(int32_t *ix);

  static std::string asMB(uint64_t x);

  static bool memInfo(uint64_t &free, uint64_t &total, uint64_t &swapAvail,uint64_t& memAvail, bool cap32bit=true);

  static uint64_t getMemFreeAtStart();
  static uint64_t getMemFreeAtBlock();
  static void pushMemFreeAtStart();
  static void popMemFreeAtStart();

  /** 
   * Determine the available space in bytes on given volume.
   * 
   * @param path - path to any file on the volume.
   * 
   * @return available space in bytes.
   */
  static int64_t getAvailableDiskSpace(const std::string& path);

  /** 
   * Determine if two files/dirs are on the same volume.
   * 
   * @param path1 - first path to any file/dir on the volume.
   * @param path2 - second path to any file/dir on the volume.
   * 
   * @return 1 if same volume, 0 if not, -1 if unsupported platform.
   */
  static int64_t isSameVolume(const std::string& path1, const std::string& path2);

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
