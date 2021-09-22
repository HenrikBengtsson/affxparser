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
// sdk/apt2/dao/FsPath.h ---
//
// $Id: FsPath.h,v 1.2 2009-11-04 20:37:32 harley Exp $
//

#ifndef _UTIL_FSPATH_H_
#define _UTIL_FSPATH_H_

// tell other files to use our methods for file handling
// this is just while we clean things up.
// #define APT_FORCE_USE_FSPATH 1

//
#include "portability/affy-base-types.h"
#include "util/AptErrno.h"

//
#include <map>
#include <string>
#include <vector>

/// @file   util/FsPath.h
/// @brief  A class for working with richer filepaths than just a string.


// We are going to use unix conventions in the internal format
// and translate them to the os requirements only as needed.
// so we define these here.
// They are not contitional on the platform. (unix & win32)
#define FSPATH_PATHSEP "/"
#define FSPATH_INTERNAL_PATHSEP ":"
#define FSPATH_EXTSEP "."

/**
 * @class FsPath
 * @brief  A class for working with richer filepaths than just a string.
 * APT would like to be able to name where to read and write data.
 * when there was just one dataset per file the filename was sufficient to
 * to do this.  The dataset name was simply mapped into the
 * file system. Now things are more complicated.  A file
 * can contain several datasets.
 *
 * Currently this is handled by also passing in more options
 * to name the group and table.  However this is done
 * different ways in APT.  The goal of FsPath is to be
 * able to name these datasets and provide a convenient set of
 * methods to work with the names.
 *
 * FsPath keeps its names in seperate pieces so the can be
 * worked on and recombined without parsing a string in the
 * code.  The directory name, file name, internal group name
 * and tablename (dataset) are all kept in the one object.
 * This FsPath can be passed to the File libraries who can
 * extract what they need.
 *
 * The format of the FsPath is:
 *     /os/path/name/filename.EXT:group/names/tablename
 *
 * (A) The path in the os. ("/os/path/name")
 * (B) The filename and extension. ("filename.EXT")
 * (C) The external/internal separator (":")
 * (D) The internal path. ("group/names")
 *     file5 could have several parts.
 *     calvin must have one and only one level of group names.
 *     TsvFile can be extended to use the filesystem to have groups.
 * (E) The tablename/dataset name ("tablename")
 *
 * With all this we could then write:
 *
 * FsPath path("path/to/file.EXT:/group/table")
 * DaoDataSet dataset;
 *
 * dataset->open(path);
 *
 * and the EXT would do the magic for Calvin, File5 or TsvFile.
 *
 * Thinking about this some more, it might be best to break out
 * the operating system calls into a seperate module which takes a string
 * or a FsPath object to do things with.
 *
 *  Note: according to APT-380, "Desktop" and other special windows dirs
 *  might be flagged as write-only, but actually be writeable.
 *  If this is really the case then "isWriteable" should be a two step process:
 *    if (s_writeable()) { return true; }
 *    if (is_windows()) { if (try_and_make_a_dir_works()) { return true; } }
 *    return false.
 *
 * Ideas:
 * * Add "void*" to the actual file being opened for reference.
 *   and marker for what type is being held.
 *
 *
 * To migrate current functionality:
 * - UncPath() here
 * - BaseName()
 * - FileRoot()
 * - Openers for raw streams.
 * - IsWritable() or equivalent
 */
class FsPath {
  public:

  /// Codes for the different formats APT might expect files to be in.
  enum FileFmt_t {
    FILEFMT_NONE = 0,
    FILEFMT_ANY,
    FILEFMT_AUTO,
    //
    FILEFMT_CALVIN,
    FILEFMT_FILE5,
    // we dont read straight text, but APT might know it is a text file
    FILEFMT_TEXT,
    FILEFMT_TSVFILE,
  };

  /// get a FsPath for a named location. (HOME, TMP, CWD)
  /// presently read from the process environment.
  static FsPath getPathFor(const std::string& loc);

  /// add a mapping from ".foo" to FILEFMT_FOO
  static void addExt2Fmt(const std::string& ext,FsPath::FileFmt_t fmt);
  /// convert the extention to the format based on the extension.
  static FileFmt_t ext2Fmt(const std::string& ext);

  /// new and empty path
  FsPath();
  /// a new copy of fspath
  FsPath(const FsPath& fspath);
  /// new fspath and call setPath.
  FsPath(const std::string& path);
  /// new fspath and the user tells us the three parts.
  FsPath(const std::string& dir,const std::string& name,const std::string& ext);

  /// clear and reset the defaults.
  void init();
  /// clear the data only.
  void clear();
  /// dump the FsPath to stdout.
  void dump();
  /// True if it does not have a value.
  bool empty();

  /// when set to false, dont throw an exception on an error.
  void setErrAbort(bool abortOnErr);
  /// the error value.
  AptErr_t getErrNum() const;
  /// clear the error; always returns APT_OK
  AptErr_t clearErr();
  /// sets the error value and throws if abortOnErr==true
  AptErr_t setErr(AptErr_t err_num,const std::string& err_msg);
  /// copies the error code from the current APT Fs error code
  AptErr_t setErrFromFs();

  /// parses the string into its parts.
  AptErr_t setPath(const std::string& pathname);
  /// copies the value
  AptErr_t setPath(const FsPath& fspath);
  /// sets the three components at once. (no parsing.)
  AptErr_t setPath(const std::string& dirname,const std::string& file_name,const std::string& file_ext);

  /// assignment.
  const FsPath& operator=(const FsPath& path) {
    copyFrom(path);
    return *this;
  }
  /// assignment again.
  void copyFrom(const FsPath& path);
  /// just copy the directory parts.
  void copyDirNameFrom(const FsPath& path);
  /// just copy the filename parts.
  void copyFileNameFrom(const FsPath& path);
  // @todo
  // void copyInternalNameFrom(const FsPath& path);

  /// do the names match?
  bool operator==(const FsPath& path) {
    return equals(path);
  }
  /// do the names match?
  bool equals(const FsPath& path);

  /// set the option of pathsep.
  // void setPathSep(const std::string& val) { m_opt_pathsep=val; };

  /// The value for windows. ("C:")
  void setDirDrive(const std::string& val);
  /// treat the entire path as a dir name.
  void setDirName(const std::string& val);
  // @todo
  // setDirName(const std::vector<std::string> dirnames);
  /// sets the file name (not the dirname or ext)
  void setFileName(const std::string& val)  { m_file_name=val; };
  /// sets the file and ext (not the dirname)
  void setFileNameExt(const std::string& file,const std::string& ext) {
    m_file_name=file;
    m_file_ext=ext;
  };
  /// sets the extension (".foo")
  void setFileExt(const std::string& val)   { m_file_ext=val; };
  /// sets the internal group name (like for file5)
  void setInternalGroupName(const std::string& val) { m_internal_group_name=val; };
  /// sets the internal table name (like for file5)
  void setInternalTableName(const std::string& val) { m_internal_table_name=val; };

  /// the orginal path given to setPath (For debugging)
  std::string getOriginalPath() const { return m_original_path; };
  /// the windows drive
  std::string getDirDrive()     const { return m_dir_drive; };
  /// the directory path (no slash)
  std::string getDirName() const;
  /// the directory path (with slash)
  std::string getDirNameSlash() const;
  /// the directory path as a list of components.
  std::vector<std::string> getDirNames() const;
  /// the filename (wo ext; "foo.bar" => "foo")
  std::string getFileName() const { return m_file_name; };
  /// the filename (wo ext; "foo.bar" => "foo")
  std::string getFileNameWoExt() const { return m_file_name; };
  /// just the extention ("foo.bar" => "bar")
  std::string getFileExt()      const { return m_file_ext; };
  /// the file and extension. (like unix basename)
  std::string getBaseName() const;
  /// the internal group name (like for file5)
  std::string getInternalGroupName()    const { return m_internal_group_name; };
  /// the internal table name (like for file5)
  std::string getInternalTableName()    const { return m_internal_table_name; };

  /// push a dirname on the end of the dir path.
  void pushDir(const std::string& dir);
  /// could we pop another name off the directory path?
  bool canPopDir() const;
  /// pops a dirname off the end of the path. ("foo/bar/baz" => "foo/bar" returns "baz")
  std::string popDir();

  /// map the extension to a filefmt.
  FsPath::FileFmt_t fmtFromExt() const;
  FsPath::FileFmt_t getFileFmt() const;
  void setFileFmt(FsPath::FileFmt_t fmt) { m_filefmt=fmt; };

  /// a printable string which could be passed to our "setPath()"
  /// but it cant be opened in the filesystem.
  /// it has the internal group and table names.
  std::string asString() const;

  /// format the path for unix. (Which is our internal format.)
  std::string asUnixPath() const;
  /// format the path and return a tmp "const char*".
  const char* asUnixPathCstr() const;

  /// what should pass to the OS to open with. (a no-op on unix)
  std::string asUncPath() const;
  /// selects asWindowsPath or asUnixPath.
  const char* osUncPathCstr() const;

  /// is this an absolute path? "/foo/..."?
  bool isAbsolute() const;
  /// Does this name a directory? (no file name or ext)
  bool isDirName() const;
  /// Does this name a file? (file name or ext is set.)
  bool isFileName() const;

  /// does this exist in the filesystem? (file or dir)
  bool exists() const;
  bool dirExists() const;
  bool fileExists() const;

  /// is this a directory we can write to?
  /// note that this is special on windows as we actually test it.
  bool isWriteableDir() const;
  bool isWriteableDir(AptErr_t& rv) const;

  /// Could we read from this path? (file or dir)
  bool isReadable() const;
  /// Could we write to this path? (file or dir)
  bool isWriteable() const;

  /// the size of the file in bytes.
  int64_t fileSize();

  /// true if is a file and is in HDF5 format.
  /// bool osIsHdf5() const;

  /// tries to make one directory (just the last part of dirpath)
  AptErr_t mkdir(bool errIfExists=true);
  /// tries to make the entire path. (like mkdir -p)
  AptErr_t mkdirPath(bool errIfExists=true);
  ///

  /// Makes 
  AptErr_t ensureWriteableDirPath();

  /// remove the directory. (dir must be empty.)
  AptErr_t rmdir(bool errIfNotExists=true);
  /// remove the path of dirs (all dirs must be empty.)
  AptErr_t rmdirPath(bool errIfNotExists=true);

  /// returns a list of names in the dir.
  AptErr_t listDir(std::vector<std::string>& names);

  /// removes a file.
  AptErr_t rm(bool errIfNotExists=false);
  /// changes the mode (dir or file)
  AptErr_t chmod(int mode,bool errIfNotExists=true);
  /// creates the file if it does not exist.
  AptErr_t touch();

  /// rename the file to the new name
  // AptErr_t renameTo(const std::string& to_name);
  // AptErr_t renameTo(const FsPath& to_path);

  /// copy the file to the path
  // AptErr_t copyTo(const std::string& to_name);
  // AptErr_t copyTo(const FsPath& to_path);

  /// the free space measured in bytes.
  int64_t getFreeDiskSpace() const;

  /// are the paths on the same volume?
  bool isSameVolume(const std::string& dirname);
  /// are the paths on the same volume?
  bool isSameVolume(const FsPath& fspath);
  /// are the paths on the same volume?
  bool isSameVolume(const FsPath& fspath,AptErr_t& rv);

  //////////

private:
  /// true=throw and error when m_err_num != APT_OK
  bool m_err_doAbort;
  /// the last error.
  AptErr_t m_err_num;
  /// the last error message
  std::string m_err_msg;

  /// The path as it was given to us.
  /// only used for debugging.
  std::string m_original_path;
  /// the windows drive  "C:"
  std::string m_dir_drive;
  /// the directory path "/var/tmp"
  std::string m_dir_name;
  /// the filename part of the name: "myfile"
  std::string m_file_name;
  /// the extension part of the name: ".cpp", ".h"
  std::string m_file_ext;
  /// tmp string used for buffer the return of a "const char*"
  mutable std::string m_tmp_cstr;
  /// the group name inside the file.
  std::string m_internal_group_name;
  /// the tablename/dataset inside the file.
  std::string m_internal_table_name;

  //
  FsPath::FileFmt_t m_filefmt;

  // how we map the extensions to FileFmts.
  static std::map<std::string,FsPath::FileFmt_t> m_ext2fmt_map;
  // for the inital setup.
  static int m_ext2fmt_map_isloaded;
};

#endif // _UTIL_FSPATH_H_
