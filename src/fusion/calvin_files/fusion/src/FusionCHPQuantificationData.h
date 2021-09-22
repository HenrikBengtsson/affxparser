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


#ifndef _FusionCHPQuantificationData_HEADER_
#define _FusionCHPQuantificationData_HEADER_

/*! \file FusionCHPQuantificationData.h This file provides CHP file reading capabilities for CHP files with a single quantification value.*/

#include "calvin_files/data/src/CHPQuantificationData.h"
#include "calvin_files/fusion/src/FusionCHPData.h"
//

namespace affymetrix_fusion_io
{

/*! This class provides storage and reading capabilities for quantification CHP files */
class FusionCHPQuantificationData : public FusionCHPData
{
private:

	/*! Constructor */
	FusionCHPQuantificationData();

	/*! The CHP object. */
	affymetrix_calvin_io::CHPQuantificationData chpData;

public:

	/*! Destructor */
	~FusionCHPQuantificationData();

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
	 * @param entry The quantification value.
	 */
	void GetQuantificationEntry(int index, affymetrix_calvin_data::ProbeSetQuantificationData &entry) { chpData.GetQuantificationEntry(index, entry); }

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
	/*! A class to register the quantification CHP reader. */
	class Reg : public FusionCHPDataReg
	{
	public:
		/*! Constructor - register the quantification file type. */
		Reg() : FusionCHPDataReg()
		{
			std::list<std::string> ids;
			ids.push_back(CHP_QUANTIFICATION_TYPE);
			SetFileTypeIds(ids);
		}

		/*! Creates a quantification CHP object.
		 * @return The quantification CHP object.
		 */
		FusionCHPData *MakeObject() { return new FusionCHPQuantificationData; }
	};

	/*! The one and only registration object. This registers the class as a CHP reader. */
	static Reg reg;

public:
	/*! Converts the type to the quantification CHP type.
	 * @param chip The pointer to the CHP data object.
	 * @return The quantification CHP data type or NULL if not compatible.
	 */
	static FusionCHPQuantificationData * FromBase(FusionCHPData *chip);
};

}

#endif
