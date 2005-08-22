/////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License,
// or (at your option) any later version.
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
/////////////////////////////////////////////////////////////////

#ifndef AFFX_FILEIO_H
#define AFFX_FILEIO_H 1

/*! \file FileIO.h This file provides file level reading capaibilities.
 */

//
#include <fstream>
#include <string>
//
#include "affy-base-types.h"

// Affy uses little-endian byte order for some of their files.
// This requires defining a the complements to the normal functions "hton".

// What kind of machine?
//   (The tests are written out like this as I was told "||" 
//    causes problems on windows.)
// touch foobarbaz.c ; gcc -E -dD foobarbaz.c
// check for these archs
#ifdef __sparc__ 
#define IS_BIG_ENDIAN 1
#endif
#ifdef __PPC__ // on linux
#define IS_BIG_ENDIAN 1
#endif
#ifdef __POWERPC__ // on darwin
#define IS_BIG_ENDIAN 1
#endif
#ifdef __BIG_ENDIAN__ // we are being told
#undef  IS_BIG_ENDIAN // avoid the warning
#define IS_BIG_ENDIAN 1
#endif


/*! Shuffle operator for 32 bit values
 * @param x The value to shuffle
 * @return The shuffled value
 */
#define affy_swap32(x) \
     ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) | \
      (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))

/*! Shuffle operator for 16 bit values
 * @param x The value to shuffle
 * @return The shuffled value
 */
#define affy_swap16(x) \
     ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8))


// Do we need to define our "to little-endian" operators?
#ifdef IS_BIG_ENDIAN

/*! Changes the 32 bit bit data to little endian order */
#define htoil(x) affy_swap32(x)

/*! Changes the 16 bit data to little endian order */
#define htois(x) affy_swap16(x)

/*! Changes the 32 bit data little endian to host order */
#define itohl(x) affy_swap32(x)

/*! Changes the 16 bit little endian data to host order */
#define itohs(x) affy_swap16(x)

#else // the machine is little-endian

/*! Identity operators */
#define htoil(x) (x)

/*! Identity operators */
#define htois(x) (x)

/*! Identity operators */
#define itohl(x) (x)

/*! Identity operators */
#define itohs(x) (x)

#endif

#ifndef IFSTREAM
/*! Input STL stream object */
#define IFSTREAM std::istream
#endif 

/*! Reads an unsigned 32 bit integer from a file
 * @param instr The input file stream
 * @param val The returned value
 */
void ReadUInt32_I(IFSTREAM& instr,uint32_t& val);

/*! Reads a signed 32 bit integer from a file
 * @param instr The input file stream
 * @param val The returned value
 */
void ReadInt32_I(IFSTREAM&  instr,int32_t& val);

/*! Reads a float from a file
 * @param instr The input file stream
 * @param val The returned value
 */
void ReadFloat_I(IFSTREAM& instr,float& val);

/*! Reads an unsigned 16 bit integer from a file
 * @param instr The input file stream
 * @param val The returned value
 */
void ReadUInt16_I(IFSTREAM& instr,uint16_t& val);

/*! Reads a signed 16 bit integer from a file
 * @param instr The input file stream
 * @param val The returned value
 */
void ReadInt16_I(IFSTREAM& instr,int16_t& val);

/*! Reads a string from a little endian file
 * @param instr The input file stream
 * @param str The returned value
 */
void ReadCString_I(IFSTREAM& instr, char*& str);

/*! Reads a string from a little endian file
 * @param instr The input file stream
 * @param str The returned value
 */
void ReadString_I(IFSTREAM& instr, std::string& str);

/*! Reads a string frome a little endian file where the string length is stored as an unsigned integer.
 * @param instr The input file stream
 * @param s The returned value
 */
void ReadUIntLenString_I(IFSTREAM &instr, std::string &s);

/*! Reads an unsigned 8 bit value from a file
 * @param instr The input file stream
 * @param val The returned value
 */
void ReadUInt8(IFSTREAM& instr,uint8_t& val);

/*! Reads an 8 bit value from a file
 * @param instr The input file stream
 * @param val The returned value
 */
void ReadInt8(IFSTREAM& instr,int8_t& val);

/*! Reads an unsigned 32 bit value from a big endian file
 * @param instr The input file stream
 * @param val The returned value
 */
void ReadUInt32_N(IFSTREAM& instr,uint32_t& val);

/*! Reads a signed 32 bit value from a big endian file
 * @param instr The input file stream
 * @param val The returned value
 */
void ReadInt32_N(IFSTREAM&  instr,int32_t& val);

/*! Reads a float value from a big endian file
 * @param instr The input file stream
 * @param val The returned value
 */
void ReadFloat_N(IFSTREAM& instr,float& val);

/*! Reads an unsigned 16 bit value from a big endian file
 * @param instr The input file stream
 * @param val The returned value
 */
void ReadUInt16_N(IFSTREAM& instr,uint16_t& val);

/*! Reads a signed 16 bit value from a big endian file
 * @param instr The input file stream
 * @param val The returned value
 */
void ReadInt16_N(IFSTREAM& instr,int16_t& val);

/*! Reads a string from a big endian file
 * @param instr The input file stream
 * @param str The returned value
 */
void ReadCString_N(IFSTREAM& instr, char*& str);

/*! Reads a string from a big endian file
 * @param instr The input file stream
 * @param str The returned value
 */
void ReadString_N(IFSTREAM& instr, std::string& str);

/*! Reads a string frome a big endian file where the string length is stored as an unsigned integer.
 * @param instr The input file stream
 * @param s The returned value
 */
void ReadUIntLenString_N(IFSTREAM &instr, std::string &s);

/*! Reads a fixed length string from a file.
 * @param instr The input file stream
 * @param str The returned string value.
 * @param len The length of the string to read
 */
void ReadFixedString(IFSTREAM& instr, std::string& str, uint32_t len);

/*! Reads a fixed length string from a file.
 * @param instr The input file stream
 * @param str The returned string value.
 * @param len The length of the string to read
 */
void ReadFixedCString(IFSTREAM& instr, char* str, uint32_t len);

/*! Reads a fixed length string from a file.
 * @param instr The input file stream
 * @param str The returned string value.
 * @param len The length of the string to read.
 */
void ReadFixedUCString(IFSTREAM& instr, unsigned char* str, uint32_t len);

/*! Reads a fixed length array of characters from a file.
 * @param instr The input file stream
 * @param str The returned string value.
 * @param len The length of the string to read
 */
void ReadCharacterArray(IFSTREAM& instr, char* str, uint32_t len);

/*! Reads the next line from a text file.
 * @param instr The input file stream
 * @param line The returned string (the data must already be allocated)
 * @param len The maximum length of the line to read.
 */
void ReadNextLine(IFSTREAM& instr,char* line,int len);

/*! This function is for older BPMAP file with incorrectly written float's
 * @param instr The input file stream.
 * @param fval The returned float
 */
void ReadFloatFromOldBPMAP_N(IFSTREAM &instr, float &fval);

/*! Gets a 32 bit unsigned int from a little endian data stream.
 * @param ptr A pointer to a little endian data stream
 * @return The host ordered value
 */
uint32_t MmGetUInt32_I(uint32_t* ptr);

/*! Gets a 32 bit signed int from a little endian data stream.
 * @param ptr A pointer to a little endian data stream
 * @return The host ordered value
 */
int32_t  MmGetInt32_I(int32_t* ptr);

/*! Gets a 16 bit unsigned int from a little endian data stream.
 * @param ptr A pointer to a little endian data stream
 * @return The host ordered value
 */
uint16_t MmGetUInt16_I(uint16_t* ptr);

/*! Gets a 16 bit signed int from a little endian data stream.
 * @param ptr A pointer to a little endian data stream
 * @return The host ordered value
 */
int16_t  MmGetInt16_I(int16_t* ptr);

/*! Gets a 8 bit unsigned int from a data stream.
 * @param ptr A pointer to the data stream
 * @return The host ordered value
 */
uint8_t  MmGetUInt8(uint8_t*   ptr);

/*! Gets a 8 bit signed int from a data stream.
 * @param ptr A pointer to the data stream
 * @return The host ordered value
 */
int8_t   MmGetInt8(int8_t*   ptr);

/*! Gets a 32 bit unsigned int from a big endian data stream.
 * @param ptr A pointer to a big endian data stream
 * @return The host ordered value
 */
uint32_t MmGetUInt32_N(uint32_t* ptr);

/*! Gets a 16 bit unsigned int from a big endian data stream.
 * @param ptr A pointer to a big endian data stream
 * @return The host ordered value
 */
uint16_t MmGetUInt16_N(uint16_t* ptr);

/*! Gets a float from a big endian data stream.
 * @param ptr A pointer to a big endian data stream
 * @return The host ordered value
 */
float    MmGetFloat_N(float*     ptr);

/*! Gets a 32 bit signed int from a big endian data stream.
 * @param ptr A pointer to a big endian data stream
 * @return The host ordered value
 */
int32_t  MmGetInt32_N(int32_t* ptr);

/*! Gets a 16 bit signed int from a big endian data stream.
 * @param ptr A pointer to a big endian data stream
 * @return The host ordered value
 */
int16_t  MmGetInt16_N(int16_t* ptr);

/*! Gets a float from a little endian data stream.
 * @param ptr A pointer to a little endian data stream
 * @return The host ordered value
 */
float    MmGetFloat_I(float*     ptr);


/*! This function is for older BPMAP file with incorrectly written float's
 * @param ptr A pointer to a little endian data stream
 * @return The host ordered value
 */
float	MmGetFloatFromOldBPMAP_N(float *ptr);

/*! Assigns a 32 bit unsigned value to a little endian data stream.
 * @param ptr The data stream to set.
 * @param val The value to store in the data stream.
 */
void MmSetUInt32_I(uint32_t* ptr,uint32_t val);

/*! Assigns a 16 bit unsigned value to a little endian data stream.
 * @param ptr The data stream to set.
 * @param val The value to store in the data stream.
 */
void MmSetUInt16_I(uint16_t* ptr,uint16_t val);

/*! Assigns an 8 bit unsigned value to a data stream.
 * @param ptr The data stream to set.
 * @param val The value to store in the data stream.
 */
void MmSetUInt8(uint8_t*   ptr,uint8_t  val);

/*! Assigns a float value to a little endian data stream.
 * @param ptr The data stream to set.
 * @param val The value to store in the data stream.
 */
void MmSetFloat_I(float*     ptr,float    val);

/*! Assigns a 32 bit unsigned value to a big endian data stream.
 * @param ptr The data stream to set.
 * @param val The value to store in the data stream.
 */
void MmSetUInt32_N(uint32_t* ptr,uint32_t val);

/*! Assigns a 16 bit unsigned value to a big endian data stream.
 * @param ptr The data stream to set.
 * @param val The value to store in the data stream.
 */
void MmSetUInt16_N(uint16_t* ptr,uint16_t val);

/*! Assigns a float value to a big endian data stream.
 * @param ptr The data stream to set.
 * @param val The value to store in the data stream.
 */
void MmSetFloat_N(float*     ptr,float    val);

/*! The size of a 32 bit integer */
#define INT32_SIZE  sizeof(int32_t )

/*! The size of a 16 bit integer */
#define INT16_SIZE  sizeof(int16_t )

/*! The size of an 8 bit integer */
#define INT8_SIZE   sizeof(int8_t  )

/*! The size of an unsigned 32 bit integer */
#define UINT32_SIZE sizeof(uint32_t)

/*! The size of an unsigned 16 bit integer */
#define UINT16_SIZE sizeof(uint16_t)

/*! The size of an unsigned 8 bit integer */
#define UINT8_SIZE  sizeof(uint8_t )

/*! The size of a floating point number */
#define FLOAT_SIZE  sizeof(float)

/*! The size of a 32 bit integer */
#define INT_SIZE    sizeof(int32_t)

// NOTE: LONG should not be used in new code!! Use INT32 or some such.

/*! The size of a 32 bit long */
#define LONG_SIZE   sizeof(int32_t)

/*! The size of a 32 bit unsigned long */
#define ULONG_SIZE  sizeof(uint32_t)

/*! The size of a 16 bit integer */
#define SHORT_SIZE  sizeof(int16_t)

/*! The size of a 16 bit unsigned integer */
#define USHORT_SIZE sizeof(uint16_t)

/*! The size of a 8 bit character */
#define CHAR_SIZE   sizeof(int8_t)

/*! The size of a 8 bit unsigned character */
#define UCHAR_SIZE  sizeof(uint8_t)

#endif // AFFX_FILEIO_H
