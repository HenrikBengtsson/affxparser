////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011 Affymetrix, Inc.
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
//
// affy/sdk/util/Fs.h ---
//

#ifndef _UTIL_FS_H_
#define _UTIL_FS_H_

//
#include "portability/affy-base-types.h"
#include "util/AptErrno.h"
#include "util/Util.h"
//
#include <fcntl.h>
#include <string>
#include <sys/stat.h>
#include <vector>

/// @file   util/Fs.h
/// @brief  A class for working with the Os FileSystem.

// We are going to use unix conventions internally
// and translate them to the os requirements only when needed.
// so we define these here.
#define FS_PATHSEP "/"
//#define FSPATH_INTERNAL_PATHSEP ":"
#define FS_EXTSEP "."

#define FS_UNC_SHORTLEN (200)

/// Adds quotes to the path.
#define FS_QUOTE_PATH(_path) std::string("'"+std::string(_path)+"'")

/**
 * @class Fs
 * @brief An APT interface to the OS file handling utilities.
 * "Fs" is the simple 'paths as strings' library.
 *
 * APT should keep all paths in normalized (unix) format.
 * Fs.cpp keeps all paths as "/".  "\"s are converted to "/"s automaticly.
 *
 * This library converts the pathnames to windows as and when needed.
 * The programmer (you) should always use unix style paths.
 *
 * The paths are filesystem paths.
 * This is different than FsPath which is a richer represenation of
 * the location of a dataset.
 * (IE: No internal tablenames or groupnames.)
 *
 * All the methods are static to the class.
 * The only state is the class state of the error number and message.
 * (Just like the system errno.)
 *
 * The last param of the methods is the optional "abortOnErr"
 * which defaults to "true".
 * When "true" an error during the call
 * results in and APT_ERR_ABORT.
 * When "false" an error code is returned and the caller can
 * deal with it as they see fit.
 *
 * The "InPlace" functions are like the normal ones, but change the strings
 * Rather than returning a new string.
 *
 * Note: The convertToUncPath will not convert an already converted path.
 *       Thus calling it twice on a path should be safe.
 *       We want it to be this way as we dont know how many times
 *       this method will be called on a path.

 * Note: Many of the file libraries will call "convertToUncPath" for you.
 *       All you need to do is pass them a normalized path.
 *       file/TsvFile, File5 and the SQLite io code does do this;
 *       The legacy file IO does not.
 *
 * Note: All APIs are Windows 32KB path size compliant. This requires
 *       the internal use of std::wstring any time a file name is
 *       used. However the std::wstring implies more than just
 *       a path longer than MAX_PATH (260) bytes in lenght.
 *       The std::wstring also implies:
 *       1.) Unicode. APT code is NOT Unicode compliant.
 *           Therefore APT code should always use 
 *           std::string APIs, except when providing
 *           someone additional path, std::string API
 *           not provided here. Do not use std::wstring
 *           for application path names as this may
 *           allow Unicode characters to creep into paths
 *           that have ASCII encoding. File names in APT
 *           are encoded with application information
 *           and parsing of file names in APT is not done
 *           with Unicode parsing. Do not use std::wstring
 *           for file names and paths. 
 *       2.) Unviseral Naming Convention, or UNC.
 *           Unfortunately UNC is cognitive of Unicode.
 *           They do not refer to the same thing.
 *           Microsoft "Universal" means Microsoft
 *           specific. Specifially network paths
 *           must start with "\\?\" and be std::wstring
 *           for ALL paths, even those shorter than MAX_PATH. 
 *           All UNC paths then are also Unicode.
 *           For convienence Fs::Unc API will return
 *           the std::string UNC version so inspection
 *           of the full path being used. Given APT is
 *           is NOT Unicode compliant then the Fs::Unc
 *           returns std::string full path for ease of use.
 *       
 *             
 * 
 */

// Maybe make this a namespace, not a class?

class Fs {
private:
  /// @brief     The user shouldnt make one one of these objects.
  ///            Make this private so they cant.
  Fs();

public:

  // @todo: Make these thread local?
  //        A mutex wouldnt solve it, as the state shouldnt be shared between threads.
  //        For now, we dont worry about threading.
  
  /// The error code of the last error.
  static AptErr_t m_err_num;

  /// The error message of the last error.
  static std::string m_err_msg;

  /// @brief     Sets err num and message.  Possibly calls Err::errAbort
  /// @param     err_num   
  /// @param     err_msg 
  /// @param     abortOnErr	When true, an error calls Err::errAbort.
  /// @return    An error code.
  static AptErr_t setErr(AptErr_t err_num,const std::string& err_msg,bool abortOnErr);

  /// @brief     The current error number
  static AptErr_t getErrNum();

  /// @brief     The current error string (Should include the pathname.)
  static std::string getErrMsg();

  /// @brief     Clears the error num and message
  /// @return    always APT_OK
  static AptErr_t clearErr();

  /// @brief     
  /// @param     path      
  /// @param     abortOnErr
  /// @return    
  static AptErr_t setErrFromErrno(const std::string& path,bool abortOnErr);

  /// @brief     
  /// @param     path      
  /// @param     sys_errno 
  /// @param     abortOnErr
  /// @return    
  static AptErr_t setErrFromErrno(const std::string& path,int sys_errno,bool abortOnErr);

  /// @brief     Get the internal path seperator.
  /// @return    Always "/"
  static std::string pathSep();
  /// @brief     Get the path seperator for the OS or replace path separators for input string. 
  /// @return    Could be "\" , "/" or the translated  string if one is passed in. 
  static std::string osPathSep( const std::string& path = "");

  /// clean up the path.
  /// @brief     
  /// @param     path      
  /// @return    
  static std::string normalizePath(const std::string& path);
  static AptErr_t normalizePathInPlace(std::string& path);

  /// turn the path into a windows path string
  /// @brief     
  /// @param     path      
  /// @return    
  static std::string windowifyPath(const std::string& path);
  static AptErr_t windowifyPathInPlace(std::string& path);

  /// turn the path into a normal (unix) path string
  /// @brief     
  /// @param     path      
  /// @return    
  static std::string unixifyPath(const std::string& path);
  static AptErr_t unixifyPathInPlace(std::string& path);
  
  /// join strings with the path sep.
  /// while we would want people to keep the path with FsPath,
  /// we should face up to that there will be string pasting going on and
  /// make it less ugly.
  /// Fs::join("foo","bar") => "foo/bar"
  static std::string join(const std::string& arg1,
                          const std::string& arg2);
  static std::string join(const std::string& arg1,
                          const std::string& arg2,
                          const std::string& arg3);
  static std::string join(const std::string& arg1,
                          const std::string& arg2,
                          const std::string& arg3,
                          const std::string& arg4);
  static std::string join(std::vector<std::string>& args);

  //
  static bool hasTrailingSlash(const std::string& path_in);
  static std::string  addTrailingSlash(const std::string& path_in);
  static std::string trimTrailingSlash(const std::string& path_in);

  /// @brief     like the unix function of the same name.
  /// @param     path      
  /// @return    the basename of the path  (abc/def/foo.bar => foo.bar)
  static std::string basename(const std::string& path);

  /// @brief     Apply basename to a vector
  /// @param     paths     
  /// @return    new vector of basenames.
  static std::vector<std::string> basename(const std::vector<std::string>& paths);

  /// @brief     just the dirname (will return "." or "/" as well.)
  /// @param     path      
  /// @return    
  static std::string dirname(const std::string& path);

  /// return the stuff after the "." including the "." ("foo.bar" => ".bar")
  // that way you dont have to worry about adding the "." later.
  // this preserves the invariant of:
  // path==join(dirname(path),basename(path)+extname(path))
  static std::string extname(const std::string& path);

  /// chop off ALL the exts.  ("path/foo.a.b.c" => "path/foo")
  static std::string noextname(const std::string& path);

  /// chop off ONE of the exts.  ("path/foo.a.b.c" => "path/foo.a.b")
  static std::string noextname1(const std::string& path);

  /// chop off N of the exts ("path/foo.a.b.c",2 => "path/foo.a")
  static std::string noextnameN(const std::string& path,int cnt);

  ///
  static void splitPath(const std::string& path,std::string& drive, std::vector<std::string>& parts);

  /// replace the dir and ext for all these filenames
  /// For example a vector of chip files for output.
  ///   vec[0]="abc/foo.cel" => "outputdir/foo.chp"
  ///   vec[1]="def/bar.cel" => "outputdir/bar.chp"
  ///   vec[2]="def/baz"     => "outputdir/baz.chp"
  static std::vector<std::string> changeDirAndExt(const std::string& new_dir,
                                                  const std::vector<std::string>& paths,
                                                  const std::string& new_ext);


  ///
  static std::string convertToUncPath(const std::string& path);
  static std::wstring convertToUncPathW(const std::string& path) {return Util::toWString(Fs::convertToUncPath(path,-1)); };
  static std::string convertToUncPath(const std::string& path,int uncshortlen);
  static std::wstring convertToUncPathW(const std::string& path,int uncshortlen) { return Util::toWString( Fs::convertToUncPath( path, uncshortlen )); };

  /// Short name for convertToUncPath.
  static std::string Unc(const std::string& path) { return Fs::convertToUncPath(path); };
  static std::wstring UncW(const std::string& path) { return Fs::convertToUncPathW(path); };

  static AptErr_t convertToUncPathInPlace(std::string& path);
  static AptErr_t convertToUncPathInPlace(std::string& path,int uncshortlen);

  /// convert only the first component of the command to unc.
  /// return the command string.
  static std::string convertCommandToUnc(const std::string& cmd);
  
  /////

  /// @brief     Change the mode of a dir or file.
  ///            This is chmodBasic because on Windows only the write bit for the user
  ///            can be set. There is no notion of group and other.
  ///            Files are always readable. 
  /// @param     path      
  /// @param     mode      
  /// @param     abortOnErr
  /// @return    
  static AptErr_t chmodBasic(const std::string& path,int mode,bool abortOnErr=true);

  /////

  ///
  static bool exists(const std::string& path);
  static bool dirExists(const std::string& path);
  static bool fileExists(const std::string& path);

  static bool isReadable(const std::string& path);
  static bool isReadable(const std::string& path,AptErr_t& rv);

  static bool isReadableDir(const std::string& path);
  static bool isReadableDir(const std::string& path,AptErr_t& rv);

  static bool isWriteable(const std::string& path);
  static bool isWriteable(const std::string& path,AptErr_t& rv);

  static bool isWriteableDir(const std::string& path);
  static bool isWriteableDir(const std::string& path,AptErr_t& rv);

  // is it?
  static bool isCalvinFile(const std::string& path);
  static bool isHdf5File(const std::string& path);
  static bool isBinaryFile(const std::string& path);

  /// @brief     This will make the dir if it isnt there.
  ///            If it is there, that is ok as long as it is writeable.
  ///            This will make a path like mkdirPath.
  /// @param     path      
  /// @param     abortOnErr
  /// @return    
  static AptErr_t ensureWriteableDirPath(const std::string& path,bool abortOnErr=true);


  /// @brief     The size of the file
  /// @param     path      The path to
  /// @param     abortOnErr If true, call Err::errAbort when a error occurs.
  /// @return    the size of the file or -1
  static int64_t fileSize(const std::string& path,bool abortOnErr=true);
  static int64_t fileSize(const std::string& path,AptErr_t& errnum);

  /// @brief     
  /// @param     path      
  /// @param     names     
  /// @param     abortOnErr
  /// @return    
  static AptErr_t listDir(const std::string& path,std::vector<std::string>& names,bool abortOnErr=true);

  /// @brief     Make one directory.
  /// @param     path      
  /// @param     abortOnErr
  /// @return    
  static AptErr_t mkdir(const std::string& path,bool abortOnErr=true);
  /// @brief     make all path of directories
  /// @param     path      
  /// @param     abortOnErr
  /// @return    
  static AptErr_t mkdirPath(const std::string& path,bool abortOnErr=true);
  
  /// An error if it doesnt exist
  static AptErr_t rm(const std::string& path,bool abortOnErr=true);
  /// An error if the rm fails.
  static AptErr_t rmIfExists(const std::string& path,bool abortOnErr=true);

  /// @brief     Remove a directory
  /// @param     path      
  /// @param     abortOnErr
  /// @return    
  static AptErr_t rmdir(const std::string& path,bool abortOnErr=true);
  /// @brief     
  /// @param     path      
  /// @param     abortOnErr
  /// @return    
  static AptErr_t rmdirPath(const std::string& path,bool abortOnErr=true);

  /// @brief     
  /// @param     path      
  /// @param     abortOnErr
  /// @return    
  static AptErr_t rm_rf(const std::string& path,bool abortOnErr=true);

  /// @brief     touchs a file (creates a zero length file if not exists.)
  /// @param     path      
  /// @param     abortOnErr
  /// @return    
  static AptErr_t touch(const std::string& path,bool abortOnErr=true);

  /// @brief     
  /// @param     path      
  /// @param     abortOnErr
  /// @return    
  static int64_t getFreeDiskSpace(const std::string& path,bool abortOnErr=true);

  /// @brief     Do these two paths refer to the same volume?
  /// @param     path1     
  /// @param     path2     
  /// @param     rv        
  /// @param     abortOnErr
  /// @return    True if they are on the same volume.
  static bool isSameVolume(const std::string& path1,
                           const std::string& path2,
                           AptErr_t& rv,
                           bool abortOnErr=true);

#ifdef _MSC_VER
  static std::string MSC_VER_GetLastErrorString( const std::string&  whence = "" );
#endif

  static int  aptOpen( const std::string & pathname, int flags = O_RDONLY);
  static int  aptOpen( const std::string & pathname, int flags, int mode );
  static void aptOpen( std::fstream & fs, const std::string & filename, std::ios_base::openmode iomode = std::ios_base::in | std::ios_base::out);
  static void aptOpen( std::ofstream & ofs, const std::string & filename, std::ios_base::openmode iomode = std::ios_base::out|std::ios_base::binary);
  static void aptOpen( std::ifstream & ifs, const std::string & filename, std::ios_base::openmode iomode = std::ios_base::in);


  /// Util carry overs. These need to be updated with abortOnError.
  /**
   * Search the affy library file path for the actual file name to open
   * @param fileName - the name of the file to find
   * @param searchPath - alternative search path
   */
  static std::string findLibFile(const std::string &fileName, const std::string &searchPath = "");
  //#endif

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

  static bool fileCopy(const std::string &in, const std::string &out, bool throwOnError = true);

  /**
   * Return true on success. False otherwise
   * @param in - file to copy
   * @param out - name of the new file
   */
  static bool fileRename(const std::string &in, const std::string &out, bool throwOnError = true);
  
};

#endif // _UTIL_FS_H_
