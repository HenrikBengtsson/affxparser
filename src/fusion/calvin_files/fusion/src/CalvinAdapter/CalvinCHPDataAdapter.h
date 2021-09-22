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


#ifndef _AffymetrixCalvinCHPDataAdapter_HEADER_
#define _AffymetrixCalvinCHPDataAdapter_HEADER_

/*! \file CalvinCHPDataAdapter.h This file defines the GCOS Fusion CHP Data adapter classes. */

#include "calvin_files/data/src/CHPData.h"
#include "calvin_files/data/src/CHPExpressionEntry.h"
#include "calvin_files/data/src/CHPGenotypeEntry.h"
#include "calvin_files/data/src/CHPUniversalEntry.h"
#include "calvin_files/fusion/src/FusionCHPDataAdapterInterface.h"
#include "calvin_files/parameter/src/ParameterNameValueType.h"
#include "calvin_files/parsers/src/CHPFileReader.h"
//
#include "file/CHPFileData.h"	
//
#include <cstring>
#include <string>
//

using namespace affymetrix_calvin_io;

namespace affymetrix_fusion_io
{
/*! \brief Header class for the Calvin CHP adapter */
class CalvinCHPHeaderAdapter : public IFusionCHPHeaderAdapter
{
public:
	/*! \brief Constructor
	 *	\param chp Calvin CHP data object.
	 */
	CalvinCHPHeaderAdapter(CHPData* chp);
	/*! \brief Destructor */
	~CalvinCHPHeaderAdapter();

public:
	/*! \brief Get numbers columns.
	 *	\return Number of columns.
	 */
	virtual int GetCols() const;
	/*! \brief Get number of rows.
	 *	\return Number of rows.
	 */
	virtual int GetRows() const;
	/*! \brief Get the number of probesets.
	 *	\return Number of probesets.
	 */
	virtual int GetNumProbeSets() const;
	/*! \brief Get assay type.
	 *	\return Assay Type.
	 *
	 *	\a	Assay Types
	 *		FusionExpression	- Expression assay
	 *		FusionGenotyping	- Genotyping assay
	 *		FusionResequencing	- Resequencing assay
	 *		FusionUniversal		- Tag (universal) assay
	 *		FusionUnknown		- Unknown assay type
	*/
	virtual AssayType GetAssayType() const;
	/*! \brief Get chip type.
	 *	\return The chip type.
	 */
	virtual std::wstring GetChipType() const;
	/*! \brief Get algorithm name.
	 *	\return Algorithm name.
	 */
	virtual std::wstring GetAlgName() const;
	/*! \brief Get algorithm version.
	 *	\return Algorithm version number.
	 */
	virtual std::wstring GetAlgVersion() const;
	/*! \brief Gets the algorithm parameters.
	 *	\param values Name/Value type list to be filled.
	 */
	virtual void GetAlgorithmParameters(affymetrix_fusion_io::FusionTagValuePairTypeList& values);
	/*! \brief Gets the algorithm parameter count.
	 *	\return Number of algorithm parameters.
	 */
	virtual u_int32_t GetAlgorithmParameterCount();
	/*! \brief Gets the summary parameter count.
	 *	\return Number of summary parameters.
	 */
	virtual u_int32_t GetSummaryParameterCount();
	/*! \brief Gets summary parameters.
	 *	\param values Name/Value type list to be filled.
	 */
	virtual void GetSummaryParameters(affymetrix_fusion_io::FusionTagValuePairTypeList& values);
	/*! \brief Gets the parent CEL file.
	 *	\return Parent CEL file name.
	 */
	virtual std::wstring GetParentCellFile() const;
	/*! \brief Gets the prog ID.
	 *	\return Prog ID
	 */
	virtual std::wstring GetProgID() const;
	/*! \brief Gets a specific algorithm parameter given a name/tag.
	 *	\param tag Parameter name.
	 *	\return Specific algorithm parameter given a name/tag
	 */
	virtual std::wstring GetAlgorithmParameter(const wchar_t *tag);

	/*! \brief Gets a specific summary parameter given a name/tag.
	 *	\param tag Parameter name.
	 *	\return Specific summary parameter given a name/tag.
	 */
	virtual std::wstring GetSummaryParameter(const wchar_t *tag);

	/*! \brief Gets the background zone information.
	 *	\param info Background zone information to be filled.
	 */
	virtual void GetBackgroundZoneInfo(affxchp::BackgroundZoneInfo& info);

	/*! \brief Gets the list of background zone positions and values.
	 *	\param zones List of background zone positions and values to be filled.
	 */
	virtual void GetBackgroundZones(affxchp::BackgroundZoneTypeList& zones);

	/*! \brief Gets the background value for a given center coordinate.
	 *	\param type Background value for a given center coordinate to be filled.
	 *	\param x X position of zone.
	 *	\param y Y position of zone.
	 */
	virtual void GetBackgroundZone(affxchp::BackgroundZoneType& type, int x, int y);

	/*! \brief Gets the magic number.
	 *	\return Magic number.
	 */
	virtual int GetMagic() const;

	/*! \brief Gets the version number.
	 *	\return Version number
	 */
	virtual int GetVersion() const;


private:
	/*! Converts from ParameterNameValueTypeVector to FusionTagValuePairTypeList.
	 *	\param nvt Parameter name value type parir vector
	 *	\return Coverted vector to a FusionTagValuePairTypeList.
	 */
	FusionTagValuePairTypeList Convert(ParameterNameValueTypeVector& nvt);

	/*! A pointer to the root data access object */
	CHPData* calvinChp;
};

/*! \brief Data class for the Calvin CHP adapter */
class CalvinCHPDataAdapter : public IFusionCHPDataAdapter
{
public:
	/*! \brief Constructor */
	CalvinCHPDataAdapter();
	virtual ~CalvinCHPDataAdapter();

	/*! \brief Accessors to header.
	 *	\return Header object
	 */
	virtual IFusionCHPHeaderAdapter& GetHeader();

	/*! \brief Can this object read the file.
	 *	\return If the cell file can be read.
	 */
	virtual bool CanReadFile();

    /*! Get the probe set name  (only valid for Command Console "calvin" files)
	 * @param index The index to the result object of interest.
     * @return The probe set name.
     */
    virtual std::string GetProbeSetName(int index);

	/*! \brief Returns the expression probe set result
	 *	\param index Index to the result object of interest.
	 *	\param result Expression result.
	 *	\return True if the expression result was found.
	 */ 
	virtual bool GetExpressionResults(int index, affymetrix_fusion_io::FusionExpressionProbeSetResults& result);

	/*! \brief Returns the genotyping probe set result
	 *	\param index Index to the result object of interest.
	 *	\param result Genotyping result.
	 *	\return True if the genotyping result was found.
	 */
	virtual bool GetGenotypingResults(int index, FusionGenotypeProbeSetResults& result);

	/*! \brief Returns the universal (tag array) probe set result
	 *	\param index Index to the result object of interest.
	 *	\param result Universal result.
	 *	\return True if the universal result was found.
	 */
	virtual bool GetUniversalResults(int index, FusionUniversalProbeSetResults& result);

	/*! \brief Gets resequencing results.
	 *	\param results Hold the resequencing results.
	 *	\return True if resequencing results were retrieved.
	 */
	virtual bool GetResequencingResults(FusionResequencingResults& results);

	/*! \brief Functions to read file. 
	 *	\return True if the cell file was read.
	 */
	virtual bool Read();

	/*! \brief Reads the header of the CHP file.
	 *	\return True if successful
	 */
	virtual bool ReadHeader();

	/*! \brief Sets the file name.
	 *	\param value Full path to the CHP file
	 */
	virtual void SetFileName(const std::string& value);

	/*! \brief Gets the file name.
	 *	\return Full path to the CHP file.
	 */
	virtual std::string GetFileName() const;

	/*! \brief Deallocates any memory used by the class object. */
	virtual void Clear();

	/*! Get the id of the file (only valid for Command Console "calvin" files)
	 * @return The unique file id.
	 */
	affymetrix_calvin_utilities::AffymetrixGuidType FileId();

	/*! Returns the GenericData object associated with a Calvin file, NULL for GCOS files. */
	affymetrix_calvin_io::GenericData *GetGenericData();

protected:
	/*! The underlying data access object */
	CHPData calvinChp;

	/*! Header adapter */
	CalvinCHPHeaderAdapter* header;

};

}
#endif // _AffymetrixGCOSCHPDataAdapter_HEADER_
