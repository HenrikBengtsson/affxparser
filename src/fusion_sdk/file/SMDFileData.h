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

/*! \file SMDFileData.h This file provides methods for reading a SMD file.
 */

#ifndef _SMDFILEDATA_H_
#define _SMDFILEDATA_H_

#include "file/IniFile.h"
//
#include <cstring>
#include <string>
#include <vector>
//


namespace affxsmd
{
/*! This class contains the frame information for each frame */
class SMDFrame
{
public:
	/*! Constructor */
	SMDFrame();

	/*! Frame index. */
	int frameIdx;
	/*! Number of cell rows in the frame. */
	int	rows;
	/*! Number of cell columsn in the frame. */
	int	cols;
	/*! Cell row index of the start of the frame. */
	int	startRow;
	/*! Cell column index of the start of the frame. */
	int	startCol;
};

/*! This class reads a SMD file. */
class SMDFileData
{
public:
	/*! Constructor */
	SMDFileData();
	/*! Destructor */
	virtual ~SMDFileData();

	/*! Sets the file name.
	 * @param name The full path to the SMD file.
	 */
	void SetFileName (const char *name) { filename = name; }

	/*! Gets the file name.
	 * @return The full path to the SMD file.
	 */
	const std::string GetFileName() const { return filename; }

	/*! Checks for the existance of the file passed to SetFileName.
	 * @return True if the file exists.
	 */
	bool Exists();

	/*! Read the SMD file.
	 *	@return Returns true if the file was successfully read.
	 */
	bool Read();

	/*! Get the SMDFrame information for a frame.
	 *	@param frameIdx The frame index.
	 *	@return A SMDFrame with information of the frame indicated by the frameIdx parameter.
	 */
	SMDFrame GetFrame(unsigned int frameIdx);

	/*! Get the number of frames in the SMD.
	 *	@return Number of frames.
	 */
	unsigned int NumFrames() { return (unsigned int)smdFrames.size(); };

	/*! The number of frames rows in the SMD (frames per column) */
	unsigned int frameRows;
	/*! The number of frame columns in the SMD (frames per row) */
	unsigned int frameCols;

protected:
	/*! Read the SMD data.
	 *	@return Returns true if the SMD data was successfully read.
	 */
	bool ReadData();

	/*! Clear all members except filename */
	void Clear();

	/*! Find a Record in a collection.
	 *	@param records A collection of Records.
	 *	@param section The section name of the Record to find.
	 *	@param key The key name of the Record to find.
	 *	@return A Record that matches the criteria, otherwise 0.
	 */
	CIniFile::Record* FindRecord(std::vector<CIniFile::Record>& records, std::string section, std::string key);

	/*! Get the integer value of a key.
	 *	@param records A collection of Records.
	 *	@param section The section name of the value.
	 *	@param key The key name of the value.
	 *	@param value The value found.
	 *	@return Returns true if the value was found in the collection.
	 */
	bool GetIntValue(std::vector<CIniFile::Record>& records, std::string section, std::string key, int& value);

	/*! A flag indicating if a SMD has been read. */
	bool isRead;
	/*! SMD file name.  Including the relation or full path. */
	std::string filename;
	/*! A collection of SMDFrame information for all frames in the SMD. */
	std::vector<SMDFrame> smdFrames;
};

}


#endif // _SMDFILEDATA_H_
