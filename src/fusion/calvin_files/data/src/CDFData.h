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


/*! file CDFData.h This file contains classes and definitions for storing CDF file data. */

#ifndef _CDFData_HEADER_
#define _CDFData_HEADER_

#include "calvin_files/data/src/CDFProbeSetInformation.h"
#include "calvin_files/data/src/CDFQCProbeSetInformation.h"
#include "calvin_files/data/src/FileHeader.h"
#include "calvin_files/data/src/GenericData.h"
#include "calvin_files/parameter/src/AffymetrixParameterConsts.h"
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//
#include <cstring>
#include <map>
#include <string>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{

/*! The maximum probe set name length. */
#define MAX_CDF_PROBE_SET_NAME_LENGTH 32

/*! The group name for the control probe set types. */
#define CDF_QC_GROUP_LBL std::wstring(L"Control Probe Set Types")

/*! The group name for the probe set names. */
#define CDF_PS_GROUP_LBL std::wstring(L"Probe Set Names")

/*! The parameter name for the number of feature rows on the array. */
#define CDF_ROWS_PARAM std::wstring(L"ROWS")

/*! The parameter name for the number of feature columns on the array. */
#define CDF_COLS_PARAM std::wstring(L"COLS")

/*! The parameter name for the number of probe sets on the array. */
#define CDF_SETS_PARAM std::wstring(L"SETS")

/*! The parameter name for the reference sequence (for resequencing arrays). */
#define CDF_REFSEQ_PARAM std::wstring(L"REFSEQ")

/*! The parameter name for the unit type value. */
#define CDF_UNIT_TYPE std::wstring(L"Unit Type")

/*! The parameter name for the direction value. */
#define CDF_DIRECTION std::wstring(L"Direction")

/*! The parameter name for the number of atoms (probe pairs or quartets) value. */
#define CDF_ATOMS std::wstring(L"Atoms")

/*! The parameter name for the number of blocks value. */
#define CDF_BLOCKS std::wstring(L"Blocks")

/*! The parameter name for the number of cells value. */
#define CDF_CELLS std::wstring(L"Cells")

/*! The parameter name for the probe set number value. */
#define CDF_PROBE_SET_NUMBER std::wstring(L"Probe Set Number")

/*! The parameter name for the number of cells per atom value. */
#define CDF_CELLS_PER_ATOM std::wstring(L"Cells Per Atom")

/*! This class stores data from a Calvin CDF file. */
class CDFData
{
public:
	/*! Constructor */
	CDFData();

	/*! Constructor with file name as input. */
	CDFData(const std::string &filename);

	/*! Destructor */
	~CDFData();

public:
	/*! Clear member variables */
	void Clear();

	/*! Set filename.
	 * @param p The name of the file.
	 */
	void SetFilename(const std::string &p);

	/*! Get filename.
	 * @return The file name.
	 */
	std::string GetFilename();

	/*! Get format version.
	 * @return The CDF format version.
	 */
	int32_t GetFormatVersion();

	/*! Gets the probe set count.
	 * @return The number of probe sets.
	 */
	int32_t GetProbeSetCnt();

	/*! The number of probe sets.
	 * @return The number of probe sets.
	 */
	void SetProbeSetCnt(u_int32_t cnt, CDFDataTypeIds type);

	/*! Get the name of a probe set name based on index.
	 *  Use this method to get the probe set name for either a regular or QC probe set.
	 * @param index The index to the probe set. Must be between 0 and the number of probe sets - 1.
	 */
	std::wstring GetProbeSetName(int32_t index);

	/*! Sets the number of rows of features in the array.
	 * @param value The row count.
	 */
	void SetArrayRows(u_int32_t value);

	/*! The number of rows of features in the array.
	 * @return The row count.
	 */
	u_int32_t GetArrayRows();

	/*! Sets the number of columns of features in the array.
	 * @param value The column count.
	 */
	void SetArrayCols(u_int32_t value);

	/*! The number of columns of features in the array.
	 * @return The column count.
	 */
	u_int32_t GetArrayCols();

	/*! Sets the reference sequence for a resequencing array.
	 * @param seq The reference sequence.
	 */
	void SetRefSequence(const std::string &seq);

	/*! Gets the reference sequence.
	 * @return The reference sequence.
	 */
	std::string GetRefSequence();

	/*! Gets the data type identifier.
	 * @return The data type identifier.
	 */
	std::string GetDataTypeId();

	/*! Gets the file header object.
	 * @return The file header object.
	 */
	FileHeader* GetFileHeader() { return &genericData.Header(); }

	/*! Gets the generic data header.
	 * @return The generic data header.
	 */
	GenericData& GetGenericData() { return genericData; }

	/*! Gets the information about a probe set.
	 * @param index The index to the probe set. Must be between 0 and the number of probe sets - 1.
	 * @param info The probe set information.
	 * @exception CDFAccessNotSupportedByModeException
	 * @exception DataSetNotOpenException
	 */
	void GetProbeSetInformation(int32_t index, CDFProbeSetInformation& info);

	/*! Gets the information about a probe set.
	 * @param probeSetName The name of the probe set.
	 * @param info The probe set information.
	 * @exception CDFAccessNotSupportedByModeException
	 * @exception DataSetNotOpenException
	 * @exception ProbeSetNotFoundException
	 */
	void GetProbeSetInformation(const std::wstring& probeSetName, CDFProbeSetInformation& info);

	/*! Gets the information about a QC probe set.
	 * @param index The index of the QC probe set.
	 * @param info The probe set information.
	 * @exception CDFAccessNotSupportedByModeException
	 * @exception DataSetNotOpenException
	 */
	void GetQCProbeSetInformation(int32_t index, CDFQCProbeSetInformation& info);

	/*! Gets the information about a QC probe set.
	 * @param probeSetName The name of the QC probe set.
	 * @param info The probe set information.
	 * @exception CDFAccessNotSupportedByModeException
	 * @exception DataSetNotOpenException
	 * @exception ProbeSetNotFoundException
	 */
	void GetQCProbeSetInformation(const std::wstring& probeSetName, CDFQCProbeSetInformation& info);

private:
	/*! The type of access for parsing the file. */
	enum AccessMode {
		NoMode,				/*! No access mode. */
		SequentialMode,		/*! Sequencial access to probe set data. */
		ProbeSetIndexMode,	/*! Optimized for access given a probe set index. */
		ProbeSetNameMode	/*! Optimized for access given a probe set name. */
	};

	/*! The columns in the probe set name set. */
	enum TOCColumns
	{
		TOCProbeSetNameCol,	/*! The probe set name column. */
		TOCFilePosCol		/*! The file position column. */
	};

	/*! Create the data groups.
	 * @param p The group name.
	 * @param probesetCnt The number of probe sets.
	 */
	void CreateDataGroups(const std::wstring& p, int32_t probesetCnt);

	/*! Create the table of contents group indexes.
	 * @param p The group name.
	 * @param probesetCnt The number of probe sets.
	 */
	void CreateContentsGroup(const std::wstring& p, int32_t probesetCnt);

	/*! Create a data group for every probe set.
	 * @param probesetCnt The number of probe sets.
	 */
	void CreateProbeSetGroups(int32_t probesetCnt);

	/*! Sets the data type identifier.
	 * @param p The probe set type.
	 * @param probesetCnt The number of probe sets.
	 */
	void SetDataTypeId(CDFDataTypeIds p, int32_t probesetCnt);

	/*! Sets an integer parameter to the header.
	 * @param name The parameter name.
	 * @param value The parameter value.
	 */
	void SetUInt32ToGenericHdr(const std::wstring& name, u_int32_t value);

	/*! Gets an integer parameter from the header given the parameter name.
	 * @param name The parameter name.
	 */
	u_int32_t GetUInt32FromGenericHdr(const std::wstring& name);

	/*! Prepares the object for sequencial access. */
	void PrepareForSequentialAccess();

	/*! Prepares the object for probe set index access. */
	void PrepareForAccessByProbeSetIndex();

	/*! Prepares the object for probe set name access. */
	void PrepareForAccessByProbeSetName();

	/*! Gets the table of contents file position by the probe set index.
	 * @param index The probe set index.
	 * @exception DataSetNotOpenException
	 */
	u_int32_t GetTOCFilePosByIndex(int32_t index);

	/*! Gets the table of contents file position by probe set name.
	 * @param name The probe set name.
	 * @exception DataSetNotOpenException
	 * @exception ProbeSetNotFoundException
	 */
	u_int32_t GetTOCFilePosByName(const std::wstring& name);

private:

	/*! The generic data header. */
	GenericData genericData;

	/*! */
	bool setTocMetaData;

	/*! Access mode */
	AccessMode mode;

	/*! sequential index */
	int32_t curIndex;

	/*! file position of the current DataGroup */
	u_int32_t curGroupFilePos;

	/*! pointer to the table of contents DataSet */
	DataSet* tocDataSet;

	/*! map of probe set name to file position*/
	std::map<std::wstring, u_int32_t> nameToFilePosMap;

	/*! Friend to the reader. */
	friend class CDFFileReader;
};

}

#endif // _CDFData_HEADER_
