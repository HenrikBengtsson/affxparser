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

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

//
#include "file/SMDFileData.h"
//
#include <sys/stat.h>

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

/*! INI file summary section name. */
static const std::string SummarySec	= "SUMMARY";

/*! INI file key name for the number of frames. */
static const std::string Frames		= "NumberOfFrames";

/*! INI file key name for the number of frames per row or frame columns. */
static const std::string FrameCols	= "FramesPerRow";

/*! INI file key name for the number of frames per column or frame rows. */
static const std::string FrameRows	= "FramesPerCol";

/*! INI file key name for the number of cell rows in the frame. */
static const std::string KeyRows		= "Rows";

/*! INI file key name for the number of cell columns in the frame. */
static const std::string KeyCols		= "Cols";

/*! INI file key name for the cell row index of the start of the frame. */
static const std::string KeyStartRow	= "StartRow";

/*! INI file key name for the cell column index of the start of the frame. */
static const std::string KeyStartCol	= "StartCol";

using namespace affxsmd;

/*
 * Constructor
 */
SMDFrame::SMDFrame()
{
	// Initialize to an invalid frame.
	frameIdx = -1;
	rows = -1;
	cols = -1;
	startRow = -1;
	startCol = -1;
}

/*
 * Constructor
 */
SMDFileData::SMDFileData()
{
	Clear();
}

/*
 * Destructor
 */
SMDFileData::~SMDFileData()
{

}

/*
 * Check for the file existance
 */
bool SMDFileData::Exists()
{
	// Find the file stats.
	struct stat st;
	return (stat(filename.c_str(), &st) == 0);
}

/*
 * Get the SMDFrame information for a frame.
 */
SMDFrame SMDFileData::GetFrame(unsigned int frameIdx)
{
	if (frameIdx >= 0 && frameIdx < smdFrames.size())
	{
		return smdFrames[frameIdx];
	}
	else
	{
		SMDFrame invalidFrame;
		return invalidFrame;
	}
}

/*
 * Read the SMD file.
 */
bool SMDFileData::Read()
{
	if (filename.length() == 0)
		return false;

	isRead = ReadData();
	return isRead;
}

/*
 * Read the SMD data.
 */
bool SMDFileData::ReadData()
{
	Clear();

	CIniFile iniFile;
	std::vector<CIniFile::Record> allRecords = iniFile.GetAllRecords(filename);
	
	if (allRecords.size() == 0)
		// Either the file does not exist or is not the correct format.
		return false;	

	int numFrames = 0;
	if (GetIntValue(allRecords, SummarySec, Frames, numFrames) == false)
		return false;

	if (numFrames < 1)
		return false;

	int numRows = 0;
	if (GetIntValue(allRecords, SummarySec, FrameRows, numRows) == false)
		return false;

	int numCols = 0;
	if (GetIntValue(allRecords, SummarySec, FrameCols, numCols) == false)
		return false;

	frameRows = numRows;
	frameCols = numCols;

	for (int iFrame = 0; iFrame < numFrames; ++iFrame)
	{
		SMDFrame frame;
		char sectionName[10];
		snprintf(sectionName,sizeof(sectionName), "%d", iFrame);

		frame.frameIdx = iFrame;

		if (GetIntValue(allRecords, sectionName, KeyRows, frame.rows) == false)
			return false;
		if (GetIntValue(allRecords, sectionName, KeyCols, frame.cols) == false)
			return false;
		if (GetIntValue(allRecords, sectionName, KeyStartRow, frame.startRow) == false)
			return false;
		if (GetIntValue(allRecords, sectionName, KeyStartCol, frame.startCol) == false)
			return false;
	
		smdFrames.push_back(frame);
	}
	
	return true;
}

/*
 * Clear all members except filename.
 */
void SMDFileData::Clear()
{
	smdFrames.clear();
	frameRows = 0;
	frameCols = 0;
	isRead = false;
}

/*
 * Find a Record in a collection.
 * Move into CIniFile?
 */
CIniFile::Record* SMDFileData::FindRecord(std::vector<CIniFile::Record>& records, std::string section, std::string key)
{
	for (std::vector<CIniFile::Record>::iterator ii = records.begin(); ii != records.end(); ++ii)
	{
		if (ii->Key == key && ii->Section == section)
			return &(*ii);
	}
	return 0;
}

/*
 * Get the integer value of a key.
 */
bool SMDFileData::GetIntValue(std::vector<CIniFile::Record>& records, std::string section, std::string key, int& value)
{
	CIniFile::Record* record = FindRecord(records, section, key);
	if (record == 0)
		return false;
	value = atoi(record->Value.c_str());
	return true;
}
