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


/*! \file ColumnInfo.cpp This file defines column information class of various types.
 */

#include "calvin_files/data/src/ColumnInfo.h"


namespace affymetrix_calvin_io
{

/*! Constructor - used only by dervied types
 *	@param name_ Name of the column.
 *	@param type_ Type of data in the column.
 *	@param size_ Size of each element of in the column.
 *	@param len_ Number of elements of type in the column.
 *	@param overhead_ Number of extra bytes in the column
 */
ColumnInfo::ColumnInfo(const std::wstring& name_, DataSetColumnTypes type_, int32_t size_, int32_t len_, int32_t overhead_) : name(name_), type(type_), size(size_), len(len_), overhead(overhead_){
}
  
/*! Constructor - used by the file read operation
 * @param name_ Name of the column.
 * @param type_ Type of data in the column.
 * @param totalSize Total size of the colum in bytes.
 */
ColumnInfo::ColumnInfo(const std::wstring& name_, DataSetColumnTypes type_, int32_t totalSize)
    : name(name_), type(type_), size(totalSize), len(1), overhead(0)
{
  if (type == ASCIICharColType || type == UnicodeCharColType) {
    overhead = 4;
    if (type == UnicodeCharColType) {
      size = sizeof(int16_t);
      len = (totalSize - overhead) / sizeof(int16_t);
    } else if (type == ASCIICharColType) {
      size = sizeof(int8_t);
      len = (totalSize - overhead) / sizeof(int8_t);
    }
  }
}

};
