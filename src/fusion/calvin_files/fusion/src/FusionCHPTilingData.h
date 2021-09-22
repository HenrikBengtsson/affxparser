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


#if !defined _AffymetrixFusionTilingCHPData_HEADER_
#define _AffymetrixFusionTilingCHPData_HEADER_

/*! \file FusionCHPTilingData.h This file provides tiling CHP file reading capabilities.*/

#include "calvin_files/data/src/CHPTilingData.h"
#include "calvin_files/fusion/src/FusionCHPData.h"
//

namespace affymetrix_fusion_io
{

/*! This class provides storage and reading capabilities for tiling CHP files */
class FusionCHPTilingData : public FusionCHPData
{
private:

	/*! Constructor */
	FusionCHPTilingData();

	/*! The CHP object. */
	affymetrix_calvin_io::CHPTilingData chpData;

public:

	/*! Destructor */
	~FusionCHPTilingData();

	/*! Get the id of the file (only valid for Command Console "calvin" files)
	 * @return The unique file id.
	 */
	affymetrix_calvin_utilities::AffymetrixGuidType FileId();

	/*! Returns the GenericData object associated with a Calvin file, NULL for GCOS files. */
	affymetrix_calvin_io::GenericData *GetGenericData();

	/*! Gets the class name. */
	affymetrix_calvin_utilities::AffymetrixGuidType GetObjectName();

	/*! Gets the number of sequence stored in the file.
	 * @return The number of sequence stored in the file.
	 */
	int GetNumberSequences() { return chpData.GetNumberSequences(); }

	/*! Gets the name of the algorithm.
	 * @return The algorithm name.
	 */
	std::wstring GetAlgName() { return chpData.GetAlgName(); }

	/*! Gets the algorithm version.
	 * @return The version.
	 */
	std::wstring GetAlgVersion() { return chpData.GetAlgVersion(); }

	/*! Gets the algorithm parameters
	 * @return The algoirhtm parameters.
	 */
	ParameterNameValueTypeList GetAlgParams() { return chpData.GetAlgParams(); }

	/*! Gets the sequence data.
	 * @return The data associated with the sequence.
	 */
	TilingSequenceData GetTilingSequenceData() { return chpData.GetTilingSequenceData(); }

	/*! Gets the number of entries in a tiling sequence.
	 * @param index The sequence index.
	 * @return The number of entries in the sequence.
	 */
	int32_t GetTilingSequenceEntryCount(int index) { return chpData.GetTilingSequenceEntryCount(index); }

	/*! Opens a group for reading.
	 * @param index The index to the sequence.
	 */
	void OpenTilingSequenceDataSet(int index) { chpData.OpenTilingSequenceDataSet(index); }

	/*! Returns the entry for the given row. The data set must be open.
	 * @param row The row index.
	 * @param e The entry.
	 */
	void GetTilingSequenceEntry(int row, affymetrix_calvin_io::CHPTilingEntry& e) { chpData.GetTilingSequenceEntry(row, e); }


private:

	/*! Reads the CHP file.
	 * @return True if successful.
	 */
	bool Read();

	/*! Reads the header of the CHP file
	 * @return True if successful
	 */
	bool ReadHeader();

	/*! A class to register the tiling CHP reader. */
	class Reg : public FusionCHPDataReg
	{
	public:
		/*! Constructor - register the tiling file type. */
		Reg() : FusionCHPDataReg()
		{
			std::list<std::string> ids;
			ids.push_back(CHP_TILING_TYPE);
			SetFileTypeIds(ids);
		}

		/*! Creates a tiling CHP object.
		 * @return The tiling CHP object.
		 */
		FusionCHPData *MakeObject() { return new FusionCHPTilingData; }
	};

	/*! The one and only registration object. This registers the class as a CHP reader. */
	static Reg reg;

public:
	/*! Converts the type to the tiling CHP type.
	 * @param chip The pointer to the CHP data object.
	 * @return The tiling CHP data type or NULL if not compatible.
	 */
	static FusionCHPTilingData * FromBase(FusionCHPData *chip);
};

}

#endif
