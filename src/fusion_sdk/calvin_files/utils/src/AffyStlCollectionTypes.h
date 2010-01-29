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


#ifndef _AffyStlCollectionTypes_HEADER_
#define _AffyStlCollectionTypes_HEADER_

#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//
#include <cstring>
#include <list>
#include <string>
#include <vector>
//

/*! 8 bit signed */
typedef std::vector<int8_t> Int8Vector;
typedef std::vector<int8_t>::iterator Int8VectorIt;
typedef std::vector<int8_t>::const_iterator Int8VectorConstIt;

typedef std::list<int8_t> Int8List;
typedef std::list<int8_t>::iterator Int8ListIt;
typedef std::list<int8_t>::const_iterator Int8ListConstIt;

/*! 16 bit signed */
typedef std::vector<int16_t> Int16Vector;
typedef std::vector<int16_t>::iterator Int16VectorIt;
typedef std::vector<int16_t>::const_iterator Int16VectorConstIt;

typedef std::list<int16_t> Int16List;
typedef std::list<int16_t>::iterator Int16ListIt;
typedef std::list<int16_t>::const_iterator Int16ListConstIt;

/*! 32 bit signed */
typedef std::vector<int32_t> Int32Vector;
typedef std::vector<int32_t>::iterator Int32VectorIt;
typedef std::vector<int32_t>::const_iterator Int32VectorConstIt;

typedef std::list<int32_t> Int32List;
typedef std::list<int32_t>::iterator Int32ListIt;
typedef std::list<int32_t>::const_iterator Int32ListConstIt;

/*! 64 bit signed */
typedef std::vector<int64_t> Int64Vector;
typedef std::vector<int64_t>::iterator Int64VectorIt;
typedef std::vector<int64_t>::const_iterator Int64VectorConstIt;

/*! 8 bit unsigned */
typedef std::vector<u_int8_t> Uint8Vector;
typedef std::vector<u_int8_t>::iterator Uint8VectorIt;
typedef std::vector<u_int8_t>::const_iterator Uint8VectorConstIt;

typedef std::list<u_int8_t> Uint8List;
typedef std::list<u_int8_t>::iterator Uint8ListIt;
typedef std::list<u_int8_t>::const_iterator Uint8ListConstIt;

/*! 16 bit unsigned */
typedef std::vector<u_int16_t> Uint16Vector;
typedef std::vector<u_int16_t>::iterator Uint16VectorIt;
typedef std::vector<u_int16_t>::const_iterator Uint16VectorConstIt;

typedef std::list<u_int16_t> Uint16List;
typedef std::list<u_int16_t>::iterator Uint16ListIt;
typedef std::list<u_int16_t>::const_iterator Uint16ListConstIt;

/*! 32 bit unsigned */
typedef std::vector<u_int32_t> Uint32Vector;
typedef std::vector<u_int32_t>::iterator Uint32VectorIt;
typedef std::vector<u_int32_t>::const_iterator Uint32VectorConstIt;

typedef std::list<u_int32_t> Uint32List;
typedef std::list<u_int32_t>::iterator Uint32ListIt;
typedef std::list<u_int32_t>::const_iterator Uint32ListConstIt;

/*! 64 bit unsigned */
typedef std::vector<u_int64_t> Uint64Vector;
typedef std::vector<u_int64_t>::iterator Uint64VectorIt;
typedef std::vector<u_int64_t>::const_iterator Uint64VectorConstIt;

typedef std::list<u_int64_t> Uin64List;
typedef std::list<u_int64_t>::iterator Uint64ListIt;
typedef std::list<u_int64_t>::const_iterator Uint64ListConstIt;

/*! float */
typedef std::vector<float> FloatVector;
typedef std::vector<float>::iterator FloatVectorIt;
typedef std::vector<float>::const_iterator FloatVectorConstIt;

typedef std::list<float> FloatList;
typedef std::list<float>::iterator FloatListIt;
typedef std::list<float>::const_iterator FloatListConstIt;

/*! wstring */
typedef std::vector<std::wstring> WStringVector;
typedef std::vector<std::wstring>::iterator WStringVectorIt;
typedef std::vector<std::wstring>::const_iterator WStringVectorConstIt;

typedef std::list<std::wstring> WStringList;
typedef std::list<std::wstring>::iterator WStringListIt;
typedef std::list<std::wstring>::const_iterator WStringListConstIt;

/*! string */
typedef std::vector<std::string> StringVector;
typedef std::vector<std::string>::iterator StringVectorIt;
typedef std::vector<std::string>::const_iterator StringVectorConstIt;

typedef std::list<std::string> StringList;
typedef std::list<std::string>::iterator StringListIt;
typedef std::list<std::string>::const_iterator StringListConstIt;

/*! bool */
typedef std::list<bool> BoolList;
typedef std::list<bool>::iterator BoolListIt;
typedef std::list<bool>::const_iterator BoolListConstIt;

typedef std::vector<bool> BoolVector;
typedef std::vector<bool>::iterator BoolVectorIt;
typedef std::vector<bool>::const_iterator BoolVectorConstIt;

#endif // _AffyStlCollectionTypes_HEADER_
