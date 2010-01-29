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

#if !defined(AFX_CHPFILEWRITER_H__A16FCA8B_7B8D_4071_81CD_F6B2BB61A169__INCLUDED_)
#define AFX_CHPFILEWRITER_H__A16FCA8B_7B8D_4071_81CD_F6B2BB61A169__INCLUDED_

/*! \file CHPFileWriter.h This file provides CHP file writing capabilities.
 */

//////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning(disable: 4786) // identifier was truncated in the debug information
#endif

//////////////////////////////////////////////////////////////////////

#include "file/CDFFileData.h"
#include "file/CHPFileData.h"
//
#include <cstring>
#include <fstream>
#include <list>
#include <string>
#include <vector>
//

//////////////////////////////////////////////////////////////////////

namespace affxchpwriter
{

//////////////////////////////////////////////////////////////////////

/*! This class is used for writing CHP files. */
class CCHPFileWriter : public affxchp::CCHPFileData
{
public:
	/*! Constructor */
	CCHPFileWriter();

	/*! Destructor */
	~CCHPFileWriter();

protected:
	/*! The output file stream object */
	std::ofstream m_NewChpFile;

	/*! A flag to indicate the first entry is being saved. */
	int m_ProbeSetIndex;

public:
	/*! Creates the new file (empty) given the file name.
	 * @return True if successful
	 */
	bool CreateNewFile();

	/*! Allocates memory for the results objects and prepares the object for writing.
	 * @param cdf The associated CDF file.
	 * @param allocateMemory Allocates memory for the results.
	 */
	void InitializeForWriting(affxcdf::CCDFFileData &cdf, bool allocateMemory=true);

	/*! Allocates memory for the results objects and prepares the object for writing.
	 * @param numRows The number of rows of features on the array.
	 * @param numCols The number of columns of features on the array.
	 * @param numProbeSets The number of probe sets.
	 * @param chipType The chip type or probe array type.
	 * @param probeSetType The type of probe sets in the file.
	 * @param allocateMemory Allocates memory for the results.
	 */
	void InitializeForWriting(int numRows, int numCols, int numProbeSets, const char *chipType, affxcdf::GeneChipProbeSetType probeSetType, bool allocateMemory=true);

	/*! Sets the parent CEL file name.
	 * @param str The parent CEL file name (full path).
	 */
	void SetParentCelFileName(const char *str);

	/*! Sets the prog ID of the algorithm used to create the CHP file data.
	 * @param str The prog ID.
	 */
	void SetProgID(const char *str);

	/*! Sets the algorithm name used to create the CHP file data.
	 * @param str The name of the algorithm.
	 */
	void SetAlgorithmName(const char *str);

	/*! Sets the algorithm version used to create the CHP file data.
	 * @param str The algorithm version.
	 */
	void SetAlgorithmVersion(const char *str);

	/*! Adds a parameter name/value pair to the list of algorithm parameters
	 * @param tag The name of the parameter
	 * @param value The value of the parameter
	 */
	void AddAlgorithmParameter(const char *tag, const char *value);

	/*! Adds a parameter name/value pair to the list of summary parameters
	 * @param tag The name of the parameter
	 * @param value The value of the parameter
	 */
	void AddChipSummaryParameter(const char *tag, const char *value);

	/*! Sets the expression probe set result
	 * @param index The index to the probe set results
	 * @param pEntry A pointer to the results
	 */
	void SetExpressionEntry(int index, affxchp::CExpressionProbeSetResults *pEntry);

	/*! Saves the results to the file.
	 * @param pEntry A pointer to the results.
	 */
	bool SaveExpressionEntry(affxchp::CExpressionProbeSetResults *pEntry);

	/*! Sets the genotyping probe set result
	 * @param index The index to the probe set results
	 * @param pEntry A pointer to the results
	 */
	void SetMappingEntry(int index, affxchp::CGenotypeProbeSetResults *pEntry);

	/*! Saves the results to the file.
	 * @param pEntry A pointer to the results.
	 */
	bool SaveMappingEntry(affxchp::CGenotypeProbeSetResults *pEntry);

	/*! Sets the universal probe set result
	 * @param index The index to the probe set results
	 * @param pEntry A pointer to the results
	 */
	void SetUniversalEntry(int index, affxchp::CUniversalProbeSetResults *pEntry);

	/*! Saves the results to the file.
	 * @param pEntry A pointer to the results.
	 */
	bool SaveUniversalEntry(affxchp::CUniversalProbeSetResults *pEntry);

	/*! Sets the resequencing results
	 * @param pResults A pointer to the results
	 */
	void SetResequencingResults(affxchp::CResequencingResults *pResults);

	/*! Saves the results to the file.
	 * @return True if successful
	 */
	bool Save();

	/*! Saves the results to the file.
	 * @return True if successful
	 */
	bool SaveHeader();

	/*! Closes the file.
	 * @return True if successful */
	bool Close();

	/*! Initializes the object which stores the background zone information
	 * @param nZones The number of zones
	 * @param smoothFactor The smooth factor used by the background algorithm.
	 */
	void AddBackgroundInfo(int nZones, float smoothFactor);

	/*! Adds a background zone to the list
	 * @param x The central x coordinate of the background zone.
	 * @param y The central y coordinate of the background zone.
	 * @param value The background value for the zone.
	 */
	void AddBackgroundZone(int x, int y, float value);
};

////////////////////////////////////////////////////////////////////

} // namespace

////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_CHPFILEWRITER_H__A16FCA8B_7B8D_4071_81CD_F6B2BB61A169__INCLUDED_)
