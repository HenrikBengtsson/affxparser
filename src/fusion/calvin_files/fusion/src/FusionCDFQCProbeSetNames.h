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


#ifndef _FusionCDFQCProbeSetNames_HEADER_
#define _FusionCDFQCProbeSetNames_HEADER_

/*! \file FusionCDFQCProbeSetNames.h This file defines the QC probe set names for the GCOS QC probe sets. */

#include "file/CDFFileData.h"
//
#include <cstring>
#include <string>
//

namespace affymetrix_fusion_io
{

/*! This class provides the QC probe set names. */
class FusionCDFQCProbeSetNames
{
public:


	/*! The probe set names for the control probe sets.
	* @param qcType The type of qc probe set.
	* @return A string value .*/
	static std::wstring GetStaticCDFQCProbeSetName(affxcdf::GeneChipQCProbeSetType qcType);
};

}

#endif //_CDFQCProbeSetNames_HEADER_
