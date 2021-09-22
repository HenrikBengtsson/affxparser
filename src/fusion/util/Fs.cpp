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
// affy/sdk/util/Fs.cpp ---
//

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

//
#include "util/Fs.h"
//
#include "util/Convert.h"
#include "util/Err.h"
#include "util/Util.h"
//
#include <algorithm>
#include <errno.h>
#include <fstream>
#include <sstream>

#ifdef _WIN32
// Windows doesn't seem to have these defined, so we'll define them
// here for utility. Note that with the windows _stat() function all
// users have id 0 and group 0.
#define S_IRUSR 00400 //owner has read permission
#define S_IWUSR 00200 //owner has write permission
#define S_IXUSR 00100 ///owner has execute permission
#define S_IRWXG 00070 //mask for group permissions
#define S_IRGRP 00040 //group has read permission
#define S_IWGRP 00020 //group has write permission
#define S_IXGRP 00010 //group has execute permission
#define S_IRWXO 00007 //mask for permissions for others (not in group)
#define S_IROTH 00004 //others have read permission
#define S_IWOTH 00002 //others have write permisson
#define S_IXOTH 00001 // others have execute permission
#endif

// stuff for getAvailableDiskSpace
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

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <direct.h>
#else
#include <dirent.h>
#include <unistd.h>
#endif

// @todo all this should go into portablity/apt-posix.h
#ifdef _WIN32
// Microsoft limit is 32767 plus slush 100
// Return and input buffer.   
#define UNC_MAX_PATH_BUFSIZ (32767 + 100)
#define POSIX_CHMOD  _wchmod
#define POSIX_MKDIR  _wmkdir
#define POSIX_UNLINK _wunlink
#ifndef S_ISDIR
#define S_ISDIR(m)   (((m) & S_IFMT) == S_IFDIR)
#endif
#ifndef S_ISREG
#define S_ISREG(m)   (((m) & S_IFMT) == S_IFREG)
#endif
#else
#define POSIX_CHMOD  ::chmod
#define POSIX_MKDIR  ::mkdir
#define POSIX_UNLINK ::unlink
#endif

// cast this to a string.
#define FS_ERRNO_STR() std::string("(errno="+ToStr(errno)+")")

//////////

/// utility function to fill in a filesystem stat struct.
AptErr_t Fs__stat(const std::string& path,
                  struct stat& stat_buf,
                  int& rv,
                  bool abortOnErr) {

  std::string errorMsg("Fs__stat");
  
#ifndef _WIN32  
  rv=stat(path.c_str(),&stat_buf);
#else  
  WIN32_FILE_ATTRIBUTE_DATA win_attr_data;
  rv = 0;
  std::wstring unc_wpath = Fs::UncW( path );
  if ( GetFileAttributesExW(unc_wpath.c_str(), GetFileExInfoStandard, &win_attr_data) ) {
    memset( &stat_buf, 0 , sizeof(struct stat));
    if ( win_attr_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
      stat_buf.st_mode |= S_IFDIR | S_IRUSR;
      if ( !(win_attr_data.dwFileAttributes & FILE_ATTRIBUTE_READONLY) ) {
        stat_buf.st_mode |= S_IWUSR;
      }
    }
    else {
      stat_buf.st_mode |= S_IFREG;
      stat_buf.st_size = (win_attr_data.nFileSizeHigh * (((int)MAXDWORD)+1)) + win_attr_data.nFileSizeLow;
      std::fstream fs;
      Fs::aptOpen(fs, path,std::ios::in|std::ios::binary);
      if (fs.is_open()) {
        fs.close();
        stat_buf.st_mode |= S_IRUSR;
        if ( !(win_attr_data.dwFileAttributes & FILE_ATTRIBUTE_READONLY) ) {
          stat_buf.st_mode |= S_IWUSR;
        }
      }
      else {
        //std::cerr <<  Fs::MSC_VER_GetLastErrorString("fs.open") << std::endl;
      }
    }
  }
  else {
    //std::cerr << Fs::MSC_VER_GetLastErrorString("Fs__stat") << std::endl;
    rv = GetLastError();
    errorMsg = Fs::MSC_VER_GetLastErrorString("Fs__stat");
  }
  
#endif  
  
  // @todo decode rv to errnum
  if (rv==0) {
    return Fs::clearErr();
  }
  return Fs::setErr(APT_ERR,errorMsg,abortOnErr);
}

bool Fs__isreadable(struct stat& stat_buf) {
  int uid;
  int gid;

#ifdef _WIN32
  uid=0;
  gid=0;
#else
  uid=getuid();
  gid=getgid();
#endif

  if ( ((uid==stat_buf.st_uid) && (S_IRUSR&stat_buf.st_mode)) ||
       ((gid==stat_buf.st_gid) && (S_IRGRP&stat_buf.st_mode)) ||
       (S_IROTH&stat_buf.st_mode) ) {
    return true;
  }
  return false;
}

bool Fs__iswriteable(struct stat& stat_buf) {
  int uid;
  int gid;

#ifdef _WIN32
  uid=0;
  gid=0;
#else
  uid=getuid();
  gid=getgid();
#endif

  if ( ((uid==stat_buf.st_uid) && (S_IWUSR&stat_buf.st_mode)) ||
       ((gid==stat_buf.st_gid) && (S_IWGRP&stat_buf.st_mode)) ||
       (S_IWOTH&stat_buf.st_mode) ) {
    return true;
  }
  return false;
}

//////////

AptErr_t Fs::m_err_num;
std::string Fs::m_err_msg;

AptErr_t Fs::setErr(AptErr_t err_num,const std::string& err_msg,bool abortOnErr) {
  m_err_num=err_num;
  m_err_msg=err_msg;
  return m_err_num;
}

AptErr_t Fs::getErrNum() {
  return m_err_num;
}

std::string Fs::getErrMsg() {
  return m_err_msg;
}

AptErr_t Fs::clearErr() {
  m_err_num=APT_OK;
  m_err_msg="";
  return m_err_num;
}

/// translate
AptErr_t Fs::setErrFromErrno(const std::string& path,bool abortOnErr) {
  return setErrFromErrno(path,errno,abortOnErr);
}

/// translate
AptErr_t Fs::setErrFromErrno(const std::string& path,int sys_errno,bool abortOnErr) {
  switch (sys_errno) {
  case 0:
    return clearErr();
  case EEXIST:
    return setErr(APT_ERR_EXISTS,"exists: "+FS_QUOTE_PATH(path),abortOnErr);
    break;
  case EACCES:
    return setErr(APT_ERR_ACCESS,"access: "+FS_QUOTE_PATH(path),abortOnErr);
    break;
  default:
    return setErr(APT_ERR,"generic error: "+FS_QUOTE_PATH(path),abortOnErr);
    break;
  }
  return setErr(APT_ERR,"generic error: "+FS_QUOTE_PATH(path),abortOnErr);
}

//////////

std::string Fs::pathSep() {
  return std::string("/");
}

std::string Fs::osPathSep( const std::string & path ) {
  // dont use FSPATH_PATHSEP here as these should be literal strings.
#ifdef _WIN32
  if ( path.empty() ) {
    return std::string("\\");
  }
  return Fs::windowifyPath(path);
#else
  if ( path.empty() ) {
    return std::string("/");
  }
  return Fs::unixifyPath(path);
#endif
  
}

//////////

// Dont use FSPATH_PATHSEP for these two methods
// as they should be literial chars.

AptErr_t Fs::normalizePathInPlace(std::string& str) {
  return unixifyPathInPlace(str);
}

std::string Fs::normalizePath(const std::string& str)
{
  std::string tmp=str;
  normalizePathInPlace(tmp);
  return tmp;
}

// this is also used to unixify the string-path to our internal conventions.
AptErr_t Fs::unixifyPathInPlace(std::string& str) {
  // fix the slashes to go the correct way.
  std::replace(str.begin(),str.end(),'\\','/');
  return APT_OK;
}

std::string Fs::unixifyPath(const std::string& str)
{
  std::string tmp=str;
  unixifyPathInPlace(tmp);
  return tmp;
}

/// do what needs to be done to windowify a string.
AptErr_t Fs::windowifyPathInPlace(std::string& str) {
  // flip the slashes.
  std::replace(str.begin(),str.end(),'/','\\');
  return APT_OK;
}

std::string Fs::windowifyPath(const std::string& str) 
{
  std::string tmp=str;
  windowifyPathInPlace(tmp);
  return tmp;
}

//////////

std::string Fs::join(const std::string& arg1,
                     const std::string& arg2)
{
  if (arg1=="") {
    return normalizePath(arg2);
  }
  if (arg2=="") {
    return normalizePath(arg1);
  }
  if (arg1=="/") {
    return "/"+normalizePath(arg2);
  }
  return normalizePath(arg1+"/"+arg2);
}

/// @todo could be changed not to norm
std::string Fs::join(const std::string& arg1,
                     const std::string& arg2,
                     const std::string& arg3)
{
  return join(join(arg1,arg2),arg3);
}

std::string Fs::join(const std::string& arg1,
                     const std::string& arg2,
                     const std::string& arg3,
                     const std::string& arg4)
{
  return join(join(join(arg1,arg2),arg3),arg4);
}

std::string Fs::join(std::vector<std::string>& args) {
  std::string tmp_str;

  for (unsigned int i=0;i<args.size();i++) {
    tmp_str=Fs::join(tmp_str,args[i]);
  }
  return tmp_str;
}

//////////

bool Fs::hasTrailingSlash(const std::string& path) {
  if ((path.size()>0)&&(path.compare(path.size()-1,1,FS_PATHSEP)==0)) {
    return true;
  }
  return false;
}

std::string Fs::addTrailingSlash(const std::string& path_in)
{
  std::string path_out=path_in;
  if ((path_in!="")&&(!hasTrailingSlash(path_out))) {
    path_out+=FS_PATHSEP;
  }
  return path_out;
}

std::string Fs::trimTrailingSlash(const std::string& path_in)
{
  std::string path_out=path_in;
  if ((path_out!=FS_PATHSEP)&&(hasTrailingSlash(path_out))) {
    path_out.erase(path_out.size()-1);
  }
  return path_out;
}

//////////

std::string Fs::basename(const std::string& path)
{
  // unixify the string.
  std::string tmp_path=normalizePath(path);

  // should this be an error?
  // no as it makes taking the basename of "" fail.
  // we have lots of "foo=Fs::basename(getOpt("foo"))" where foo
  // is unset.
  //if (tmp_path=="") {
  //APT_ERR_ABORT("cant take the basename of '"+tmp_path+"'");
  //}

  //
  std::string::size_type pos;
  pos=tmp_path.rfind(FS_PATHSEP);
  if (pos==std::string::npos) {
    return tmp_path;
  }
  tmp_path=tmp_path.substr(pos+1);
  //printf("Fs::basename('%s')=>'%s'\n",path.c_str(),tmp_path.c_str());
  return tmp_path;
}

std::vector<std::string> Fs::basename(const std::vector<std::string>& paths)
{
  std::vector<std::string> out;
  out.reserve(paths.size());

  for (unsigned int i=0;i<paths.size();i++) {
    out.push_back(basename(paths[i]));
  }
  return out;
}

std::string Fs::dirname(const std::string& path)
{
  // unixify the string.
  std::string tmp_path=path;
  unixifyPathInPlace(tmp_path);

  if (tmp_path=="") {
    APT_ERR_ABORT("cant take the dirname of '"+tmp_path+"'");
  }
  if ((tmp_path=="/")||(tmp_path==".")) {
    return tmp_path;
  }
  // a trailing slash is not included.
  if (tmp_path.compare(tmp_path.size()-1,1,FS_PATHSEP)==0) {
    tmp_path.erase(tmp_path.size()-1);
  }

  //
  std::string::size_type pos;
  pos=tmp_path.rfind(FS_PATHSEP);
  if (pos==std::string::npos) {
    return ".";
  }
  tmp_path=tmp_path.substr(0,pos);
  //printf("Fs::dirname('%s')=>'%s'\n",path.c_str(),tmp_path.c_str());
  return tmp_path;
}

std::string Fs::extname(const std::string& path)
{
  // unixify the string.
  std::string tmp_path=basename(path);

  //
  std::string::size_type pos;
  pos=tmp_path.find(FS_EXTSEP);
  if (pos==std::string::npos) {
    return "";
  }
  tmp_path=tmp_path.substr(pos);
  //printf("Fs::extname('%s')=>'%s'\n",path.c_str(),tmp_path.c_str());
  return tmp_path;
}


std::string Fs::noextname(const std::string& path)
{
  // unixify the string.
  std::string tmp_path=path;
  unixifyPathInPlace(tmp_path);

  //
  std::string::size_type slash_pos;
  slash_pos=tmp_path.rfind(FS_PATHSEP);
  if (slash_pos==std::string::npos) {
    slash_pos=0;
  }
  std::string::size_type ext_pos;
  ext_pos=tmp_path.find(FS_EXTSEP,slash_pos);
  if (ext_pos!=std::string::npos) {
    tmp_path.erase(ext_pos);
  }
  //printf("Fs::noextname('%s')=>'%s'\n",path.c_str(),tmp_path.c_str());
  return tmp_path;
}


std::string Fs::noextname1(const std::string& path)
{
  // unixify the string.
  std::string tmp_path=path;
  unixifyPathInPlace(tmp_path);

  //
  std::string::size_type slash_pos;
  slash_pos=tmp_path.rfind(FS_PATHSEP);
  if (slash_pos==std::string::npos) {
    slash_pos=0;
  }
  std::string::size_type ext_pos;
  ext_pos=tmp_path.rfind(FS_EXTSEP);
  if ((ext_pos!=std::string::npos)&&(ext_pos>slash_pos)) {
    tmp_path.erase(ext_pos);
  }
  //printf("Fs::noextname1('%s')=>'%s'\n",path.c_str(),tmp_path.c_str());
  return tmp_path;
}

std::string Fs::noextnameN(const std::string& path, int cnt)
{
  // unixify the string.
  std::string tmp_path=path;
  unixifyPathInPlace(tmp_path);

  // no change.
  if (cnt==0) {
    return tmp_path;
  }

  //
  std::string::size_type fn_start;
  fn_start=tmp_path.rfind(FS_PATHSEP);
  if (fn_start==std::string::npos) {
    fn_start=0;
  }
  else {
    fn_start+=1; // the first char after the "/"
  }
  std::string::size_type fn_end=tmp_path.size();

  std::string::size_type tmp_pos=std::string::npos;
  for (int n=0;n<cnt;n++) {
    //printf("Fs::noextnameN(): str=%d,%d '%s'\n",int(fn_start),int(fn_end),
    //       tmp_path.substr(fn_start,fn_end-fn_start).c_str());
    tmp_pos=tmp_path.rfind(FS_EXTSEP,fn_end-1);
    if ((tmp_pos==std::string::npos)||(tmp_pos<fn_start)) {
      //printf("break; tmp_pos=%d\n",int(tmp_pos));
      break;
    }
    fn_end=tmp_pos;
  }
  //
  tmp_path.erase(fn_end);

  //printf("Fs::noextnameN('%s',%d)=>'%s'\n",path.c_str(),cnt,tmp_path.c_str());
  return tmp_path;
}

void Fs::splitPath(const std::string& path,std::string& drive,std::vector<std::string>& parts) {
  std::string::size_type s_pos;
  std::string::size_type e_pos;
  
  // clear our outputs before doing work.
  drive="";
  parts.clear();
  clearErr();

  std::string tmp_path=normalizePath(path);
  
  // split off the drive. ("C:")
  if ((tmp_path.size()>=2)&&(tmp_path[1]==':')) {
      drive=tmp_path.substr(0,2);
      tmp_path=tmp_path.substr(2);
  }
  else {
    drive="";
  }

  if (tmp_path=="") {
    return;
  }

  s_pos=0;
  e_pos=0;
  while (true) {
    e_pos=tmp_path.find(FS_PATHSEP,s_pos);
    //printf("  %d-%d = '%s'\n",int(s_pos),int(e_pos),tmp_path.substr(s_pos,e_pos-s_pos).c_str());
    if (e_pos==0) {
      // an abs path, push "/" as the first value.
      parts.push_back(FS_PATHSEP);
    }
    else if (e_pos==std::string::npos) {
      parts.push_back(tmp_path.substr(s_pos));
      // and we are done.
      break;
    }
    else {
      parts.push_back(tmp_path.substr(s_pos,e_pos-s_pos));
    }
    //printf("  %d-%d = '%s'\n",int(s_pos),int(e_pos),parts[parts.size()-1].c_str());

    // advance
    s_pos=e_pos+1;
    if (s_pos>=tmp_path.size()) {
      break;
    }
  }
}

// @todo should have a switch to choose between "noextname1" and "noextname".
std::vector<std::string> Fs::changeDirAndExt(const std::string& new_dir,
                                                 const std::vector<std::string>& paths,
                                                 const std::string& new_ext)
{
  std::vector<std::string> out;
  out.reserve(paths.size());

  for(unsigned int i=0;i<paths.size();i++) {
    out.push_back(join(new_dir,noextname1(basename(paths[i]))+new_ext));
  }
  return out;
}

//////////


std::string Fs::convertToUncPath(const std::string& str) {
  return Fs::convertToUncPath(str,FS_UNC_SHORTLEN);
}

std::string Fs::convertToUncPath(const std::string& str,int uncshortlen) {
  std::string tmp=str;
#ifdef _WIN32
  convertToUncPathInPlace(tmp,uncshortlen);
#else
  unixifyPathInPlace(tmp);
#endif
  return tmp;
}

AptErr_t Fs::convertToUncPathInPlace(std::string& path) {
  return Fs::convertToUncPathInPlace(path,FS_UNC_SHORTLEN);
}

AptErr_t Fs::convertToUncPathInPlace(std::string& path,int uncshortlen) {
#ifndef _WIN32
  return Fs::normalizePathInPlace(path);
#else
  // if the path is less than 260 it can be used as is.
  // (lets round down to 220)
  // actually lets not do it to make testing better.
  // if (out.size()<240) {
  //  return out;
  // }

  // nothing to do?
  if (path=="") {
    return clearErr();
  }

  // flip the slashes to back slashes...
  // do this in case some one joined an already converted path with "/"s.
  std::replace(path.begin(),path.end(),'/','\\');

  // now check if it has already been done.
  if ( path.substr(0,4) == std::string("\\\\?\\") ) {
    return APT_OK;
  }

  if (path.substr(0,2) == std::string("\\\\") ) {
    path = std::string("\\\\?\\UNC") + path.substr(1);
    return APT_OK;
  }

  // dont bother converting short paths.
  if ((uncshortlen==0)||(((int)path.size())<uncshortlen)) {
    return APT_OK;
  }


  std::wstring wsrcpath = Util::toWString(path);
  wchar_t wdstpath_char[UNC_MAX_PATH_BUFSIZ];
  
  // Compute the absolute path. We use the wide char
  // version which can handle > MAX_PATH
  DWORD rv=GetFullPathNameW(wsrcpath.c_str(),UNC_MAX_PATH_BUFSIZ, wdstpath_char, NULL);

  if (rv==0) {
    APT_ERR_ABORT("GetFullPathNameW failed.");
  }

  path = Util::toString( wdstpath_char );

  // if there is something there, tack on the "\\?\" prefix.
  // so we and windows know this is a UNC path.
  if (path!="") {
    path="\\\\?\\"+path;
  }
  //
  return APT_OK;
#endif
}

//////////

std::string Fs::convertCommandToUnc(const std::string& cmd) {
  // find the first sep
  std::string::size_type pos;
  pos=cmd.find(" ");

  // no sep? then no change.
  if (pos==std::string::npos) {
    return cmd;
  }

  // split the command into exe and args.
  std::string cmd_exe=cmd.substr(0,pos);
  std::string cmd_args=cmd.substr(pos);

  // change the command
  std::string cmd_exe_unc=Fs::convertToUncPath(cmd_exe);
  
  // put it back together.
  return cmd_exe_unc+cmd_args;
}

//////////

bool Fs::exists(const std::string& path) {
  int stat_rv;
  struct stat stat_buf;

  stat_rv = Fs__stat(path,stat_buf,stat_rv,false);
  if (stat_rv==0) {
    clearErr();
    return true;
  }
  //
  setErrFromErrno(path,false);
  return false;
}

bool Fs::dirExists(const std::string& path) {
  int stat_rv;
  struct stat stat_buf;

  Fs__stat(path,stat_buf,stat_rv,false);
  if (stat_rv!=0) {
    setErrFromErrno(path,false);
    return false;
  }
  
  if (!(S_ISDIR(stat_buf.st_mode))) {
    setErr(APT_ERR,"dirExists(): not a dir",false);
    return false;
  }
  
  clearErr();
  return true;
}

bool Fs::fileExists(const std::string& path) {
  int stat_rv;
  struct stat stat_buf;

  Fs__stat(path,stat_buf,stat_rv,false);
  if (stat_rv!=0) {
    setErrFromErrno(path,false);
    return false;
  }
  
  if (!S_ISREG(stat_buf.st_mode)) {
    setErr(APT_ERR,"fileExists(): not a file",false);
    return false;
  }
  
  clearErr();
  return true;
}

bool Fs::isReadable(const std::string& path) {
  AptErr_t rv;
  return isReadable(path,rv);
}

bool Fs::isReadable(const std::string& path,AptErr_t& rv)
{
  int stat_rv;
  struct stat stat_buf;

  Fs__stat(path,stat_buf,stat_rv,false);
  if (stat_rv!=APT_OK) {
    setErrFromErrno(path,false);
    return false;
  }
  return Fs__isreadable(stat_buf);
}

bool Fs::isWriteable(const std::string& path,AptErr_t& rv)
{
  int stat_rv;
  struct stat stat_buf;

  Fs__stat(path,stat_buf,stat_rv,false);
  if (stat_rv!=APT_OK) {
    setErrFromErrno(path,false);
    return false;
  }
  return Fs__iswriteable(stat_buf);
}

bool Fs::isWriteable(const std::string& path) {
  AptErr_t rv;
  return isWriteable(path,rv);
}

bool Fs::isReadableDir(const std::string& path) {
  AptErr_t rv;
  return isReadableDir(path,rv);
}

bool Fs::isReadableDir(const std::string& path,AptErr_t& rv)
{
  int stat_rv;
  struct stat stat_buf;

  Fs__stat(path,stat_buf,stat_rv,false);
  if (stat_rv!=APT_OK) {
    setErrFromErrno(path,false);
    return false;
  }
  if (!(S_ISDIR(stat_buf.st_mode))||(Fs__isreadable(stat_buf)==false)) {
    return false;
  }
  return true;
}

bool Fs::isWriteableDir(const std::string& path) {
  AptErr_t rv;
  return isWriteableDir(path,rv);
}

bool Fs::isWriteableDir(const std::string& path,AptErr_t& rv) {
  int stat_rv;
  struct stat stat_buf;

  Fs__stat(path,stat_buf,stat_rv,false);
  // no path.
  if (stat_rv!=0) {
    setErrFromErrno(path,false);
    return false;
  }

  // we would like to just write this:
  // return S_ISDIR(stat_buf.st_mode)&&Fs__iswriteable(stat_buf);
  // but we cant on windows.  Why?
  // Sometimes windows has it marked read only, but that is just
  // shorthand for "special".
  // See http://jira.ev.affymetrix.com:8080/browse/APT-380

  // this isnt a dir, so we fail.
  if (S_ISDIR(stat_buf.st_mode)==0) {
    return false;
  }
  // os says it is writeable, so we succeed
  if (Fs__iswriteable(stat_buf)==true) {
    return true;
  }

  // so now it is a dir, but os says it is read-only...

#ifdef _WIN32
  // however... on windows we check to see if we can write to it anyways.
  /// @todo generate a better test name.
  std::string probe_path=Fs::join(path,"Fs-Test-2009323948.tmp");

  if (fileExists(probe_path)) {
    APT_ERR_ABORT("Probefile "+FS_QUOTE_PATH(probe_path)+" exists already. It shouldnt.");
  }
  // try and create the file.
  Fs::touch(probe_path,false);
  // did it work?
  if (fileExists(probe_path)) {
    // yep get rid of it
    Fs::rm(probe_path);
    // and say we did.
    return true;
  }
#endif

  // unix doesnt lie... we cant write, say we cant.
  return false;
}

//////////

/// @brief     Does the file exist and start with the sequence of bytes.
/// @param     path      path to the file to test.
/// @param     bytes     vector of ints with a terminal "0".
/// @return    true if the file does exist and have the byte sequence.
static bool file_starts_with(const std::string& path,const int* magic_bytes) {
  std::fstream fs;

  Fs::aptOpen(fs, path, std::ios::in|std::ios::binary);
  if (!fs.good()) {
    return false;
  }

  for (int i=0;magic_bytes[i]!=0;i++) {
    if (fs.get()!=magic_bytes[i]) {
      fs.close();
      return false;
    }
  }
  fs.close();
  return true;
}

bool Fs::isHdf5File(const std::string& path) {
  int magic_bytes[]={ 0x89, 0x48, 0x44, 0x46, 0x0d, 0x00 }; // H D F
  return file_starts_with(path,magic_bytes);
}

bool Fs::isCalvinFile(const std::string& path) {
  int magic_bytes[]={ 0x3b, 0x01, 0x00 }; // the short calvin magic.
  return file_starts_with(path,magic_bytes);
}

bool Fs::isBinaryFile(const std::string& path)
{
  std::fstream fs;
  
  Fs::aptOpen(fs, path, std::ios::in|std::ios::binary);
  if (!fs.good()) {
    return false;
  }

  int cnt_txt=0;
  int cnt_bin=0;
  for (int i=0;i<1024;i++) {
    int c=fs.get();
    // the end?
    if (c==-1) {
      break;
    }
    // text?
    if (((c>=0x20)&&(c<=0x7E)) // " " -- "~"
        || (c==0x09) // "\t"
        || (c==0x0A) // "\n"
        || (c==0x0D) // "\r"
        ) {
      cnt_txt++;
      continue;
    }
    cnt_bin++;
  }

  // maybe compute some ratio?
  double cnt_total=cnt_bin+cnt_txt;
  if (cnt_total==0) {
    return false;
  }
  // for now, if there are any, we take the file as being binary.
  if (cnt_bin>0) {
    return true;
  }
  // not binary. (Which is not the same as text.)
  return false;
}

//////////

int64_t Fs::fileSize(const std::string& path,bool abortOnErr) {
  AptErr_t errnum;
  int64_t file_size=fileSize(path,errnum);
  //
  if ((errnum!=APT_OK)&&(abortOnErr==true)) {
    setErr(getErrNum(),getErrMsg(),abortOnErr);
  }
  return file_size;
}

int64_t Fs::fileSize(const std::string& path,AptErr_t& errnum) {
  int stat_rv;
  struct stat stat_buf;

  int64_t file_size=-1;
  if ((errnum=Fs__stat(path,stat_buf,stat_rv,false))==APT_OK) {
    file_size=stat_buf.st_size;
  }
  return file_size;
}

//////////

AptErr_t Fs::chmodBasic(const std::string& path,int mode,bool abortOnErr)
{
  int rv;
#ifdef _WIN32
  // http://technet.microsoft.com/en-us/library/bb463216.aspx
  // This only works at the user level.
  // http://msdn.microsoft.com/en-us/library/1z319a54.aspx
  // When both constants are given, they are joined with the bitwise OR operator ( |  ).
  // If write permission is not given, the file is read-only. Note that all files are always readable;
  // it is not possible to give write-only permission. Thus, the modes _S_IWRITE and _S_IREAD | _S_IWRITE are equivalent.
  std::wstring unc_path=Fs::UncW(path);
  rv=POSIX_CHMOD(unc_path.c_str(),mode);
#else
  rv=POSIX_CHMOD(path.c_str(),mode);
#endif
  if (rv!=0) {
    return setErr(APT_ERR,"chmod failed. "+FS_ERRNO_STR(),abortOnErr);
  }

  return clearErr();
}

//////////

AptErr_t Fs::listDir(const std::string& path,std::vector<std::string>& names,bool abortOnErr)
{
  clearErr();

  names.resize(0);

  if (path=="") {
    setErr(APT_ERR_NOTEXISTS,"blank path.",abortOnErr);
  }

#ifdef _WIN32
  // We need the "*" to match files.
  // This isnt just opening the dir and reading the contents,
  // but rather "what matches?".
  std::wstring unc_path=Fs::UncW(join(path,"*"));
  //
  WIN32_FIND_DATAW ffd;
  HANDLE hFind = INVALID_HANDLE_VALUE;
  DWORD rv=0;

  hFind = FindFirstFileW(unc_path.c_str(), &ffd);
  if (INVALID_HANDLE_VALUE == hFind) {
    return setErr(APT_ERR_NOTEXISTS,Util::toString(unc_path),abortOnErr);
  }

  while (true) {
    names.push_back(Util::toString(ffd.cFileName));
    if (FindNextFileW(hFind, &ffd)==0) {
      break;
    }
  }
  FindClose(hFind);
#else
  DIR *d_p;
  struct dirent* de_p;

  d_p=opendir(path.c_str());
  if (d_p==NULL) {
    return setErr(APT_ERR,"osListDir failed. "+FS_ERRNO_STR(),abortOnErr);
  }
  //int cnt=0;
  std::string tmp_name;
  while ((de_p=readdir(d_p))!=NULL) {
    //printf(" %2d: '%s'\n",cnt++,de_p->d_name);
    //
    tmp_name=de_p->d_name;
    if ((tmp_name==".")||(tmp_name=="..")) {
      continue;
    }
    // add it
    names.push_back(tmp_name);
  }
  closedir(d_p);
#endif

  // return them in order.
  sort(names.begin(),names.end());
  return clearErr();
}

//////////

AptErr_t Fs::mkdir(const std::string& path,
                   //bool errIfExists,
                   bool abortOnErr) {
  int rv;

#ifdef _WIN32
  std::wstring unc_path=Fs::UncW(path);
  rv=POSIX_MKDIR(unc_path.c_str());
#else
  rv=POSIX_MKDIR(path.c_str(), 0777);
#endif
  //
   if (rv!=0) {
     return setErrFromErrno(path,abortOnErr);
   }
   return clearErr();
}

AptErr_t Fs::mkdirPath(const std::string& path,
                       bool abortOnErr) {
  clearErr();

  std::string drive;
  std::vector<std::string> parts;
  splitPath(path,drive,parts);

  if (parts.size()==0) {
      return APT_OK;
  }

  std::string tmp_path;
  std::string tmp_drive_path;
  for (size_t i=0;i<parts.size();i++) {
    tmp_path=join(tmp_path,parts[i]);
    tmp_drive_path=drive+tmp_path;
    if (!dirExists(tmp_drive_path)) {
      mkdir(tmp_drive_path,false);
      if (!dirExists(tmp_drive_path)) {
        return setErr(APT_ERR,"mkdirPath failed."+FS_QUOTE_PATH(tmp_drive_path),abortOnErr);
      }
    }
  }

  return clearErr();
}

AptErr_t Fs::ensureWriteableDirPath(const std::string& path,
                                    bool abortOnErr) {
  if (!isWriteableDir(path)) {
    mkdirPath(path,false);
    if (isWriteableDir(path)) {
      return clearErr();
    }
    return setErr(APT_ERR,path,abortOnErr);
  }
  return clearErr();
}

//////////

/// @todo take an atime to adjust the file access to.
AptErr_t Fs::touch(const std::string& path,bool abortOnErr)
{
  std::fstream fs;

  Fs::aptOpen(fs, path,  std::ios::in|std::ios::binary);

  // try and open for reading.
  if (fs.is_open()) {
    // worked so we are done.
    fs.close();
    return clearErr();
  }

  // open for read didnt work, try to write to it.
  Fs::aptOpen(fs, path, std::ios::out|std::ios::binary);

  if (fs.is_open()) {
    // worked (Which created the file.)
    fs.close();
    return clearErr();
  }

  return setErr(APT_ERR,"touch of "+FS_QUOTE_PATH(path)+" failed.",abortOnErr);
}

//////////


#ifdef _WIN32
int64_t Fs::getFreeDiskSpace(const std::string& path,bool abortOnErr) {
  ULARGE_INTEGER lpFreeBytesAvailable;
  ULARGE_INTEGER lpTotalNumberOfBytes;
  ULARGE_INTEGER lpTotalNumberOfFreeBytes;

  clearErr();

  std::wstring unc_path=Fs::UncW(path);

  if (!GetDiskFreeSpaceExW(unc_path.c_str(),
                          &lpFreeBytesAvailable,
                          &lpTotalNumberOfBytes,
                          &lpTotalNumberOfFreeBytes)) {
    setErr(APT_ERR,"getFreeDiskSpaceNative of "+FS_QUOTE_PATH(path)+" failed.",abortOnErr);
    return -1;
  }
  
  return lpTotalNumberOfFreeBytes.QuadPart;
}
#endif

#if defined(__linux__) || defined(__APPLE__)
int64_t Fs::getFreeDiskSpace(const std::string& path,bool abortOnErr) {
  struct statfs retBuf;
  clearErr();

  if (statfs(path.c_str(), &retBuf)!=0) {
    setErr(APT_ERR,"getFreeDiskSpaceNative of "+FS_QUOTE_PATH(path)+" failed. "+FS_ERRNO_STR(),abortOnErr);
    return -1;
  }
  return int64_t(retBuf.f_bavail) * int64_t(retBuf.f_bsize);
}
#endif

//////////

AptErr_t Fs::rm(const std::string& path,
                //bool errIfNotExists,
                bool abortOnErr) {
  int rv;
#ifdef _WIN32
  std::wstring unc_path=Fs::UncW(path);
  rv=POSIX_UNLINK(unc_path.c_str());
#else
  rv=POSIX_UNLINK(path.c_str());
#endif
  if (rv!=0) {
    return setErrFromErrno(path,abortOnErr);
  }
  return clearErr();
}

AptErr_t Fs::rmIfExists(const std::string& path,
                bool abortOnErr) {
  if (fileExists(path)) {
    return rm(path,abortOnErr);
  }
  return clearErr();
}

//////////

AptErr_t Fs::rmdir(const std::string& path,bool abortOnErr)
{
  clearErr();

  if (path=="/") {
    return setErr(APT_ERR,"Wont remove root dir.",abortOnErr);
  }
  if (!dirExists(path)) {
    return setErr(APT_ERR_NOTEXISTS,"does not exist.",abortOnErr);
  }

#ifdef _WIN32
  std::wstring unc_path=Fs::UncW(path);
  bool rv=RemoveDirectoryW(unc_path.c_str())==TRUE;
  if (rv==true) {
    return setErr(APT_OK,"",abortOnErr);
  }
#else
  // posix
  int rv=::rmdir(path.c_str());
  if (rv==0) {
    return clearErr();
  }
  /// @todo handle errs
  return setErr(APT_ERR,"rmdir "+FS_ERRNO_STR(),abortOnErr);
#endif
  return clearErr();
}

AptErr_t Fs::rmdirPath(const std::string& path,
                       bool abortOnErr) {
  if (path=="") {
    return clearErr();
  }
  
  std::string drive;
  std::vector<std::string> tmp_parts;
  splitPath(path,drive,tmp_parts);
  
  std::string tmp_path;
  std::string tmp_drive_path;
  while (tmp_parts.size()>0) {
    tmp_path=Fs::join(tmp_parts);
    tmp_drive_path=drive+tmp_path;
    Fs::rmdir(tmp_drive_path,abortOnErr);
    if (Fs::dirExists(tmp_drive_path)) {
      return setErr(APT_ERR,tmp_drive_path,abortOnErr);
    }
    tmp_parts.pop_back();
  }
  
  return clearErr();
}

//////////

AptErr_t Fs::rm_rf(const std::string& path,bool abortOnErr)
{
  // try and protect against trival mistakes.
  if ((path=="/")||(path==".")||(path=="")) {
    return setErr(APT_ERR,"wont attempt to remove "+FS_QUOTE_PATH(path),abortOnErr);
  }
  
  if (fileExists(path)) {
    return rm(path);
  }
  if (dirExists(path)) {
    std::vector<std::string> names;
    listDir(path,names);
    for (unsigned int i=0;i<names.size();i++) {
      rm_rf(Fs::join(path,names[i]));
    }
    rmdir(path);
    if (dirExists(path)) {
      return setErr(APT_ERR,"dir still there.",abortOnErr);
    }
    return clearErr();
  }
  //
  return setErr(APT_ERR_NOTEXISTS,"rm_rf(): "+FS_QUOTE_PATH(path)+" does not exist.",abortOnErr);
}

//////////

#if defined(__APPLE__) || defined(__linux__)
bool Fs::isSameVolume(const std::string& path1,
                      const std::string& path2,
                      AptErr_t& rv,
                      bool abortOnErr)
{
  int stat_rv;
  struct stat stat_buf1;
  struct stat stat_buf2;

  clearErr();

  Fs__stat(path1,stat_buf1,stat_rv,abortOnErr);
  Fs__stat(path2,stat_buf2,stat_rv,abortOnErr);

  if (stat_buf1.st_dev==stat_buf2.st_dev) {
    return true;
  }
  return false;
}
#endif

#ifdef _WIN32
bool Fs::isSameVolume(const std::string& path1,
                      const std::string& path2,
                      AptErr_t& rv,
                      bool abortOnErr)
{

  rv = APT_OK;
  wchar_t volumeNameBuffer1[UNC_MAX_PATH_BUFSIZ];
  wchar_t volumeNameBuffer2[UNC_MAX_PATH_BUFSIZ];
  
  clearErr();

  std::wstring tmp_path1=Fs::UncW(path1);
  std::wstring tmp_path2=Fs::UncW(path2);

  if (!GetVolumePathNameW(tmp_path1.c_str(),
                         volumeNameBuffer1,
                         UNC_MAX_PATH_BUFSIZ)) {
    rv = APT_ERR;
    setErr(APT_ERR,"Fs::isSameVolume() - GetVolumePathName() failed.",abortOnErr);
    return false;
  }

  if (!GetVolumePathNameW(tmp_path2.c_str(),
                         volumeNameBuffer2,
                         UNC_MAX_PATH_BUFSIZ)) {
    rv = APT_ERR;
    setErr(APT_ERR,"Fs::isSameVolume() - GetVolumePathName() failed.",abortOnErr);
    return false;
  }

  //
  if ( wcscmp(volumeNameBuffer1,volumeNameBuffer2) == 0 ) {
    // now we know they are on the same volume...
    return true;
  }
  // not the same volume.
  return false;

}
#endif

#ifdef _WIN32
std::string Fs::MSC_VER_GetLastErrorString( const std::string & whence )
{
  // Retrieve the system error message for the last-error code

  
  LPVOID lpMsgBuf;
  DWORD dw = GetLastError();

  FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                dw,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR) &lpMsgBuf,
                0, NULL );

  std::stringstream msgSStr;

  if ( ! whence.empty() ) {
    msgSStr << whence << " failed with error " << dw << ": " << (LPTSTR) lpMsgBuf ;
  }
  else {
    msgSStr << (LPTSTR) lpMsgBuf;
  }
  LocalFree(lpMsgBuf);

  return msgSStr.str();
}

#endif

void Fs::aptOpen( std::fstream & fs, const std::string & filename, std::ios_base::openmode iomode) {

#ifdef _WIN32
  // PATCH: Apparently std::basic_ofstream<char>::open(const wchar_t*,
  //   std::ios_base::openmode&) may not be defined.  The problem is
  //   the type of argument #1; it is defined for 'const char*' but
  //   not 'const wchar_t*'.  So, instead of calling Fs::UncW(filename)
  //   we call Fs::Unc(filename), which return char*.  I'm not 100%
  //   sure what the catch is, but it may be that some filenames that
  //   require UTF-16 will not work. /HB 2012-08-29
  std::string wtemp =  Fs::Unc(filename) ;
  fs.open(wtemp.c_str(), iomode);
#else
  fs.open(filename.c_str(), iomode);
#endif   
}

void Fs::aptOpen( std::ofstream & ofs, const std::string & filename, std::ios_base::openmode iomode) {

#ifdef _WIN32
  // PATCH: Apparently std::basic_ofstream<char>::open(const wchar_t*,
  //   std::ios_base::openmode&) may not be defined.  The problem is
  //   the type of argument #1; it is defined for 'const char*' but
  //   not 'const wchar_t*'.  So, instead of calling Fs::UncW(filename)
  //   we call Fs::Unc(filename), which return char*.  I'm not 100%
  //   sure what the catch is, but it may be that some filenames that
  //   require UTF-16 will not work. /HB 2012-08-29
  std::string wtemp = Fs::Unc(filename);
  ofs.open(wtemp.c_str(), iomode);
#else
  ofs.open(filename.c_str(), iomode);
#endif   
}

void Fs::aptOpen( std::ifstream & ifs, const std::string & filename, std::ios_base::openmode iomode) {

#ifdef _WIN32
  // PATCH: Apparently std::basic_ofstream<char>::open(const wchar_t*,
  //   std::ios_base::openmode&) may not be defined.  The problem is
  //   the type of argument #1; it is defined for 'const char*' but
  //   not 'const wchar_t*'.  So, instead of calling Fs::UncW(filename)
  //   we call Fs::Unc(filename), which return char*.  I'm not 100%
  //   sure what the catch is, but it may be that some filenames that
  //   require UTF-16 will not work. /HB 2012-08-29
  std::string wtemp = Fs::Unc(filename);
  ifs.open(wtemp.c_str(), iomode);
#else
  ifs.open(filename.c_str(), iomode);
#endif   
}

int Fs::aptOpen( const std::string & pathname, int flags ) {

#ifdef _WIN32
  std::wstring wtemp = Fs::UncW(pathname);
  return _wopen(wtemp.c_str(), flags);
#else
  return open(pathname.c_str(), flags);
#endif   

}

int Fs::aptOpen( const std::string & pathname, int flags, int mode ) {

#ifdef _WIN32
  std::wstring wtemp = Fs::UncW(pathname);
  return _wopen(wtemp.c_str(), flags, mode);
#else
  return open(pathname.c_str(), flags, mode);
#endif   

}

std::string Fs::findLibFile(const std::string &fileName, const std::string &searchPath){

    if(fileName == "") {
        // empty string is not valid file name. just return it back
        return fileName;
    } else if(Fs::fileExists(fileName)) {
        // file exists as already specified
        return fileName;
    } else {
        // Now lets search for the file
        std::vector<std::string> searchPathVec;
        if(searchPath != "") {
            // For path separator use ';' on windows.
            // Use ':' or ';' on unix.
#ifdef _WIN32
          Util::chopString(searchPath,';',searchPathVec);
#else
          Util::chopString(searchPath,":;",searchPathVec);
#endif
        } else {
            char *sp = getenv("AFFX_ANALYSIS_FILES_PATH");
            if(sp == NULL) {
                return fileName;
            } else {
              Util::chopString(sp,':',searchPathVec);
            }
        }

        for(int i=0; i< searchPathVec.size(); i++) {
          if(Fs::fileExists(Fs::join(searchPathVec[i] , fileName)))
             return Fs::join(searchPathVec[i] , fileName);
        }
    }

    // did not find anything, so just return what we started with
    return fileName;
}

/**
 * Open an ofstream for writing to. Abort if can't open
 * for some reason.
 * @param out - stream to be opened.
 * @param fileName - name of file to be opened.
 */
void Fs::mustOpenToWrite(std::ofstream &out, const std::string &fileName) {
  assert(!fileName.empty());
  Fs::aptOpen(out, fileName);
  if(!out.is_open() || !out.good()) {
    APT_ERR_ABORT("Couldn't open file: " +FS_QUOTE_PATH(fileName) + " to write.");
  }

  // Set to throw an exception if something bad happens rather than silently fail.
  out.exceptions(std::ofstream::eofbit | std::ofstream::failbit | std::ofstream::badbit );
}

/**
 * Close an output stream making sure that it is ok before doing so.
 * @param out - stream to be closed.
 */
void Fs::carefulClose(std::ofstream &out) {
  // If file is open, do some checks to make sure that it was successful...
  if(out.is_open()) {
    if(out.bad()) {
      APT_ERR_ABORT("Fs::carefulClose() - ofstream bad.");
    }
  }
  out.close();
}

/**
 * Close an output stream making sure that it is ok before doing so.
 * @param out - stream to be closed.
 */
void Fs::carefulClose(std::fstream &out) {
  // If file is open, do some checks to make sure that it was successful...
  if(out.is_open()) {
    if(out.bad()) {
      APT_ERR_ABORT("Fs::carefulClose() - ofstream bad.");
    }
  }
  out.close();
}


/**
 * Return true on success. False otherwise
 * @param in - file to copy
 * @param out - name of the new file
 */
bool Fs::fileCopy(const std::string &in, const std::string &out, bool throwOnError) {
  ///@todo there is probably a better way to copy files and check for errors
  bool success = true;

  std::ifstream is;
  std::ofstream os;
  std::string iName = in;
  std::string oName = out;

  Fs::aptOpen(is, in, std::ios::binary);

  Fs::aptOpen(os, out, std::ios::binary);

  if(!is.good() || !os.good())
      success = false;

  os << is.rdbuf();
  if(!is.good() || !os.good())
      success = false;

  is.close();
  os.close();
  if(!is.good() || !os.good())
      success = false;

  if(throwOnError && !success)
      APT_ERR_ABORT("Unable to copy file '" + iName + "' to '" + oName + "'");
  return success;
}

/**
 * Not intended to direct use. Only indirectly via Fs::fileRename()
 * We make multiple attempts to get around read lock issues.
 */
static bool _uncheckedFileRename(const std::string &in, const std::string &out, int tries, int sec) {
    tries--;
    sec *= 3;
    bool success = true;
#ifdef _WIN32
    success = (MoveFileExW(Fs::convertToUncPathW(in).c_str(), Fs::convertToUncPathW(out).c_str(),
                          MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH) != 0);
#else
    success = (rename(in.c_str(), out.c_str()) == 0);
#endif
    if(!success && (tries > 0)) {
#ifdef _WIN32
      Sleep(sec);
#else      
      sleep(sec);
#endif        
      return _uncheckedFileRename(in, out, tries, sec);
    }
    
    return success;
}
/**
 * Return true on success. False otherwise
 * @param in - file to move
 * @param out - name of the new file
 */
bool Fs::fileRename(const std::string &in, const std::string &out, bool throwOnError) {
    bool success = true;
    success = _uncheckedFileRename(in,out,4,10);
  if(throwOnError && !success)
      APT_ERR_ABORT("Unable to rename file '" + in + "' to '" + out + "'");
  return success;
}

