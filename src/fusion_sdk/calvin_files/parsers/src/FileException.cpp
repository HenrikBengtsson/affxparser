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



#include "calvin_files/parsers/src/FileException.h"
//

namespace affymetrix_calvin_exceptions
{
  const  std::wstring FileNotFoundException::ToString() {
    return SystemInfo() + L"affymetrix_calvin_exceptions::FileNotFoundException thrown.";
  }
  const std::wstring InvalidVersionException::ToString() {
    return SystemInfo() + L"affymetrix_calvin_exceptions::InvalidVersionException thrown.";
  }

  const std::wstring InvalidFileTypeException::ToString() {
    return SystemInfo() + L"affymetrix_calvin_exceptions::InvalidFileTypeException thrown.";
  }

  const std::wstring UnableToOpenFileException::ToString() {
    return SystemInfo() + L"affymetrix_calvin_exceptions::UnableToOpenFileException thrown.";
  }

  const std::wstring FileNotOpenException::ToString() {
    return SystemInfo() + L"affymetrix_calvin_exceptions::FileNotOpenException thrown.";
  }
}
