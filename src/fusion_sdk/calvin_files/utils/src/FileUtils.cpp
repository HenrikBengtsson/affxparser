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

#ifdef _MSC_VER
#include "windows.h"
#endif

#include "calvin_files/utils/src/FileUtils.h"
//
#include <sys/stat.h>
#include <sys/types.h>
//

#ifdef _MSC_VER
#include <direct.h>
#define         S_ISDIR(m)   (((m) & S_IFMT) == S_IFDIR)
#else
#include <unistd.h>
#include <dirent.h>
#endif




using namespace affymetrix_calvin_utilities;
using namespace std;

static string LockFileExtension = ".lock";

/*
 * Check the existance of the file.
 */
bool FileUtils::Exists(const char *fileName)
{
    ///@todo will not correctly handle long names passed in
    ///      with leading "\\?\". Such files can exist, but
    ///      will fail this check.
	struct stat st;
	return (stat(fileName, &st) == 0);
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
	fileStream.open(lockFile.c_str(), ios::out);
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

	struct stat st;
	DIR *dirp = opendir(pathName);
	struct dirent *dp;
	bool cont = (dirp != NULL);
	while (cont)
	{
		dp = readdir(dirp);
		if (dp)
		{
			string file = basePath + dp->d_name;
			stat(file.c_str(), &st);
			if ((st.st_mode & S_IFDIR) != S_IFDIR)
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

	struct stat st;
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
				stat(filePath.c_str(), &st);
				if ((st.st_mode & S_IFDIR) != S_IFDIR)
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

