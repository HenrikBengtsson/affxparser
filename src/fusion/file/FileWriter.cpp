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

//////////////////////////////////////////////////////////////////////
#ifdef _MSC_VER
#include "windows.h"
#endif
//
#include "file/FileWriter.h"
//
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string.h>
#include <string>
//
#ifndef _MSC_VER
#include <inttypes.h>
#include <netinet/in.h>
#include <sys/types.h>
#endif

using namespace std;

//
void 
WriteUInt32_I(OFSTREAM& outstr,uint32_t val)
{
  val = htoil(val);
 	outstr.write((char *)&val,sizeof(val));
}
void
WriteInt32_I(OFSTREAM& outstr,int32_t val)
{
  WriteUInt32_I(outstr,(uint32_t)val);
}
//
void 
WriteUInt32_N(OFSTREAM& outstr,uint32_t val)
{
  val = htonl(val);
 	outstr.write((char *)&val,sizeof(val));
}
void
WriteInt32_N(OFSTREAM& outstr,int32_t val)
{
  WriteUInt32_N(outstr,(uint32_t)val);
}

//
void
WriteUInt16_I(OFSTREAM& outstr,uint16_t val)
{
  val = htois(val);
 	outstr.write((char *)&val,sizeof(val));
}
void
WriteInt16_I(OFSTREAM& outstr,int16_t val)
{
  WriteUInt16_I(outstr,(uint16_t)val);
}
void
WriteUInt16_N(OFSTREAM& outstr,uint16_t val)
{
  val = htons(val);
 	outstr.write((char *)&val,sizeof(val));
}
void
WriteInt16_N(OFSTREAM& outstr,int16_t val)
{
  WriteUInt16_N(outstr,(uint16_t)val);
}

//
void 
WriteUInt8(OFSTREAM& outstr,uint8_t val)
{
 	outstr.write((char *)&val,sizeof(val));
}
void 
WriteInt8(OFSTREAM& outstr,int8_t val)
{
  WriteUInt8(outstr,(uint8_t)val);
}

//
void 
WriteFloat_I(OFSTREAM& outstr,float fval)
{
  type_punned pun;
  pun.v_float=fval;
	WriteUInt32_I(outstr,pun.v_uint32);
}
void 
WriteFloat_N(OFSTREAM& outstr,float fval)
{
  type_punned pun;
  pun.v_float=fval;
	WriteUInt32_N(outstr,pun.v_uint32);
}

// Customers wanted repeatablity with old
// text files with only had a single digit after the decimal point.

void
WriteFloatLowPrecision(OFSTREAM& outstr,float fval)
{
  fval=(float)((floor((fval+0.05)*10))/10.0);
	WriteFloat_I(outstr,fval);
}

//====================

// normal C strings.
void 
WriteFixedCString(OFSTREAM& outstr,const char* str,size_t outlen)
{
  uint32_t slen=(uint32_t)strlen(str);
  outstr.write(str,((outlen<slen)?(uint32_t)outlen:slen));
  // pad with nulls
  for (uint32_t i=slen;i<outlen;i++) {
    outstr.put(0);
  }
}

void 
WriteCString(OFSTREAM& outstr,const char* str)
{
  uint32_t slen=(uint32_t)strlen(str);
 	WriteUInt32_I(outstr,slen);
  WriteFixedCString(outstr,str,slen);
}
void 
WriteCString(OFSTREAM& outstr,std::string str)
{
  WriteCString(outstr,str.c_str());
}

// C++ strings

void 
WriteFixedString(OFSTREAM& outstr,const std::string str,size_t outlen)
{
  WriteFixedCString(outstr,str.c_str(),outlen);
}

void
WriteString_I(OFSTREAM& outstr,const std::string str)
{
	WriteCString(outstr,str.c_str());
}

void
WriteString_N(OFSTREAM& outstr,const std::string str)
{
	int len = (int) str.length();
	WriteInt32_N(outstr, len);
	WriteFixedString(outstr, str, len);
}

// Character Array
void
WriteCharacterArray(OFSTREAM& outstr, char *c, size_t outlen)
{
        for (uint32_t i=0; i<outlen; i++) { outstr.put(c[i]); }
}
