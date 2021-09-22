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


#include "calvin_files/fusion/src/FusionCDFQCProbeSetNames.h"
//

using namespace affxcdf;
using namespace affymetrix_fusion_io;

/*! The probe set names for the control probe sets.
 * This array needs to be the same size as the number of values
 * defined in the GeneChipQC enumerant (in the GCOS File SDK CDFFileData.h file) .*/
std::wstring FusionCDFQCProbeSetNames::GetStaticCDFQCProbeSetName(GeneChipQCProbeSetType qcType)
{
	switch (qcType)
	{
	case UnknownQCProbeSetType:
		return L"UnknownQC";
		break;

	case CheckerboardNegativeQCProbeSetType:
		return L"CheckerboardNegativeQC";
		break;

	case CheckerboardPositiveQCProbeSetType:
		return L"CheckerboardPositiveQC";
		break;

	case HybNegativeQCProbeSetType:
		return L"HybNegativeQC";
		break;

	case HybPositiveQCProbeSetType:
		return L"HybPositiveQC";
		break;

	case TextFeaturesNegativeQCProbeSetType:
		return L"TextFeaturesNegativeQC";
		break;

	case TextFeaturesPositiveQCProbeSetType:
		return L"TextFeaturesPositiveQC";
		break;

	case CentralNegativeQCProbeSetType:
		return L"CentralNegativeQC";
		break;

	case CentralPositiveQCProbeSetType:
		return L"CentralPositiveQC";
		break;

	case GeneExpNegativeQCProbeSetType:
		return L"GeneExpNegativeQC";
		break;

	case GeneExpPositiveQCProbeSetType:
		return L"GeneExpPositiveQC";
		break;

	case CycleFidelityNegativeQCProbeSetType:
		return L"CycleFidelityNegativeQC";
		break;

	case CycleFidelityPositiveQCProbeSetType:
		return L"CycleFidelityPositiveQC";
		break;

	case CentralCrossNegativeQCProbeSetType:
		return L"CentralCrossNegativeQC";
		break;

	case CentralCrossPositiveQCProbeSetType:
		return L"CentralCrossPositiveQC";
		break;

	case CrossHybNegativeQCProbeSetType:
		return L"CrossHybNegativeQC";
		break;

	case CrossHybPositiveQCProbeSetType:
		return L"CrossHybPositiveQC";
		break;

	case SpatialNormalizationNegativeQCProbeSetType:
		return L"SpatialNormalizationNegativeQC";
		break;

	case SpatialNormalizationPositiveQCProbeSetType:
		return L"SpatialNormalizationPositiveQC";
		break;

	default:
		return L"UnknownQC";
		break;
	}
}
