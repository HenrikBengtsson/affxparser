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

#include "calvin_files/parsers/src/FileInput.h"
//
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string.h>
#include <string>
//

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#else
#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>
#endif

// Some machines (sparc) dont support unaligned memory access
// The mmaped files are chock full of unaligned accesses.
// When AFFY_NOUNALIGNED_MEM is defined we will do the 
// alignment in software.
// This feature can be enabled on intel for testing.

// This is here to enable this feature automaticly on the sparc and ppc.
// they cant do unaligned loads.
#ifdef __sparc__
#define AFFY_UNALIGNED_IN_SW
#endif

using namespace affymetrix_calvin_io;


/*
 * Read an 8 bit number from the file.
 */
int8_t FileInput::ReadInt8(std::ifstream &instr)
{
	int8_t val = 0;
	instr.read((char *)&val, sizeof(val));
	return val;
}

/*
 * Read a 16 bit number from the file.
 */
int16_t FileInput::ReadInt16(std::ifstream &instr)
{
	int16_t val = 0;
	instr.read((char *)&val, sizeof(val));
	val = ntohs(val);
	return val;
}

/*
 * Read a 32 bit number from the file.
 */
int32_t FileInput::ReadInt32(std::ifstream &instr)
{
	int32_t val = 0;
	instr.read((char *)&val, sizeof(val));
	val = ntohl(val);
	return val;
}


/*
 * Read an 8 bit unsigned number from the file.
 */
u_int8_t FileInput::ReadUInt8(std::ifstream &instr)
{
	u_int8_t val = 0;
	instr.read((char *)&val, sizeof(val));
	return val;
}

/*
 * Read a 16 bit unsigned number from the file.
 */
u_int16_t FileInput::ReadUInt16(std::ifstream &instr)
{
	u_int16_t val = 0;
	instr.read((char *)&val, sizeof(val));
	val = ntohs(val);
	return val;
}

/*
 * Read a 32 bit unsigned number from the file.
 */
u_int32_t FileInput::ReadUInt32(std::ifstream &instr)
{
	u_int32_t val = 0;
	instr.read((char *)&val, sizeof(val));
	val = ntohl(val);
	return val;
}

/*
 * Read a 32 bit floating point value from a file.
 */
float FileInput::ReadFloat(std::ifstream &instr)
{
	type_punned pun;
	pun.v_uint32=ReadInt32(instr);
	return pun.v_float;
}

/*
 * Read a string from the file stream.
 */
std::string FileInput::ReadString8(std::ifstream &instr)
{
	int32_t len = FileInput::ReadInt32(instr);
	return FileInput::ReadString8(instr, len);
}

/*
 * Read a string from the file stream.
 */
std::string FileInput::ReadString8(std::ifstream &instr, int32_t len)
{
	char *buf = new char [len+1];
	instr.read(buf, len);
	buf[len]=0;
	std::string s=buf;
	delete[] buf;
	return s;
}

/*
 * Read the length (integer) then the string from the file.
 */
std::wstring FileInput::ReadString16(std::ifstream &instr)
{
	int32_t len = FileInput::ReadInt32(instr);
	return FileInput::ReadString16(instr, len);
}

/*
 * Read a string from the file.
 */
std::wstring FileInput::ReadString16(std::ifstream &instr, int32_t len)
{
	u_int16_t cvalue;
	wchar_t* s = new wchar_t[len+1];
	s[len] = 0;
	for (int i=0; i<len; i++)
	{
		instr.read((char *)&cvalue, sizeof(int16_t));
		cvalue = ntohs(cvalue);
		s[i] = cvalue;
	}
	std::wstring result = s;	// assign wchar_t array to wstring because it will trunc string len to first 0 which may not be the same as len.
	delete [] s;
	return result;
}

/*
 * Reads a blob from a file.
 */
int32_t FileInput::ReadBlob(std::ifstream &instr, const void*& value)
{
	int32_t size = FileInput::ReadInt32(instr);
	char *buf = new char[size];
	instr.read(buf, size);
	value = buf;
	return size;
}

/*
 * Read an 8 bit number from a memory stream.
 */
int8_t FileInput::ReadInt8(char * &instr)
{
	int8_t val = *(int8_t *)instr;
	instr += sizeof(int8_t);
	return val;
}

/*
 * Read an 8 bit unsigned number from a memory stream.
 */
u_int8_t FileInput::ReadUInt8(char * &instr)
{
	u_int8_t val = *(u_int8_t *)instr;
	instr += sizeof(int8_t);
	return val;
}

/*
 * Read a 32 bit floating point value from a memory stream.
 */
float FileInput::ReadFloat(char * &instr)
{
	float fval;
	int32_t ival = ReadInt32(instr);
	memcpy(&fval, &ival, sizeof(ival));
	return fval;
}

#ifndef AFFY_UNALIGNED_IN_SW  // If unaligned accesses to memory are allowed

/*
 * Read a 16 bit number from a memory stream.
 */
int16_t FileInput::ReadInt16(char * &instr)
{
	int16_t val = *(int16_t *)instr;
	val = ntohs(val);
	instr += sizeof(int16_t);
	return val;
}

/*
 * Read a 32 bit number from a memory stream.
 */
int32_t FileInput::ReadInt32(char * &instr)
{
	int32_t val = *(int32_t *)instr;
	val = ntohl(val);
	instr += sizeof(int32_t);
	return val;
}

/*
 * Read a 16 bit unsigned number from a memory stream.
 */
u_int16_t FileInput::ReadUInt16(char * &instr)
{
	u_int16_t val = *(u_int16_t *)instr;
	val = ntohs(val);
	instr += sizeof(int16_t);
	return val;
}

/*
 * Read a 32 bit unsigned number from a memory stream.
 */
u_int32_t FileInput::ReadUInt32(char * &instr)
{
	u_int32_t val = *(u_int32_t *)instr;
	val = ntohl(val);
	instr += sizeof(int32_t);
	return val;
}

/*
 * Read a string from the file stream.
 */
std::wstring FileInput::ReadString16(char * &instr, int32_t len)
{
	u_int16_t *cvalue = (u_int16_t *)instr;
	wchar_t* s = new wchar_t[len+1];
	s[len] = 0;
	for (int i=0; i<len; i++)
	{
		s[i] = (wchar_t) ntohs(cvalue[i]);
	}
	instr += (sizeof(u_int16_t)*len);
	std::wstring result = s;	// assign wchar_t array to wstring because it will trunc string len to first 0 which may not be the same as len.
	delete [] s;
	return result;
}

#else // AFFY_UNALIGNED_IN_SW - We dont have unaligned access to memory

/*
 * Read a 16 bit number from a memory stream.
 */
int16_t FileInput::ReadInt16(char * &instr)
{
	u_int8_t *ptr = (u_int8_t *)instr;
	int16_t val=0;
	val|=(*ptr++)<< 8;
	val|=(*ptr++);
	instr += sizeof(int16_t);
	return val;
}

/*
 * Read a 32 bit number from a memory stream.
 */
int32_t FileInput::ReadInt32(char * &instr)
{
	u_int8_t *ptr = (u_int8_t *)instr;
	int32_t val=0;
	val|=(*ptr++)<<24;
	val|=(*ptr++)<<16;
	val|=(*ptr++)<< 8;
	val|=(*ptr++);
	instr += sizeof(int32_t);
	return val;
}

/*
 * Read a 16 bit unsigned number from a memory stream.
 */
u_int16_t FileInput::ReadUInt16(char * &instr)
{
	u_int8_t *ptr = (u_int8_t *)instr;
	u_int16_t val=0;
	val|=(*ptr++)<< 8;
	val|=(*ptr++);
	instr += sizeof(u_int16_t);
	return val;
}

/*
 * Read a 32 bit unsigned number from a memory stream.
 */
u_int32_t FileInput::ReadUInt32(char * &instr)
{
	u_int8_t *ptr = (u_int8_t *)instr;
	u_int32_t val=0;
	val|=(*ptr++)<<24;
	val|=(*ptr++)<<16;
	val|=(*ptr++)<< 8;
	val|=(*ptr++);
	instr += sizeof(u_int32_t);
	return val;
}

/*
 * Read a string from the file stream.
 */
std::wstring FileInput::ReadString16(char * &instr, int32_t len)
{
	u_int8_t *ptr = (u_int8_t *)instr;
	wchar_t* s = new wchar_t[len+1];
	s[len] = 0;
	for (int i=0; i<len; i++)
	{
		s[i] = 0;
		s[i]|=(*ptr++)<< 8;
		s[i]|=(*ptr++);
	}
	instr += (sizeof(u_int16_t)*len);
	std::wstring result = s;	// assign wchar_t array to wstring because it will trunc string len to first 0 which may not be the same as len.
	delete [] s;
	return result;
}

#endif

/*
 * Read the length (integer) then the string.
 */
std::string FileInput::ReadString8(char * &instr)
{
	int32_t len = FileInput::ReadInt32(instr);
	return FileInput::ReadString8(instr, len);
}

/*
 * Read a string from the file stream.
 */
std::string FileInput::ReadString8(char * &instr, int32_t len)
{
	char *buf = new char [len+1];
	memcpy(buf, instr, len);
	buf[len]=0;
	std::string s=buf;
	delete[] buf;
	instr += len;
	return s;
}

/*
 * Read the length (integer) then the string from the file stream.
 */
std::wstring FileInput::ReadString16(char * &instr)
{
	int32_t len = FileInput::ReadInt32(instr);
	return FileInput::ReadString16(instr, len);
}
