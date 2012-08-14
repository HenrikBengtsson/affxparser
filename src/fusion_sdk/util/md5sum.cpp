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
#include <util/md5sum.h>
//
#include <util/Fs.h>
#include <util/md5.h>
//
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>

#ifdef _WIN32
#include <winsock2.h> 
#endif
#ifndef _MSC_VER
#include <netinet/in.h>
#include <stdint.h>
#endif

using namespace std;

/// @brief     Creation of md5sum an object
/// @return    an md5sum
affx::md5sum::md5sum() {
  init();
}

/// @brief     Init the starting state of the MD5 operation
/// @return    non-zero on error
int
affx::md5sum::init()
{
  MD5Init(&m_context);
  return 0;
}

/// @brief     update the MD5 state with a block of data
/// @param     buf        Pointer to buffer of data
/// @param     buf_size  Number of bytes in the buffer
/// @return    non-zero on error
int
affx::md5sum::update(void* buf,uint32_t buf_size)
{
  MD5Update(&m_context,(const u_int8_t*)buf,buf_size);
  return 0;
}

/// @brief     update the MD5 state with a string
/// @param     str       String to add to the MD5
/// @return    non-zero on error
int
affx::md5sum::update(const std::string& str)
{
  MD5Update(&m_context,(const u_int8_t*)str.c_str(),(u_int32_t)str.size());
  return 0;
}
int
affx::md5sum::update(char const * const str)
{
  MD5Update(&m_context,(const u_int8_t*)str,(u_int32_t)strlen(str));
  return 0;
}

/// @brief     update the MD5 state with a int in network byte order.
/// @param     val       Value to update the md5 with.
/// @return    non-zero on error
int affx::md5sum::update_nbo(int32_t val)
{
  uint32_t val_bigendian=htonl((uint32_t)val);
  MD5Update(&m_context,(uint8_t*)&val_bigendian,sizeof(uint32_t));
  return 0;
}

/// @brief     update the MD5 state with a int in network byte order.
/// @param     val       Value to update the md5 with.
/// @return    non-zero on error
int affx::md5sum::update_nbo(uint32_t val)
{
  uint32_t val_bigendian=htonl(val);
  MD5Update(&m_context,(uint8_t*)&val_bigendian,sizeof(uint32_t));
  return 0;
}

/// @brief     update the MD5 state with a int in network byte order.
/// @param     val       Value to update the md5 with.
/// @return    non-zero on error
int affx::md5sum::update_nbo(uint16_t val)
{
  uint16_t val_bigendian=htons(val);
  MD5Update(&m_context,(uint8_t*)&val_bigendian,sizeof(uint16_t));
  return 0;
}

/// @brief     update the MD5 state with a int in network byte order.
/// @param     val       Value to update the md5 with.
/// @return    non-zero on error
int affx::md5sum::update_nbo(uint8_t val)
{
  // single byte, no swap needed.
  MD5Update(&m_context,(uint8_t*)&val,sizeof(uint8_t));
  return 0;
}

/// @brief     Update the md5 with all the values of the vector
/// @param     vec   vector of ints to add to the checksum.
/// @return    
int affx::md5sum::update_nbo(const std::vector<int>& vec)
{
  for (int i=0;i<vec.size();i++) {
    update_nbo(vec[i]);
  }

  return 0;
}

/*
 * Not needed for now.
 *
/// @brief     update the MD5 state with a int in network byte order.
/// @param     val       Value to update the md5 with.
/// @return    non-zero on error
int affx::md5sum::update_nbo(float val)
{
  uint32_t val_bigendian=htonl(val);
  MD5Update(&m_context,&val_bigendian,sizeof(uint32_t));
  return 0;
}

/// @brief     update the MD5 state with a int in network byte order.
/// @param     val       Value to update the md5 with.
/// @return    non-zero on error
int affx::md5sum::update_nbo(double val)
{
  uint16_t val_bigendian=htons(val);
  MD5Update(&m_context,&val_bigendian,sizeof(uint16_t));
  return 0;
}
*/

/// append a nibble to a string
#define MD5SUM_NIB_APPEND(str,nib) \
{ \
int n=nib; \
char c=(n<10)?('0'+n):('a'+n-10); \
str.append(1,c); \
}

/// @brief     Conclude the computation of the MD5 and generate the output
/// @param     sum       The hex-encoded MD5 output
/// @return    non-zero on error
int
affx::md5sum::final(std::string& sum)
{
  u_int8_t sum_bits[MD5_DIGEST_LENGTH];
  MD5Final(sum_bits,&m_context);

  // convert all the bits to hex
  sum="";
  sum.reserve(MD5_DIGEST_LENGTH*2); // "*2" for hex encoding

  for (int b=0;b<MD5_DIGEST_LENGTH;b++) {
    int bits=sum_bits[b];
    MD5SUM_NIB_APPEND(sum,bits>>4);
    MD5SUM_NIB_APPEND(sum,bits&0x0f);
  }

  // prepare for the next round
  init();
  return 0;
}

//////////

/// @brief     Compute the MD5 of a std::string
/// @param     str       string to checksum
/// @param     sum       output checksum
/// @return    non-zero on error
int
affx::md5sum::ofString(std::string str,std::string& sum)
{
  init();
  update(str);
  return final(sum);
}

/// @brief     Compute the MD5 of a file
/// @param     filename  name of the file to
/// @param     sum
/// @return    non-zero on error
int
affx::md5sum::ofFile(const std::string filename,std::string& sum)
{
  ifstream istm;
  char* buf_ptr;
  int buf_size=(1<<12); // 4MB

  // clear the output now in case of an error later.
  sum="";
  init();

  buf_ptr=new char[buf_size];
  assert(buf_ptr!=NULL);

  std::string uncfilename=Fs::convertToUncPath(filename);

  //istm.exceptions(ios_base::bad_bit|ios_base::fail_bit);
  istm.open(uncfilename.c_str(),ios_base::binary);

  if (istm.fail()) {
    delete[] buf_ptr;
    return -1;
  }

  do {
    istm.read(buf_ptr,buf_size);
    update(buf_ptr,(int)istm.gcount());
  } while (!istm.eof());

  delete[] buf_ptr;
  istm.close();

  return final(sum);
}
