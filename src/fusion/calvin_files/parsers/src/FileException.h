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


#ifndef _FileException_HEADER_
#define _FileException_HEADER_

/*! \file FileException.h This file provides class definition for the file exceptions.
 */

#include "calvin_files/exception/src/ExceptionBase.h"
//

namespace affymetrix_calvin_exceptions
{

class FileNotFoundException : public CalvinException
{
public:
  FileNotFoundException() : CalvinException() {}
  FileNotFoundException(std::wstring _Source, std::wstring _Description, std::wstring _TimeStamp, std::string _FileName, u_int16_t _LineNumber, u_int64_t _ErrorCode):
    CalvinException(_Source, _Description, _TimeStamp, _FileName,  _LineNumber, _ErrorCode) {}
    
  const std::wstring ToString();
};

class InvalidVersionException : public CalvinException
{
public:
  InvalidVersionException() : CalvinException() {}
  InvalidVersionException(std::wstring _Source, std::wstring _Description, std::wstring _TimeStamp, std::string _FileName, u_int16_t _LineNumber, u_int64_t _ErrorCode):
    CalvinException(_Source, _Description, _TimeStamp, _FileName,  _LineNumber, _ErrorCode) {}
    
  const std::wstring ToString();
};

class InvalidFileTypeException : public CalvinException
{
public:
  InvalidFileTypeException() : CalvinException() {}
  InvalidFileTypeException(std::wstring _Source, std::wstring _Description, std::wstring _TimeStamp, std::string _FileName, u_int16_t _LineNumber, u_int64_t _ErrorCode):
    CalvinException(_Source, _Description, _TimeStamp, _FileName,  _LineNumber, _ErrorCode) {}
    
  const std::wstring ToString();
};

class UnableToOpenFileException : public CalvinException
{
public:
  UnableToOpenFileException() : CalvinException() {}
  UnableToOpenFileException(std::wstring _Source, std::wstring _Description, std::wstring _TimeStamp, std::string _FileName, u_int16_t _LineNumber, u_int64_t _ErrorCode):
    CalvinException(_Source, _Description, _TimeStamp, _FileName,  _LineNumber, _ErrorCode) {}
    
  const std::wstring ToString();
};

class FileNotOpenException : public CalvinException
{
public:
  FileNotOpenException() : CalvinException() {}
  FileNotOpenException(std::wstring _Source, std::wstring _Description, std::wstring _TimeStamp, std::string _FileName, u_int16_t _LineNumber, u_int64_t _ErrorCode):
    CalvinException(_Source, _Description, _TimeStamp, _FileName,  _LineNumber, _ErrorCode) {}
    
  const std::wstring ToString();
};

}

#endif // _FileException_HEADER_
