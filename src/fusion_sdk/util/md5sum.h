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

/// @file   md5sum.h
/// @brief  C++ utility functions to make computing MD5s simple

//
#ifndef _MSC_VER
#include <stdint.h>
#endif

//
#include <util/md5.h>
//
#include <cstring>
#include <string>
#include <vector>


namespace affx {
  class md5sum;
}

/// @brief an object oriented interface to generating MD5s.
///        provides methods to do strings and files with one call.
class affx::md5sum {
public:
  MD5Context m_context; ///< The basic MD5 context as wrapped by md5sum
  //
  md5sum();
  //
  int init();
  int update(void* buff,uint32_t buf_size);
  int update(const std::string& str);
  int update(char const * const str);

  //
  int update_nbo(int32_t val);
  int update_nbo(uint32_t val);
  int update_nbo(uint16_t val);
  int update_nbo(uint8_t  val);

  //
  int update_nbo(const std::vector<int>& vec);

  // not needed for now.
  // int update_nbo(val);
  // int update(double val);
  //
  int final(std::string& str);
  // handy methods
  int ofString(std::string str,std::string& sum);
  int ofFile(std::string filename,std::string& sum);
};
