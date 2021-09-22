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


#ifndef _MemMapFile_HEADER_
#define _MemMapFile_HEADER_

/*! \file MemMapFile.h This file defines a class that encapsulates Windows memory-mapped
 *	file access methods.
 */

#ifdef _MSC_VER
#include <windows.h>
#endif

//
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//
#include <cstring>
#include <string>
//

namespace affymetrix_calvin_io
{

typedef enum RWAccess { READ = 1, WRITE, READ_AND_WRITE };
typedef enum ShareMode { EXCLUSIVE, ALLOWREAD, ALLOWWRITE, ALLOWREAD_AND_WRITE };

/*! This class encapsulates Window memory-mapped file access methods. */
class MemMapFile
{
public:

	/*! Constructor */
	MemMapFile();

	/*! Destructor */
	~MemMapFile() { Close(); }

	/*! Sets the name of the file to access.
	 *	@param value Name of the file.
	 */
	void SetFilename(std::string value) { fileName = value; }

	/*! Get the name of the file.
	 *	@return Name of the file.
	 */
	std::string GetFilename() const { return fileName; }

	/*! A flag that indicates if the memory-mapped view is to be flushed after unmapping.
	 *	@param value True indicates to flush a file after closing.
	 */
	void SetFlushMMView(bool value) { flushMMView = value; }

	/*! Creates a file.
	 *	@param size Size, in bytes, of the file to create.
	 */
	bool Create(__int64 size);

	/*! Opens a file using memory-mapping.
	 *	@param rwAccess Type of file access desried; read, write or both.
	 *	@param shareMode Share mode.
	 *	@return Returns true if successful.
	 */
	bool Open(RWAccess rwAccess, ShareMode shareMode);

	/*! Maps a view of the file.
	 *	The data data can be accessed by calling GetDataPtr
	 *	@param start Start file position of the mapped view.
	 *	@param bytes The number of bytes to map into the view.
	 *	@return Returns true if successful.
	 */
	bool MapData(__int64 start, DWORD bytes);

	/*! Closes the mapped view and file access. */
	void Close();

	/*! Get the pointer to the mapped view.
	 *	@return Returns the address of the mapped view of the file.
	 */
	char* GetDataPtr() { return data; }

	/*! Get the file position of the first byte mapped into the view.
	 *	@return Returns the file position of the first byte mapped into the view.
	 */
	u_int32_t GetFirstMappedBytePos() const { return startMapPos; }

	/*! Get the number of bytes mapped into the current view.
	 *	@return Returns the number of bytes mapped into the current view.
	 */
	u_int32_t GetBytesMapped() const { return bytesMapped; }

	/*! Get the error message of the most recent error.
	 *	@return Returns the error message from the most recent error.
	 */
	std::wstring GetLastError() { return errorMsg; }

	/*! Checks if a view is mapped.
	 *	@return Returns true if a view is mapped.
	 */
	bool IsViewMapped() { return (mappedData != 0); }

protected:
	/*! Unmaps a view */
	void UnmapFile();

	/*! Gets the error message for the last error in a Window API call.
	 *	@return Returns the last Windows API error message.
	 */
	std::wstring GetErrorMsg();

protected:
	/*! Name of the file. */
	std::string fileName;
	/*! map mode */
	DWORD dwMapMode;
	/*! Handle returned by CreateFileMapping */
	HANDLE fileMapHandle;
	/*! Handle returned by CreateFile */
	HANDLE fileHandle;
	/*! pointer to the mapped data, doesn't account for allocation granularity. */
	void*	mappedData;
	/*! pointer to the mapped data, allocation granularity has been accounted for. */
	char*	data;
	/*! The file position of the first byte mapped into the view */
	u_int32_t startMapPos;
	/*! The number of bytes mapped into the view */
	u_int32_t bytesMapped;
	/*! Size of the file */
	__int64 fileSize;
	/*! Flag that indicates if the memory-mapped view is to be flushed after unmapping */
	bool flushMMView;
	/*! The number extra bytes mapped at the start of a view because of allocation granularity */
	u_int32_t offsetToStart;	// used by FlushViewOfFile
	/*! Error message */
	std::wstring errorMsg;
};

}

#endif	// _MemMapFile_HEADER_
