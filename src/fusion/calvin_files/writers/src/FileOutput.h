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


#ifndef _FileOutput_HEADER_
#define _FileOutput_HEADER_

/*! \file FileOutput.h This file provides functions for writing atomic data items to a file.
 */

#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//
#include <cstring>
#include <fstream>
#include <string>
//

namespace affymetrix_calvin_io
{

class FileOutput
{
public:
	/*! Writes an 8 bit integer from a big endian file.
	*
	* @param outstr The output file stream.
	* @param value The value to write to the file.
	*/
	static void WriteInt8(std::ofstream &outstr, int8_t value);

	/*! Writes a 16 bit integer from a big endian file.
	*
	* @param outstr The output file stream.
	* @param value The value to write to the file.
	*/
	static void WriteInt16(std::ofstream &outstr, int16_t value);

	/*! Writes a 32 bit integer from a big endian file.
	*
	* @param outstr The output file stream.
	* @param value The value to write to the file.
	*/
	static void WriteInt32(std::ofstream &outstr, int32_t value);

	/*! Writes an 8 bit unsigned integer from a big endian file.
	*
	* @param outstr The output file stream.
	* @param value The value to write to the file.
	*/
	static void WriteUInt8(std::ofstream &outstr, u_int8_t value);

	/*! Writes a 16 bit unsigned integer from a big endian file.
	*
	* @param outstr The output file stream.
	* @param value The value to write to the file.
	*/
	static void WriteUInt16(std::ofstream &outstr, u_int16_t value);

	/*! Writes a 32 bit unsigned integer from a big endian file.
	*
	* @param outstr The output file stream.
	* @param value The value to write to the file.
	*/
	static void WriteUInt32(std::ofstream &outstr, u_int32_t value);

	/*! Writes a 32 bit floating point number from a big endian file.
	*
	* @param outstr The output file stream.
	* @param value The value to write to the file.
	*/
	static void WriteFloat(std::ofstream &outstr, float value);

	/*! Writes a 16 bit unicode string of fixed size from a big endian file.
	*
	* @param outstr The output file stream.
	* @param value The value to write to the file.
	* @param len The length of the string.
	*/
	static void WriteString16(std::ofstream &outstr, const wchar_t *value, int32_t len);

	/*! Writes a 16 bit unicode string from a big endian file.
	*
	* @param outstr The output file stream.
	* @param value The value to write to the file.
	*/
	static void WriteString16(std::ofstream &outstr, const std::wstring &value);

	/*! Writes an 8 bit string of fixed size from a big endian file.
	*
	* @param outstr The output file stream.
	* @param value The value to write to the file.
	* @param len The length of the string.
	*/
	static void WriteString8(std::ofstream &outstr, const char *value, int32_t len);

	/*! Writes an 8 bit string from a big endian file.
	*
	* @param outstr The output file stream.
	* @param value The value to write to the file.
	*/
	static void WriteString8(std::ofstream &outstr, const std::string &value);

    /*! Writes an 8 bit string from a big endian file with a fixed maximum size.  
    *   If the string length is less than maxLn then null's are appended.
	*
	* @param outstr The output file stream.
	* @param value The value to write to the file.
    * @param maxLn Maximum length of the string.
	*/
    static void WriteString8(std::ofstream &outstr, const std::string &value, int32_t maxLn);

    /*! Writes an 16 bit string from a big endian file with a fixed maximum size.  
    *   If the string length is less than maxLn then null's are appended.
	*
	* @param outstr The output file stream.
	* @param value The value to write to the file.
    * @param maxLn Maximum length of the string.
	*/
    static void WriteString16(std::ofstream &outstr, const std::wstring &value, int32_t maxLn);

	/*! Writes a blob to a file (as is; no htonl).  It is the responsibility of the caller to ensure
	*	the blob is in a portable form.
	* @param outstr The output file stream.
	* @param value A pointer to the blob.
	* @param size The size of the blob.
	*/
	static void WriteBlob(std::ofstream &outstr, const void* value, int32_t size);

	/*! Writes a blob to a file with reserved space(as is; no htonl).  It is the responsibility of the caller to ensure
	*	the blob is in a portable form.
	* @param outstr The output file stream.
	* @param value A pointer to the blob.
	* @param data_size The size of the blob.
	* @param reserved_size The size of the reserved space (total size including the blob size).
	*/
	static void WriteBlob(std::ofstream &outstr, const void* value, int32_t data_size, int32_t reserved_size);
};

}

#endif // _FileOutput_HEADER_
