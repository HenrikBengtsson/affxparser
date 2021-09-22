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


#if !defined _AffymetrixFusionCHPLegacyData_HEADER_
#define _AffymetrixFusionCHPLegacyData_HEADER_

/*! \file FusionCHPLegacyData.h This file provides CHP file reading capabilities.*/

//////////////////////////////////////////////////////////////////////

#include "calvin_files/fusion/src/FusionCHPData.h"
#include "calvin_files/fusion/src/FusionCHPDataAdapterInterface.h"
//
#include <cstring>
#include <fstream>
#include <list>
#include <string>
#include <vector>
//

//////////////////////////////////////////////////////////////////////

namespace affymetrix_fusion_io
{

class IFusionCHPDataAdapter;

//////////////////////////////////////////////////////////////////////

/*! This class provides storage for the CHP file header */
class FusionCHPHeader
{
public:
	/*! Constructor */
	FusionCHPHeader();

	/*! Destructor */
	~FusionCHPHeader();

	/*! Gets the number of feature columns
	 * @return The number of feature columns
	 */
	int GetCols() const;

	/*! Gets the number of feature rows
	 * @return The number of feature rows
	 */
	int GetRows() const;

	/*! Gets the number of probe sets
	 * @return The number of probe sets
	 */
	int GetNumProbeSets() const;

	/*! Gets the assay type
	 * @return The assay type
	 */
	AssayType GetAssayType() const;

	/*! Gets the chip type
	 * @return The chip type
	 */
	std::wstring GetChipType() const;

	/*! Gets the algorithm name
	 * @return The algorithm name
	 */
	std::wstring GetAlgName() const;

	/*! Gets the algorithm version
	 * @return The algorithm version
	 */
	std::wstring GetAlgVersion() const;

	/*! Gets the algorithm parameters
    * @param values The fusion tag value pair type list
	 */
	void AlgorithmParameters(FusionTagValuePairTypeList& values);

	/*! Gets the algorithm parameters count
    * @return The number of algorithm parameters.
	 */
	u_int32_t AlgorithmParameterCount();

	/*! Gets the summary parameters
	 * @return The summary parameters
	 */
	void SummaryParameters(FusionTagValuePairTypeList& p);

	/*! Gets the summary parameters count
    * @return The number of summary parameters.
	 */
	u_int32_t SummaryParameterCount();

	/*! Gets the parent CEL file
	 * @return The parent CEL file
	 */
	std::wstring GetParentCellFile() const;

	/*! Gets the prog ID
	 * @return The prog ID
	 */
	std::wstring GetProgID() const;

	/*! Gets a specific algorithm parameter given a name/tag
	 * @return The specific algorithm parameter given a name/tag
	 */
	std::wstring GetAlgorithmParameter(const wchar_t* tag);

	/*! Gets a specific summary parameter given a name/tag
	 * @return The specific summary parameter given a name/tag
	 */
	std::wstring GetSummaryParameter(const wchar_t* tag);

	/*! Gets the background zone information
	 * @return The background zone information
	 */
	void GetBackgroundZoneInfo(affxchp::BackgroundZoneInfo& info);

	/*! Gets the list of background zone positions and values
	 * @return The list of background zone positions and values
	 */
	void GetBackgroundZones(affxchp::BackgroundZoneTypeList& zones);

	/*! Gets the background value for a given center coordinate
	 * @return The background value for a given center coordinate
	 */
	void GetBackgroundZone(affxchp::BackgroundZoneType& type, int x, int y);

	/*! Gets the magic number
	 * @return The magic number
	 */
	int GetMagic() const;

	/*! Gets the version number
	 * @return The version number
	 */
	int GetVersion() const;

	/*! Check the adapter and throw exception if not set
	 *	@exception FileNotOpenException
	 */
	void CheckAdapter() const;

	/*! Clears the members. */
	void Clear();

protected:

	/* Friend to the data object. */
	friend class FusionCHPLegacyData;

	/*! The adapter interface. */
	IFusionCHPDataAdapter* adapter;
};

////////////////////////////////////////////////////////////////////

/*! This class provides storage and reading capabilities for CHP files */
class FusionCHPLegacyData : public FusionCHPData
{
private:

	/*! Constructor */
	FusionCHPLegacyData();

public:

	/*! Destructor */
	~FusionCHPLegacyData();

	/*! Get the id of the file (only valid for Command Console "calvin" files)
	 * @return The unique file id.
	 */
	affymetrix_calvin_utilities::AffymetrixGuidType FileId();

	/*! Returns the GenericData object associated with a Calvin file, NULL for GCOS files. */
	affymetrix_calvin_io::GenericData *GetGenericData();

	/*! Gets the class name. */
	affymetrix_calvin_utilities::AffymetrixGuidType GetObjectName();

    /*! Get the probe set name  (only valid for Command Console "calvin" files of expression or genotype assay)
	 * @param index The index to the result object of interest.
     * @return The probe set name.
     */
    std::string GetProbeSetName(int index);

	/*! Returns the expression probe set result
	 * @param index The index to the result object of interest.
	 * @param result The expression result.
	 * @return True if the expression result was found.
	 */ 
	bool GetExpressionResults(int index, FusionExpressionProbeSetResults& result);

	/*! Returns the genotyping probe set result
	 * @param index The index to the result object of interest.
	 * @param result The genotyping result.
	 * @return True if the genotyping result was found.
	 */
	bool GetGenotypingResults(int index, FusionGenotypeProbeSetResults& result);

	/*! Returns the universal (tag array) probe set result
	 * @param index The index to the result object of interest.
	 * @param result The universal result.
	 * @return True if the universal result was found.
	 */
	bool GetUniversalResults(int index, FusionUniversalProbeSetResults& result);

	/*! Returns the resequencing results.
	 * @param results The results.
	 * @return True if the reseq result was found.
	 */
	bool GetReseqResults(FusionResequencingResults &results);

	/*! Deallocates any memory used by the class object */
	void Clear();

	/*! Gets the headre. */
    FusionCHPHeader& GetHeader();

private:

	/*! Opens the file for reading.
	 * @param bReadHeaderOnly Flag to indicate if the header is to be read only.
	 * @return True if successful.
	 */
	bool Open(bool bReadHeaderOnly=false);

	/*! Check the adapter and throw exception if not set
	 *	@exception FileNotOpenException
	 */
	void CheckAdapter() const;

	/*! Create the adapter. */
	void CreateAdapter();

	/*! Delete the adapter. */
	void DeleteAdapter();

	/*! Reads the CHP file.
	 * @return True if successful.
	 */
	bool Read();

	/*! Reads the header of the CHP file
	 * @return True if successful
	 */
	bool ReadHeader();

	/*! The fusion CHP adapter. */
	IFusionCHPDataAdapter* adapter;

	/*! The CHP header */
	FusionCHPHeader header;

	/*! A class to register the legacy CHP reader. */
	class Reg : public FusionCHPDataReg
	{
	public:
 		/*! Constructor - register the legacy file type. */
                Reg(); 

		/*! Creates a legacy CHP object.
		 * @return The legacy CHP object.
		 */
		FusionCHPData *MakeObject() { return new FusionCHPLegacyData; }
	};

	/*! The one and only registration object. This registers the class as a CHP reader. */
	static Reg reg;

public:
	/*! Converts the type to the legacy CHP type.
	 * @param chip The pointer to the CHP data object.
	 * @return The legacy CHP data type or NULL if not compatible.
	 */
	static FusionCHPLegacyData * FromBase(FusionCHPData *chip);

};

////////////////////////////////////////////////////////////////////

} // namespace

////////////////////////////////////////////////////////////////////

#endif

