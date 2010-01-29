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

#ifndef AFFX_FILEWRITER_H
#define AFFX_FILEWRITER_H

/*! \file FileWriter.h This file provides file level writing capaibilities.
 */

#include "file/FileIO.h"
//
#include "portability/affy-base-types.h"
//
#include <cstring>
#include <fstream>
#include <string>
//

#ifndef OFSTREAM
/*! The output stream object */
#define OFSTREAM std::ofstream
#endif

/*! Writes an unsigned 32 bit int to a little endian file
 * @param outstr The output file stream
 * @param val The value to write
 */
void WriteUInt32_I(OFSTREAM& outstr,uint32_t val);

/*! Writes a signed 32 bit int to a little endian file
 * @param outstr The output file stream
 * @param val The value to write
 */
void WriteInt32_I(OFSTREAM& outstr,int32_t val);

/*! Writes an unsigned 16 bit int to a little endian file
 * @param outstr The output file stream
 * @param val The value to write
 */
void WriteUInt16_I(OFSTREAM& outstr,uint16_t val);

/*! Writes a signed 16 bit int to a little endian file
 * @param outstr The output file stream
 * @param val The value to write
 */
void WriteInt16_I(OFSTREAM& outstr,int16_t val);

/*! Writes an unsigned 32 bit int to a big endian file
 * @param outstr The output file stream
 * @param val The value to write
 */
void WriteUInt32_N(OFSTREAM& outstr,uint32_t val);

/*! Writes a signed 32 bit int to a big endian file
 * @param outstr The output file stream
 * @param val The value to write
 */
void WriteInt32_N(OFSTREAM& outstr,int32_t val);

/*! Writes an unsigned 16 bit int to a big endian file
 * @param outstr The output file stream
 * @param val The value to write
 */
void WriteUInt16_N(OFSTREAM& outstr,uint16_t val);

/*! Writes a signed 16 bit int to a big endian file
 * @param outstr The output file stream
 * @param val The value to write
 */
void WriteInt16_N(OFSTREAM& outstr,int16_t val);

/*! Writes an unsigned 8 bit int to a file
 * @param outstr The output file stream
 * @param val The value to write
 */
void WriteUInt8(OFSTREAM& outstr,uint8_t val);

/*! Writes a signed 8 bit int to a file
 * @param outstr The output file stream
 * @param val The value to write
 */
void WriteInt8(OFSTREAM& outstr,int8_t val);

/*! Writes a float int to a big endian file
 * @param outstr The output file stream
 * @param val The value to write
 */
void WriteFloat_N(OFSTREAM& outstr,float val);

/*! Writes a float int to a little endian file
 * @param outstr The output file stream
 * @param val The value to write
 */
void WriteFloat_I(OFSTREAM& outstr,float val);

/*! Writes a float to a little endian file with just 1 digit of precision.
 * @param outstr The output file stream
 * @param val The value to write
 */
void WriteFloatLowPrecision(OFSTREAM& outstr,float val);

/*! Writes a string to the output file
 * @param outstr The output file stream.
 * @param str The string value to write
 * @param len The length of the string
 */
void WriteFixedCString(OFSTREAM& outstr,const char* str,size_t len);

/*! Writes a string to a little endian binary file
 * @param outstr The output file stream.
 * @param str The string value to write
 */
void WriteCString(OFSTREAM& outstr,const char* str);
void WriteCString(OFSTREAM& outstr,std::string str);

/*! Writes a fixed number of bytes of a string to the output file
 * Pads with nulls if the string is shorter than bytes to write
 * @param outstr The output file stream.
 * @param str The string value to write
 * @param len The length of the string
 */
void WriteFixedString(OFSTREAM& outstr,const std::string str,size_t len);

/*! Writes a string to a little endian binary file
 * Pads with nulls if the string is shorter than bytes to write
 * @param outstr The output file stream.
 * @param str The string value to write
 */
void WriteString_I(OFSTREAM& outstr,const std::string str);

/*! Writes a string to a big endian binary file
 * @param outstr The output file stream.
 * @param str The string value to write
 */
void WriteString_N(OFSTREAM& outstr,const std::string str);


/*! Writes packed character array to the output file
 * @param outstr The output file stream.
 * @param str The string value to write
 * @param len The length of the string
 */
void WriteCharacterArray(OFSTREAM& outstr,char* str,size_t len);

#ifdef FILEWRITER_USE_OLD_AND_BAD_FUNCTIONS

/*! Writes a long to a big endian file - for backwards compability only. Use the newer functions instead */
#define WriteLong(s,v)    WriteInt32_N(s,v)

/*! Writes an unsigned long to a big endian file - for backwards compability only. Use the newer functions instead */
#define WriteULong(s,v)   WriteUInt32_N(s,v)

/*! Writes an unsigned char to a file - for backwards compability only. Use the newer functions instead */
#define WriteUChar(s,v)   WriteUInt8(s,v)

/*! Writes a short to a big endian file - for backwards compability only. Use the newer functions instead */
#define WriteShort(s,v)   WriteInt16_N(s,v)

/*! Writes an unsigned short to a big endian file - for backwards compability only. Use the newer functions instead */
#define WriteUShort(s,v)  WriteUInt16_N(s,v)

/*! Writes a float to a big endian file - for backwards compability only. Use the newer functions instead */
#define WriteFloat(s,v)   WriteFloat_N(s,v)

/*! Writes an int to a little big file - for backwards compability only. Use the newer functions instead */
#define WriteInt(s,v)                 WriteInt32_N(s,v)

/*! Writes an int to a little endian file - for backwards compability only. Use the newer functions instead */
#define WRITE_INT(s,v)                 WriteInt32_I(s,v)

/*! Writes an unsigned int to a little endian file - for backwards compability only. Use the newer functions instead */
#define WRITE_UINT(s,v)                WriteUInt32_I(s,v)

/*! Writes a long to a little endian file - for backwards compability only. Use the newer functions instead */
#define WRITE_LONG(s,v)                WriteInt32_I(s,v)

/*! Writes an unsigned long to a little endian file - for backwards compability only. Use the newer functions instead */
#define WRITE_ULONG(s,v)               WriteUInt32_I(s,v)

/*! Writes a short to a little endian file - for backwards compability only. Use the newer functions instead */
#define WRITE_SHORT(s,v)               WriteInt16_I(s,v)

/*! Writes an int to a little endian file - for backwards compability only. Use the newer functions instead */
#define WRITE_USHORT(s,v)              WriteUInt16_I(s,v)

/*! Writes a boolean to a little endian file - for backwards compability only. Use the newer functions instead */
#define WRITE_BOOL(s,v)                WriteInt8_I(s,v)

/*! Writes a char to a file - for backwards compability only. Use the newer functions instead */
#define WRITE_CHAR(s,v)                WriteInt8(s,v)

/*! Writes an unsigned char to a file - for backwards compability only. Use the newer functions instead */
#define WRITE_UCHAR(s,v)               WriteUInt8(s,v)

/*! Writes a float to a little endian file - for backwards compability only. Use the newer functions instead */
#define WRITE_FLOAT(s,v)               WriteFloat_I(s,v)

/*! Writes an low precision float to a little endian file - for backwards compability only. Use the newer functions instead */
#define WRITE_LOW_PRECISION_FLOAT(s,v) WriteFloatLowPrecision(s,v)

/*! Writes a string to a little endian file - for backwards compability only. Use the newer functions instead */
#define WRITE_STRING(s,v)              WriteCString(s,v.c_str())

#endif // old and bad

#endif // AFFY_FILEWRITER_H
