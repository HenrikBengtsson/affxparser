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

#include "ExceptionBase.h"

namespace affymetrix_calvin_exceptions
{

class DataSetNotOpenException : public CalvinException
{
};

class ColumnIndexOutOfBoundsException : public CalvinException
{
};

class RowIndexOutOfBoundsException : public CalvinException
{
};

class UnexpectedColumnTypeException : public CalvinException
{
};

class DataGroupNotFoundException : public CalvinException
{
};

class DataSetNotFoundException : public CalvinException
{
};

class ProbeSetNotFoundException : public CalvinException
{
};

class CDFAccessNotSupportedByModeException : public CalvinException
{
};

class DataSetRemapException : public CalvinException
{
};

}

#endif // _DataException_HEADER_
