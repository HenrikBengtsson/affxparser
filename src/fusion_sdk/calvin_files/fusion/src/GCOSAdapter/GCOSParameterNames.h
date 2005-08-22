/////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
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

#ifndef _GCOSParameterNames_HEADER_
#define _GCOSParameterNames_HEADER_

namespace affymetrix_fusion_io
{

// Should we move these to their own file?
/*! The parameter name for DATIO7 ImageData::fArcRadius (float) */
const wchar_t ARCRADIUS_NAME[] = L"gcos-fusion-arcradius";
/*! The parameter name for DATIO7 ImageData::fLaserSpotSize (float) */
const wchar_t LASER_SPOTSIZE_NAME[] = L"gcos-fusion-laser-spotsize";
/*! The parameter name for DATIO7 ImageData::SetExperimentName() (text) */
const wchar_t EXPERIMENT_NAME_NAME[] = L"gcos-fusion-experiment-name";	// awkward naming
/*! The parameter name format for DATIO7 ImageData::GetChipInfo() (text)
 *	This is a simulated parameter.  It's value is derived from many other 
 *	parameter values.  This parameter will not appear in the iteration
 *	returned by GetParameterIts.
 */
const wchar_t GET_CHIP_INFO_NAME[] = L"gcos-fusion-get-chipinfo";
/*! The parameter name for DATIO7 ImageData::GetScanInfo() (text)
 *	This is a simulated parameter.  It's value is derived from many other 
 *	parameter values.  This parameter will not appear in the iteration
 *	returned by GetParameterIts.
 */
const wchar_t GET_SCAN_INFO_NAME[] = L"gcos-fusion-get-scaninfo";
/*! The parameter name format for DATIO7 ImageData::SetChipInfo(int) and ImageData::GetChipInfo(int) (text)
 *	The value for the variable part of the name may be between 0-9 inclusive
 *	Not sure that this is beig used.
 */
const wchar_t CHIPINFO_NAME_FORMAT[] = L"gcos-fusion-set-chipinfo-%d";

}

#endif	//_GCOSParameterNames_HEADER_