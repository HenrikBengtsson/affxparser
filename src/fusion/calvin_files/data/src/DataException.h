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


#ifndef _DataException_HEADER_
#define _DataException_HEADER_

/*! \file DataException.h This file provides class definition for the data exceptions.
 */

#include "calvin_files/exception/src/ExceptionBase.h"
//

namespace affymetrix_calvin_exceptions
{

class DataSetNotOpenException : public CalvinException
{
public:
  DataSetNotOpenException() : CalvinException() {}
  DataSetNotOpenException(std::wstring _Source, std::wstring _Description, std::wstring _TimeStamp, std::string _FileName, u_int16_t _LineNumber, u_int64_t _ErrorCode):
    CalvinException(_Source, _Description, _TimeStamp, _FileName,  _LineNumber, _ErrorCode) {}
  const std::wstring ToString();
};

class ColumnIndexOutOfBoundsException : public CalvinException
{
public:
  ColumnIndexOutOfBoundsException() : CalvinException() {}
  ColumnIndexOutOfBoundsException(std::wstring _Source, std::wstring _Description, std::wstring _TimeStamp, std::string _FileName, u_int16_t _LineNumber, u_int64_t _ErrorCode):
    CalvinException(_Source, _Description, _TimeStamp, _FileName,  _LineNumber, _ErrorCode) {}
  const std::wstring ToString();
};

class RowIndexOutOfBoundsException : public CalvinException
{
public:
  RowIndexOutOfBoundsException() : CalvinException() {}
  RowIndexOutOfBoundsException(std::wstring _Source, std::wstring _Description, std::wstring _TimeStamp, std::string _FileName, u_int16_t _LineNumber, u_int64_t _ErrorCode):
    CalvinException(_Source, _Description, _TimeStamp, _FileName,  _LineNumber, _ErrorCode) {}
  const std::wstring ToString();
};

class UnexpectedColumnTypeException : public CalvinException
{
public:
  UnexpectedColumnTypeException() : CalvinException() {}
  UnexpectedColumnTypeException(std::wstring _Source, std::wstring _Description, std::wstring _TimeStamp, std::string _FileName, u_int16_t _LineNumber, u_int64_t _ErrorCode):
    CalvinException(_Source, _Description, _TimeStamp, _FileName,  _LineNumber, _ErrorCode) {}
  const std::wstring ToString();
};

class DataGroupNotFoundException : public CalvinException
{
public:
  DataGroupNotFoundException() : CalvinException() {}
  DataGroupNotFoundException(std::wstring _Source, std::wstring _Description, std::wstring _TimeStamp, std::string _FileName, u_int16_t _LineNumber, u_int64_t _ErrorCode):
    CalvinException(_Source, _Description, _TimeStamp, _FileName,  _LineNumber, _ErrorCode) {}
  const std::wstring ToString();
};

class DataSetNotFoundException : public CalvinException
{
public:
  DataSetNotFoundException() : CalvinException() {}
  DataSetNotFoundException(std::wstring _Source, std::wstring _Description, std::wstring _TimeStamp, std::string _FileName, u_int16_t _LineNumber, u_int64_t _ErrorCode):
    CalvinException(_Source, _Description, _TimeStamp, _FileName,  _LineNumber, _ErrorCode) {}
    
  const std::wstring ToString();
};

class ProbeSetNotFoundException : public CalvinException
{
public:
  ProbeSetNotFoundException() : CalvinException() {}
  ProbeSetNotFoundException(std::wstring _Source, std::wstring _Description, std::wstring _TimeStamp, std::string _FileName, u_int16_t _LineNumber, u_int64_t _ErrorCode):
    CalvinException(_Source, _Description, _TimeStamp, _FileName,  _LineNumber, _ErrorCode) {}
  const std::wstring ToString();
};

class CDFAccessNotSupportedByModeException : public CalvinException
{
public:
  CDFAccessNotSupportedByModeException() : CalvinException() {}
  CDFAccessNotSupportedByModeException(std::wstring _Source, std::wstring _Description, std::wstring _TimeStamp, std::string _FileName, u_int16_t _LineNumber, u_int64_t _ErrorCode):
    CalvinException(_Source, _Description, _TimeStamp, _FileName,  _LineNumber, _ErrorCode) {}
  const std::wstring ToString();
};

class DataSetRemapException : public CalvinException
{
public:
  DataSetRemapException() : CalvinException() {}
  DataSetRemapException(std::wstring _Source, std::wstring _Description, std::wstring _TimeStamp, std::string _FileName, u_int16_t _LineNumber, u_int64_t _ErrorCode):
    CalvinException(_Source, _Description, _TimeStamp, _FileName,  _LineNumber, _ErrorCode) {}

  const std::wstring ToString();

};

}

#endif // _DataException_HEADER_
