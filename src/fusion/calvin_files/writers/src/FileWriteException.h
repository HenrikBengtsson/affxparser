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


#ifndef _FileWriteException_HEADER_
#define _FileWriteException_HEADER_

/*! \file FileWriteException.h This file provides class definition for the file writing exceptions.
 */ 

#include "calvin_files/exception/src/ExceptionBase.h"
//

namespace affymetrix_calvin_exceptions
{

class FileCreateException : public CalvinException
{
public:
  FileCreateException() : CalvinException() {}
  FileCreateException(std::wstring _Source, std::wstring _Description, std::wstring _TimeStamp, std::string _FileName, u_int16_t _LineNumber, u_int64_t _ErrorCode):
    CalvinException(_Source, _Description, _TimeStamp, _FileName,  _LineNumber, _ErrorCode) {}
  const std::wstring ToString();
};

class FileWriteException : public CalvinException
{
public:
  FileWriteException() : CalvinException() {}
  FileWriteException(std::wstring _Source, std::wstring _Description, std::wstring _TimeStamp, std::string _FileName, u_int16_t _LineNumber, u_int64_t _ErrorCode):
    CalvinException(_Source, _Description, _TimeStamp, _FileName,  _LineNumber, _ErrorCode) {}
  const std::wstring ToString();
};

};

#endif // _FileWriteException_HEADER_
