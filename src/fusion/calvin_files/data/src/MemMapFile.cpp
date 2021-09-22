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


#include "calvin_files/data/src/MemMapFile.h"
//

using namespace affymetrix_calvin_io;

/*
 * Gets the error message for the last error in a Window API call.
 */
std::wstring MemMapFile::GetErrorMsg()
{
	LPVOID lpMsgBuf;
	if (!FormatMessageW( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPWSTR) &lpMsgBuf,
			0,
			NULL ))
	{
		// Handle the error.
		return L"";
	}

	std::wstring message = (wchar_t*)lpMsgBuf;

	// Free the buffer.
	LocalFree( lpMsgBuf );

	return message;
}

/*
 * Constructor
 */
MemMapFile::MemMapFile()
{
	fileSize = 0;
	dwMapMode = 0;
	mappedData = 0;
	data = 0;
	startMapPos = 0;
	bytesMapped = 0;
	fileMapHandle = NULL;
	fileHandle = INVALID_HANDLE_VALUE;
	flushMMView = false;
	offsetToStart = 0;
}

/*
 * Creates a file.
 */
bool MemMapFile::Create(__int64 size)
{
	Close();

	if (fileName.length() == 0)
		return false;

	// create a memory-mapped file
	dwMapMode = FILE_MAP_READ;
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		fileHandle = CreateFile(fileName.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ,
												NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		dwMapMode = FILE_MAP_WRITE;
	}
	if (fileHandle != INVALID_HANDLE_VALUE)
	{
		fileSize = size;
		DWORD dwHigh = DWORD(size >> 32);
		DWORD dwLow  = DWORD(size & 0xFFFFFFFF);

		DWORD dwProtect = (dwMapMode == FILE_MAP_READ) ? PAGE_READONLY : PAGE_READWRITE;
		fileMapHandle = CreateFileMapping(fileHandle, NULL, dwProtect, dwHigh, dwLow, NULL);
		if (fileMapHandle == NULL)
		{
			errorMsg = GetErrorMsg();
      CloseHandle (fileHandle);
      fileHandle = INVALID_HANDLE_VALUE;
			return false;
		}
	}
	else
	{
		errorMsg = GetErrorMsg();
		return false;
	}

	return true;
}

/*
 * Opens a file using memory-mapping.
 */
bool MemMapFile::Open(RWAccess rwAccess, ShareMode shareMode)
{
	Close ();

	if (fileName.length() == 0)
		return false;

	// Open a file handle
	DWORD dwAccess = GENERIC_READ;
	dwMapMode = FILE_MAP_READ;
	if (rwAccess & WRITE)
	{
		dwAccess |= GENERIC_WRITE;
		dwMapMode = FILE_MAP_WRITE;
	}
	DWORD dwShare = 0;
	if (shareMode & ALLOWREAD)
		dwShare |= FILE_SHARE_READ;
	if (shareMode & ALLOWWRITE)
		dwShare |= FILE_SHARE_WRITE;
	DWORD dwCreate = (rwAccess & WRITE) ? ((rwAccess & READ) ?
										OPEN_ALWAYS : CREATE_ALWAYS) : OPEN_EXISTING;
	fileHandle = CreateFile(fileName.c_str(), dwAccess, dwShare, NULL,
											dwCreate, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileHandle == INVALID_HANDLE_VALUE)
		return false;

	DWORD dwSizeHigh = 0;
	DWORD dwSizeLow  = GetFileSize(fileHandle, &dwSizeHigh);
	fileSize = (__int64(dwSizeHigh) << 32) + __int64(dwSizeLow);

	// Create a file map handle
	DWORD dwProtect = (dwMapMode == FILE_MAP_READ) ? PAGE_READONLY : PAGE_READWRITE;
	fileMapHandle = CreateFileMapping(fileHandle, NULL, dwProtect, dwSizeHigh, dwSizeLow, NULL);
	if (fileMapHandle == NULL)
	{
		errorMsg = GetErrorMsg();
		CloseHandle(fileHandle);
		fileHandle  = INVALID_HANDLE_VALUE;
		return false;
	}

	return true;
}

/*
 * Maps a view of the file.
 */
bool MemMapFile::MapData(__int64 start, DWORD bytes)
{
	if (fileHandle == INVALID_HANDLE_VALUE)
		return false;
	if (fileMapHandle == NULL)
		return false;

	SYSTEM_INFO sysinfo;
	GetSystemInfo (&sysinfo);
	__int64 qwFileOffset = start;
	DWORD dwOffset = DWORD(qwFileOffset % sysinfo.dwAllocationGranularity);
	qwFileOffset = (qwFileOffset / sysinfo.dwAllocationGranularity) *
								sysinfo.dwAllocationGranularity;
	DWORD dwOffsetHigh = DWORD(qwFileOffset >> 32);
	DWORD dwOffsetLow  = DWORD(qwFileOffset & 0xFFFFFFFF);
	if (start + __int64(bytes) > fileSize)
		bytes = (DWORD)(fileSize - start);
	DWORD dwBytesToMap = bytes + dwOffset;

	if (mappedData != 0)
	{
		if (flushMMView)
			FlushViewOfFile(mappedData, offsetToStart+bytesMapped);
		UnmapViewOfFile (mappedData);
	}
	mappedData = MapViewOfFile(fileMapHandle, dwMapMode, dwOffsetHigh, dwOffsetLow, dwBytesToMap);
	if (mappedData == NULL)
	{
		data = 0;
		bytesMapped = 0;
		CloseHandle (fileMapHandle);
		fileMapHandle = NULL;
		return false;
	}
	bytesMapped = bytes;
	offsetToStart = dwOffset;
	data = (char *)mappedData + dwOffset;
	startMapPos = (u_int32_t)start;	// need to rethink for large files.
	return true;
}

/*
 * Closes the mapped view and file access.
 */
void MemMapFile::Close()
{
	UnmapFile();
}

/*
 * Unmaps a view
 */
void MemMapFile::UnmapFile()
{
	if (fileHandle != INVALID_HANDLE_VALUE)
	{
		if (fileMapHandle != NULL)
		{
			if (mappedData != 0 )
			{
				if (flushMMView)
					FlushViewOfFile(mappedData, offsetToStart+bytesMapped);
				UnmapViewOfFile(mappedData);
				mappedData = 0;
			}
			CloseHandle(fileMapHandle);
			fileMapHandle = NULL;
			data = 0;
		}
		CloseHandle (fileHandle);
		fileHandle = INVALID_HANDLE_VALUE;
	}
}

