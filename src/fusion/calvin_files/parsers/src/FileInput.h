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


#ifndef _FileInput_HEADER_
#define _FileInput_HEADER_

/*! \file FileInput.h This file provides functions for reading atom data items from a file or stream.
 */

#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//
#include <cstring>
#include <fstream>
#include <string>
//

namespace affymetrix_calvin_io
{

class FileInput
{
public:
	/*! Reads an 8 bit integer from a big endian file.
	*
	* @param instr The input file stream.
	* @return The integer read from the file.
	*/
	static int8_t ReadInt8(std::ifstream &instr);

	/*! Reads a 16 bit integer from a big endian file.
	*
	* @param instr The input file stream.
	* @return The integer read from the file.
	*/
	static int16_t ReadInt16(std::ifstream &instr);

	/*! Reads a 32 bit integer from a big endian file.
	*
	* @param instr The input file stream.
	* @return The integer read from the file.
	*/
	static int32_t ReadInt32(std::ifstream &instr);

	/*! Reads an 8 bit unsigned integer from a big endian file.
	*
	* @param instr The input file stream.
	* @return The integer read from the file.
	*/
	static u_int8_t ReadUInt8(std::ifstream &instr);

	/*! Reads a 16 bit unsigned integer from a big endian file.
	*
	* @param instr The input file stream.
	* @return The integer read from the file.
	*/
	static u_int16_t ReadUInt16(std::ifstream &instr);

	/*! Reads a 32 bit unsigned integer from a big endian file.
	*
	* @param instr The input file stream.
	* @return The integer read from the file.
	*/
	static u_int32_t ReadUInt32(std::ifstream &instr);

	/*! Reads a 32 bit floating point number from a big endian file.
	*
	* @param instr The input file stream.
	* @return The floating point number read from the file.
	*/
	static float ReadFloat(std::ifstream &instr);

	/*! Reads an 8 bit integer from a big endian file stream (memory map pointer).
	*
	* @param instr The input file stream.
	* @return The integer read from the file stream.
	*/
	static int8_t ReadInt8(char * &instr);

	/*! Reads a 16 bit integer from a big endian file stream (memory map pointer).
	*
	* @param instr The input file stream.
	* @return The integer read from the file stream.
	*/
	static int16_t ReadInt16(char * &instr);

	/*! Reads a 32 bit integer from a big endian file stream (memory map pointer).
	*
	* @param instr The input file stream.
	* @return The integer read from the file stream.
	*/
	static int32_t ReadInt32(char * &instr);

	/*! Reads an 8 bit unsigned integer from a big endian file stream (memory map pointer).
	*
	* @param instr The input file stream.
	* @return The integer read from the file stream.
	*/
	static u_int8_t ReadUInt8(char * &instr);

	/*! Reads a 16 bit unsigned integer from a big endian file stream (memory map pointer).
	*
	* @param instr The input file stream.
	* @return The integer read from the file stream.
	*/
	static u_int16_t ReadUInt16(char * &instr);

	/*! Reads a 32 bit unsigned integer from a big endian file stream (memory map pointer).
	*
	* @param instr The input file stream.
	* @return The integer read from the file stream.
	*/
	static u_int32_t ReadUInt32(char * &instr);

	/*! Reads a 32 bit floating point number from a big endian file stream (memory map pointer).
	*
	* @param instr The input file stream.
	* @return The floating point number read from the file stream.
	*/
	static float ReadFloat(char * &instr);

	/*! Reads a 16 bit unicode string of fixed size from a big endian file.
	*
	* @param instr The input file stream.
	* @param len The length of the string.
	* @return The string read from the file stream.
	*/
	static std::wstring ReadString16(std::ifstream &instr, int32_t len);

	/*! Reads a 16 bit unicode string from a big endian file.
	*
	* @param instr The input file stream.
	* @return The string read from the file stream.
	*/
	static std::wstring ReadString16(std::ifstream &instr);

	/*! Reads an 8 bit string of fixed size from a big endian file.
	*
	* @param instr The input file stream.
	* @param len The length of the string.
	* @return The string read from the file stream.
	*/
	static std::string ReadString8(std::ifstream &instr, int32_t len);

	/*! Reads an 8 bit string from a big endian file.
	*
	* @param instr The input file stream.
	* @return The string read from the file stream.
	*/
	static std::string ReadString8(std::ifstream &instr);

	/*! Reads a 16 bit unicode string of fixed size from a big endian file stream (memory map pointer).
	*
	* @param instr The input file stream.
	* @param len The length of the string.
	* @return The string read from the file stream.
	*/
	static std::wstring ReadString16(char * &instr, int32_t len);

	/*! Reads a 16 bit unicode string from a big endian file stream (memory map pointer).
	*
	* @param instr The input file stream.
	* @return The string read from the file stream.
	*/
	static std::wstring ReadString16(char * &instr);

	/*! Reads an 8 bit string of fixed size from a big endian file stream (memory map pointer).
	*
	* @param instr The input file stream.
	* @param len The length of the string.
	* @return The string read from the file stream.
	*/
	static std::string ReadString8(char * &instr, int32_t len);

	/*! Reads an 8 bit string from a big endian file stream (memory map pointer).
	*
	* @param instr The input file stream.
	* @return The string read from the file stream.
	*/
	static std::string ReadString8(char * &instr);

	/*! Reads a blob from a file (as is; no htonl).  It is the responsibility of the caller to
	* convert elements of the blob to system types.
	* @param instr The input file stream.
	* @param value A returned pointer to the blob.  The caller must delete[] the returned pointer.
	* @return The size of the blob read from the file.
	*/
	static int32_t ReadBlob(std::ifstream &instr, const void*& value);
};

}

#endif // _FileInput_HEADER_
