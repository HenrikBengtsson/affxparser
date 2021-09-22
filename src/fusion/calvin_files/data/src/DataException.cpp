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



#include "calvin_files/data/src/DataException.h"
//

namespace affymetrix_calvin_exceptions
{

  const  std::wstring DataSetNotOpenException::ToString() {
    return SystemInfo() + L"affymetrix_calvin_exceptions::DataSetNotOpenException thrown.";
  }
  const std::wstring ColumnIndexOutOfBoundsException::ToString() {
    return SystemInfo() + L"affymetrix_calvin_exceptions::ColumnIndexOutOfBoundsException thrown.";
  }

  const std::wstring  RowIndexOutOfBoundsException::ToString() {
    return SystemInfo() + L"affymetrix_calvin_exceptions::RowIndexOutOfBoundsException thrown.";
  }

  const std::wstring UnexpectedColumnTypeException::ToString() {
    return SystemInfo() + L"affymetrix_calvin_exceptions::UnexpectedColumnTypeException thrown.";
  }

  const std::wstring DataGroupNotFoundException::ToString() {
    return SystemInfo() + L"affymetrix_calvin_exceptions::DataGroupNotFoundException thrown.";
  }

  const std::wstring DataSetNotFoundException::ToString() {
    return SystemInfo() + L"affymetrix_calvin_exceptions::DataSetNotFoundException thrown.";
  }

  const std::wstring ProbeSetNotFoundException::ToString() {
    return SystemInfo() + L"affymetrix_calvin_exceptions::ProbeSetNotFoundException thrown.";
  }

  const std::wstring CDFAccessNotSupportedByModeException::ToString() {
    return SystemInfo() + L"affymetrix_calvin_exceptions::CDFAccessNotSupportedByModeException thrown.";
  }

  const std::wstring DataSetRemapException::ToString() {
    return SystemInfo() + L"affymetrix_calvin_exceptions::DataSetRemapException thrown.";
  }
  
}
