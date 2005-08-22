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

#if !defined _AffymetrixFusionCHPData_HEADER_
#define _AffymetrixFusionCHPData_HEADER_

/*! \file FusionCHPData.h This file provides CHP file reading capabilities.*/

//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#pragma warning(disable: 4786) // identifier was truncated in the debug information
#endif

//////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include <list>
#include <fstream>

#include "CHPFileData.h"	// path required to distinguish between Calvin and GCOS files.
#include "CalvinCHPDataAdapter.h"
#include "FusionCHPDataAdapterInterface.h"

using namespace affxchp;

//////////////////////////////////////////////////////////////////////

namespace affymetrix_fusion_io
{
	class IFusionCHPDataAdapter;

//////////////////////////////////////////////////////////////////////

/*! This class provides storage for the CHP file header */
class FusionCHPHeader
{
	friend class FusionCHPData;
public:
	/*! Constructor */
	FusionCHPHeader();

	/*! Destructor */
	~FusionCHPHeader();

public:
	/*! Defines the assay type for the array */

public:
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
    * @param values The fusion tag value pair type list
	 */
	u_int32_t AlgorithmParameterCount();

	/*! Gets the summary parameters
	 * @return The summary parameters
	 */
	void SummaryParameters(FusionTagValuePairTypeList& p);

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
	void GetBackgroundZoneInfo(BackgroundZoneInfo& info);

	/*! Gets the list of background zone positions and values
	 * @return The list of background zone positions and values
	 */
	void GetBackgroundZones(BackgroundZoneTypeList& zones);

	/*! Gets the background value for a given center coordinate
	 * @return The background value for a given center coordinate
	 */
	void GetBackgroundZone(BackgroundZoneType& type, int x, int y);

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

	void FusionCHPHeader::Clear();

protected:
	IFusionCHPDataAdapter* adapter;
};

////////////////////////////////////////////////////////////////////

/*! This class provides storage and reading capabilities for CHP files */
class FusionCHPData  
{
public:
	/*! Constructor */
	FusionCHPData();

	/*! Destructor */
	~FusionCHPData();

protected:
	/*! Opens the file for reading.
	 * @param bReadHeaderOnly Flag to indicate if the header is to be read only.
	 * @return True if successful.
	 */
	bool Open(bool bReadHeaderOnly=false);

public:

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

	// Functions to read file.
	bool Read();

	/*! Reads the header of the CHP file
	 * @return True if successful
	 */
	bool ReadHeader();

	/*! Determines if the file specified by the FileName property exists.
	 * @return True if the file exists.
	 */
	bool Exists();

	/*! Sets the file name.
	 * @param name The full path to the CHP file
	 */
	void SetFileName(const char *name);

	/*! Gets the file name.
	 * @return The full path to the CHP file.
	 */
	std::string GetFileName() const;

	/*! Deallocates any memory used by the class object */
	void Clear();

	/*! Check the adapter and throw exception if not set
	 *	@exception FileNotOpenException
	 */
	void CheckAdapter() const;
	void CreateAdapter();
	void DeleteAdapter();

    FusionCHPHeader& GetHeader();


private:
		IFusionCHPDataAdapter* adapter;
		FusionCHPHeader header;
		std::string filename;

};

////////////////////////////////////////////////////////////////////

} // namespace

////////////////////////////////////////////////////////////////////

#endif // _AffymetrixFusionCHPData_HEADER_
