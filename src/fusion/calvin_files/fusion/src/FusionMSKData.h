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


#ifndef _AffymetrixFusionMSKData_HEADER_
#define _AffymetrixFusionMSKData_HEADER_

/*! \file FusionMSKData.h This file defines the Fusion MSK Data classes
 */

#include "file/MSKFileData.h"
//
using namespace affxmsk;

namespace affymetrix_fusion_io
{

/*! Storage for the MSK file. */
class FusionMSKFile : public CMSKFileData
{
};

}

#endif
