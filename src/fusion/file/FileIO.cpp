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

//
//////////////////////////////////////////////////////////////////////
#ifdef _MSC_VER
#include "windows.h"
#endif
//
#include "file/FileIO.h"
//
#include "portability/affy-base-types.h"
#include "util/Convert.h"
#include "util/Err.h"
//
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string.h>
#include <string>
//
#ifdef _MSC_VER
#pragma warning(disable: 4996) // don't show deprecated warnings.
#else
#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>
#endif

// Some machines (sparc) dont support unaligned memory access
// The mmaped files are chock full of unaligned accesses.
// When AFFY_UNALIGNED_IN_SW is defined we will do the 
// alignment in software.
// This feature can be enabled on intel for testing.

// This is here to enable this feature automaticly on the sparc and ppc.
// they cant do unaligned loads.
#ifdef __sparc__
#define AFFY_UNALIGNED_IN_SW
#endif
#ifdef __POWERPC__
#define AFFY_UNALIGNED_IN_SW
#endif

using namespace std;

void
ReadUInt32_I(IFSTREAM& instr, uint32_t& val) 
{
  uint32_t v=0;
  instr.read((char*)&v,sizeof(v));
  val=itohl(v);
}
void
ReadInt32_I(IFSTREAM& instr,int32_t& val) 
{
  ReadUInt32_I(instr,(uint32_t&)val);
}
void
ReadFloat_I(IFSTREAM& instr,float& val) 
{
  ReadUInt32_I(instr,(uint32_t&)val);
}
//
void
ReadUInt16_I(IFSTREAM& instr, uint16_t& val) 
{
  uint16_t v=0;
  instr.read((char*)&v,sizeof(v));
  val=itohs(v);
}
void
ReadInt16_I(IFSTREAM& instr,int16_t& val) 
{
  ReadUInt16_I(instr,(uint16_t&)val);
}

// No byte swapping needed.
void
ReadUInt8(IFSTREAM& instr, uint8_t& val) 
{
  instr.read((char*)&val,sizeof(val));
}
void
ReadInt8(IFSTREAM& instr,int8_t& val) 
{
  ReadUInt8(instr,(uint8_t&)val);
}

//====================
//
// Network byte order
//

void
ReadUInt32_N(IFSTREAM& instr, uint32_t& val) 
{
  uint32_t v=0;
  instr.read((char*)&v,sizeof(v));
  val=ntohl(v);
}
void
ReadInt32_N(IFSTREAM& instr,int32_t& val) 
{
  ReadUInt32_N(instr,(uint32_t&)val);
}
void
ReadFloat_N(IFSTREAM& instr,float& val) 
{
  ReadUInt32_N(instr,(uint32_t&)val);
}
//
void
ReadUInt16_N(IFSTREAM& instr, uint16_t& val) 
{
  uint16_t v=0;
  instr.read((char*)&v,sizeof(v));
  val=ntohs(v);
}
void
ReadInt16_N(IFSTREAM& instr, int16_t& val) 
{
  ReadUInt16_N(instr,(uint16_t&)val);
}

// When the old BPMAP files were written, the output 
// values were mangled in this way:
// * Start with a float value.
// * convert it to an integer.
// * byte-swap it
// * write it as a float.
// This function reverses the process to get the data.
void
ReadFloatFromOldBPMAP_N(IFSTREAM &instr, float &fval)
{
  type_punned pun;

	instr.read((char *)&pun.v_uint32,FLOAT_SIZE);
#if BYTE_ORDER == BIG_ENDIAN
  pun.v_uint32=affy_swap32(pun.v_uint32);
  pun.v_uint32=(uint32_t)pun.v_float;
  pun.v_uint32=affy_swap32(pun.v_uint32);
#else
  pun.v_uint32=(uint32_t)pun.v_float;
  pun.v_uint32=ntohl(pun.v_uint32);
#endif
  fval=pun.v_uint32;
}

//==============================

// c char*
void
ReadFixedCString(IFSTREAM& instr, char* str, uint32_t len)
{
  instr.read(str,len);
}

void
ReadFixedUCString(IFSTREAM& instr, unsigned char* str, uint32_t len)
{
  instr.read((char *)str,len);
}

void
ReadCString_I(IFSTREAM& instr, char*& str)
{
  uint32_t slen;
  ReadUInt32_I(instr,slen);
  str=new char[slen+1];
  instr.read(str,slen);
  // ensure null -- this replaces the last char read.
  str[slen]=0; 
}
void
ReadCString_I(IFSTREAM& instr,std::string& str)
{
  uint32_t slen;
  ReadUInt32_I(instr,slen);
  str.resize(slen);
  instr.read(&str[0],slen);
}

void
ReadCharacterArray(IFSTREAM& instr, char* str, uint32_t len)
{
  instr.read(str,len);
}

// The data file may or may not have the null terminator as
// part of the length. Using a STD::STRING function as the buffer to
// read directly into may result in the string with a length one more
// than is intended. The CHAR * buffer will contain an extra null that
// will get removed when copying to the output STD::STRING.
void
ReadFixedString(IFSTREAM& instr, string& str, uint32_t len)
{
  char *s = new char[len+1];
  instr.read(s,len);
  s[len]=0;
  str = s;
  delete[] s;
}

void
ReadString_I(IFSTREAM& instr, string& str)
{
  uint32_t len;
  ReadUInt32_I(instr,len);
  ReadFixedString(instr,str,len);
}

void
ReadUIntLenString_I(IFSTREAM& instr, std::string &s)
{
	uint32_t len;
	ReadUInt32_I(instr, len);
	ReadFixedString(instr, s, len);
}

void
ReadCString_N(IFSTREAM& instr, char*& str)
{
  uint32_t slen;
  ReadUInt32_N(instr,slen);
  str=new char[slen+1];
  instr.read(str,slen);
  // ensure null -- this replaces the last char read.
  str[slen]=0; 
}

void
ReadString_N(IFSTREAM& instr, string& str)
{
  uint32_t len;
  ReadUInt32_N(instr,len);
  ReadFixedString(instr,str,len);
}

void
ReadUIntLenString_N(IFSTREAM& instr, std::string &s)
{
	uint32_t len;
	ReadUInt32_N(instr, len);
	ReadFixedString(instr, s, len);
}

void 
ReadNextLine(IFSTREAM& instr,char* line,int len)
{
	// zero the buffer to make debugging a bit better.
	memset(line,0,len);
	strcpy(line,"");

	while (!instr.eof())
	{
		instr.getline(line,len);
		if (strlen(line)>0)
		{
			if (line[strlen(line)-1]=='\r')
				line[strlen(line)-1]='\0';
			if (strlen(line)>0)
				return;
		}
	}
}


//==============================
//

// There are two issues here: alignment and byte order.  
// Byte order can be handled with the "htoi" functions.
// Alignment has to be handled with functions.

// The "Get*" functions are for reading data from a mem-mapped file.
// OLD: Callers of these really should use "itoh*" directly.
//      Opps! They cant as "itoh" does not handle aligment.

// The signed versions
int32_t 
MmGetInt32_I(int32_t* ptr)
{
  return MmGetUInt32_I((uint32_t*)ptr);
}
int16_t 
MmGetInt16_I(int16_t* ptr)
{
  return MmGetUInt16_I((uint16_t*)ptr);
}
int32_t 
MmGetInt32_N(int32_t* ptr)
{
  return MmGetUInt32_N((uint32_t*)ptr);
}
int16_t 
MmGetInt16_N(int16_t* ptr)
{
  return MmGetUInt16_N((uint16_t*)ptr);
}
int8_t 
MmGetInt8(int8_t* ptr)
{
  return MmGetUInt8((uint8_t*)ptr);
}

//==========
// If unaligned accesses to memory are allowed, (intel)
// use these functions. 

#ifndef AFFY_UNALIGNED_IN_SW

//// 32
uint32_t
MmGetUInt32_I(uint32_t* ptr)
{
  return itohl(*ptr);
}
void
MmSetUInt32_I(uint32_t* ptr,uint32_t val)
{
  *(uint32_t*)ptr=htoil(val);
}
//
uint32_t
MmGetUInt32_N(uint32_t* ptr)
{
  return ntohl(*ptr);
}
void
MmSetUInt32_N(uint32_t* ptr,uint32_t val)
{
  *(uint32_t*)ptr=htonl(val);
}
//// 16
uint16_t
MmGetUInt16_I(uint16_t* ptr)
{
  return itohs(*ptr);
}
void
MmSetUInt16_I(uint16_t* ptr,uint16_t val)
{
  *(uint16_t*)ptr=htois(val);
}
//
uint16_t
MmGetUInt16_N(uint16_t* ptr)
{
  return ntohs(*ptr);
}
void
MmSetUInt16_N(uint16_t* ptr,uint16_t val)
{
  *(uint16_t*)ptr=htons(val);
}
//
uint8_t
MmGetUInt8(uint8_t* ptr)
{
  return *ptr; // no byte-swapping needed.
}
void
MmSetUInt8(uint8_t* ptr,uint8_t val)
{
  *ptr=val;
}

//
float
MmGetFloat_I(float* ptr)
{
  type_punned pun;
  pun.v_float=*ptr;
  pun.v_uint32=itohl(pun.v_uint32);
  return pun.v_float;
}
void
MmSetFloat_I(float* ptr,float val)
{
  type_punned pun;
  pun.v_float=val;
  pun.v_uint32=htoil(pun.v_uint32);
  *ptr=pun.v_float;
}
float
MmGetFloat_N(float* ptr)
{
  type_punned pun;
  pun.v_float=*ptr;
  pun.v_uint32=ntohl(pun.v_uint32);
  return pun.v_float;
}
void
MmSetFloat_N(float* ptr,float val)
{
  type_punned pun;
  pun.v_float=val;
  pun.v_uint32=htonl(pun.v_uint32);
  *ptr=pun.v_float;
}

// See the notes in 
float
MmGetFloatFromOldBPMAP_N(float *ptr)
{
  float fval;

#if BYTE_ORDER == LITTLE_ENDIAN
  // There isnt any punning going on here.
  // cast, swap, cast back.
 	fval = (float)ntohl((uint32_t)*ptr);
#else
  type_punned pun;
  pun.v_float=*ptr;
  pun.v_uint32=affy_swap32(pun.v_uint32);
 	fval=(float)affy_swap32((uint32_t)pun.v_float);
#endif
  return fval;
}

#endif

// We dont have unaligned access to memory, fake it
// The conversion from and to little endian is done as part of
// the unaligned mem access

#ifdef AFFY_UNALIGNED_IN_SW

//// 32
uint32_t
MmGetUInt32_I(uint32_t* ptr)
{
  uint8_t* cptr=(uint8_t*)ptr;
  uint32_t val=0;
  val|=(*cptr++);
  val|=(*cptr++)<< 8;
  val|=(*cptr++)<<16;
  val|=(*cptr++)<<24;
  return val;
}
void
MmSetUInt32_I(uint32_t* ptr,uint32_t val)
{
  uint8_t* cptr=(uint8_t*)ptr;
  *cptr++=((val    )&0xFF);
  *cptr++=((val>> 8)&0xFF);
  *cptr++=((val>>16)&0xFF);
  *cptr++=((val>>24)&0xFF);
}

uint32_t
MmGetUInt32_N(uint32_t* ptr)
{
  uint8_t* cptr=(uint8_t*)ptr;
  uint32_t val=0;
  val|=(*cptr++)<<24;
  val|=(*cptr++)<<16;
  val|=(*cptr++)<< 8;
  val|=(*cptr++);
  return val;
}
void
MmSetUInt32_N(uint32_t* ptr,uint32_t val)
{
  uint8_t* cptr=(uint8_t*)ptr;
  // val=htonl(val); // with it in big order
  *cptr++=((val>>24)&0xFF);
  *cptr++=((val>>16)&0xFF);
  *cptr++=((val>> 8)&0xFF);
  *cptr++=((val    )&0xFF);
}

//// 16
uint16_t
MmGetUInt16_I(uint16_t* ptr)
{
  uint8_t* cptr=(uint8_t*)ptr;
  uint16_t val=0;
  val|=(*cptr++);
  val|=(*cptr++)<<8;
  return val;
}
void
MmSetUInt16_I(uint16_t* ptr,uint16_t val)
{
  uint8_t* cptr=(uint8_t*)ptr;
  *cptr++=((val    )&0xFF);
  *cptr++=((val>> 8)&0xFF);
}
uint16_t
MmGetUInt16_N(uint16_t* ptr)
{
  uint8_t* cptr=(uint8_t*)ptr;
  uint16_t val=0;
  val|=(*cptr++)<< 8;
  val|=(*cptr++);
  return val;
}
void
MmSetUInt16_N(uint16_t* ptr,uint16_t val)
{
  uint8_t* cptr=(uint8_t*)ptr;
  *cptr++=((val>>8)&0xFF);
  *cptr++=((val   )&0xFF);
}

//// 8
uint8_t
MmGetUInt8(uint8_t* ptr)
{
  return *(uint8_t*)ptr;
}
void
MmSetUInt8(uint8_t* ptr,uint8_t val)
{
  *(uint8_t*)ptr=val;
}
//
float
MmGetFloat_I(float* ptr)
{
  uint32_t val=MmGetUInt32_I((uint32_t*)ptr);
  return *((float*)&val);
}
void
MmSetFloat_I(float* ptr,float val)
{
  MmSetUInt32_I((uint32_t*)ptr,*(uint32_t*)&val);
}
float
MmGetFloat_N(float* ptr)
{
  uint32_t val=MmGetUInt32_N((uint32_t*)ptr);
  return *((float*)&val);
}

float
MmGetFloatFromOldBPMAP_N(float *ptr)
{
	float fval;
#if BYTE_ORDER == LITTLE_ENDIAN
	fval = (float) ntohl((uint32_t)MmGetFloat_I(ptr));
#else
	int i1=*(int *)ptr;
 	i1=affy_swap32(i1);
  /// @todo this should be type punned. - jhg
	fval=(float)affy_swap32((uint32_t)(*(float *)&i1)); // cast to float, then deref
#endif
	return fval;
}

void
MmSetFloat_N(float* ptr,float val)
{
  MmSetUInt32_N((uint32_t*)ptr,*(uint32_t*)&val);
}
#endif


//////////



//
// FILE* of the above
//

#ifdef FILEIO_WITH_STDIO

void
ReadUInt32_I(FILE* file, uint32_t& val) 
{
  uint32_t v=0;
  fread(&v,sizeof(v),1,file);
  val=itohl(v);
}
void
ReadInt32_I(FILE* file,int32_t& val) 
{
  ReadUInt32_I(file,(uint32_t&)val);
}
void
ReadFloat_I(FILE* file,float& val) 
{
  ReadUInt32_I(file,(uint32_t&)val);
}
//
void
ReadUInt16_I(FILE* file, uint16_t& val) 
{
  uint16_t v=0;
  fread(&v,sizeof(v),1,file);
  val=itohs(v);
}
void
ReadInt16_I(FILE* file,int16_t& val) 
{
  ReadUInt16_I(file,(uint16_t&)val);
}

//====================
//
// No byte swapping needed for bytes
//
void
ReadUInt8(FILE* file, uint8_t& val) 
{
  fread((char*)&val,sizeof(val),1,file);
}
void
ReadInt8(FILE* file,int8_t& val) 
{
  ReadUInt8(file,(uint8_t&)val);
}

//====================
//
// Network (big-endian) byte order
//
void
ReadUInt32_N(FILE* file, uint32_t& val) 
{
  uint32_t v=0;
  fread(&v,sizeof(v),1,file);
  val=ntohl(v);
}
void
ReadInt32_N(FILE* file,int32_t& val) 
{
  ReadUInt32_N(file,(uint32_t&)val);
}
void
ReadFloat_N(FILE* file,float& val) 
{
  ReadUInt32_N(file,(uint32_t&)val);
}
//
void
ReadUInt16_N(FILE* file, uint16_t& val) 
{
  uint16_t v=0;
  fread((char*)&v,sizeof(v),1,file);
  val=ntohs(v);
}
void
ReadInt16_N(FILE* file, int16_t& val) 
{
  ReadUInt16_N(file,(uint16_t&)val);
}

void
ReadCString_I(FILE* file, char*& str)
{
  uint32_t slen;
  ReadUInt32_I(file,slen);
  str=new char[slen+1];
  fread(str,slen,1,file);
  str[slen]=0; 
}

void
ReadCString_I(FILE* file,std::string& str)
{
  uint32_t slen;
  ReadUInt32_I(file,slen);
  str.resize(slen);
  fread(&str[0],slen,1,file);
}

#endif

#ifdef FILEIO_WITH_ZLIB

void
ReadUInt32_I(gzFile gzfile, uint32_t& val) 
{
  uint32_t v=0;
  int cnt=gzread(gzfile,&v,sizeof(v));
  if (cnt!=sizeof(v)) {
    Err::errAbort("ReadUInt32_I: bad read.");
  }
  val=itohl(v);
  // printf("ReadUInt32_I(%p)==%d\n",gzfile,val);
}
void
ReadInt32_I(gzFile gzfile,int32_t& val) 
{
  ReadUInt32_I(gzfile,(uint32_t&)val);
}
void
ReadFloat_I(gzFile gzfile,float& val) 
{
  ReadUInt32_I(gzfile,(uint32_t&)val);
}
//
void
ReadUInt16_I(gzFile gzfile, uint16_t& val) 
{
  uint16_t v=0;
  int cnt=gzread(gzfile,&v,sizeof(v));
  if (cnt!=sizeof(v)) {
    Err::errAbort("ReadUInt16_I: bad read.");
  }
  val=itohs(v);
}
void
ReadInt16_I(gzFile gzfile,int16_t& val) 
{
  ReadUInt16_I(gzfile,(uint16_t&)val);
}

//====================
//
// No byte swapping needed for bytes
//
void
ReadUInt8(gzFile gzfile, uint8_t& val) 
{
  int cnt=gzread(gzfile,(char*)&val,sizeof(val));
  if (cnt!=sizeof(val)) {
    Err::errAbort("ReadUInt8: bad read.");
  }
}
void
ReadInt8(gzFile gzfile,int8_t& val) 
{
  ReadUInt8(gzfile,(uint8_t&)val);
}

//====================
//
// Network (big-endian) byte order
//
void
ReadUInt32_N(gzFile gzfile, uint32_t& val) 
{
  uint32_t v=0;
  int cnt=gzread(gzfile,&v,sizeof(v));
  if (cnt!=sizeof(v)) {
    Err::errAbort("ReadUInt32_N: bad read.");
  }
  val=ntohl(v);
}
void
ReadInt32_N(gzFile gzfile,int32_t& val) 
{
  ReadUInt32_N(gzfile,(uint32_t&)val);
}
void
ReadFloat_N(gzFile gzfile,float& val) 
{
  ReadUInt32_N(gzfile,(uint32_t&)val);
}
//
void
ReadUInt16_N(gzFile gzfile, uint16_t& val) 
{
  uint16_t v=0;
  int cnt=gzread(gzfile,&v,sizeof(v));
  if (cnt!=sizeof(v)) {
    Err::errAbort("ReadUInt16_N: bad read.");    
  }
  val=ntohs(v);
}
void
ReadInt16_N(gzFile gzfile, int16_t& val) 
{
  ReadUInt16_N(gzfile,(uint16_t&)val);
}

void
ReadCString_I(gzFile gzfile,std::string& str)
{
  uint32_t slen;
  ReadUInt32_I(gzfile,slen);
  str.resize(slen);
  int cnt=gzread(gzfile,&str[0],slen);
  if (cnt!=slen) {
    Err::errAbort("ReadCString_I: bad read. ("+ToStr(cnt)+"!="+ToStr(slen)+")");
  }
  /*
  printf("== %3d: '",slen);
  for (int i=0;i<slen;i++) {
    int c=gzgetc(gzfile);
    if (c==-1) {
      Err::errAbort("ReadCString_I: bad read. ["+ToStr(i)+"/"+ToStr(slen)+"]");
    }
    str[i]=c;
    printf(" %02x:%c",c,c);
    fflush(NULL);
  }
  printf("'\n");
  */
}

void
ReadFixedString(gzFile gzfile, string& str, uint32_t len)
{
  str.resize(len);
  int cnt=gzread(gzfile,&str[0],len);
  if (cnt!=len) {
    Err::errAbort("ReadFixedString: bad read.");
  }
}

void
ReadFixedCString(gzFile gzfile, char* str, uint32_t len)
{
  int cnt=gzread(gzfile,str,len);
  if (cnt!=len) {
    Err::errAbort("ReadFixedCString: bad read.");
  }
}

#endif
