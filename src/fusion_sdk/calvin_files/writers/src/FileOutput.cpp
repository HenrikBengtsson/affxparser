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
//
#include "calvin_files/writers/src/FileOutput.h"
//
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string.h>
#include <string>
//

#ifndef _MSC_VER
#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>
#endif

using namespace affymetrix_calvin_io;

/*
 * Write an 8 bit number to the file.
 */
void FileOutput::WriteInt8(std::ofstream &outstr, int8_t value)
{
	outstr.write((char *)&value, sizeof(value));
}

/*
 * Write a 16 bit number to the file.
 */
void FileOutput::WriteInt16(std::ofstream &outstr, int16_t value)
{
	value = htons(value);
	outstr.write((char *)&value, sizeof(value));
}

/*
 * Write a 32 bit number to the file.
 */
void FileOutput::WriteInt32(std::ofstream &outstr, int32_t value)
{
	value = htonl(value);
	outstr.write((char *)&value, sizeof(value));
}

/*
 * Write an 8 bit unsigned number to the file.
 */
void FileOutput::WriteUInt8(std::ofstream &outstr, u_int8_t value)
{
	outstr.write((char *)&value, sizeof(value));
}

/*
 * Write a 16 bit unsigned number to the file.
 */
void FileOutput::WriteUInt16(std::ofstream &outstr, u_int16_t value)
{
	value = htons(value);
	outstr.write((char *)&value, sizeof(value));
}

/*
 * Write a 32 bit unsigned number to the file.
 */
void FileOutput::WriteUInt32(std::ofstream &outstr, u_int32_t value)
{
	value = htonl(value);
	outstr.write((char *)&value, sizeof(value));
}

/*
 * Write a 32 bit floating point value from a file.
 */
void FileOutput::WriteFloat(std::ofstream &outstr, float value)
{
	type_punned pun;
	pun.v_float=value;
	WriteInt32(outstr,pun.v_int32);
}

/*
 * Write a string to the file stream.
 */
void FileOutput::WriteString8(std::ofstream &outstr, const std::string &value)
{
	int32_t len = (int32_t) value.length();
	FileOutput::WriteInt32(outstr, len);
	FileOutput::WriteString8(outstr, value.c_str(), len);
}

/*
 * Write a string to the file stream. Pad with nulls.
 */
void FileOutput::WriteString8(std::ofstream &outstr, const char *value, int32_t len)
{
  // how much padding will we need?
  int pad_len;
  int str_len;

  if (value==NULL) {
    pad_len=len;
  }
  else {
    str_len=strlen(value);
    if (len<str_len) {
      outstr.write(value,len);
      return;
    }
    outstr.write(value,str_len);
    pad_len=len-str_len;
  }
  // padding
  while (pad_len!=0) {
    outstr.put(0);
    pad_len--;
  }
}

/*
 * Write the length (integer) then the string to the file.
 */
void FileOutput::WriteString16(std::ofstream &outstr, const std::wstring &value)
{
	int32_t len = (int32_t) value.length();
	FileOutput::WriteInt32(outstr, len);
	FileOutput::WriteString16(outstr, value.c_str(), len);
}

/*
 * Write a string to the file stream.
 */
void FileOutput::WriteString16(std::ofstream &outstr, const std::wstring &value, int32_t maxLn)
{
    int32_t len = (int32_t) value.length();
    if(len <= maxLn)
    {
		u_int16_t cvalue;
		const wchar_t *str = value.c_str();
		FileOutput::WriteInt32(outstr, maxLn);
		for (int i=0; i<len; i++)
		{
			cvalue = (u_int16_t) str[i];
			cvalue = htons(cvalue);
			outstr.write((char *)&cvalue, sizeof(u_int16_t));
		}
        int32_t bufLn = maxLn - len;
        for(int i = 0; i < bufLn; i++)
        {
            WriteString16(outstr, L"\0", 1);
        }
    }
}

/*
 * Write a string to the file.
 */
void FileOutput::WriteString16(std::ofstream &outstr, const wchar_t *value, int32_t len)
{
	u_int16_t cvalue;
	for (int i=0; i<len; i++)
	{
		cvalue = (u_int16_t) value[i];
		cvalue = htons(cvalue);
		outstr.write((char *)&cvalue, sizeof(u_int16_t));
	}
}

/*
 * Write a string to the file stream.
 */
void FileOutput::WriteString8(std::ofstream &outstr, const std::string &value, int32_t maxLn)
{
    int32_t len = (int32_t) value.length();
    if(len <= maxLn)
    {
	    WriteString8(outstr, value);
        int32_t bufLn = maxLn - len;
        for(int i = 0; i < bufLn; i++)
        {
            WriteString8(outstr, "\0", 1);
        }
    }
}

/*
 * Write a blob to the file stream.
 */
void FileOutput::WriteBlob(std::ofstream &outstr, const void* value, int32_t size)
{
	FileOutput::WriteInt32(outstr, size);
	outstr.write((const char*)value, size);
}

/*
 * Write a blob to the file stream with reserved space
 */
void FileOutput::WriteBlob(std::ofstream &outstr, const void* value, int32_t data_size, int32_t reserved_size)
{
	FileOutput::WriteInt32(outstr, reserved_size);
	//write the new value
	outstr.write((const char*)value, data_size);
	//fill in the reserved space
	for(int i=0;i<reserved_size-data_size;i++)
	{
		outstr.put('\0');
	}
}
