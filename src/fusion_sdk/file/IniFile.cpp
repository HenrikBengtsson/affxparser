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

#include "file/IniFile.h"
//

using namespace std;

// Default constructor
CIniFile::CIniFile(void)
{
}

// Destructor
CIniFile::~CIniFile(void)
{
}

// A function to trim whitespace from both sides of a given string
void CIniFile::Trim(std::string& str, const std::string & ChrsToTrim, int TrimDir)
{
    size_t startIndex = str.find_first_not_of(ChrsToTrim);
    if (startIndex == std::string::npos){str.erase(); return;}
    if (TrimDir < 2) str = str.substr(startIndex, str.size()-startIndex);
    if (TrimDir!=1) str = str.substr(0, str.find_last_not_of(ChrsToTrim) + 1);
}

// Load the contents of the INI file
bool CIniFile::Load(std::string FileName, std::vector<Record>& content)
{
	string s;												// Holds the current line from the ini file
	string CurrentSection;									// Holds the current section name

	ifstream inFile (FileName.c_str());						// Create an input filestream
	if (!inFile.is_open()) return false;					// If the input file doesn't open, then return
	content.clear();										// Clear the content vector

	string comments = "";									// A string to store comments in

	while(!std::getline(inFile, s).eof())					// Read until the end of the file
	{
		Trim(s);											// Trim whitespace from the ends
		if(!s.empty())										// Make sure its not a blank line
		{
			Record r;										// Define a new record

			if((s[0]=='#')||(s[0]==';'))					// Is this a commented line?
			{
				if ((s.find('[')==string::npos)&&			// If there is no [ or =
					(s.find('=')==string::npos))			// Then it's a comment
				{
					comments += s + '\n';					// Add the comment to the current comments string
				}
				else 
				{
					r.Commented = s[0];						// Save the comment character
					s.erase(s.begin());						// Remove the comment for further processing
					Trim(s);
				}											// Remove any more whitespace
			}
			else r.Commented = ' ';							// else mark it as not being a comment

			if(s.find('[')!=string::npos)					// Is this line a section?
			{		
				s.erase(s.begin());							// Erase the leading bracket
				s.erase(s.find(']'));						// Erase the trailing bracket
				r.Comments = comments;						// Add the comments string (if any)
				comments = "";								// Clear the comments for re-use
				r.Section = s;								// Set the Section value
				r.Key = "";									// Set the Key value
				r.Value = "";								// Set the Value value
				CurrentSection = s;
			}

			if(s.find('=')!=string::npos)					// Is this line a Key/Value?
			{
				r.Comments = comments;						// Add the comments string (if any)
				comments = "";								// Clear the comments for re-use
				r.Section = CurrentSection;					// Set the section to the current Section
				r.Key = s.substr(0,s.find('='));			// Set the Key value to everything before the = sign
				r.Value = s.substr(s.find('=')+1);			// Set the Value to everything after the = sign
			}
			if(comments == "")								// Don't add a record yet if its a comment line
				content.push_back(r);						// Add the record to content
		}
	}
	
	inFile.close();											// Close the file
	return true;
}

// Returns all sections names of a given file
std::vector<std::string> CIniFile::GetSectionNames(std::string FileName)
{
	vector<string> data;									// Holds the return data
	vector<Record> content;									// Holds the current record													// Holds the current record

	if (Load(FileName, content))							// Make sure the file is loaded
	{
		for (int i=0;i<(int)content.size();i++)				// Loop through the content
		{
			if(content[i].Key =="")							// If there is no key value, then its a section
				data.push_back(content[i].Section);			// Add the section to the return data
		}
	}

	return data;											// Return the data
}

// Returns all key contents of a specific section
std::vector<CIniFile::Record> CIniFile::GetSection(std::string SectionName, std::string FileName)
{
	vector<Record> data;									// Holds the return data
	vector<Record> content;									// Holds the current record

	if (Load(FileName, content))							// Make sure the file is loaded
	{
		for (int i=0;i<(int)content.size();i++)				// Loop through the content
		{
			if((content[i].Section == SectionName) &&		// If this is the section name we want
				(content[i].Key != ""))						// but not the section name itself
				data.push_back(content[i]);					// Add the record to the return data
		}
	}
	
	return data;											// Return the data
}

// Returns the contents of a specific key within a given section
std::vector<CIniFile::Record> CIniFile::GetRecord(std::string KeyName, 
                                                  std::string SectionName,
                                                  std::string FileName)
{
	vector<Record> data;									// Holds the return data
	vector<Record> content;									// Holds the current record

	if (Load(FileName, content))							// Make sure the file is loaded
	{
		vector<Record>::iterator iter = std::find_if(content.begin(), 
				content.end(), 
				CIniFile::RecordSectionKeyIs(SectionName,KeyName));	// Locate the Record

		if (iter == content.end()) return data;				// The Record was not found

		data.push_back (*iter);								// The Record was found
	}
	return data;											// Return the Record
}

// Returns the value of a specific key within a given section
std::string CIniFile::GetValue(std::string KeyName, std::string SectionName, std::string FileName)
{
	vector<Record> content = GetRecord(KeyName,SectionName, FileName);	// Get the Record

	if(!content.empty())									// Make sure there is a value to return
		return content[0].Value;							// And return the value

	return "";												// No value was found
}

// Returns all key contents of all sections
std::vector<CIniFile::Record> CIniFile::GetSections(std::string FileName)
{
	vector<Record> data;									// Holds the return data
	vector<Record> content;									// Holds the current record													// Holds the current record

	if (Load(FileName, content))							// Make sure the file is loaded
	{
		for (int i=0;i<(int)content.size();i++)				// Loop through the content
		{
			if(content[i].Key == "")						// If this is a section 
				data.push_back(content[i]);					// Add the record to the return data
		}
	}
	
	return data;											// Return the data
}

// Returns all key contents of the file.
std::vector<CIniFile::Record> CIniFile::GetAllRecords(std::string FileName)
{
	vector<Record> content;								// Collection for the Records
	Load(FileName, content);							// Read the file.
	return content;
}

