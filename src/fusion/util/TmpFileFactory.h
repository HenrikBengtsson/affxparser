////////////////////////////////////////////////////////////////
//
// Copyright (C) 2009 Affymetrix, Inc.
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

/// @file TmpFileFactory.h
/// @brief for usage examples, look at "util/test-tmpfilefactory.cpp"

#ifndef _TMPFILEFACTORY_H_
#define _TMPFILEFACTORY_H_
//
#include <cstring>
#include <string>
#include <vector>
//

/// @brief     Allocate temporary filenames and arrange to remove them when done.
class TmpFileFactory {
public:
  /// filenames we have allocated
  std::vector<std::string> m_tmpfilenames;
  /// directory names we have allocated
  std::vector<std::string> m_tmpdirnames;
  /// the directory in which the files will be allocated within.
  std::string m_tmpdir;
  /// the default prefix for the file and dir names.
  std::string m_prefix;
  /// the default suffix for the file names.
  std::string m_suffix;
  /// Controls the behavior to help debugging.
  int m_opt_debug;
  /// When set produces extra output for debugging.
  int m_opt_verbose;
  ///
  static int m_fileCount;
  int m_maxTrialCnt;

  /// @brief     The default constructor.
  TmpFileFactory();

  /// @brief     The destructor
  virtual ~TmpFileFactory();

  /// @brief     Remove all the files and directories we have generated.
  ///            Called from ~TmpFileFactory()
  void cleanUp();

  /// @brief     Return the next value of the file counter.
  /// @return    the next value.
  static int incFileCounter();

  /// @brief     Sets the directory in which the tmp files will be allocated.
  /// @param     dirname   
  void setTmpdir(const std::string& tmpdir);

  /// @brief     Generate and create a directory for tmp files.
  /// @return    The directory path
  std::string makeTmpDir();
  
  /// @brief     Set the default prefix for tmp names.
  /// @param     prefix    The prefix to use.
  void setPrefix(const std::string& prefix);
  /// @brief     Sets the default suffix (".tmp") for name from
  /// @param     suffix    
  void setSuffix(const std::string& suffix);

  /// @brief     Generates a string which is unique for this run of the program.
  /// @return    A 'unique' string.
  std::string genUniqueString();

  /// @brief     Generate a name which does not exist in the filesystem.
  ///            Does NOT remember to delete the filename later.
  /// @param     prefix    prefix for the unique string.
  /// @param     suffix    suffix for the unique string.
  /// @return    the unique pathname (dir+prefix+unique+suffix)
  std::string genFilename_basic(const std::string& prefix,const std::string& suffix);

  /// @brief     Generates a unique filename with user supplied prefix and suffix.
  ///            Remembers to delete it later.
  /// @param     prefix    user override to prefix
  /// @param     suffix    user override to suffix
  /// @return    a unique pathname
  std::string genFilename(const std::string& prefix,const std::string& suffix);

  /// @brief     Generates a unique filename with the default prefix and suffix.
  ///            Remembers to delete it later.
  /// @return    a unique file name.
  std::string genFilename();

  /// @brief     generate a vector of filenames in one call.
  /// @param     cnt       The number of filenames to generate.
  /// @return    the vec of filenames.
  std::vector<std::string> genFilenames(int cnt);

  /// @brief    Generates and creates a unique directory
  /// @return   the pathname to the directory.
  std::string genDirname();

  /// @brief     Remeber this filename to delete later.
  /// @param     filename   What to remove when "cleanUp()" is called.
  void rememberToRemove(const std::string& filename);

  /// @brief     Remeber this dirname to delete later.
  /// @param     filename   What to remove when "cleanUp()" is called.
  void rememberToRemoveDir(const std::string& dirname);

  /// @brief     Sets the debugging options
  ///            0=None
  ///            1=Dont remove the tmp files, leave them for debugging.
  /// @param     level     
  void setDebug(int level);
  /// @brief     Sets the verbose level
  ///            0=No output
  ///            1 or more = logs more actions.
  /// @param     level     
  void setVerbose(int level);

  /// @brief     Get The suggested tmp directory for this system.
  /// @return    The suggested path name.
  std::string getSystemTmpDir();

};

// These two functions provide access to a global TmpFileFactory
// Which can be used by everyone. Do remember to call
// GlobalTmpFileFactoryFree as you exit main
// to make sure it cleans up for you.

/// @brief     Returns a pointer to the global tmpfilefactory.
///            Allocates it if needed.
/// @return    The global tmpfilefactory
TmpFileFactory* GlobalTmpFileFactory();

/// @brief     Frees the global tmpfilefactory.
void GlobalTmpFileFactoryFree();

#endif /* _TMPFILEFACTORY_H_ */
