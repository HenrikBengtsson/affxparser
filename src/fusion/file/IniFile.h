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

/*! \file IniFile.h This file provides reading capaibilities for INI files.
 */

#ifndef _IniFileData_h_
#define _IniFileData_h_

//////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning(disable: 4786) // identifier was truncated in the debug information
#endif

#include <algorithm>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>
//

/*! This class provides storages and parsing capabilities of INI files. */
class CIniFile
{
public:

	/*! Structure for storing INI information */
	struct Record
	{
		std::string Comments;
		char Commented;
		std::string Section;
		std::string Key;
		std::string Value;
	};

	/*! Enumerant for identifying characters used for comments */
	enum CommentChar
	{
		Pound = '#',
		SemiColon = ';'
	};

	/*! Constructor */
	CIniFile(void);

	/*! Destructor */
	virtual ~CIniFile(void);

	/*! Returns the contents of a specific key within a given section
	 * @param KeyName Key name of interest
	 * @param SectionName Section name of interest
	 * @param FileName Name of file
	 * @return Record structure that contains the contents of specific section and key
	 */
	std::vector<Record> GetRecord(std::string KeyName, std::string SectionName, std::string FileName);

	/*! Returns all key contents of a specific section
	 * @param SectionName Section name of interest
	 * @param FileName Name of file
	 * @return Vector of record structures that contain the contents of all keys in a section
	 */
	std::vector<Record> GetSection(std::string SectionName, std::string FileName);

	/*! Returns all sections names of a given file
	 * @param FileName Name of file
	 * @return Vector of section names
	 */
	std::vector<std::string> GetSectionNames(std::string FileName);
	
	/*! Returns the value of a specific key within a given section
	 * @param KeyName Key name of interest
	 * @param SectionName Section name of interest
	 * @param FileName Name of file
	 * @return String that contains the value associated with a specific key wihtin a given section
	 */
	std::string GetValue(std::string KeyName, std::string SectionName, std::string FileName);

	/*! Returns all key contents of the file.
	 *	@param FileName Name of file.
	 *	@return Vector of all record structures that contain the contents of all keys in all sections of the file.
	 */
	std::vector<Record> GetAllRecords(std::string FileName);

private:
	/*! Returns all key contents of all sections
	 * @param FileName Name of file
	 * @return Vector of record structures that contain the key contents all sections
	 */
	std::vector<Record> GetSections(std::string FileName);

	/*! Load the contents of the INI file
	 * @param FileName Name of file
	 * @param content Vector of record structures that contain all keys within all sections
	 * @return True if successful
	 */
	bool Load(std::string FileName, std::vector<Record>& content);	

	/*! Function to trim whitespace from both sides of a given string
	 * @param str String to perform the whitespace trimming on
	 * @param ChrsToTrim String of white space characters to search when trimming input string
	 * @param TrimDir Direction to perform the whitespace trimming on (left, right, both)
	 */
	void Trim(std::string& str, const std::string & ChrsToTrim = " \t\n\r", int TrimDir = 0);

	/*! Structure for determining a match of a given section and key */
	struct RecordSectionKeyIs : std::unary_function<Record, bool>
	{
		/*! The section name */
		std::string section_;

		/*! The section name */
		std::string key_;

		/*! Constructor */
		RecordSectionKeyIs(const std::string& section, const std::string& key): section_(section),key_(key){}

		/*! Operator for checking a section name and key name match
		 * @param rec The record object to perform match against
		 * @return True if successful
		 */
		bool operator()( const Record& rec ) const
		{
			return ((rec.Section == section_)&&(rec.Key == key_));
		}
	};
};

#endif // _IniFileData_h_
