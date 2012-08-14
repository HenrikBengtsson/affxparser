////////////////////////////////////////////////////////////////
//
// Copyright (C) 2004 Affymetrix, Inc.
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
// cvs:affy/sdk/file/test-fileio.cpp ---
// 
// $Id: test-fileio.cpp,v 1.14 2009-09-25 17:49:00 mspald Exp $
// 
// Commentary:
// * Run the FileIO.cpp though some tests.
//
// g++ test-fileio


//
// NOTE: These are cast into "std::fstream"
#include "file/FileIO.h"
#include "file/FileWriter.h"
//
#include "portability/affy-base-types.h"
//
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
//

using namespace std;

#define HDR_SIZE 16
#define HDR_W(ostr) WRITE_STRING(ostm,str)
#define HDR_R() do { string str; \
   ReadFixedString(istm,str,HDR_SIZE); \
   printf("%s\n",str.c_str()); \
  } while (0)

#define TEST_STR "0123456789AB"
#define TEST_CNT_INT32 (2*(16/4))
#define TEST_CNT_INT16 (2*(16/2))
#define TEST_CNT_INT8  (2*(16/1))
#define TEST_CNT_FLOAT (2*(16/4))
#define TEST_CNT_STRING  4

int
main(int argc,char *argv[])
{
  int32_t a=1;
  int32_t i,h,n;
  float f,g;
  string str;
  ofstream ostm;
  ifstream istm;

  //
  if ((a<argc)&&(strcmp(argv[a],"-v")==0)) {
    a++;
    for (i=0;i<argc;i++) {
      cout << i << ": " << argv[i] << "\n";
    }
  }

  // OUTPUT
  if ((a<argc)&&(strcmp(argv[a],"-o")==0)) {
    a++;
    cout << "OUT: " << argv[a] << " ==============================\n";
    ostm.open(argv[a],fstream::out|fstream::binary);
    a++;
    // 
    HDR_W("Int32s!");
    for (i=0;i<TEST_CNT_INT32;i++) {
      h=0x01020300+i;
      WriteUInt32_I(ostm,h);
      WriteUInt32_I(ostm,h+0xF0000000);
    }
    HDR_W("Int16s!");
    for (i=0;i<TEST_CNT_INT16;i++) {
      h=i;
      WriteInt16_I(ostm,h);
      h=0xF000+i;
      WriteUInt16_I(ostm,h);
    }
    HDR_W("Int8s!");
    for (i=0;i<TEST_CNT_INT8;i++) {
      int8_t c=i;
      WriteUInt8(ostm,c);
    }
    HDR_W("Floats!");
    f=2.0;
    for (i=0;i<TEST_CNT_FLOAT;i++) {
      WriteFloat_I(ostm,f);
      f*=2.0;
    }
    HDR_W("Fixed Str!");
    for (i=0;i<4;i++) {
      WriteString_N(ostm,TEST_STR);
    }
    ostm.close();
  }

  // INPUT
  if ((a<argc)&&(strcmp(argv[a],"-i")==0)) {
    a++;
    cout << "IN:  " << argv[a] << " ==============================\n";
    istm.open(argv[a],fstream::in|fstream::binary);
    a++;
    //
#define CMP_XY() printf("O: %2d: x=%08X => y=%08X  %s\n",i,x,y,((x==y)?"":"Error"))
    HDR_R();
    for (i=0;i<TEST_CNT_INT32;i++) {
      uint32_t x,y;
      x=0x01020300+i;
      ReadUInt32_I(istm,y);
      CMP_XY();
      x=x+0xF0000000;
      ReadUInt32_I(istm,y);
      CMP_XY();
    }
    HDR_R();
    for (i=0;i<TEST_CNT_INT16;i++) {
      uint16_t x,y;
      x=i;
      ReadUInt16_I(istm,y);
      CMP_XY();
      x=x+0xF000;
      ReadUInt16_I(istm,y);
      CMP_XY();
    }
    HDR_R();
    for (i=0;i<TEST_CNT_INT8;i++) {
      uint8_t x,y;
      x=i;
      ReadUInt8(istm,y);
      CMP_XY();
    }
    HDR_R();
    f=2.0;
    for (i=0;i<TEST_CNT_FLOAT;i++) {
      ReadFloat_I(istm,g);
      printf("O: %2d: f=%9.2f => g=%9.2f  %s\n",i,f,g,((f==g)?"":"Error"));
      f*=2.0;
    }
    HDR_R();
    for (i=0;i<TEST_CNT_STRING;i++) {
      ReadString_N(istm,str);
      printf("O: %2d: '%s' %s\n",i,str.c_str(),((strcmp(str.c_str(),TEST_STR)==0)?"":"Error"));
    }
    //
    istm.close();
  }

  // 
  if (0) {
    for (f=1;f<256;f*=2.0) {
      printf("F=(%f)=0x%08X\n",f,*(uint32_t*)&f);
    }
  }


#define UNALIGN_BUF_SIZE 100  
#define UNALIGN_LOOP(type_t,func_set,func_get,fmt) \
  do { \
  uint8_t buf[UNALIGN_BUF_SIZE]; \
  printf("UNALIGN TEST(" #type_t ")==========\n"); \
  for (int off=0;off<16;off++) { \
    for (int idx=0;idx<1;idx++) { \
      type_t vi=(idx+off)*0x00000101; \
      type_t vo; \
      uint8_t* ptr=&buf[((idx*sizeof(type_t))+off)]; \
         func_set((type_t*)ptr,vi); \
         vo=func_get((type_t*)ptr);                                     \
      printf("ptr=(idx=%2d,off=%2d)=%p  vi=" #fmt " vo=" #fmt "\n",idx,off,ptr,vi,vo,((vi==vo)?"":"Error")); \
    } \
  } } while (0)

  // UNALIGNED
   if ((a<argc)&&(strcmp(argv[a],"-a")==0)) {
    a++;
    // Try eack of the types and functions.
    UNALIGN_LOOP(uint32_t,MmSetUInt32_I,MmGetUInt32_I,%08X);
    UNALIGN_LOOP(uint16_t,MmSetUInt16_I,MmGetUInt16_I,%04hX);
    UNALIGN_LOOP(uint8_t ,MmSetUInt8   ,MmGetUInt8   ,%02hhX);
    UNALIGN_LOOP(float   ,MmSetFloat_I ,MmGetFloat_I ,%16.2f);
  }
   //
  n=0;
}
