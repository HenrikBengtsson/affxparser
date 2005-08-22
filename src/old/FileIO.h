/////////////////////////////////////////////////////////////////
//
// Copyright (C) 2004 Affymetrix, Inc.
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

#if !defined(AFX_FILEIO_H__8A5CA65E_C2E0_4C36_9B60_30C2020D5DC0__INCLUDED_)
#define AFX_FILEIO_H__8A5CA65E_C2E0_4C36_9B60_30C2020D5DC0__INCLUDED_

//////////////////////////////////////////////////////////////////////

#include <fstream>
#include <string>

//////////////////////////////////////////////////////////////////////

#define FLOAT_SIZE sizeof(float)
#define INT_SIZE sizeof(int)
#define UINT_SIZE sizeof(unsigned int)
#define SHORT_SIZE sizeof(short)
#define USHORT_SIZE sizeof(unsigned short)
#define CHAR_SIZE sizeof(char)
#define UCHAR_SIZE sizeof(unsigned char)

//////////////////////////////////////////////////////////////////////

void SwapBytes(char *src, char *dest, int size);

//////////////////////////////////////////////////////////////////////

void READ_INT(std::ifstream &instr, int &value);
void READ_UINT(std::ifstream &instr, unsigned int &value);
void READ_SHORT(std::ifstream &instr, short &value);
void READ_USHORT(std::ifstream &instr, unsigned short &value);
void READ_CHAR(std::ifstream &instr, char &value);
void READ_UCHAR(std::ifstream &instr, unsigned char &value);
void READ_BOOL(std::ifstream &instr, char &value);
void READ_FLOAT(std::ifstream &instr, float &value);
void READ_STRING(std::ifstream &instr, char * &value);
void READ_FIXED_STRING(std::ifstream &instr, char *value, int len);
void GET_NEXT_LINE(std::ifstream &instr, char *line, int len);
void READ_INT(const char* instr, int &value);

//////////////////////////////////////////////////////////////////////

void ReadInt(std::ifstream &instr, int &ival);
void ReadUChar(std::ifstream &instr, unsigned char &ucval);
void ReadUShort(std::ifstream &instr, unsigned short &usval);
void ReadUInt(std::ifstream &instr, unsigned int &uval);
void ReadFixedString(std::ifstream &instr, std::string &s, int len);
void ReadFixedUChar(std::ifstream &instr, unsigned char *s, int len);
void ReadUIntLenString(std::ifstream &instr, std::string &s);
void ReadString(std::ifstream &instr, std::string &s);
void ReadFloat(std::ifstream &instr, float &fval);
void ReadFloatFromOldBPMAP(std::ifstream &instr, float &fval);
void ReadUInt(const char *instr, unsigned int &uval);
void ReadFloat(const char *instr, float &fval);
void ReadFloatFromOldBPMAP(const char *instr, float &fval);
void ReadInt(const char *instr, int &ival);

//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_FILEIO_H__8A5CA65E_C2E0_4C36_9B60_30C2020D5DC0__INCLUDED_)
