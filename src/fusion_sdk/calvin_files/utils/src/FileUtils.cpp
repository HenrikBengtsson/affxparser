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

#ifdef _WIN32
#include "windows.h"
#endif

#include "calvin_files/utils/src/FileUtils.h"
//
#include "util/Fs.h"
//
#include <fcntl.h>
#include <sstream>
#include <string>
#include <sys/types.h>
//

#ifdef _WIN32
#include <direct.h>
#define 	S_ISDIR(m)   (((m) & S_IFMT) == S_IFDIR)
#else
#include <unistd.h>
#include <dirent.h>
#endif

#ifdef WIN32
#include <io.h>
#include <stdio.h>
#define BUFFSIZE 10000
#endif

#define POSIX_OPEN open
#define POSIX_CLOSE close

using namespace affymetrix_calvin_utilities;
using namespace std;

static string LockFileExtension = ".lock";

/** Little template function to make string conversion easy. 
    this isn't the fastest way to do things, but it is easy. */
template <class T> 
std::string FUToStr(const T &t) {
  std::ostringstream s;
  s.setf(std::ios_base::boolalpha);
  s << t;
  
  if(s.str() == "-1.#INF")
      return(FUToStr("-inf"));
  else if(s.str() == "1.#INF")
      return(FUToStr("inf"));
  else if(s.str() == "-1.#IND")
      return(FUToStr("nan"));
  else if(s.str() == "1.#IND")
      return(FUToStr("nan"));
  else if(s.str() == "-Inf")
      return(FUToStr("-inf"));
  else if(s.str() == "Inf")
      return(FUToStr("inf"));

  return s.str();
};

///@todo copy of the code from Util::subChar. Ugly Hack to prevent APT dependencies
static void subChar(std::string &s, char from, char to) {
    std::string::size_type pos = 0;
    for(pos = 0; pos < s.size(); pos++) {
        if(s[pos] == from) {
            s[pos] = to;
        }
    }
}

///@todo copy of the code from Util::convertPathName. Ugly Hack to prevent APT dependencies
/* Private method to this file used to convert a relative path
   into an absolute path, and if successful to add "\\?\"
   prefix. This prevents the system APIs for failing on long
   filenames/paths by forcing them to pass the filename
   directly down into the low level file system APIs. */
#if defined (WIN32)
std::string _getFullPathWindowsLocal(const std::string &in) {
    DWORD  retval=1;
    wchar_t  *wbuffer = new wchar_t[BUFFSIZE];  // Wide Char buffer for input path
    wchar_t  *buffer = new wchar_t[BUFFSIZE];   // Wide Char buffer for output absolute path
    char   *abuffer = new char[BUFFSIZE];       // Asci Char buffer for output absolute path
    unsigned int strLen = 0;

    // Setup wbuffer with our input
    const char *inPtr = in.c_str();
    strLen = strlen(inPtr);
    if(strLen > BUFFSIZE - 1) {
      std::string se = "Cannot handle string longer than " + FUToStr(BUFFSIZE) + ": '" +
                in + "' is " + FUToStr(strLen);
      // PATCH: std::exception(const char*) does not have to exist according
      //        to standards.  Using subclass runtime_error instead. /HB
		throw new std::runtime_error(se.c_str());
    }
    mbstowcs(wbuffer,inPtr, strLen + 1);

    // Compute the absolute path. We use the wide char
    // version which can handle > MAX_PATH
    retval = GetFullPathNameW(wbuffer,BUFFSIZE - 1,buffer,NULL);

    // If no characters were converted, then simply return the original string
    if (retval < 1) {
        delete [] wbuffer;
        delete [] buffer;
        delete [] abuffer;
        return in;
    }

    // If we will overflow our buffer, then abort.
    if(retval > BUFFSIZE-1) {
      // PATCH: std::exception(const char*) does not have to exist according
      //        to standards.  Using subclass runtime_error instead. /HB
      throw new std::runtime_error("Unexpected failure. Converted more characters than expected");
    }

    // Convert the absolute path to asci char from wide char
    wcstombs(abuffer, (wchar_t *)buffer, retval + 1);

    // Free up memory and return our string
    string rs;
    rs = abuffer;
    delete [] wbuffer;
    delete [] buffer;
    delete [] abuffer;
    if(rs == "") {
        // If our result is empty, return input string
        return in;
    } else {
        // Otherwise we have an absolute path, so add the "\\?\" magic
        if(rs == "") {
            return in;
        } else {
            if(rs.substr(0,2) == "\\\\")
                return rs;
            else
                return "\\\\?\\" + rs;
        }
    }
}
#endif

///@todo copy of the code from Util::convertPathName. Ugly Hack to prevent APT dependencies
std::string convertPathNameLocal(const std::string &path, bool singleFile) {
  std::string s = path;
  if(s.find(':') != std::string::npos)
      return s;

#if defined (WIN32)
  // Convert forward slash to back slash -- 92 is the ascii code for '\'
  subChar(s, '/', 92);
  // If we are given a single file name, then try and cope with extra long names
  // by using '\\?\C:\...\' magic
  if(singleFile) {
    // First we split into a path and filename parts
    size_t pos=s.rfind("\\");
    string pathPart, filePart;
    if(pos != std::string::npos) {
        pathPart = s.substr(0,pos);
        filePart = s.substr(pos+1);
    } else {
        // No path part found, so assume CWD
        pathPart = ".";
        filePart = s;
    }
    //Verbose::out(1,"Path Part = '" + pathPart + "'");
    //Verbose::out(1,"File Part = '" + filePart + "'");
    // Now we call this method to convert the minimal path part to
    // an absolute path. _getFullPathWindows() will add the
    // "\\?\" magic if appropriate.
    pathPart = _getFullPathWindowsLocal(pathPart);
    s = pathPart + '\\' + filePart;
  }
#else
  // Unix deals with long filenames/paths as one would expect.
  // So all we need to do is flip back slashes to forward slashes.
  // 92 is the ascii code for '\'
  subChar(s, 92, '/');
#endif
  //Verbose::out(1, "Converted '" + path + "' to '" + s + "'");
  return s;
}

/*
 * Check the existance of the file.
 */
bool FileUtils::Exists(const char *fileName)
{
  ///@todo this is an ugly hack to prevent APT dependencies. 
  ///      The following is a copy from Util::fileReadable()
  int f;
  f = Fs::aptOpen(fileName, O_RDONLY);
  if(f < 0) {
    f = Fs::aptOpen(convertPathNameLocal(fileName,true).c_str(), O_RDONLY);
    if(f < 0) {
      return false;
    }
  }
  POSIX_CLOSE(f);
  return true;
}


/*
 * Delete the input file.
 */
bool FileUtils::RemoveFile(const char *fileName)
{
	return (remove(fileName) == 0);
}

/*
 * Check if either the file or the lock file exists.
 * Return false the file does not exist of if the lock file does exist.
 * Otherwise create the lock file and return the status of the creation.
 */
bool FileUtils::LockFile(const char *fileName)
{
	if (FileUtils::Exists(fileName) == false)
		return false;

	string lockFile = fileName + LockFileExtension;
	if (FileUtils::Exists(lockFile.c_str()) == true)
		return false;

	ofstream fileStream;
        Fs::aptOpen(fileStream, lockFile, ios::out);
	bool isOpen = fileStream.is_open();
	fileStream.close();
	return isOpen;
}

/*
 * If the lock file does not exist then return a false.
 * Otherwise remove the lock file and return the status.
 */
bool FileUtils::UnlockFile(const char *fileName)
{
	string lockFile = fileName + LockFileExtension;
	if (FileUtils::Exists(lockFile.c_str()) == false)
		return true;
	return (remove(lockFile.c_str()) == 0);
}

list<string> FileUtils::ListFiles(const char *pathName, const char *ext)
{
	list<string> files;
	string basePath = pathName;
	if (basePath.length() > 0)
	{
		if (basePath[basePath.length()-1] != '\\' && basePath[basePath.length()-1] != '/')
		{
			basePath += "/";
		}
	}
	string exten = ext;

#ifdef _MSC_VER

	WIN32_FIND_DATA findData;
	string search = basePath + "*.";
	if (exten.length() == 0)
		search += "*";
	else
		search += exten;
	HANDLE hHandle = FindFirstFile(search.c_str(), &findData);
	BOOL bFound = (hHandle != INVALID_HANDLE_VALUE);
	while (bFound)
	{
		if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			string file = basePath + findData.cFileName;
			files.push_back(file);
		}

		// Find the next file.
		bFound = ::FindNextFile(hHandle, &findData);
	}
	::FindClose(hHandle);

#else

	DIR *dirp = opendir(pathName);
	struct dirent *dp;
	bool cont = (dirp != NULL);
	while (cont)
	{
		dp = readdir(dirp);
		if (dp)
		{
			string file = basePath + dp->d_name;
			if (Fs::fileExists(file))
			{
				if (exten.length() == 0)
				{
					files.push_back(file);
				}
				else
				{
					if (file.rfind(ext) == file.length()-exten.length())
					{
						files.push_back(file);
					}
				}
			}
		}
		cont = (dp != NULL && dirp != NULL);
	}
	if (dirp)
		closedir(dirp);

#endif

	return files;
}

/* Deletes a file. */
void FileUtils::RemovePath(const char *path)
{
	list<string> files;
	string basePath = path;
	if (basePath.length() > 0)
	{
		if (basePath[basePath.length()-1] != '\\' && basePath[basePath.length()-1] != '/')
		{
			basePath += "/";
		}
	}

#ifdef _MSC_VER

	WIN32_FIND_DATA findData;
	string search = basePath + "*.*";
	HANDLE hHandle = FindFirstFile(search.c_str(), &findData);
	BOOL bFound = (hHandle != INVALID_HANDLE_VALUE);
	while (bFound)
	{
		string fileName = findData.cFileName;
		if (fileName != "." && fileName != "..")
		{
			string filePath = basePath + findData.cFileName;
			if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				FileUtils::RemoveFile(filePath.c_str());
			}
			else
			{
				FileUtils::RemovePath(filePath.c_str());
			}
		}

		// Find the next file.
		bFound = ::FindNextFile(hHandle, &findData);
	}
	::FindClose(hHandle);
	_rmdir(path);

#else

	DIR *dirp = opendir(path);
	struct dirent *dp;
	bool cont = (dirp != NULL);
	while (cont)
	{
		dp = readdir(dirp);
		if (dp)
		{
			string fileName = dp->d_name;
			if (fileName != "." && fileName != "..")
			{
				string filePath = basePath + dp->d_name;
				if (Fs::fileExists(filePath))
				{
					FileUtils::RemoveFile(filePath.c_str());
				}
				else
				{
					FileUtils::RemovePath(filePath.c_str());
				}
			}
		}
		cont = (dp != NULL && dirp != NULL);
	}
	if (dirp)
		closedir(dirp);
	rmdir(path);

#endif

}

