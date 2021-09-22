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
#ifndef _CHPData_HEADER_
#define _CHPData_HEADER_

/*! \file CHPData.h Defines a class to use as a base class for older CHP file data (expression, genotyping, reseq, universal). */

#include "calvin_files/data/src/CHPBackgroundZone.h"
#include "calvin_files/data/src/CHPExpressionEntry.h"
#include "calvin_files/data/src/CHPGenotypeEntry.h"
#include "calvin_files/data/src/CHPReseqEntry.h"
#include "calvin_files/data/src/CHPUniversalEntry.h"
#include "calvin_files/data/src/FileHeader.h"
#include "calvin_files/data/src/GenericData.h"
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
#include "calvin_files/utils/src/Coords.h"
//
#include <cstring>
#include <string>
//

namespace affymetrix_calvin_io
{

/*! The id for the expression CHP files. */
#define CHP_EXPRESSION_ASSAY_TYPE std::string("affymetrix-expression-probeset-analysis")

/*! The id for the expression CHP file data group. */
#define CHP_EXPR_GROUP std::wstring(L"Expression Results")

/*! The id for the resequencing CHP files. */
#define CHP_RESEQUENCING_ASSAY_TYPE std::string("affymetrix-resequencing-probeset-analysis")

/*! The id for the resequencing CHP file data group. */
#define CHP_RESEQ_GROUP std::wstring(L"Resequencing Results")

/*! The id for the genotyping CHP files. */
#define CHP_GENOTYPING_ASSAY_TYPE std::string("affymetrix-genotyping-probeset-analysis")

/*! The id for the genotyping CHP file data group. */
#define CHP_GENO_GROUP std::wstring(L"Genotyping Results")

/*! The id for the universal CHP files. */
#define CHP_UNIVERSAL_ASSAY_TYPE std::string("affymetrix-universal-probeset-analysis")

/*! The id for the universal CHP file data group. */
#define CHP_UNIV_GROUP std::wstring(L"Universal Results")

/*! The id for the prog ID. */
#define CHP_PROGID std::wstring(L"affymetrix-progid")

/*! The id for the parent cel file. */
#define CHP_PARENT_CELL std::wstring(L"affymetrix-parent-celfile")

/*! The group name for the background zone group. */
#define CHP_BG_ZONE_GROUP std::wstring(L"Background Zone Data")

/*! The group name for the force call group (for resequencing only). */
#define CHP_RESEQ_FORCE_CALL_GROUP std::wstring(L"Force Call Data")

/*! The group name for the orig call group (for resequencing only). */
#define CHP_RESEQ_ORIG_CALL_GROUP std::wstring(L"Orig Call Data")

/*! Defines a base class for older CHP file data. */ 
class CHPData
{
public:
	/*! Constructor */
	CHPData();

	/*! Constructor with file and type.
	 * @param filename The name of the CHP file.
	 * @param assayType The type of data in the CHP file.
	 */
	CHPData(const std::string& filename, const std::string &assayType);

	/*! Destructor */
	virtual ~CHPData();

protected:
	/*! Flag indicating if the probe set names were stored in wide character format. */
	bool wideProbeSetNames;
	/*! The maximum length of a probe set name. */
	int maxProbeSetName;
	/*! The generic file data object. */
	GenericData genericData;
	/*! keep rows from being read from the header all the time */
	int32_t cachedRows;
	/*! keep cols from being read from the header all the time */
	int32_t cachedCols;
	/*! expression entries DataSet */
	DataSet* entriesExp;
	/*! genotyping entries DataSet */
	DataSet* entriesGeno;
	/*! universal entries DataSet */
	DataSet* entriesUniv;
	/*! resequencing entries DataSet */
	DataSet* entriesReseq;
	/*! chp background zones DataSet */
	DataSet* bgZones;
	/*! chp force call DataSet */
	DataSet* forceSet;
	/*! chp orig DataSet */
	DataSet* origSet;

public:

	/*! Clears the members. */
	void Clear();
	
	/*! Sets the name of the CHP file. */
	void SetFilename(const std::string &p);

	/*! The name of the CHP file. */
	std::string GetFilename() const;

	/*! Gets the version in the file. */
	u_int8_t GetVersion();

	/*! Gets the files magic number. */
	u_int8_t GetMagic();

	/*! Gets the number of rows of features on the array. */
	int32_t GetRows();

	/*! Sets the number of rows of features on the array. */
	void SetRows(int32_t value);
	
	/*! Gets the number of columns of features on the array. */
	int32_t GetCols();

	/*! Sets the number of columns of features on the array. */
	void SetCols(int32_t value);
	
	/*! Gets the CHP file prog Id. */
	std::wstring GetProgId();

	/*! Sets the prog ID for the CHP file. */
	void SetProgId(const std::wstring& value);
	
	/*! Sets the array type */
	std::wstring GetArrayType();

	/*! Gets the assay type */
	std::string GetAssayType();

	/*! Sets the array type.  */
	void SetArrayType(const std::wstring& value);
	
	/*! Gets the algorithm name  */
	std::wstring GetAlgName();

	/*! Sets the algorithm name. */
	void SetAlgName(const std::wstring& value);
	
	/*! Gets the name of the parent CEL file. */
	std::wstring GetParentCell();

	/*! Sets the name of the parent CEL file */
	void SetParentCell(const std::wstring& value);
	
	/*! Gets the algorithm version.  */
	std::wstring GetAlgVersion();

	/*! Sets the algorithm version. */
	void SetAlgVersion(const std::wstring& value);

	/*! Adds a parameter to the alg parameters section */
	void AddAlgParam(const std::wstring& name, const std::wstring& param);

	/*! Adds a parameter to the alg parameters section */
	void AddAlgParam(const std::wstring& name, int param);

	/*! Adds a parameter to the alg parameters section */
	void AddAlgParam(const std::wstring& name, float param);

	/*! Gets the alg parameters */
	ParameterNameValueTypeVector GetAlgParams();

	/*! Gets a single algorithm parameter by name. */
	ParameterNameValueType GetAlgParam(const std::wstring& tag);

	/*! Adds a parameter to the chip summary section */
	void AddChipSum(const std::wstring& name, const std::wstring& param);

	/*! Adds a parameter to the chip summary section */
	void AddChipSum(const std::wstring& name, float param);

	/*! Adds a parameter to the chip summary section */
	void AddChipSum(const std::wstring& name, int param);

	/*! Gets all the chip summary parameters */
	ParameterNameValueTypeVector GetChipSums();

	/*! Gets a chip summary parameter by name  */
	ParameterNameValueType GetChipSum(const std::wstring& tag);

	/*! Gets the file header */
	FileHeader* GetFileHeader() { return &genericData.Header(); }

	/*! Gets the file data object. */
	GenericData& GetGenericData() { return genericData; }	// should be a friend method only

	/*! Sets the number of entries (probe sets)
	 * @param ln The number of probe sets.
	 * @param maxln The maximum length of a probe set name.
	 * @param hasCompData A flag for expression results, true if comparison data exists.
	 */
	void SetEntryCount(int32_t ln, int32_t maxln, bool hasCompData = false);

	/*! Gets the number of entries (probe sets) */
	int32_t GetEntryCount();

	/*! Gets the number of background zones. */
	int32_t GetBackgroundZoneCnt();

	/*! Sets the number of background zones. */
	void SetBackgroundZoneCnt(int32_t ln);

	/*! Gets CHP background zone value
	 *	@param row The row from which to start copying
	 *	@param zone The data object to be filled
	 */
	void GetBackgroundZone(int32_t row, CHPBackgroundZone& zone);

	/*! Gets the background zones. */
	void GetBackgroundZones(int32_t row, int32_t rowCnt, CHPBackgroundZoneVector& zones);

	/*! Gets the expression entry (probe set). */
	void GetEntry(int32_t row, CHPUniversalEntry& e);

	/*! Gets the expression entry (probe set). */
	void GetEntry(int32_t row, CHPGenotypeEntry& e);

	/*! Gets the expression entry (probe set). */
	void GetEntry(int32_t row, CHPExpressionEntry& e);

	/*! Gets the resequence entry. */
	void GetEntry(int32_t row, CHPReseqEntry& e);

	/*! Gets the number of force calls. */
	int32_t GetForceCnt();

	/*! Sets the number of force calls. */
	void SetForceCnt(int32_t ln);

	/*! Gets the force call value
	 *	@param row The row index
	 *	@param force The data object to be filled
	 */
	void GetForceCall(int32_t row, CHPReseqForceCall& force);

	/*! Gets the number of orig calls. */
	int32_t GetOrigCnt();

	/*! Sets the number of orig calls. */
	void SetOrigCnt(int32_t ln);

	/*! Gets the original call value from the orig set.
	 *	@param row The row index
	 *	@param orig The orginal call value.
	 */
	void GetOrigCall(int32_t row, CHPReseqOrigCall &orig);

	/*! The maximum length of a probe set name. */
	int GetMaxProbeSetName() const { return maxProbeSetName; }

protected:

	/*! Prepares the data set. */
	void PrepareGenoEntryDataSet();

	/*! Prepares the data set. */
	void PrepareExprEntryDataSet();

	/*! Prepares the data set. */
	void PrepareUnivEntryDataSet();

	/*! Prepares the data set. */
	void PrepareReseqEntryDataSet();

	/*! Adds columns to the data set. */
	void AddUnivColumns(DataSetHeader& hdr);

	/*! Adds columns to the data set. */
	void AddExprColumns(DataSetHeader& hdr, bool hasCompData);

	/*! Adds columns to the data set. */
	void AddGenoColumns(DataSetHeader& hdr);

	/*! Adds columns to the data set. */
	void AddReseqColumns(DataSetHeader& hdr);

	/*! Gets a string parameter to the header. */
	std::wstring GetWStringFromGenericHdr(const std::wstring& name);

	/*! Adds a string parameter to the header. */
	void SetWStringToGenericHdr(const std::wstring& name, const std::wstring value, int32_t reserve=-1);

	/*! Gets an integer parameter from the header. */
	int32_t GetInt32FromGenericHdr(const std::wstring& name);

	/*! Adds an integer parameter to the header. */
	void SetInt32ToGenericHdr(const std::wstring& name, int32_t value);

	/*! Adds an float parameter to the header. */
	void SetFloatToGenericHdr(const std::wstring& name, float value);

	/*! Prepares the data set for the bg set. */
	void PrepareBackgroundZoneDataSet();

	/*! Prepares the data set for the force call set. */
	void PrepareForceDataSet();

	/*! Prepares the data set for the orig call set. */
	void PrepareOrigDataSet();

	/*! Adds columns to the data set. */
	void AddColumns(DataSetHeader& hdr, bool hasCompData);
};

}

#endif
