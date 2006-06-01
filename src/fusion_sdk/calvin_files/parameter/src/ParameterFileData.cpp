/////////////////////////////////////////////////////////////////
//
// Copyright (C) 2006 Affymetrix, Inc.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License,
// or (at your option) any later version.
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
/////////////////////////////////////////////////////////////////

#include "ParameterFileData.h"

using namespace affymetrix_calvin_parameter;

/*
 * Clear the members.
 */
ParameterFileData::ParameterFileData()
{
    Clear();
}

/*
 * Clear the members.
 */
void ParameterFileData::Clear()
{
    parameters.clear();

    parameterFileAttributes.company.clear();
    parameterFileAttributes.userName.clear();
    parameterFileAttributes.contentVersion.clear();

    implementationAttributes.name.clear();
    implementationAttributes.version.clear();
    implementationAttributes.executableFileName.clear();
    implementationAttributes.description.clear();
}
