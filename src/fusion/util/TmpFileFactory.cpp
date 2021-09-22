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

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

//
#include "util/TmpFileFactory.h"
//
#include "portability/affy-system-api.h"
#include "util/Fs.h"
#include "util/Util.h"
//
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

//////////

// The global count of items allocated by this program.
// We increment this counter for each file and dir.
int TmpFileFactory::m_fileCount=0;

int TmpFileFactory::incFileCounter() {
  m_fileCount++;
  return m_fileCount;
}

//////////

/// Declared static to scope it to this file.
static TmpFileFactory* global_tmpfilefactory;

TmpFileFactory* GlobalTmpFileFactory() {
  if (global_tmpfilefactory==NULL) {
    global_tmpfilefactory=new TmpFileFactory();
  }
  if (global_tmpfilefactory==NULL) {
    Err::errAbort("GlobalTmpFileFactory: Unable to allocate.");
  }
  return global_tmpfilefactory;
}

void GlobalTmpFileFactoryFree() {
  if (global_tmpfilefactory!=NULL) {
    global_tmpfilefactory->cleanUp();
    delete global_tmpfilefactory;
  }
}

//////////

TmpFileFactory::TmpFileFactory()
{
  m_maxTrialCnt=10;
  //
  m_prefix="tmpfile-";
  m_suffix=".tmp";
  m_tmpdir=".";
  //
  m_opt_debug=0;
  m_opt_verbose=0;

  // look at the env var to see if debugging should be turned on.
#ifdef unix
  char* val=getenv("APT_TMPFILE_DEBUG");
  if (val!=NULL) {
    std::string val_string=val;
    if (val_string=="1") {
      m_opt_debug=1;
    }
  }
#endif

};

TmpFileFactory::~TmpFileFactory()
{
  // get rid of the tmp files when we die
  cleanUp();
};

void
TmpFileFactory::setDebug(int level)
{
  m_opt_debug=level;
}
void
TmpFileFactory::setVerbose(int level)
{
  m_opt_verbose=level;
}

void
TmpFileFactory::setTmpdir(const std::string& tmpdir)
{
  m_tmpdir=tmpdir;
}

void TmpFileFactory::setPrefix(const std::string& prefix)
{
  m_prefix=prefix;
}
void TmpFileFactory::setSuffix(const std::string& suffix)
{
  m_suffix=suffix;
}

void TmpFileFactory::rememberToRemove(const std::string& filename)
{
  m_tmpfilenames.push_back(filename);
}

void TmpFileFactory::rememberToRemoveDir(const std::string& dirname)
{
  m_tmpdirnames.push_back(dirname);
}

//////////

void
TmpFileFactory::cleanUp()
{
  std::string path;

  for (size_t i=0;i<m_tmpfilenames.size();i++) {
    path=m_tmpfilenames[i];
    if (m_opt_verbose>0) {
      /// @todo: replace with "Log"
      std::cout << "TmpFileFactory::cleanUp(): rm '"+path+"'"<<std::endl;
    }
    if (m_opt_debug<1) {
      Fs::rm(path);
    }
  }

  // remove the dirs in reverse order as the early dirs
  // might contain the later (higher indexed) dirs.
  for (int i=(int)m_tmpdirnames.size()-1;0<=i;i--) {
    path=m_tmpdirnames[i];
    if (m_opt_verbose>0) {
      /// @todo: replace with "Log"
      std::cout << "TmpFileFactory::cleanUp(): rmdir '"+path+"'"<<std::endl;
    } 
    if (m_opt_debug<1) {
      Fs::rmdir(path);
    }
  }
}

//////////

std::string
TmpFileFactory::genUniqueString()
{
  char buf[50];
  
  // the combination of pid and file counter should make it unique.
  // add some "random" digits to help make it so.
  snprintf(buf,sizeof(buf),"%d-%06d-%06d%06d",
           getpid(),
           incFileCounter(),
           (rand()%1000000),(rand()%1000000));

  return std::string(buf);
}

std::string
TmpFileFactory::genFilename_basic(const std::string& prefix,const std::string& suffix)
{
  int trialcnt=0;
  std::string tmpfilename;

  while (1) {
    tmpfilename=Fs::join(m_tmpdir,prefix+genUniqueString()+suffix);
        // Does this file exist?
    if (Fs::fileExists(tmpfilename)==false) {
      if (m_opt_verbose>=1) {
        std::cout<< "TmpFileFactory::genFilename()=='"+tmpfilename+"'\n";
      }
      return tmpfilename;
    }
    if (trialcnt++>m_maxTrialCnt) {
      Err::errAbort("unable to allocate a tmpfile! Last filename tried: '"+tmpfilename+"'");
    }
  }
}

std::string
TmpFileFactory::genFilename(const std::string& prefix,const std::string& suffix)
{
  std::string filename=genFilename_basic(prefix, suffix);
  // we will want to remove it later.
  rememberToRemove(filename);
  return filename;
}

std::string
TmpFileFactory::genFilename()
{
  // use the default prefix and suffix
  return genFilename(m_prefix,m_suffix);
}

std::vector<std::string>
TmpFileFactory::genFilenames(int cnt)
{
  std::vector<std::string> vec;
  for (int i=0;i<cnt;i++) {
    vec.push_back(genFilename());
  }
  return vec;
}

std::string
TmpFileFactory::genDirname()
{
  // no suffix
  std::string dirname=genFilename_basic(m_prefix,"");
  //
  if ( !Fs::dirExists(dirname) ) {
    Fs::mkdirPath(dirname);
  }
  //
  if (Fs::isWriteableDir(dirname)!=APT_OK) {
    Err::errAbort("TmpFileFactory::genDirname(): cant write to '"+dirname+"'");
  }
  rememberToRemoveDir(dirname);

  return dirname;
}

std::string
TmpFileFactory::getSystemTmpDir()
{
#ifdef WIN32
  // what is the tmp file dir for windows?
  return ".";
#else
  return "/var/tmp";
#endif
}

