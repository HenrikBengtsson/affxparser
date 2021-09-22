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

/*! \file MDLFileData.h This file provides reading capaibilities for MDL files.
 */

#ifndef _MdlFileData_h_
#define _MdlFileData_h_

//////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning(disable: 4786) // identifier was truncated in the debug information
#include <windows.h>
#endif

//////////////////////////////////////////////////////////////////////

#include "portability/affy-base-types.h"
//
#include <cstring>
#include <string>
#include <vector>
//

//////////////////////////////////////////////////////////////////////

namespace affxmdl
{

/*! Model version */
#define PLIER_MODEL_FILE_VERSION	1

/*! Reserved size */
#define RESERVED_SIZE 1020

/*! Analysis type */
enum MDLAnalysisType
{
	/*! PLIER analysis */
	MDL_PLIER=0
};

//////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
/*! Use one byte alignment for memory mapping functionality */
#pragma pack(push, 1)
#endif

/*! PLIER model file header structure */
typedef struct _MDLHeader
{
	int32_t	nVersion;					/*! Model file version number */
	int32_t	nNumProbes;					/*! Number pf probes */
	int32_t	nAnalysisType;				/*! PLIER analysis type */
	char	sChipType[64];			/*! Probe array type */
	char	sModifiedDate[36];		/*!Last modified date */
	int32_t	bMasked;					/*! Model file generated with masking */
	char	sReserve[RESERVED_SIZE]; /*! Reserved for future use */
} MDLHeader;

#ifdef _MSC_VER
/*! End of one byte alignment */
#pragma pack(pop)
#endif

/*! This is the size of the object */
#define MDLHEADERSIZE sizeof(MDLHeader)

/*! PLIER model file header class */
class CMDLFileHeader
{
protected:
	int32_t		m_nVersion;				/*! Model file version number */
	int32_t		m_nNumProbes;			/*! Number pf probes */
	int32_t		m_nAnalysisType;		/*! PLIER analysis type */
	bool		m_bMasked;			/*! Model file generated with masking */
	std::string	m_strChipType;		/*! Probe array type */
	std::string	m_strModifiedDate;	/*! Last modified date */
	std::string	m_strReserve;		/*! Reserved for future use */

	friend class CMDLFileData; /*! Friend to the parent class */

public:

	/*! Operator for Header assignment
	 * @param rhs The header object to copy
	 * @return The copied header
	 */
	CMDLFileHeader &operator=(CMDLFileHeader &rhs);

	/*! The version number
	 * @return The version number.
	 */
	int32_t GetVersion() const					{ return m_nVersion; }

	/*! The number of probes
	 * @return The number of probes */
	int32_t GetNumProbes() const				{ return m_nNumProbes; }

	/*! The analysis type
	 * @return The analysis type */
	int32_t GetAnalysisType() const				{ return m_nAnalysisType; }

	/*! The mask flag
	 * @return The mask flag
	 */
	bool GetMasked() const					{ return m_bMasked; }

	/*! The chip type
	 * @return The chip type
	 */
	std::string GetChipType() const			{ return m_strChipType; }

	/*! The modified date
	 * @return The modified date
	 */
	std::string GetModifiedDate() const		{ return m_strModifiedDate; }

	/*! Reserved */
	std::string GetReserve() const			{ return m_strReserve; }

	/*! Sets the version number
	 * @param value The version number */
	void SetVersion(int32_t value)				{ m_nVersion = value; }

	/*! Sets the number of probes
	 * @param value The number of probes */
	void SetNumProbes(int32_t value)			{ m_nNumProbes = value; }

	/*! Sets the analysis type
	 * @param value The analysis type */
	void SetAnalysisType(int32_t value)			{ m_nAnalysisType = value; }

	/*! Sets the mask flag
	 * @param value The mask flag */
	void SetMasked(bool value)				{ m_bMasked = value; }

	/*! Sets the chip type
	 * @param value The chip type */
	void SetChipType(const char *value)		{ m_strChipType = value; }

	/*! Sets the modification date
	 * @param value The date */
	void SetModifiedDate(const char *value)	{ m_strModifiedDate = value; }

	/*! Constructor
	 * @param nProbes The number of probes
	 * @param analysisType The type of analysis
	 */
	CMDLFileHeader(int32_t nProbes=0, int32_t analysisType=MDL_PLIER);
};

//////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
/*! Use one byte alignment for memory mapping functionality */
#pragma pack(push, 1)
#endif

/*! PLIER model data structure */
typedef struct _MDLData
{
	double	Affinity;				/*! Affinity value */
	float	RelativeBkg;			/*! Reserved for future use */
	float	Saturation;				/*! Reserved for future use */
	float	Offset;					/*! Reserved for future use */
} MDLData;

#ifdef _MSC_VER
/*! End of one byte alignment */
#pragma pack(pop)
#endif

/*! This is the size of the object */
#define MDLDATASIZE sizeof(MDLData)

/*! PLIER model probe data class */
class CMDLProbeData
{
protected:
	double	m_dAffinity;			/*! Probe affinity value */
	float	m_fRelativeBkg;			/*! Reserved for future use */
	float	m_fSaturation;			/*! Reserved for future use */
	float	m_fOffset;				/*! Reserved for future use */

	int32_t		m_nIndex;				/*! An index for the object */
	bool	m_bMapped;				/*! Flag indicating if memory mapped */
	char	*m_lpData;				/*! Memory map pointer */

	friend class CMDLFileData;		/*! Friend to parent object */

	/*! Copies the class object, one member at a time
	 * @param orig The object to copy
	 */
	void MakeShallowCopy(CMDLProbeData &orig);

public:

	/*! The affinity value
	 * @return The affinity value
	 */
	double GetAffinity() const		{ return m_dAffinity; }

	/*! The relative background
	 * @return The relative background
	 */
	float GetRelativeBkg() const	{ return m_fRelativeBkg; }

	/*! The saturation
	 * @return The saturation
	 */
	float GetSaturation() const		{ return m_fSaturation; }

	/*! The offset
	 * @return The offset
	 */
	float GetOffset() const			{ return m_fOffset; }

	/*! Sets the affinity
	 * @param value The affinity
	 */
	void SetAffinity(double value)		{ m_dAffinity = value; }

	/*! Sets the relative background
	 * @param value The relative background
	 */
	void SetRelativeBkg(float value)	{ m_fRelativeBkg = value; }

	/*! Sets the saturation
	 * @param value The saturation
	 */
	void SetSaturation(float value)		{ m_fSaturation = value; }

	/*! Sets the offset
	 * @param value The offset
	 */
	void SetOffset(float value)			{ m_fOffset = value; }

	/*! Constructor */
	CMDLProbeData();
};

//////////////////////////////////////////////////////////////////////

/*! PLIER model file stores the model data as the structure defined above.
 (1)	It is probe array type specific. Only the model file
		  with the same probe array type can be used for PLIER analysis
 (2)	It stores the model data for each probe (or cells).
 (3)	It starts storing model data for all PM probes and follows by
			all MM probes.
 (4)	Sum(a(j)) = # of probes
*/
class CMDLFileData
{
protected:
	/*! The file header */
	CMDLFileHeader	m_Header;

	/*! The file name */
	std::string		m_strFileName;

	/*! Stores an error message */
	std::string		m_strError;

	/*! The probe data */
	std::vector<CMDLProbeData> m_ProbeData;

	/*! Gets the header
	 * @param pHeader The header data
	 */
	void GetHeaderInfo(MDLHeader* pHeader);

	/*! Sets the header
	 * @param pHeader The header data
	 */
	void SetHeaderInfo(MDLHeader* pHeader);

	/*! Sets the data in the class
	 * @param pData The data
	 * @param data The probe data
	 */
	void SetDataInfo(MDLData* pData, CMDLProbeData *data);

	/*! Gets the data from the class
	 * @param pData The data
	 * @param data The probe data
	 */
	void GetDataInfo(MDLData* pData, CMDLProbeData &data);

	/*! Opens the file
	 * @param bReadHeaderOnly Flag to read only the header
	 * @return True if successful
	 */
	bool Open(bool bReadHeaderOnly=false);

	/*! Reads the file
	 * @param bReadHeaderOnly Flag to read only the header
	 * @return True if successful
	 */
	bool ReadFile(bool bReadHeaderOnly=false);

	/*! Reads the file header
	 * @param instr The file stream.
	 * @return True if successful
	 */
	bool ReadFileHeader(std::ifstream &instr);

#if defined(_USE_MEM_MAPPING_) && defined(_MSC_VER)
	bool ReadFileUsingMemMap(bool bReadHeaderOnly);
	void  *m_lpFileMap;
	char  *m_lpData;
	HANDLE m_hFileMap;
	HANDLE m_hFile;
	bool m_bFileOpen;
	bool m_bFileMapped;
	int32_t m_nHeaderOffset;
#endif

public:

	/*! Sets the file name
	 * @param value The file name
	 */
	void SetFileName(const char *value)			{ m_strFileName = value; }

	/*! Returns the file name
	 * @return The file name
	 */
	std::string GetFileName() const				{ return m_strFileName; }

	/*! The error message
	 * @return The error message
	 */
	std::string GetError() const				{ return m_strError; }

	/*! Returns the header
	 * @return The file header object
	 */
	CMDLFileHeader &GetHeader() { return m_Header; }

	/*! Sets the header
	 * @param header The header to save
	 */
	void SetHeader(CMDLFileHeader& header) { m_Header = header; }

	/*! Gets the probe data
	 * @param index Index to the probe data object
	 * @param data The probe data
	 */
	void GetProbeData(int32_t index, CMDLProbeData& data);

	/*! Sets the probe data
	 * @param index Index to the probe data object
	 * @param data The probe data
	 */
	void SetProbeData(int32_t index, CMDLProbeData data);

	/*! Reads the entire file
	 * @return True if successful
	 */
	bool Read();

	/*! Reads the file header only
	 * @return True if successful
	 */
	bool ReadHeader();

	/*! Writes the file
	 * @return True if successful
	 */
	bool Write();

	/*! Checks if the file exists
	 * @return True if the file exists
	 */
	bool Exists();

	/*! Closes the file */
	void Close();

	/*! Initializes the object for writing
	 * @param nSize The size
	 * @param defAffinity The default affynity value
	 * @param analysisType The analysis type
	 */
	void InitializeForWriting(int32_t nSize=0, double defAffinity=1.0, int32_t analysisType=MDL_PLIER);

	/*! Outputs the file context for debugging
	 * @param sFile The output file name
	 * @param nProbes The number of probes
	 * @return True if successful
	 */
	bool PrintMDLToTextFile(const char* sFile, int32_t nProbes);

	/*! Constructor */
	CMDLFileData(int32_t nSize=0, double defAffinity=1.0, int32_t analysisType=MDL_PLIER);

	/*! Destructor */
	~CMDLFileData();
};

////////////////////////////////////////////////////////////////////

} // namespace

#endif // _MdlFileData_h_

