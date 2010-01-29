////////////////////////////////////////////////////////////////
//
// Copyright (C) 2006 Affymetrix, Inc.
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


#ifndef _FusionCHPQuantificationDetectionData_HEADER_
#define _FusionCHPQuantificationDetectionData_HEADER_

/*! \file FusionCHPQuantificationDetectionData.h This file provides CHP file reading capabilities for CHP files with a quantification/detection pair.*/

#include "calvin_files/data/src/CHPQuantificationDetectionData.h"
#include "calvin_files/fusion/src/FusionCHPData.h"
//

namespace affymetrix_fusion_io
{

/*! This class provides storage and reading capabilities for quantification/detection CHP files */
class FusionCHPQuantificationDetectionData : public FusionCHPData
{
private:

	/*! Constructor */
	FusionCHPQuantificationDetectionData();

	/*! The CHP object. */
	affymetrix_calvin_io::CHPQuantificationDetectionData chpData;

public:

	/*! Destructor */
	~FusionCHPQuantificationDetectionData();

	/*! Get the id of the file (only valid for Command Console "calvin" files)
	 * @return The unique file id.
	 */
	affymetrix_calvin_utilities::AffymetrixGuidType FileId();

	/*! Returns the GenericData object associated with a Calvin file, NULL for GCOS files. */
	affymetrix_calvin_io::GenericData *GetGenericData();

    /*! Returns the data set header. */
    affymetrix_calvin_io::DataSetHeader &GetDataSetHeader() { return chpData.GetDataSetHeader(); }

	/*! Gets the class name. */
	affymetrix_calvin_utilities::AffymetrixGuidType GetObjectName();

	/*! Gets the name of the algorithm.
	 * @return The algorithm name.
	 */
	std::wstring GetAlgName() { return chpData.GetAlgName(); }

	/*! Gets the algorithm version.
	 * @return The version.
	 */
	std::wstring GetAlgVersion() { return chpData.GetAlgVersion(); }

	/*! Sets the array type */
	std::wstring GetArrayType() { return chpData.GetArrayType(); }

	/*! Gets the algorithm parameters
	 * @return The algoirhtm parameters.
	 */
	ParameterNameValueTypeList GetAlgParams() { return chpData.GetAlgParams(); }

	/*! Gets the summary parameters
	 * @return The summary parameters.
	 */
	ParameterNameValueTypeList GetSummaryParams() { return chpData.GetSummaryParams(); }

	/*! Gets the number of entries (probe sets) */
	int32_t GetEntryCount() { return chpData.GetEntryCount(); }

	/*! Gets the sequence data.
	 * @param index The row index.
	 * @param entry The quantification/detection value.
	 */
	void GetQuantificationDetectionEntry(int index, affymetrix_calvin_data::ProbeSetQuantificationDetectionData &entry) { chpData.GetQuantificationDetectionEntry(index, entry); }

private:

	/*! Reads the CHP file.
	 * @return True if successful.
	 */
	bool Read();

	/*! Reads the header of the CHP file
	 * @return True if successful
	 */
	bool ReadHeader();

public:
	/*! A class to register the quantification/detection CHP reader. */
	class Reg : public FusionCHPDataReg
	{
	public:
		/*! Constructor - register the quantification/detection file type. */
		Reg() : FusionCHPDataReg()
		{
			std::list<std::string> ids;
			ids.push_back(CHP_QUANTIFICATION_DETECTION_TYPE);
			SetFileTypeIds(ids);
		}

		/*! Creates a quantification/detection CHP object.
		 * @return The quantification/detection CHP object.
		 */
		FusionCHPData *MakeObject() { return new FusionCHPQuantificationDetectionData; }
	};

	/*! The one and only registration object. This registers the class as a CHP reader. */
	static Reg reg;

public:
	/*! Converts the type to the quantification/detection CHP type.
	 * @param chip The pointer to the CHP data object.
	 * @return The quantification/detection CHP data type or NULL if not compatible.
	 */
	static FusionCHPQuantificationDetectionData * FromBase(FusionCHPData *chip);
};

}

#endif
