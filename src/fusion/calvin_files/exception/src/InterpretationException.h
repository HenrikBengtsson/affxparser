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


#ifndef _InterpretationException_HEADER_
#define _InterpretationException_HEADER_

#include "calvin_files/exception/src/ExceptionBase.h"
//

/*! \file InterpretationException.h This file defines exceptions that are the result 
 *  of the system not being able to interpret input.
*/

namespace affymetrix_calvin_exceptions
{

class FormatException : public CalvinException
{
public:
  FormatException() : CalvinException() {}
  FormatException(std::wstring _Source, std::wstring _Description, std::wstring _TimeStamp, std::string _FileName, u_int16_t _LineNumber, u_int64_t _ErrorCode):
    CalvinException(_Source, _Description, _TimeStamp, _FileName,  _LineNumber, _ErrorCode) {}
  const std::wstring ToString();
};

}

#endif
