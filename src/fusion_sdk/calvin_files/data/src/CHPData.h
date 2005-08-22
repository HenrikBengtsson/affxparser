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
#ifndef _CHPData_HEADER_
#define _CHPData_HEADER_

#include <string>
#include "FileHeader.h"
#include "GenericData.h"
#include "Coords.h"
#include "AffymetrixBaseTypes.h"
#include "CHPBackgroundZone.h"
#include "CHPExpressionEntry.h"
#include "CHPGenotypeEntry.h"
#include "CHPUniversalEntry.h"

#ifdef WIN32
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{

#define CHP_EXPRESSION_ASSAY_TYPE std::string("affymetrix-expression-probeset-analysis")
#define CHP_GENOTYPING_ASSAY_TYPE std::string("affymetrix-genotyping-probeset-analysis")
#define CHP_RESEQUENCING_ASSAY_TYPE std::string("affymetrix-resequencing-probeset-analysis")
#define CHP_UNIVERSAL_ASSAY_TYPE std::string("affymetrix-universal-probeset-analysis")

#define CHP_EXPRESSION_ARRAY_TYPE std::wstring(L"affymetrix-expression-probeset-analysis")
#define CHP_GENOTYPING_ARRAY_TYPE std::wstring(L"affymetrix-genotyping-probeset-analysis")
#define CHP_RESEQUENCING_ARRAY_TYPE std::wstring(L"affymetrix-resequencing-probeset-analysis")
#define CHP_UNIVERSAL_ARRAY_TYPE std::wstring(L"affymetrix-universal-probeset-analysis")

#define CHP_ROWS std::wstring(L"Rows")
#define CHP_COLS std::wstring(L"Cols")
#define CHP_PROGID std::wstring(L"ProgId")
#define CHP_ARRAY_TYPE std::wstring(L"ArrayType")
#define CHP_ALG_NAME std::wstring(L"AlgName")
#define CHP_ALG_VERSION std::wstring(L"AlgVersion")
#define CHP_ALG_PARAM std::wstring(L"AlgParam-")
#define CHP_CHIP_SUM std::wstring(L"ChipSummary-") 
#define CHP_PARENT_CELL std::wstring(L"ParentCellFile")
#define CHP_EXPR_GROUP std::wstring(L"Expression Results")
#define CHP_GENO_GROUP std::wstring(L"Genotyping Results")
#define CHP_RESEQ_GROUP std::wstring(L"Resequencing Results")
#define CHP_UNIV_GROUP std::wstring(L"Universal Results")
#define CHP_BG_ZONE_GROUP std::wstring(L"Background Zone Data")

class CHPData
{
public:

	CHPData();

	CHPData(const std::string& filename, const std::string& type);

	~CHPData();

private:

	GenericData genericData;

	/*! keep rows from being read from the header all the time */
	int32_t cachedRows;
	/*! keep cols from being read from the header all the time */
	int32_t cachedCols;
	/*! chp entries DataSet */
	DataSet* entries;
	/*! chp background zones DataSet */
	DataSet* bgZones;

public:

	/*!  */
	void Clear();
	/*!  */
	void SetFilename(const std::string &p);
	/*!  */
	std::string GetFilename() const;

	u_int8_t GetVersion();

	u_int8_t GetMagic();
		/*!  */
	int32_t GetRows();
	/*!  */
	void SetRows(int32_t value);
	/*!  */
	int32_t GetCols();
	/*!  */
	void SetCols(int32_t value);
	/*!  */
	std::wstring GetProgId();
	/*!  */
	void SetProgId(const std::wstring& value);
	/*!  */
	std::wstring GetArrayType();
	/*!  */
	std::string GetAssayType();
	/*!  */
	void SetArrayType(const std::wstring& value);
	/*!  */
	std::wstring GetAlgName();
	/*!  */
	void SetAlgName(const std::wstring& value);
	/*!  */
	std::wstring GetParentCell();
	/*!  */
	void SetParentCell(const std::wstring& value);
	/*!  */
	std::wstring GetAlgVersion();
	/*!  */
	void SetAlgVersion(const std::wstring& value);
	/*!  */
	void AddAlgParam(const std::wstring& name, const std::wstring& param);
	/*!  */
	ParameterNameValueTypeVector CHPData::GetAlgParams();
	/*!  */
	ParameterNameValueType GetAlgParam(const std::wstring& tag);
	/*!  */
	void AddChipSum(const std::wstring& name, const std::wstring& param);
	/*!  */
	ParameterNameValueTypeVector GetChipSums();
	/*!  */
	ParameterNameValueType GetChipSum(const std::wstring& tag);
	/*!  */
	FileHeader* GetFileHeader() { return &genericData.Header(); }
	/*!  */
	GenericData& GetGenericData() { return genericData; }	// should be a friend method only
	/*!  */
	void SetEntryCount(int32_t ln);
	/*!  */
	int32_t GetEntryCount();

	/*! Gets CHP value
	 *	@param row The row from which to start copying
	 *	@param entry The data object to be filled
	 */
	//void GetEntry(int32_t row, CHPEntry& entry);

	//void GetEntries(int32_t row, int32_t rowCnt, CHPEntryVector& entries);

	void GetExpressionEntry(int32_t row, CHPExpressionEntry& e);

	void GetGenotypeEntry(int32_t row, CHPGenotypeEntry& e);

	void GetUniversalEntry(int32_t row, CHPUniversalEntry& e);

	int32_t GetBackgroundZoneCnt();

	void SetBackgroundZoneCnt(int32_t ln);

	/*! Gets CHP background zone value
	 *	@param row The row from which to start copying
	 *	@param zone The data object to be filled
	 */
	void GetBackgroundZone(int32_t row, CHPBackgroundZone& zone);

	void GetBackgroundZones(int32_t row, int32_t rowCnt, CHPBackgroundZoneVector& zones);

private:

	void InsertDataSetHeader(const DataSetHeader &hdr);

	void UpdateDataSetRowCount(const DataSetHeader &hdr);

	std::wstring GetWStringFromGenericHdr(const std::wstring& name);

	void SetWStringToGenericHdr(const std::wstring& name, const std::wstring value);

	int32_t GetInt32FromGenericHdr(const std::wstring& name);

	void SetInt32ToGenericHdr(const std::wstring& name, int32_t value);

	void PrepareUnivEntryDataSet();

	void PrepareExprEntryDataSet();

	void PrepareGenoEntryDataSet();

	void PrepareBackgroundZoneDataSet();

	void AddExpressionColumns(DataSetHeader& hdr);

	void AddResequencingColumns(DataSetHeader& hdr);

	void AddUniversalColumns(DataSetHeader& hdr);

	void AddGenotypingColumns(DataSetHeader& hdr);

	void AddColumns(DataSetHeader& hdr);
};

}

#endif // _FileHeader_HEADER_
