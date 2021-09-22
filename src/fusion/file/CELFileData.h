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

#ifndef _CELFILEDATA_H_
#define _CELFILEDATA_H_

// file io options

// enable reading of ".gz" celfiles.
// #define CELFILE_USE_ZLIB 1
#ifndef CELFILE_USE_ZLIB
#define CELFILE_USE_STDSTREAM 1
#endif

//////////
// uint32_t and friends
#include "file/FileIO.h"
#include "file/GridCoordinates.h"
#include "file/TagValuePair.h"
//
#include "portability/affy-base-types.h"
//
#include <cstring>
#include <map>
#include <string>
//
#ifdef CELFILE_USE_ZLIB
#ifndef FILEIO_WITH_ZLIB
#error CELFILE_USE_ZLIB requires FILEIO_WITH_ZLIB
#endif
#endif
#if defined (__CYGWIN__)
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
///  Structure alignment requirement for g++
#define STRUCT_ALIGNMENT __attribute__ ((packed))
#define PAGE_SHIFT	12
///  Set page size value for memory mapping used under CYGWIN
#define PAGE_SIZE	(1UL << PAGE_SHIFT)
///  Set page mask value for memory mapping used under CYGWIN
#define PAGE_MASK	(~(PAGE_SIZE-1))
#elif defined (_MSC_VER)
#include <windows.h>
///  Structure alignment requirement for g++
///  @remark Structure alignment for Visual C++ is included in #pragma
#define STRUCT_ALIGNMENT
#elif defined (__APPLE__)
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/user.h>
///  Structure alignment requirement for g++
///  @remark Structure alignment for Mac OS X is included in #pragma
#define STRUCT_ALIGNMENT
#else // UNIX 
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/user.h>
///  Structure alignment requirement for g++
#define STRUCT_ALIGNMENT __attribute__ ((packed))
#endif

namespace affxcel
{

#ifdef _MSC_VER 
#pragma pack(push, 1)
#endif
#ifdef __APPLE__
#pragma options align=packed
#endif
///////////////////////////////////////////////////////////////////////////////
///  typedef as CELFileEntryType
///  @brief Structure of CEL file entries for text and xda format
///////////////////////////////////////////////////////////////////////////////
typedef struct _CELFileEntryType
{
	///  Intensity
	float Intensity /* \cond */ STRUCT_ALIGNMENT /*! \endcond */ ;
	///  Standard deviation intensity
	float Stdv /* \cond */ STRUCT_ALIGNMENT /*! \endcond */ ;
	///  Number of pixels
	short Pixels /* \cond */ STRUCT_ALIGNMENT /*! \endcond */ ;
} CELFileEntryType;

///////////////////////////////////////////////////////////////////////////////
///  typedef as CELFileTranscriptomeEntryType
///  @brief Structure of CEL file entries for bcel format
///////////////////////////////////////////////////////////////////////////////
typedef struct _CELFileTranscriptomeEntryType
{
	///  Intensity
	unsigned short Intensity /* \cond */ STRUCT_ALIGNMENT /*! \endcond */ ;
	///  Standard deviation intensity
	unsigned short Stdv /* \cond */ STRUCT_ALIGNMENT /*! \endcond */ ;
	///  Number of pixels
  // The STRUCT_ALIGNMENT isnt needed for gcc.
  // and if used it generates a warning.
	unsigned char Pixels /* \cond */ /* STRUCT_ALIGNMENT */ /*! \endcond */ ;
} CELFileTranscriptomeEntryType;

#ifdef _MSC_VER
#pragma pack(pop)
#endif
#ifdef __APPLE__
#pragma options align=reset
#endif

///////////////////////////////////////////////////////////////////////////////
///  affxcel::CCELFileHeaderData
///  @brief CEL file header information
///
///  @remarks Object instance is included in affxcel::CCELFileData
///////////////////////////////////////////////////////////////////////////////
class CCELFileHeaderData
{
private:
	///  Magic number for identifying XDA format
	int m_Magic;
	///  CEL file format version number
	int m_Version;
	///  Number of columns in array
	int m_nCols;
	///  Number of rows in array
	int m_nRows;
	///  Number of cells in array
	int m_nCells;
	///  Header information concatenated in a string
	std::string m_Header;
	///  Algorithm name
	std::string m_Alg;
	///  Algorithm parameters
	std::string m_Params;
	///  Chip type of array
	std::string m_ChipType;
	///  DAT header string
	std::string m_DatHeader;
	///  Cell margin
	int m_Margin;
	///  Number of outliers
  	uint32_t m_nOutliers;
	///  Number of masked cells
  	uint32_t m_nMasked;
	///  Grid coordinates of array
	GridCoordinatesType m_CellGrid;

	///  STL map of algorithm parameters
	std::map<std::string, std::string> m_Parameters;
	///  STL map of algorithm parameter keys
	std::map<int, std::string> m_ParameterIndices;

public:
	/*! Constructor */
	CCELFileHeaderData();

	/*! Destructor */
	~CCELFileHeaderData();

	/*! Clears the members */
	void Clear();

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  SetMagic
	///  @brief Set magic number for xda format
	///  @param  i int  	Magic number
	///  @return void	
	///////////////////////////////////////////////////////////////////////////////
	void SetMagic(int i) { m_Magic = i; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetMagic
	///  @brief Retrieve magic number from xda format
	///  @return int	Magic number
	///////////////////////////////////////////////////////////////////////////////
	int GetMagic() { return m_Magic; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  SetVersion
	///  @brief Set file format version number
	///  @param  i int  	Version number
	///  @return void	
	///////////////////////////////////////////////////////////////////////////////
	void SetVersion(int i) { m_Version = i; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetVersion
	///  @brief Retrieve fiel format version number
	///  @return int	Version number
	///////////////////////////////////////////////////////////////////////////////
	int GetVersion() { return m_Version; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  SetCols
	///  @brief Set number of columns in array
	///  @param  i int  	Number of columns
	///  @return void	
	///////////////////////////////////////////////////////////////////////////////
	void SetCols(int i) { m_nCols = i; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetCols
	///  @brief Retrieve number of columns in array
	///  @return int	Number of columns
	///////////////////////////////////////////////////////////////////////////////
	int GetCols() { return m_nCols; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  SetRows
	///  @brief Set number of rows in array
	///  @param  i int  	Number of rows
	///  @return void	
	///////////////////////////////////////////////////////////////////////////////
	void SetRows(int i) { m_nRows = i; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetRows
	///  @brief Retrieve number of rows in array
	///  @return int	Number of rows
	///////////////////////////////////////////////////////////////////////////////
	int GetRows() { return m_nRows; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  SetCells
	///  @brief Set number of cells in array
	///  @param  i int  	Number of cells
	///  @return void	
	///////////////////////////////////////////////////////////////////////////////
	void SetCells(int i) { m_nCells = i; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetCells
	///  @brief Retrieve number of cells in array
	///  @return int	Number of cells
	///////////////////////////////////////////////////////////////////////////////
	int GetCells() { return m_nCells; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  AppendHeader
	///  @brief Append data to header string
	///  @param  p const char * 	Header information
	///  @return void	
	///////////////////////////////////////////////////////////////////////////////
	void AppendHeader(const char* p) { m_Header.append(p); }
	
	///////////////////////////////////////////////////////////////////////////////
	///  inline public  SetHeader
	///  @brief Set header information of CEL File in a single string
	///  @param  p const char * 	Header string
	///  @return void	
	///////////////////////////////////////////////////////////////////////////////
	void SetHeader(const char* p) { m_Header = p; }

	///////////////////////////////////////////////////////////////////////////////
	///  public  GetHeader
	///  @brief Concatenate header data into a string and return it
	///  @return std::string	Header string
	///////////////////////////////////////////////////////////////////////////////
	std::string GetHeader();

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  SetAlg
	///  @brief Set algorithm name
	///  @param  p const char * 	Algorithm name
	///  @return void	
	///////////////////////////////////////////////////////////////////////////////
	void SetAlg(const char* p) { m_Alg = p; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetAlg
	///  @brief Retrieve algorithm name
	///  @return std::string	Algorithm name
	///////////////////////////////////////////////////////////////////////////////
	std::string GetAlg() { return m_Alg; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  SetParams
	///  @brief Set algorithm parameters
	///  @param  p const char * 	Algorithm parameters
	///  @return void	
	///////////////////////////////////////////////////////////////////////////////
	void SetParams(const char* p) { m_Params = p; ParseAlgorithmParameters(); }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetParams
	///  @brief Retrieve algorithm parameters
	///  @return std::string	Algorithm parameters
	///////////////////////////////////////////////////////////////////////////////
	std::string GetParams() { return m_Params; }

	///////////////////////////////////////////////////////////////////////////////
	///  public  ParseChipType
	///  @brief Parse chip type of array from header string
	///  @return void	
	///////////////////////////////////////////////////////////////////////////////
	void ParseChipType();

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  SetChipType
	///  @brief Set chip type of array
	///  @param  p const char * 	Chip type
	///  @return void	
	///////////////////////////////////////////////////////////////////////////////
	void SetChipType(const char* p) { m_ChipType = p; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetChipType
	///  @brief Retrieve chip type of array
	///  @return std::string	Chip type
	///////////////////////////////////////////////////////////////////////////////
	std::string GetChipType() { return m_ChipType; }

	///////////////////////////////////////////////////////////////////////////////
	///  public  ParseDatHeader
	///  @brief Parse DAT header from header string
	///  @return void	
	///////////////////////////////////////////////////////////////////////////////
	void ParseDatHeader();
	
	///////////////////////////////////////////////////////////////////////////////
	///  public overloaded  SetDatHeader
	///  @brief Set default DAT header
	///  @return void	
	///////////////////////////////////////////////////////////////////////////////
	void SetDatHeader();

	///////////////////////////////////////////////////////////////////////////////
	///  inline public overloaded  SetDatHeader
	///  @brief Set DAT header with input string
	///  @param  p const char * 	DAT header
	///  @return void	
	///////////////////////////////////////////////////////////////////////////////
	void SetDatHeader(const char* p) { m_DatHeader = p; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetDatHeader
	///  @brief Retrieve DAT header
	///  @return std::string	DAT header
	///////////////////////////////////////////////////////////////////////////////
	std::string GetDatHeader() { return m_DatHeader; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  SetMargin
	///  @brief Set cell margin
	///  @param  i int  	Cell margin
	///  @return void	
	///////////////////////////////////////////////////////////////////////////////
	void SetMargin(int i);

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetMargin
	///  @brief Retrieve cell margin
	///  @return int	Cell margin
	///////////////////////////////////////////////////////////////////////////////
	int GetMargin() { return m_Margin; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  SetOutliers
	///  @brief Set number of outliers
	///  @param  l unsigned int32_t  	Number of outliers
	///  @return void	
	///////////////////////////////////////////////////////////////////////////////
	void SetOutliers(uint32_t l) { m_nOutliers = l; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetOutliers
	///  @brief Retrieve number of outliers
	///  @return unsigned int32_t	Number of outliers
	///////////////////////////////////////////////////////////////////////////////
  	uint32_t GetOutliers() { return m_nOutliers; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  IncrementOutliers
	///  @brief Increment number of outliers by 1
	///  @return void	
	///////////////////////////////////////////////////////////////////////////////
	void IncrementOutliers() { m_nOutliers++; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  DecrementOutliers
	///  @brief Decrement number of outliers by 1
	///  @return void	
	///////////////////////////////////////////////////////////////////////////////
	void DecrementOutliers() { m_nOutliers--; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  SetMasked
	///  @brief Set number of masked cells
	///  @param  l unsigned int32_t  	Number of masked cells
	///  @return void	
	///////////////////////////////////////////////////////////////////////////////
	void SetMasked(uint32_t l) { m_nMasked = l; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetMasked
	///  @brief Retrieve number of masked cells
	///  @return uint32_t	Number of masked cells
	///////////////////////////////////////////////////////////////////////////////
	uint32_t GetMasked() { return m_nMasked; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  IncrementMasked
	///  @brief Increment number of masked cells by 1
	///  @return void	
	///////////////////////////////////////////////////////////////////////////////
	void IncrementMasked() { m_nMasked++; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  DecrementMasked
	///  @brief Decrement number of masked cells by 1
	///  @return void	
	///////////////////////////////////////////////////////////////////////////////
	void DecrementMasked() { m_nMasked--; }

	/*! Parses the algorithm parameters from the parameter string */
	void ParseAlgorithmParameters();

	/*! Adds a parameter to the list
	 * @param tag The parameter name.
	 * @param value The parameter value.
	 */
	void AddAlgorithmParameter(std::string& tag, std::string& value);

	/*! Adds a parameter to the list
	 * @param tag The parameter name.
	 * @param value The parameter value.
	 */
	void AddAlgorithmParameter(const char *tag, const char *value);

	/*! Updates the parameter value.
	 * @param tag The parameter name.
	 * @param value The parameter value.
	 */
	void SetAlgorithmParameter(const char *tag, const char *value);

	/*! Set or Update the parameter value.
	 * @param tag The parameter name.
	 * @param value The parameter value.
	 */
	void SetAddAlgorithmParameter(std::string& tag, std::string& value);

	/*! Set or Update the parameter value.
	 * @param tag The parameter name.
	 * @param value The parameter value.
	 */
	void SetAddAlgorithmParameter(const char *tag, const char *value);

	/*! Retrieves a parameter value.
	 * @param tag The parameter name.
	 * @return The parameter value.
	 */
	std::string GetAlgorithmParameter(const char *tag);

	/*! Retrieves a parameter name.
	 * @param index The index to the parameter array.
	 * @return The parameter name.
	 */
	std::string GetAlgorithmParameterTag(int index);

	/*! Retrieves the number of parameter name/value pairs.
	 * @return The number of parameter name/value pairs.
	 */
	int GetNumberAlgorithmParameters() { return (int) m_Parameters.size(); }

	/*! Retrieves a parameters as a formatted string.
	 * @return The parameter name/value pairs.
	 */
	std::string GetAlgorithmParameters();

	/*! Sets the grid coordinates.
	 * @param grid The new grid coordinates.
	 */
	void SetGridCorners(GridCoordinatesType grid);

	/*! Parses the grid coordinates from the header. */
	void ParseCorners();

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetGridCorners
	///  @brief Retrieve grid coordinates
	///  @return GridCoordinatesType	Grid coordinates
	///////////////////////////////////////////////////////////////////////////////
	GridCoordinatesType GetGridCorners() { return m_CellGrid; }
};

//////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///  affxcel::CCELFileData
///
///  @brief CEL file object
///
///  Include read and write support of the following file formats:
///  - text (version 3)
///  - xda binary (version 4)
///  - transcriptome binary (internal use only)
///  - compact binary (not supported by software other than Bruce)
///
///  All file formats include the same header information. The intensity data
///  for each cell are stored as follows:
///  - text format
///		- mean (data type: float - 4 bytes)
///		- stdev (data type: float - 4 bytes)
///		- pixels (data type: short - 2 bytes)
///  - xda binary format
///		- mean (data type: float - 4 bytes)
///		- stdev (data type: float - 4 bytes)
///		- pixels (data type: short - 2 bytes)
///  - trancriptome binary format
///		- mean (data type: unsigned short - 2 bytes)
///		- stdev (data type: unsigned short - 2 bytes)
///		- pixels (data type: unsigned char - 1 byte)
///  - compact binary format
///		- mean (data type: unsigned short - 2 bytes)
///////////////////////////////////////////////////////////////////////////////
class CCELFileData  
{
public:
	/// CEL file formats
	enum
	{
		UNKNOWN = 0,
		TEXT_CEL = 1,
		XDA_BCEL = 2,
		TRANSCRIPTOME_BCEL = 3,
		COMPACT_BCEL = 4
	};
	
	/// CEL file reading states
	enum
	{
		CEL_ALL=1,
		CEL_DATA=2,
		CEL_OUTLIER=4,
		CEL_MASK=8
	};

public:
	/*! Constructor */
	CCELFileData();
	/*! Destructor */
	~CCELFileData();

protected:
	/// CEL file format
	int m_FileFormat;
	/// Error string
	std::string m_strError;
	/// CEL file name without path
	std::string m_FileName;
	/// CEL file header data object
	CCELFileHeaderData m_HeaderData;
	/// Pointer to intensity entries for each cell (used for text and xda formats)
	CELFileEntryType *m_pEntries;
	/// Pointer to intensity entries for each cell (used for transcriptome bcel format)
	CELFileTranscriptomeEntryType *m_pTransciptomeEntries;
	/// Pointer to intensity entries for each cell (used for compact cel format)
	unsigned short *m_pMeanIntensities;

	/// STL map for masked cell coordinates
	std::map<int, bool> m_MaskedCells;
	/// STL map for outlier coordinates
	std::map<int, bool> m_Outliers;

	/// CEL file reading state
	int m_nReadState;
	/// Flag to determine if masked cell data should be read
	bool m_bReadMaskedCells;
	/// Flag to determine if outlier data should be read
	bool m_bReadOutliers;

#ifdef _MSC_VER
	/// File handle used by CreateFileMapping in _MSC_VER
	HANDLE m_hFile;
	/// File map handle used by MapViewOfFile in _MSC_VER
	HANDLE m_hFileMap;
#else
	/// Pointer to file object
	FILE* m_File;
	/// Memory mapping size used by mmap (POSIX)
	size_t m_MapLen;
#endif

	/// Pointer to memory mapping data 
	char  *m_lpData;
	/// Pointer to memory mapping file view
	void  *m_lpFileMap;

	/*! Opens the file.
	 * @param bReadHeaderOnly Flag indicating if the header is only to be read.
	 * @return True if successful.
	 */
	bool Open(bool bReadHeaderOnly = false);

	/*! Reads the text version of the CEL file.
	 * @param bReadHeaderOnly Flag indicating if the header is only to be read.
	 * @return True if successful.
	 */
	bool ReadTextCel(bool bReadHeaderOnly = false);

	/*! Reads the XDA version of the CEL file.
	 * @param bReadHeaderOnly Flag indicating if the header is only to be read.
	 * @return True if successful.
	 */
	bool ReadXDABCel(bool bReadHeaderOnly = false);

	/*! Reads the transcriptome groups custom CEL file.
	 * @param bReadHeaderOnly Flag indicating if the header is only to be read.
	 * @return True if successful.
	 */
	bool ReadTranscriptomeBCel(bool bReadHeaderOnly = false);

	/*! Reads the compact binary CEL file.
	 * @param bReadHeaderOnly Flag indicating if the header is only to be read.
	 * @return True if successful.
	 */
	bool ReadCompactBCel(bool bReadHeaderOnly = false);

	/*! Determines the type of CEL file. */
	void DetermineFileFormat();

	/*! Rounds a value to the nearest interger.
	 * @param f The value to round.
	 * @return The rounded value.
	 */
	unsigned short RoundNumber(float f);

public:
	/*! Sets the error string.
	 * @param str The error.
	 */
	void SetError(const std::string& str);
	void SetError(const char* str);

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetError
	///  @brief Retrieve error string
	///  @return std::string	Error string
	///////////////////////////////////////////////////////////////////////////////
	std::string GetError() { return m_strError; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetThisPtr
	///  @brief Retrieve the pointer of the current object instance
	///  @return affxcel::CCELFileData *	Pointer to current object instance
	///////////////////////////////////////////////////////////////////////////////
	affxcel::CCELFileData *GetThisPtr() { return this; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public constant  SetFileName
	///  @brief Set CEL file name 
	///  @param str const char*		File name
	///  @return void	
	///////////////////////////////////////////////////////////////////////////////
	void SetFileName(const char *str) { m_FileName = str; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public constant  GetFileName
	///  @brief Retrieve CEL file name
	///  @return std::string	File name
	///////////////////////////////////////////////////////////////////////////////
	std::string GetFileName() const { return m_FileName; }

	// Accessors for header information.
	std::string GetHeaderKey(const char* key);

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetVersion
	///  @brief Call affxcel::CCELFileHeaderData::GetVersion() to retrieve file format version number
	///  @return int	File format version number
	///  @see affxcel::CCELFileHeaderData::GetVersion
	///////////////////////////////////////////////////////////////////////////////
	int GetVersion() { return m_HeaderData.GetVersion(); }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetCols
	///  @brief Call affxcel::CCELFileHeaderData::GetCols() to retrieve number of columns in array
	///  @return int	Number of columns in array
	///  @see affxcel::CCELFileHeaderData::GetCols
	///////////////////////////////////////////////////////////////////////////////
	int GetCols() { return m_HeaderData.GetCols(); }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetRows
	///  @brief Call affxcel::CCELFileHeaderData::GetRows() to retrieve number of rows in array
	///  @return int	Number of rows in array
	///  @see affxcel::CCELFileHeaderData::GetRows
	///////////////////////////////////////////////////////////////////////////////
	int GetRows() { return m_HeaderData.GetRows(); }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetNumCells
	///  @brief Call affxcel::CCELFileHeaderData::GetNumCells() to retrieve number of cells in array
	///  @return int	Number of cells in array
	///  @see affxcel::CCELFileHeaderData::GetNumCells
	///////////////////////////////////////////////////////////////////////////////
	int GetNumCells() { return m_HeaderData.GetCells(); }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetHeaderString
	///  @brief Call affxcel::CCELFileHeaderData::GetHeader() to retrieve header in a single string
	///  @return std::string	Header string
	///  @see affxcel::CCELFileHeaderData::GetHeader
	///////////////////////////////////////////////////////////////////////////////
	std::string GetHeaderString() { return m_HeaderData.GetHeader(); }

	///////////////////////////////////////////////////////////////////////////////
	///  public  GetHeader
	///  @brief Gets the header object.
	///  @return CCELFileHeaderData	The header object.
	///////////////////////////////////////////////////////////////////////////////
	CCELFileHeaderData &GetHeader() { return m_HeaderData; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetAlg
	///  @brief Call affxcel::CCELFileHeaderData::GetAlg() to retrieve algorithm name
	///  @return std::string	Algorithm name
	///  @see affxcel::CCELFileHeaderData::GetAlg
	///////////////////////////////////////////////////////////////////////////////
	std::string GetAlg() { return m_HeaderData.GetAlg(); }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetParams
	///  @brief Call affxcel::CCELFileHeaderData::GetParams() to retrieve algorithm parameters
	///  @return std::string	Algorithm parameters
	///  @see affxcel::CCELFileHeaderData::GetParams
	///////////////////////////////////////////////////////////////////////////////
	std::string GetParams() { return m_HeaderData.GetAlgorithmParameters(); }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetAlgorithmParameter
	///  @brief Call affxcel::CCELFileHeaderData::GetAlgorithmParameter() to retrieve algorithm parameter of specified tag
	///  @param tag const char* Algorithm parameter tag
	///  @return std::string	Algorithm parameter value
	///  @see affxcel::CCELFileHeaderData::GetAlgorithmParameter
	///////////////////////////////////////////////////////////////////////////////
	std::string GetAlgorithmParameter(const char *tag) { return m_HeaderData.GetAlgorithmParameter(tag); }

	/*! Retrieves the algorithm parameter name (tag) for a given index position.
	 * @param index The zero based index to the parameter array (0 to the number of alg parameters - 1).
	 * @return The parameter name (tag).
	 */
	std::string GetAlgorithmParameterTag(int index) { return m_HeaderData.GetAlgorithmParameterTag(index); }

	/*! Retrieves the number of algorithm parameters.
	 * @return The number of algorithm parameters.
	 */
	int GetNumberAlgorithmParameters() { return m_HeaderData.GetNumberAlgorithmParameters(); }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetAlgorithmParameters
	///  @brief Call affxcel::CCELFileHeaderData::GetAlgorithmParameters() to retrieve algorithm parameters
	///  @return std::string	Algorithm parameters
	///  @see affxcel::CCELFileHeaderData::GetAlgorithmParameters
	///////////////////////////////////////////////////////////////////////////////
	std::string GetAlgorithmParameters() { return m_HeaderData.GetAlgorithmParameters(); }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetDatHeader
	///  @brief Call affxcel::CCELFileHeaderData::GetDatHeader() to retrieve DAT header
	///  @return std::string	DAT header
	///  @see affxcel::CCELFileHeaderData::GetDatHeader
	///////////////////////////////////////////////////////////////////////////////
	std::string GetDatHeader() { return m_HeaderData.GetDatHeader(); }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetChipType
	///  @brief Call affxcel::CCELFileHeaderData::GetChipType() to retrieve chip type
	///  @return std::string	Chip type
	///  @see affxcel::CCELFileHeaderData::GetChipType
	///////////////////////////////////////////////////////////////////////////////
	std::string GetChipType() { return m_HeaderData.GetChipType(); }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetCellMargin
	///  @brief Call affxcel::CCELFileHeaderData::GetCellMargin() to retrieve cell margin
	///  @return int	Cell margin
	///  @see affxcel::CCELFileHeaderData::GetCellMargin
	///////////////////////////////////////////////////////////////////////////////
	int GetCellMargin() { return m_HeaderData.GetMargin(); }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetNumOutliers
	///  @brief Call affxcel::CCELFileHeaderData::GetNumOutliers() to retrieve number of outliers
	///  @return uint32_t	Number of outliers
	///  @see affxcel::CCELFileHeaderData::GetNumOutliers
	///////////////////////////////////////////////////////////////////////////////
	uint32_t GetNumOutliers() { return m_HeaderData.GetOutliers(); }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetNumMasked
	///  @brief Call affxcel::CCELFileHeaderData::GetNumMasked() to retrieve number of masked cells
	///  @return uint32_t	Number of masked cells
	///  @see affxcel::CCELFileHeaderData::GetNumMasked
	///////////////////////////////////////////////////////////////////////////////
	uint32_t GetNumMasked() { return m_HeaderData.GetMasked(); }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetGridCorners
	///  @brief Call affxcel::CCELFileHeaderData::GetGridCorners() to retrieve grid coordinates
	///  @return GridCoordinatesType	Grid coordinates
	///  @see affxcel::CCELFileHeaderData::GetGridCorners
	///////////////////////////////////////////////////////////////////////////////
	GridCoordinatesType GetGridCorners() { return m_HeaderData.GetGridCorners(); }

	// Index/position conversions

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  IndexToX
	///  @brief Get x coordinates from index
	///  @return int	X coordinates
	///////////////////////////////////////////////////////////////////////////////
	int IndexToX(int index) { return index % m_HeaderData.GetCols(); }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  IndexToY
	///  @brief Get y coordinates from index
	///  @return int	Y coordinates
	///////////////////////////////////////////////////////////////////////////////
	int IndexToY(int index) { return index / m_HeaderData.GetCols(); }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  XYToIndex
	///  @brief Convert x, y coordinates to index
	///  @return int	Index
	///////////////////////////////////////////////////////////////////////////////
	int XYToIndex(int x, int y) { return XYToIndex(x,y, m_HeaderData.GetRows(), m_HeaderData.GetCols()); }

	/*! Maps X/Y coordinates to CEL file index.
	 * @param x The x coordinate.
	 * @param y The y coordinate.
	 * @param r The number of rows.
	 * @param c The number of columns.
	 * @return The index to the intensity arrays.
	 */
	static int XYToIndex(int x, int y, int r, int c)
	{
		return ((y*c) + x);
	}

	/*! Retrieves a CEL file entry.
	 * @param index The index to the CEL file entries.
	 * @param entry The CEL file entry.
	 */
	void GetEntry(int index, CELFileEntryType &entry);

	/*! Retrieves a CEL file entry.
	 * @param x The X coordinate.
	 * @param y The Y coordinate.
	 * @param entry The CEL file entry.
	 */
	void GetEntry(int x, int y, CELFileEntryType &entry);

	/*! Retrieves a CEL file entry.
	 * @param index The index to the CEL file entries.
	 * @param entry The CEL file entry.
	 */
	void GetTranscriptomeEntry(int index, CELFileTranscriptomeEntryType &entry);

	/*! Retrieves a CEL file entry.
	 * @param x The X coordinate.
	 * @param y The Y coordinate.
	 * @param entry The CEL file entry.
	 */
	void GetTranscriptomeEntry(int x, int y, CELFileTranscriptomeEntryType &entry);

	/*! Retrieves a CEL file intensity.
	 * @param index The index to the CEL file entries.
	 * @return The CEL file intensity.
	 */
	float GetIntensity(int index);

  /// @brief     Get a vector of intensities with one call.
  /// @param     index         starting index
  /// @param     intensities   vector to fill.
  /// @return    non-zero on error
	int GetIntensities(int index,std::vector<float>& intensities);

	/*! Retrieves a CEL file intensity.
	 * @param x The X coordinate.
	 * @param y The Y coordinate.
	 * @return The CEL file intensity.
	 */
	float GetIntensity(int x, int y);

	/*! Retrieves a CEL file stdv value.
	 * @param index The index to the CEL file entries.
	 * @return The CEL file stdv value.
	 */
	float GetStdv(int index);

	/*! Retrieves a CEL file stdv value.
	 * @param x The X coordinate.
	 * @param y The Y coordinate.
	 * @return The CEL file stdv value.
	 */
	float GetStdv(int x, int y);

	/*! Retrieves a CEL file pixel count.
	 * @param index The index to the CEL file entries.
	 * @return The CEL file pixel count.
	 */
	short GetPixels(int index);

	/*! Retrieves a CEL file pixel count.
	 * @param x The X coordinate.
	 * @param y The Y coordinate.
	 * @return The CEL file pixel count.
	 */
	short GetPixels(int x, int y);

	/*! Retrieves a CEL file mask flag.
	 * @param x The X coordinate.
	 * @param y The Y coordinate.
	 * @return True if the feature is masked.
	 */
	bool IsMasked(int x, int y);

	/*! Retrieves a CEL file mask flag.
	 * @param index The index to the CEL file entries.
	 * @return True if the feature is masked.
	 */
	bool IsMasked(int index);

	/*! Retrieves a CEL file outlier flag.
	 * @param x The X coordinate.
	 * @param y The Y coordinate.
	 * @return True if the feature is an outlier.
	 */
	bool IsOutlier(int x, int y);

	/*! Retrieves a CEL file outlier flag.
	 * @param index The index to the CEL file entries.
	 * @return True if the feature is an outlier.
	 */
	bool IsOutlier(int index);


	// For reading a file.

	/*! Closes the file */
	void Close();

	/*! Returns the file size.
	 * @return The file size.
	 */
	uint32_t GetFileSize();

	/*! Checks if the file exists.
	 * @return True if the file exists.
	 */
	bool Exists();
  std::string ResolveName();

	/*! Reads the header of the CEL file.
	 * @return True if successful.
	 */
	bool ReadHeader();

	/*! Reads the CEL file.
	 * @param bIncludeMaskAndOutliers Flag to indicate if the mask and outlier sections should also be read.
	 * @return True if successful.
	 */
	bool Read(bool bIncludeMaskAndOutliers=true);

	/*! Checks if the file type is XDA.
	 * @return True if XDA type.
	 */
	bool IsXDACompatibleFile();

	/*! Checks if the file type is version 3.
	 * @return True if version 3 type.
	 */
	bool IsVersion3CompatibleFile();

	/*! Checks if the file type is transcriptome.
	 * @return True if transcriptome type.
	 */
	bool IsTranscriptomeBcelFile();

	/*! Checks if the file type is compact.
	 * @return True if compact type.
	 */
	bool IsCompactCelFile();

	/*! Checks if the file type is previous unspported version of compact.
	 * @return True if compact type.
	 */
	bool IsUnsupportedCompactCelFile();

	/*! Determine CEL file format and call appropriate function to read file using the specified file name.
	 * @param filename The name of the file.
	 * @param nState Reading state 
	 * @return bool	true if success, false if fail
	 *
	 * \a nState can be one or combination of the following values:
	 *       CEL_ALL      Read all information in file (default)
	 *       CEL_DATA     Read header and intensities only
	 *		 CEL_OUTLIER  Read header, intensities and outliers
	 *		 CEL_MASK     Read header, intensities and masked cells
	 */
	bool ReadEx(const char *filename=0, int nState=CEL_ALL);

	/*! Returns the reading state.
	 * @return The reading state.
	 */
	int GetReadState() { return m_nReadState; };

	// For writing a new CEL file

	/*! Clears the members. */
	void Clear();

	/*! Get the file format type.
	 * @return The file format type.
	 */
	int  GetFileFormat() { return m_FileFormat; }

	/*! Sets the file format type.
	 * @param i The file format type.
	 */
	void SetFileFormat(int i);

	/*! Sets the name of the algorithm used to create the CEL file.
	 * @param str The algorithm name.
	 */
	void SetAlgorithmName(const char *str);

	/*! Adds a parameter to the parameter list.
 	* @param tag The parameter name.
 	* @param value The parameter value.
 	*/
	void SetAlgorithmParameter(const char *tag, const char *value);

	/*! Adds a parameter to the parameter list.
 	* @param tag The parameter name.
 	* @param value The parameter value.
 	*/
	void AddAlgorithmParameter(const char *tag, const char *value);

	/*! Adds a parameter to the parameter list.
 	* @param tag The parameter name.
 	* @param value The parameter value.
 	*/
	void SetAddAlgorithmParameter(const char *tag, const char *value);

	/*! Sets dimentions (rows/cols) of the CEL file.
	 * @param rows The number of rows.
	 * @param cols The number of columns.
	 */
	void SetDimensions(int rows, int cols);

	/*! Sets probe array (chip) type.
	 * @param str The probe array type.
	 */
	void SetChipType(const char *str);

	/*! Sets the margin used in creating the CEL file.
	 * @param margin The margin used in creating the CEL file.
	 */
	void SetMargin(int margin);

	/*! Sets the grid coordinates.
	 * @param grid The grid coordinates.
	 */
	void SetGridCorners(GridCoordinatesType grid);

	/*! Sets a CEL file entry.
	 * @param x The X coordinate.
	 * @param y The Y coordinate.
	 * @param pEntry The CEL file entry.
	 */
	void SetCellEntry(int x, int y, CELFileEntryType *pEntry);

	/*! Sets a CEL file entry.
	 * @param index The index to the CEL file entries.
	 * @param pEntry The CEL file entry.
	 */
	void SetCellEntry(int index, CELFileEntryType *pEntry);

	/*! Allocates entries for a text or XDA file. */
	void AllocateEntries();

	/*! Sets a CEL file entry.
	 * @param x The X coordinate.
	 * @param y The Y coordinate.
	 * @param pEntry The CEL file entry.
	 */
	void SetTranscriptomeCellEntry(int x, int y, CELFileTranscriptomeEntryType *pEntry);

	/*! Sets a CEL file entry.
	 * @param index The index to the CEL file entries.
	 * @param pEntry The CEL file entry.
	 */
	void SetTranscriptomeCellEntry(int index, CELFileTranscriptomeEntryType *pEntry);

	/*! Allocates memory for the transcriptome file type entries. */
	void AllocateTranscriptomeEntries();

	/*! Sets the intensity value.
	 * @param index The index to the CEL file.
	 * @param intensity The intensity value.
	 */
	void SetIntensity(int index, float intensity);

	/*! Sets the intensity value.
	 * @param x The X coordinate.
	 * @param y The Y coordinate.
	 * @param intensity The intensity value.
	 */
	void SetIntensity(int x, int y, float intensity);

	/*! Sets the stdev value.
	 * @param index The index to the CEL file.
	 * @param stdev The stdev value.
	 */
	void SetStdv(int index, float stdev);

	/*! Sets the stdev value.
	 * @param x The X coordinate.
	 * @param y The Y coordinate.
	 * @param stdev The stdev value.
	 */
	void SetStdv(int x, int y, float stdev);

	/*! Sets the pixel count.
	 * @param index The index to the CEL file.
	 * @param pixels The pixel count.
	 */
	void SetPixels(int index, short pixels);

	/*! Sets the pixel count.
	 * @param x The X coordinate.
	 * @param y The Y coordinate.
	 * @param pixels The pixel count.
	 */
	void SetPixels(int x, int y, short pixels);

	/*! Sets the mask flag.
	 * @param index The index to the CEL file.
	 * @param mask The mask flag.
	 */
	void SetMask(int index, bool mask);

	/*! Sets the mask flag.
	 * @param x The X coordinate.
	 * @param y The Y coordinate.
	 * @param masked The mask flag.
	 */
	void SetMask(int x, int y, bool masked);

	/*! Sets the outlier flag.
	 * @param index The index to the CEL file.
	 * @param outlier The outlier flag.
	 */
	void SetOutlier(int index, bool outlier);

	/*! Sets the outlier flag.
	 * @param x The X coordinate.
	 * @param y The Y coordinate.
	 * @param outlier The outlier flag.
	 */
	void SetOutlier(int x, int y, bool outlier);

	/*! Unmap the file. */
	void Munmap();

	/*! Store data in memory if CEL file is memory mapped. */
	void EnsureNotMmapped();
};

//////////////////////////////////////////////////////////////////////

} // namespace

////////////////////////////////////////////////////////////////////
#endif // _CELFILEDATA_H_
