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
// affy/sdk/util/FsPath.cpp ---
//

//
#include "util/FsPath.h"
//
#include "util/AptErrno.h"
#include "util/Convert.h"
#include "util/Err.h"
#include "util/Fs.h"
#include "util/Util.h"
//
#include <algorithm>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>

// OS stuff for memInfo, getAvailableDiskSpace
#ifdef __APPLE__
#include <mach/mach.h>
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


//////////

int FsPath::m_ext2fmt_map_isloaded=0;
std::map<std::string,FsPath::FileFmt_t> FsPath::m_ext2fmt_map;

//////////

struct FsPath_Ext2fmt_t {
  const char* m_ext;
  FsPath::FileFmt_t m_fmt;
};

/// @todo add more file extensions here.
FsPath_Ext2fmt_t initial_ext2fmt[]=
  {
    {"tsv",   FsPath::FILEFMT_TSVFILE },
    {"txt",   FsPath::FILEFMT_TSVFILE },
    {"bb",    FsPath::FILEFMT_TSVFILE },
    //
    {"a5",    FsPath::FILEFMT_FILE5 },
    {"hdf5",  FsPath::FILEFMT_FILE5 },
    {"ref",   FsPath::FILEFMT_FILE5 },
    //
    {"chp",   FsPath::FILEFMT_CALVIN },
    {"cychp", FsPath::FILEFMT_CALVIN },
    // end marker.
    {NULL,FsPath::FILEFMT_NONE}
  };

void FsPath::addExt2Fmt(const std::string& ext,FsPath::FileFmt_t fmt)
{
  m_ext2fmt_map[ext]=fmt;
}

/// @todo handle case (e.g. 'A5' && 'a5' are equivalent
FsPath::FileFmt_t FsPath::ext2Fmt(const std::string& ext)
{
  if (m_ext2fmt_map_isloaded==0) {
    FsPath_Ext2fmt_t* p=initial_ext2fmt;
    while (p->m_ext!=NULL) {
      addExt2Fmt(p->m_ext,p->m_fmt);
      p++;
    }
    m_ext2fmt_map_isloaded=1;
  }

  std::map<std::string,FsPath::FileFmt_t>::iterator i;
  i=m_ext2fmt_map.find(ext);
  if (i==m_ext2fmt_map.end()) {
    return FsPath::FILEFMT_NONE;
  }
  return i->second;
}

//////////

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif

FsPath FsPath::getPathFor(const std::string& loc)
{
  FsPath the_path;

  // @todo put windows code here.
  char* val=getenv(loc.c_str());
  if (val==NULL) {
    return the_path;
  }
  // these locations are always dirs.
  if ((loc=="HOME") ||
      (loc=="PWD") ||
      (loc=="TMP")) {
    the_path.setDirName(val);
  }

  else {
    the_path.setPath(val);
  }
  return the_path;
}

//////////

FsPath::FsPath() {
  init();
}

FsPath::FsPath(const FsPath& fspath) {
  copyFrom(fspath);
}

FsPath::FsPath(const std::string& path) {
  init();
  setPath(path);
}

FsPath::FsPath(const std::string& dir,const std::string& name,const std::string& ext) {
  init();
  m_dir_name=Fs::addTrailingSlash(dir);
  m_file_name=name;
  m_file_ext=ext;
}

// In "harley-speak" init is a bigger "clear()"
void FsPath::init() {
  //
  m_err_doAbort=true;
  //
  clear();
}

//
void FsPath::clear() {
  m_original_path="";
  //
  m_dir_drive="";
  m_dir_name="";
  m_file_name="";
  m_file_ext="";
  m_filefmt=FsPath::FILEFMT_AUTO;
  //
  m_internal_group_name="";
  m_internal_table_name="";
}


#define FSPATH_DUMP_D(_x) { printf("   %-30s = '%d'\n",#_x,_x); }
#define FSPATH_DUMP_S(_x) { printf("   %-30s = '%s'\n",#_x,_x.c_str()); }

void FsPath::dump() {
  printf("== FsPath(%p):\n",this);
  FSPATH_DUMP_S(m_original_path);
  FSPATH_DUMP_S(m_dir_name);
  FSPATH_DUMP_S(m_file_name);
  FSPATH_DUMP_S(m_file_ext);
  FSPATH_DUMP_D(m_filefmt);
  FSPATH_DUMP_S(m_internal_group_name);
  FSPATH_DUMP_S(m_internal_table_name);
  std::string str;
  str=asUnixPath();
  printf("   As asUnixPath(): '%s'\n",str.c_str());
  str=asString();
  printf("   As asString(): '%s'\n",str.c_str());
}

bool FsPath::empty() {
  if ((m_dir_name=="") &&
      (m_file_name=="") &&
      (m_file_ext=="") &&
      (m_internal_group_name=="") &&
      (m_internal_table_name=="")) {
    return true;
  }
  return false;
}

void FsPath::copyFrom(const FsPath& path) {
  // dont bother to do anything when self assigning.
  if (this==&path) {
    return;
  }
  // first opts...
//  m_opt_pathsep=path.m_opt_pathsep;
//  m_opt_internalsep=path.m_opt_internalsep;
//  m_opt_extsep=path.m_opt_extsep;
  // ... then values
  // m_original_path=path.m_original_path;
  m_dir_drive=path.m_dir_drive;
  m_dir_name=path.m_dir_name;
  m_file_name=path.m_file_name;
  m_file_ext=path.m_file_ext;
  m_filefmt=path.m_filefmt;
  m_internal_group_name=path.m_internal_group_name;
  m_internal_table_name=path.m_internal_table_name;
}

void FsPath::copyDirNameFrom(const FsPath& path) {
  m_dir_name=path.m_dir_name;
}

void FsPath::copyFileNameFrom(const FsPath& path) {
  m_file_name=path.m_file_name;
  m_file_ext=path.m_file_ext;
  m_filefmt=path.m_filefmt;
}

#define FSPATH_NEQ(_slot) { if (_slot!=path._slot) { return false; } }
bool FsPath::equals(const FsPath& path) {
  // FSPATH_NEQ(m_original_path) => for debugging - we dont care about its value.
  FSPATH_NEQ(m_dir_name);
  FSPATH_NEQ(m_file_name);
  FSPATH_NEQ(m_file_ext);
  FSPATH_NEQ(m_internal_group_name);
  FSPATH_NEQ(m_internal_table_name);
  //m_filefmt => derived dont test.
  //
  return true;
}

void FsPath::setDirDrive(const std::string& val) {
  // dont run unixify string; This should just be a letter and colon: "C:"
  m_dir_drive=val;
};

void FsPath::setDirName(const std::string& val) {
  std::string tmp=val;

  // convert it to our internal (unix) format.
  Fs::unixifyPathInPlace(tmp);

  // split off the drive
  if (tmp[1]==':') {
    m_dir_drive=tmp.substr(0,2);
    tmp.erase(0,2);
  }

  // assign with an optinal trailing slash
  m_dir_name=Fs::addTrailingSlash(tmp);
}

//////////

void FsPath::setErrAbort(bool abortOnErr) {
  m_err_doAbort=abortOnErr;
}

AptErr_t FsPath::getErrNum() const {
  return m_err_num;
}

AptErr_t FsPath::setErr(AptErr_t err_num,const std::string& err_msg)
{
  m_err_num=err_num;
  m_err_msg=err_msg;

  if (m_err_num!=APT_OK) {
    // for debugging
    //printf("FsPath::setErr: err=%d msg='%s'\n",m_err_num,m_err_msg.c_str());
    //
    if (m_err_doAbort==true) {
      Err::errAbort(err_msg);
    }
  }
  return m_err_num;
}

AptErr_t FsPath::setErrFromFs() {
  return setErr(Fs::getErrNum(),Fs::getErrMsg());
}

AptErr_t FsPath::clearErr() {
  m_err_num=APT_OK;
  m_err_msg="";
  return m_err_num;
}

//////////

AptErr_t FsPath::setPath(const FsPath& fspath) {
    this->copyFrom(fspath);
    return APT_OK;
}

AptErr_t FsPath::setPath(const std::string& dirname,
                         const std::string& file_name,
                         const std::string& file_ext)
{
  m_dir_name=Fs::addTrailingSlash(dirname);
  m_file_name=file_name;
  m_file_ext=file_ext;
  return APT_OK;
}

// See class docs for examples of valid paths.
// No escaped backslashed characters allowed
AptErr_t FsPath::setPath(const std::string& path) {
  std::string tmp_os_path;
  std::string tmp_internal_path;
  std::string::size_type pos;

  clear();

  // save it off the original path
  m_original_path=path;

  if (path=="") {
    return APT_OK;
  }

  // tmp gets chopped up as we work and path is const, so we need a copy.
  tmp_os_path=path;

  // convert any MS-like paths to normal paths.
  Fs::unixifyPathInPlace(tmp_os_path);

  // if there is a drive letter, pull it off first
  // note this is ambiguous with a single letter file name with an internal 
  // group/table
  // "c:foo"
  if (tmp_os_path[1]==':') {
    m_dir_drive=tmp_os_path.substr(0,2);
    tmp_os_path.erase(0,2);
  }
  
  // The last ":" of the string seperates the internal name
  // ...../FILE.EXT:INTERNAL_GROUP/INTERNAL_TABLE.
  pos=tmp_os_path.rfind(FSPATH_INTERNAL_PATHSEP);
  // order is important, pull out the internal ":group/table" first.
  if (pos!=std::string::npos) {
    tmp_internal_path=tmp_os_path.substr(pos+1);
    tmp_os_path.erase(pos,tmp_os_path.size());
  }

  // now we have two parts...
  // ...do the os part first...
  pos=tmp_os_path.rfind(FSPATH_PATHSEP);
  if (pos==std::string::npos) {
    // no dir parts.
    m_dir_name="";
    m_file_name=tmp_os_path;
  }
  else {
    m_dir_name=Fs::addTrailingSlash(tmp_os_path.substr(0,pos+1));
    if (pos<tmp_os_path.size()) {
      m_file_name=tmp_os_path.substr(pos+1);
    }
  }

  //dump();

  // try at ".EXT"
  pos=m_file_name.rfind(FSPATH_EXTSEP);
  if (pos!=std::string::npos) {
    m_file_ext=m_file_name.substr(pos+1);
    m_file_name.erase(pos);
  }

  // ...and the internal name second.
  pos=tmp_internal_path.rfind(FSPATH_PATHSEP);
  if (pos==std::string::npos) {
    m_internal_table_name=tmp_internal_path;
  }
  else {
    m_internal_group_name=tmp_internal_path.substr(0,pos);
    m_internal_table_name=tmp_internal_path.substr(pos+1);
  }
  //
  //dump();
  //
  return APT_OK;
}

//////////

std::string FsPath::getDirName() const {
  return m_dir_drive+Fs::trimTrailingSlash(m_dir_name); 
};

std::string FsPath::getDirNameSlash() const {
  return m_dir_drive+Fs::addTrailingSlash(m_dir_name); 
};

std::vector<std::string> FsPath::getDirNames() const {
  std::string drive;
  std::vector<std::string> parts;
  
  Fs::splitPath(m_dir_name,drive,parts);
  return parts;
}

//////////

FsPath::FileFmt_t FsPath::fmtFromExt() const {
  return ext2Fmt(m_file_ext);
}

FsPath::FileFmt_t FsPath::getFileFmt() const {
  if (m_filefmt==FsPath::FILEFMT_AUTO) {
    return fmtFromExt();
  }
  return m_filefmt;
}

std::string FsPath::getBaseName() const {
  std::string out;
  out=m_file_name;

  if (m_file_ext!="") {
    out+=FSPATH_EXTSEP+m_file_ext;
  }
  return out;
}

std::string FsPath::asUnixPath() const {
  std::string out;

  out=m_dir_name;

  // add a sep if needed.
  if ((out.size()>0)&&(out.compare(out.size()-1,1,FSPATH_PATHSEP)!=0)) {
    out+=FSPATH_PATHSEP;
  }

  // now tack on the name if there.
  out+=m_file_name;
  if (m_file_ext!="") {
    out+=FSPATH_EXTSEP+m_file_ext;
  }

  // add the "C:" if there
  if (m_dir_drive!="") {
    out=m_dir_drive+out;
  }

  //
  return out;
}

const char* FsPath::asUnixPathCstr() const {
  // put the string an a buffer.
  m_tmp_cstr=asUnixPath();
  // return the "char*" for the user.
  return m_tmp_cstr.c_str();
}

// read the following for why this is required:
//   http://msdn.microsoft.com/en-us/library/aa365247(VS.85).aspx
//   "Naming Files, Paths, and Namespaces"
// In short: paths longer that 260 chars cant be processed, unless
// (a) they are absolute
// (b) they start with "\\?\".
//

std::string FsPath::asUncPath() const {
  std::string tmp=asUnixPath();
#ifdef _MSC_VER
  Fs::convertToUncPathInPlace(tmp);
#endif
  return tmp;
}

const char* FsPath::osUncPathCstr() const {
  // put the string an a buffer.
  m_tmp_cstr=asUncPath();
  // return the "char*" for the user.
  return m_tmp_cstr.c_str();
}

std::string FsPath::asString() const {
  std::string out;

  out=m_dir_drive+m_dir_name+m_file_name;

  //
  if (m_file_ext!="") {
    out+=FSPATH_EXTSEP+m_file_ext;
  }
  //
  if ((m_internal_group_name!="")||(m_internal_table_name!="")) {
    out+=FSPATH_INTERNAL_PATHSEP;
    if (m_internal_group_name!="") {
      out+=m_internal_group_name+FSPATH_PATHSEP;
    }
    if (m_internal_table_name!="") {
      out+=m_internal_table_name;
    }
  }

  //
  return out;
}


//////////

bool FsPath::isAbsolute() const {
  if (m_dir_name.find(FSPATH_PATHSEP)==0) {
    return true;
  }
  return false;
}

bool FsPath::isFileName() const {
  if ((m_file_name!="") || (m_file_ext!="")) {
    return true;
  }
  return false;
}

bool FsPath::isDirName() const {
  if ((m_dir_name!="") && (m_file_name=="") && (m_file_ext=="")) {
    return true;
  }
  return false;
}

//////////

bool FsPath::exists() const {
  return Fs::exists(asUnixPath());
}

bool FsPath::dirExists() const {
  return Fs::dirExists(asUnixPath());
}

bool FsPath::fileExists() const {
  return Fs::fileExists(asUnixPath());
}

//////////

void FsPath::pushDir(const std::string& dir) {
  m_dir_name=Fs::addTrailingSlash(Fs::addTrailingSlash(m_dir_name)+dir);
}

bool FsPath::canPopDir() const {
  if ((m_dir_name==FSPATH_PATHSEP) || (m_dir_name==".") || (m_dir_name=="")) {
    return false;
  }
  return true;
}

std::string FsPath::popDir() {
  std::string popped;
  std::string::size_type pos;

  //dump();
  //printf("popDir('%s')==",osPathCstr());

  if (m_dir_name=="") {
    return "";
  }
  // cant pop an abs path.
  if (m_dir_name==FSPATH_PATHSEP) {
    return FSPATH_PATHSEP;
  }

  //
  m_dir_name=Fs::trimTrailingSlash(m_dir_name);
  pos=m_dir_name.rfind(FSPATH_PATHSEP);

  // no "/" found, the entire thing.
  if (pos==std::string::npos) {
    popped=m_dir_name;
    m_dir_name="";
  }
  else if (pos==0) {
    popped=m_dir_name.substr(pos+1);
    m_dir_name=FSPATH_PATHSEP;
  }
  else {
    popped=m_dir_name.substr(pos+1);
    m_dir_name.erase(pos,m_dir_name.size()-pos);
  }

  //printf("'%s'\n",popped.c_str());
  return popped;
}

//////////

bool FsPath::isReadable() const
{
  return Fs::isReadable(asUnixPath());
}

bool FsPath::isWriteableDir() const
{
  return Fs::isWriteableDir(asUnixPath());
}
bool FsPath::isWriteableDir(AptErr_t& rv) const
{
  return Fs::isWriteableDir(asUnixPath());
}

AptErr_t FsPath::ensureWriteableDirPath()
{
  // already writeable?
  if (isWriteableDir()) {
    return APT_OK;
  }
  // create the path.
  mkdirPath(false);
  // check that we can write to the last dir.
  if (isWriteableDir()==true) {
    return APT_OK;
  }
  return setErr(APT_ERR,"FsPath::ensureWriteableDir() failed.");
}

//////////


//////////

AptErr_t FsPath::chmod(int mode,bool errIfNotExists)
{
  Fs::chmodBasic(asUnixPath(),mode);
  return setErrFromFs();
}

AptErr_t FsPath::mkdir(bool errIfExists)
{
  Fs::mkdir(asUnixPath());
  return setErrFromFs();
}

AptErr_t FsPath::mkdirPath(bool errIfExists)
{
  std::vector<std::string> dir_names=getDirNames();
  FsPath tmp_path(*this);
  tmp_path.clear();
  tmp_path.setErrAbort(false);

  clearErr();

  for (unsigned int i=0;i<dir_names.size();i++) {
    tmp_path.pushDir(dir_names[i]);
    tmp_path.mkdir(errIfExists);
    if (tmp_path.m_err_num!=APT_OK) {
      setErr(tmp_path.m_err_num,tmp_path.m_err_msg);
      break;
    }
  }

  return getErrNum();
}

// There are two kinds of error here.
// * If the file does not exist
// * if the file exists after we attempt to remove it.
// how should these to errors be handled/reported?

AptErr_t FsPath::rm(bool errIfNotExists)
{
  if ((errIfNotExists==true)&&(Fs::fileExists(asUnixPath())==false)) {
    return setErr(APT_ERR_NOTEXISTS,asUnixPath());
  }
  Fs::rm(asUnixPath());
  //return setErrFromFs();
  return APT_OK;
}

AptErr_t FsPath::rmdir(bool errIfNotExists)
{
  std::string tmp_path=asUnixPath();

  if (Fs::dirExists(tmp_path)==false) {
    if (errIfNotExists==true) {
      return setErr(APT_ERR_NOTEXISTS,tmp_path);
    }
    return clearErr();
  }
  //
  Fs::rmdir(tmp_path);
  //
  if (Fs::dirExists(tmp_path)) {
    return setErr(APT_ERR_NOTREMOVED,"didnt remove dir.");
  }
  return clearErr();
}

AptErr_t FsPath::rmdirPath(bool errIfNotExists)
{
  FsPath tmp_path(*this);
  std::string dir_name;

  while (true) {
    if (!tmp_path.canPopDir()) {
      break;
    }
    AptErr_t rv=tmp_path.rmdir(errIfNotExists);
    if (rv!=APT_OK) {
      return rv;
    }
    tmp_path.popDir();
  }
  return clearErr();
}

//////////

/// returns a list of names in the dir.                                                                                                       
AptErr_t FsPath::listDir(std::vector<std::string>& names) {
  return Fs::listDir(asUnixPath(),names);
}

//////////

AptErr_t FsPath::touch()
{
  Fs::touch(asUnixPath());
  return setErrFromFs();
}

//////////

int64_t FsPath::getFreeDiskSpace() const {
  return Fs::getFreeDiskSpace(asUnixPath());
}

//////////

bool FsPath::isSameVolume(const std::string& path2)
{
  AptErr_t rv;
  return Fs::isSameVolume(asUnixPath(),path2,rv,m_err_doAbort);
}

bool FsPath::isSameVolume(const FsPath& path2)
{
  AptErr_t rv;
  return Fs::isSameVolume(asUnixPath(),path2.asUnixPath(),rv,m_err_doAbort);
}
