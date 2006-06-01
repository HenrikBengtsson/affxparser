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

// WARNING: This class is deprecated! Use file/TsvFile instead.

#ifndef _TSVFileData_h_
#define _TSVFileData_h_

#include <string>
#include <map>
#include <vector>
#include <fstream>

#if defined (_MSC_VER)
#include <windows.h>
#elif defined (__CYGWIN__)
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#else
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/user.h>
#endif

#include "affy-base-types.h"

namespace affxtsv
{

///////////////////////////////////////////////////////////////////////////////
///  affxtsv global utilities
///  \brief Global data conversion utilities
///
///  @remarks No object context; for general use
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///  global  toInt
///  \brief Make an int from a c string.
///  @param num						c string representation of number to be parsed.
///  @param i						Integer representation of num, 0 if not successful
///  @return bool					true if successful; false otherwise
///////////////////////////////////////////////////////////////////////////////
bool toInt (const char *num, int32_t& i);

///////////////////////////////////////////////////////////////////////////////
///  global  toFloat
///  \brief Make a float from a c string.
///  @param num						c string representation of number to be parsed.
///  @param f						Float representation of num, 0.0 if not successful
///  @return bool					true if successful; false otherwise
///////////////////////////////////////////////////////////////////////////////
bool toFloat (const char *num, float& f);

///////////////////////////////////////////////////////////////////////////////
///  affxtsv::CTSVFileConvertOutputs
///  \brief Columns names, data types for TSV file data conversion.
///
///  @remarks Describes caller choices for data conversion.
///  For each requested output type the user passes a string containing
///  the corresponging column name, as a scalar if a single output
///  of that type, as a vector if multiple outputs of that type are requested.
///////////////////////////////////////////////////////////////////////////////
class CTSVFileConvertOutputs
{
public:
	// This class is just a container to exchange data - there's no need to
	// protect this data from the user.  All data and methods are public.

	///  Column name for single string output
	std::string SingleStringName;
	///  Column name for single int output
	std::string SingleIntName;
	///  Column name for single float output
	std::string SingleFloatName;
	///  Column names for multiple strings output
	std::vector<std::string> MultipleStringsNames;
	///  Column names for multiple ints output
	std::vector<std::string> MultipleIntsNames;
	///  Column names for multiple floats output
	std::vector<std::string> MultipleFloatsNames;

	///  Constructor
	CTSVFileConvertOutputs (const std::string& SingleString = "",
	    const std::vector<std::string>& MultipleStrings = std::vector<std::string>(),
	    const std::string& SingleInt = "",
	    const std::vector<std::string>& MultipleInts = std::vector<std::string>(),
	    const std::string& SingleFloat = "",
	    const std::vector<std::string>& MultipleFloats = std::vector<std::string>());
	///  Copy assignment
	CTSVFileConvertOutputs& operator= (const CTSVFileConvertOutputs&);

	///////////////////////////////////////////////////////////////////////////////
	///  public Clear
	///  \brief Clear member variables
	///  @return void
	///////////////////////////////////////////////////////////////////////////////
	void Clear();

	///////////////////////////////////////////////////////////////////////////////
	///  public swap
	///  \brief Swap contents
	///  @return void
	///////////////////////////////////////////////////////////////////////////////
	void swap (CTSVFileConvertOutputs& other);
};

///////////////////////////////////////////////////////////////////////////////
///  affxtsv::CTSVFileConvertData
///  \brief Typed pointers for returning data to caller.
///
///  @remarks Contains pointers to user allocated lvalues into which the
///  converted data is returned.
///////////////////////////////////////////////////////////////////////////////
class CTSVFileConvertData
{
public:
	///  Constructor
	CTSVFileConvertData (std::string* SingleStringPtr = 0, std::vector<std::string>* MultipleStringsPtr = 0,
	    int32_t* SingleIntPtr = 0, std::vector<int32_t>* MultipleIntsPtr = 0,
	    float* SingleFloatPtr = 0, std::vector<float>* MultipleFloatsPtr = 0);

	// A container to return output to the caller; all data is public
	///  Output pointer for single string output
	std::string* SingleStringPtr;
	///  Output pointer for single int output
	int32_t* SingleIntPtr;
	///  Output pointer for single float output
	float* SingleFloatPtr;
	///  Pointer to vector for multiple strings output
	std::vector<std::string>* MultipleStringsPtr;
	///  Pointer to vector for multiple ints output
	std::vector<int32_t>* MultipleIntsPtr;
	///  Pointer to vector for multiple floats output
	std::vector<float>* MultipleFloatsPtr;
};

///////////////////////////////////////////////////////////////////////////////
///  affxcel::CTSVFileColumnHeader
///  \brief TSV file column header information
///
///  @remarks Object instance is included in affxclf::CTSVFileData
///////////////////////////////////////////////////////////////////////////////
class CTSVFileColumnHeader
{
private:
	///  Map of column indices keyed by lower case column name
	std::map<std::string, int32_t> m_ColumnIndices;
	///  Array of column names
	std::vector<std::string> m_ColumnNames;

public:
	///  Constructor
	CTSVFileColumnHeader() {}
	///  Destructor
	~CTSVFileColumnHeader();

	///////////////////////////////////////////////////////////////////////////////
	///  public  Append
	///  \brief Append specified column to the end of list
	///  @param  str const char*  	Column name
	///  @return bool	 	False if column name is a duplicate
	///////////////////////////////////////////////////////////////////////////////
	bool Append(const char* str);

	///////////////////////////////////////////////////////////////////////////////
	///  public  GetColumnName
	///  \brief Get column name by specified index
	///  @param  index int32_t	Column index
	///  @return std::string	Column name	
	///////////////////////////////////////////////////////////////////////////////
	std::string GetColumnName(int32_t index);

	///////////////////////////////////////////////////////////////////////////////
	///  public  GetColumnIndex
	///  \brief Get column index by specified column name 
	///  @param  str const char*  	Column name
	///  @return int32_t	Column index	
	///////////////////////////////////////////////////////////////////////////////
	int32_t GetColumnIndex(const char* str);

	///////////////////////////////////////////////////////////////////////////////
	///  public  GetColumnCount
	///  \brief Get number of columns
	///  @return int32_t	Column count	
	///////////////////////////////////////////////////////////////////////////////
	int32_t GetColumnCount() { return (int32_t) m_ColumnNames.size(); }

	///////////////////////////////////////////////////////////////////////////////
	///  public  GetColumnNames
	///  \brief Get vector of column names 
	///  @param  names std::vector<std::string>&  	Column names as output
	///  @return void	
	///////////////////////////////////////////////////////////////////////////////
	void GetColumnNames(std::vector<std::string>& names);

	///////////////////////////////////////////////////////////////////////////////
	///  public  RemoveAll
	///  \brief Remove all columns  
	///  @return void	
	///////////////////////////////////////////////////////////////////////////////
	void RemoveAll();
};

///////////////////////////////////////////////////////////////////////////////
///  affxtsv::CTSVFileData
///  \brief TSV file object
///
///  This class handles IO to tab separated files (TSV) and 
///  Netaffx comma separated file (CSV). It supports 3 reading modes:
///  - stream
///    When in 'stream' mode, the next_entry() method is available which 
///	   allows iterating over the rows in the clf file.
///  - index
///    When using 'index' mode, the file is initially parsed for the file 
///    locations of each row; the file is not parsed into memory. 
///    GetInfo() method is available for random data access.
///  - hash
///    When using 'hash' mode, the entire file is parsed into memory.
///    GetInfo() method is available for random data access.
///////////////////////////////////////////////////////////////////////////////
class CTSVFileData  
{
private:
	///  Error string
	std::string m_Error;
	///  File name
	std::string m_FileName;
	///  File type
	int16_t m_FileType;
	///  Reading mode
	int16_t m_nMode;
	///  Index key
	std::string m_Key;
	///  Meta tags
	std::multimap<std::string, std::string> m_MetaTags;
	/// Column headers
	CTSVFileColumnHeader m_ColumnHeader;
	///  Map of key and entry position
	std::multimap<std::string, std::streampos> m_KeyIndices;
	///  Map of key and entries
	std::multimap<std::string, std::vector<std::string>*> m_Entries;

	///  File input stream
	std::ifstream m_FileIn;
	///  End of file flag
	bool m_Eof;
	///  Chunk buffer for file reading
	char* m_Buffer;
	///  Size of buffer been read
	std::streamsize m_BufferSize;
	///  Current reading position in buffer
	int32_t m_BufferPos;
	///  Start position of buffer in file
	std::streampos m_BufferStartPos;
	///  Requested type converted outputs
	CTSVFileConvertOutputs m_Outputs;
	///  Column index of requested single string output
	int32_t m_SingleStringIndex;
	///  Column index of requested single int output
	int32_t m_SingleIntIndex;
	///  Column index of requested single float output
	int32_t m_SingleFloatIndex;
	///  Vector of column indices of requested multiple strings output
	std::vector<int32_t> m_MultipleStringsIndices;
	///  Vector of column indices of requested multiple ints output
	std::vector<int32_t> m_MultipleIntsIndices;
	///  Vector of column indices of requested multiple floats output
	std::vector<int32_t> m_MultipleFloatsIndices;

public:
	///  Reading mode
	enum 
	{
		TSV_INDEX_MODE = 1,		///  Index mode
		TSV_STREAM_MODE,		///  Stream mode
		TSV_HASH_MODE			///  Hash mode
	};

	///  File type
	enum
	{
		TSV_FILE = 1,			///  TSV file format
		NETAFFX_CSV_FILE		///  Netaffx csv file format
	};

public:
	///  Constructor
	CTSVFileData();
	///  Destructor
	~CTSVFileData();

	///////////////////////////////////////////////////////////////////////////////
	///  public  SetError
	///  \brief Set error string
	///  @param str const char*	 Error string
	///  @return void
	///////////////////////////////////////////////////////////////////////////////
	void SetError(const char* str);

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetError
	///  \brief Get error string
	///  @return const char*	 Error string
	///////////////////////////////////////////////////////////////////////////////
	std::string GetError() const { return m_Error; }

	///////////////////////////////////////////////////////////////////////////////
	///  public  SetFileName
	///  \brief Set file name
	///  @param str const char*	 File name
	///  @return void
	///////////////////////////////////////////////////////////////////////////////
	void SetFileName(const char *str);

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetFileName
	///  \brief Get file name
	///  @return const char*	 Error string
	///////////////////////////////////////////////////////////////////////////////
	std::string GetFileName() const { return m_FileName; }

	///////////////////////////////////////////////////////////////////////////////
	///  public  SetReadMode
	///  \brief Set reading mode
	///  @param mode const int16_t	 Reading mode
	///  @return void
	///////////////////////////////////////////////////////////////////////////////
	void SetReadMode(const int16_t mode);

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetReadMode
	///  \brief Get reading mode
	///  @return int16_t	 Reading mode
	///////////////////////////////////////////////////////////////////////////////
	int16_t GetReadMode() const { return m_nMode; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  SetFileType
	///  \brief Set file type
	///  @param type const int16_t	 File type
	///  @return void
	///////////////////////////////////////////////////////////////////////////////
	void SetFileType(const int16_t type) { m_FileType = type; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetFileType
	///  \brief Get file type
	///  @return int16_t	 File type
	///////////////////////////////////////////////////////////////////////////////
	int16_t GetFileType() const { return m_FileType; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  IsTSV
	///  \brief Check if file is TSV format
	///  @return bool	 true if TSV; false otherwise
	///////////////////////////////////////////////////////////////////////////////
	bool IsTSV() { return (m_FileType == TSV_FILE); }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  IsNetAffxCSV
	///  \brief Check if file is CSV format
	///  @return bool	 true if CSV; false otherwise
	///////////////////////////////////////////////////////////////////////////////
	bool IsNetAffxCSV() { return (m_FileType == NETAFFX_CSV_FILE); }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  SetKey
	///  \brief Set index key
	///  @param str const char*	 Index key
	///  @return void
	///////////////////////////////////////////////////////////////////////////////
	void SetKey(const char* str);

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetKey
	///  \brief Get index key
	///  @return std::string	 Index key
	///////////////////////////////////////////////////////////////////////////////
	std::string GetKey() const { return m_Key; }

	///////////////////////////////////////////////////////////////////////////////
	///  inline public  SetOutputs
	///  \brief Set type converted outputs
	///  @param const CTSVFileConvertOutputs& outputs Type converted outputs
	///  @return void
	///////////////////////////////////////////////////////////////////////////////
	void SetOutputs(const CTSVFileConvertOutputs& outputs) { m_Outputs = outputs; }

	///////////////////////////////////////////////////////////////////////////////
	///  public  Close
	///  \brief Close file and clean memory
	///  @return void
	///////////////////////////////////////////////////////////////////////////////
	void Close();

	///////////////////////////////////////////////////////////////////////////////
	///  public  GetFileSize
	///  \brief Get size of current file
	///  @return int32_t	 File size
	///////////////////////////////////////////////////////////////////////////////
	int32_t GetFileSize();

	///////////////////////////////////////////////////////////////////////////////
	///  public  Exists
	///  \brief Check if current file exists
	///  @return bool	 true if file exists; false otherwise
	///////////////////////////////////////////////////////////////////////////////
	bool Exists();

	///////////////////////////////////////////////////////////////////////////////
	///  public  GetMetaTagNames
	///  \brief Get vector of meta tag names in file header
	///  @param names std::vector<std::string>&	 Meta tag names vector for output
	///  @return void
	///////////////////////////////////////////////////////////////////////////////
	void GetMetaTagNames(std::vector<std::string>& names);

	///////////////////////////////////////////////////////////////////////////////
	///  public  GetMetaTagValues
	///  \brief Get vector of meta tag values in file header
	///  @param values std::vector<std::string>&	 Meta tag values vector for output
	///  @return void
	///////////////////////////////////////////////////////////////////////////////
	void GetMetaTagValues(const char* name, std::vector<std::string>& values);

	///////////////////////////////////////////////////////////////////////////////
	///  public  GetColumnHeader
	///  \brief Get column headers
	///  @param header std::vector<std::string>&	 Header vector for output
	///  @return void
	///////////////////////////////////////////////////////////////////////////////
	void GetColumnHeader(std::vector<std::string>& header);

	///////////////////////////////////////////////////////////////////////////////
	///  public  NameInColumnHeader
	///  \brief Return true if requested column name present in the column headers
	///  @param name char*	 		Requested name
	///  @return bool			true if found, else false
	///////////////////////////////////////////////////////////////////////////////
	bool NameInColumnHeader (const char* name);

	///////////////////////////////////////////////////////////////////////////////
	///  public  Open
	///  \brief Open TSV/CSV file
	///  @param fileName const char*	File name
	///  @param mode const short		Reading mode (Stream mode as default)
	///  @param filetype const char*	File type (TSV as default)
	///  @param key const char*		Index key (None as default)
	///  @param outputs CTSVFileConvertOutputs&	Type converted outputs (None as default)
	///  @return bool			true if successful; false otherwise
	///////////////////////////////////////////////////////////////////////////////
	bool Open(const char* fileName, const short mode = TSV_STREAM_MODE, const char* filetype = "tab", const char* key = NULL,
	    const CTSVFileConvertOutputs* outputs = 0);

	///////////////////////////////////////////////////////////////////////////////
	///  public  NextEntry
	///  \brief Get next entry in file when using stream mode
	///  @param data std::map<std::string, int32_t>&	Next record entry for output
	///  @return bool					true if successful; false otherwise
	///////////////////////////////////////////////////////////////////////////////
	bool NextEntry(std::map<std::string, std::string>& data);

	///////////////////////////////////////////////////////////////////////////////
	///  public  NextEntryConverted
	///  \brief Get next entry in file when using stream mode, convert to user selected
	///  data types
	///  @param data CTSVFileConvertData&	Pointers to store converted data
	///  @return bool					true if successful; false otherwise
	///////////////////////////////////////////////////////////////////////////////
	bool NextEntryConverted (CTSVFileConvertData& data);

	///////////////////////////////////////////////////////////////////////////////
	///  public  GetInfo
	///  \brief Get entry data when using index or hash mode
	///  @param key const std::string&	Index key
	///  @param data std::vector<std::map<std::string, std::string> >&	Entry data for output
	///  @return bool					true if successful; false otherwise
	///////////////////////////////////////////////////////////////////////////////
	bool GetInfo(const std::string& key, std::vector<std::map<std::string, std::string> >& data);

	///////////////////////////////////////////////////////////////////////////////
	///  public  GetSubFields
	///  \brief Parse sub fields in CSV entry column
	///  @param column const std::string&	Column entry
	///  @param fields std::vector<std::string>&	Vector of sub fields
	///  @return void
	///////////////////////////////////////////////////////////////////////////////
	void GetSubFields(const std::string& column, std::vector<std::string>& fields);

	///////////////////////////////////////////////////////////////////////////////
	///  public  GetSubEntries
	///  \brief Parse sub entries in CSV entry column
	///  @param column const std::string&	Column entry
	///  @param entries std::vector<std::string>&	Vector of sub entries
	///  @return void
	///////////////////////////////////////////////////////////////////////////////
	void GetSubEntries(const std::string& column, std::vector<std::string>& entries);

private:
	///////////////////////////////////////////////////////////////////////////////
	///  private  ReadBuffer
	///  \brief Read buffer from file
	///  @return bool	true if successful; false otherwise
	///////////////////////////////////////////////////////////////////////////////
	bool ReadBuffer();

	///////////////////////////////////////////////////////////////////////////////
	///  private  GetLine
	///  \brief Get next line of record
	///  @return std::string	Line string
	///////////////////////////////////////////////////////////////////////////////
	std::string GetLine();

	///////////////////////////////////////////////////////////////////////////////
	///  private  Trim
	///  \brief Trim leading and trailing spaces of specified string
	///  @param str const std::string&   String with spaces to be trimmed
	///  @return std::string	String with spaces trimmed
	///////////////////////////////////////////////////////////////////////////////
	std::string Trim(const std::string& str);

	///////////////////////////////////////////////////////////////////////////////
	///  private  TrimDoubleQuote
	///  \brief Trim leading and trailing double quotes of specified string
	///  @param str const std::string&   String with double quotes to be trimmed
	///  @return std::string	String with double quotes trimmed
	///////////////////////////////////////////////////////////////////////////////
	std::string TrimDoubleQuote(const std::string& str);

	///////////////////////////////////////////////////////////////////////////////
	///  private  GetColumns
	///  \brief Get next line of record in vector of columns
	///  @param columns std::vector<std::string>&   Columns of data for output
	///  @return void
	///////////////////////////////////////////////////////////////////////////////
	void GetColumns(std::vector<std::string>& columns);

	///////////////////////////////////////////////////////////////////////////////
	///  private  ParseMetaTags
	///  \brief Parse meta tags from file header
	///  @return void
	///////////////////////////////////////////////////////////////////////////////
	void ParseMetaTags();

	///////////////////////////////////////////////////////////////////////////////
	///  private  ParseColumnHeader
	///  \brief Parse column header from file
	///  @return true if successful
	///////////////////////////////////////////////////////////////////////////////
	bool ParseColumnHeader();

	///////////////////////////////////////////////////////////////////////////////
	///  private  ParseOutput
	///  \brief Parse requested type-converted output
	///  @return true if successful
	///////////////////////////////////////////////////////////////////////////////
	bool ParseOutput();

	///////////////////////////////////////////////////////////////////////////////
	///  private  StreamOpen
	///  \brief Open file in stream mode
	///  @return void
	///////////////////////////////////////////////////////////////////////////////
	bool StreamOpen();

	///////////////////////////////////////////////////////////////////////////////
	///  private  HashOpen
	///  \brief Open file in hash mode
	///  @return void
	///////////////////////////////////////////////////////////////////////////////
	bool HashOpen();

	///////////////////////////////////////////////////////////////////////////////
	///  private  IndexOpen
	///  \brief Open file in index mode
	///  @return void
	///////////////////////////////////////////////////////////////////////////////
	bool IndexOpen();

	///////////////////////////////////////////////////////////////////////////////
	///  public  GetHashInfo
	///  \brief Get entry data when using hash mode
	///  @param key const std::string&	Index key
	///  @param data std::vector<std::map<std::string, std::string> >&	Entry data for output
	///  @return bool					true if successful; false otherwise
	///////////////////////////////////////////////////////////////////////////////
	bool GetHashInfo(const std::string& key, std::vector<std::map<std::string, std::string> >& data);

	///////////////////////////////////////////////////////////////////////////////
	///  public  GetIndexInfo
	///  \brief Get entry data when using index mode
	///  @param key const std::string&	Index key
	///  @param data std::vector<std::map<std::string, std::string> >&	Entry data for output
	///  @return bool					true if successful; false otherwise
	///////////////////////////////////////////////////////////////////////////////
	bool GetIndexInfo(const std::string& key, std::vector<std::map<std::string, std::string> >& data);

	///////////////////////////////////////////////////////////////////////////////
	///  private  GetData
	///  \brief Get next entry in file
	///  @param data std::map<std::string, std::string>&	Next record entry for output
	///  @return bool					true if successful; false otherwise
	///////////////////////////////////////////////////////////////////////////////
	bool GetData(std::map<std::string, std::string>& data);

	///////////////////////////////////////////////////////////////////////////////
	///  private  GetDataConverted
	///  \brief Get next entry in file, convert to user selected data types
	///  @param data CTSVFileConvertData&		Pointers to store converted data
	///  @return bool				true if successful; false otherwise
	///////////////////////////////////////////////////////////////////////////////
	bool GetDataConverted (CTSVFileConvertData& data);

	///////////////////////////////////////////////////////////////////////////////
	///  private  Clear
	///  \brief Clear member variables
	///  @return void
	///////////////////////////////////////////////////////////////////////////////
	void Clear();
};

//////////////////////////////////////////////////////////////////////

} // namespace

#endif // _TSVFileData_h_

