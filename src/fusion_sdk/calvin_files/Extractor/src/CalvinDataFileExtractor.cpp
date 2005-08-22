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

#include "CmdLine.h"
#include "AffyStlCollectionTypes.h"
#include "GenericFileReader.h"
#include "Parameter.h"
#include <iostream>

using namespace std;
using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_parameter;

#define OUTPUT_DATA "DATA"
#define OUTPUT_HEADER "HEADER"
#define INPUT_FILE_ARG "-i"
#define DATA_TYPE_ARG "-t"
#define GROUP_ARG "-g"
#define SET_ARG "-s"
#define ROW_START_ARG "-rs"
#define ROW_END_ARG "-re"
#define HELP_ARG "-h"
#define DEFAULT_GROUP "0"
#define DEFAULT_SET "0"
#define DEFAULT_ROW_START "0"
#define DEFAULT_ROW_END "-1"

/*
 * Show a help message.
 */
void ShowHelp()
{
	cout << "Command line arguments:" << endl;
	cout << " " << INPUT_FILE_ARG << " <the name of the input file>" << endl;
	cout << " " << DATA_TYPE_ARG << " <\"" << OUTPUT_DATA << "\" for data or \"" << OUTPUT_HEADER << "\" for header information. DATA is the default.>" << endl;
	cout << " " << GROUP_ARG << " <the group index (zero based) to export. " << DEFAULT_GROUP << " is the default.>" << endl;
	cout << " " << SET_ARG << " <the set index (zero based) to export. " << DEFAULT_SET << " is the default.>" << endl;
	cout << " " << ROW_START_ARG << " <the start row (zero based) to extract. " << DEFAULT_ROW_START << " is the default.>" << endl;
	cout << " " << ROW_END_ARG << " <the end row (zero based) to extract> or " << DEFAULT_ROW_END << " for the last row. " << DEFAULT_ROW_END << " is the default.>" << endl;
}

/*
 * Read the file into a data object.
 */
bool ReadFile(const string &fileName, GenericData &gdata)
{
	GenericFileReader reader;

	// Read the file.
	reader.SetFilename(fileName);
	try
	{
		reader.ReadHeader(gdata);
	}
	catch(...)
	{
		return false;
	}
	return true;
}

/*
 * Output the data to the command line.
 */
void OutputData(const string &fileName, int groupIndex, int setIndex, int startRow, int endRow)
{
	GenericData gdata;

	// Read the file.
	if (ReadFile(fileName, gdata) == false)
	{
		cout << "Error reading the file." << endl;
		return;
	}
	DataSet *set = gdata.DataSet(groupIndex, setIndex);
	if (set->Open() == false)
	{
		cout << "Error reading the file." << endl;
		return;
	}

	// Output the rows of data
	int8_t i8;
	u_int8_t iu8;
	int16_t i16;
	u_int16_t iu16;
	int32_t i32;
	u_int32_t iu32;
	float f32;
	std::string s;
	std::wstring ws;
	int nRows = set->Rows();
	int nCols = set->Cols();
	startRow = min(max(startRow, 0), nRows-1);
	if (endRow < 0)
		endRow = nRows-1;
	endRow = min(endRow, nRows-1);
	for (int row=startRow; row<=endRow; row++)
	{
		for (int col=0; col<nCols; col++)
		{
			switch (set->Header().GetColumnType(col).GetColumnType())
			{
			case ByteColType:
				set->GetData(row, col, i8);
				cout << i8;
				break;
			case UByteColType:
				set->GetData(row, col, iu8);
				cout << iu8;
				break;
			case ShortColType:
				set->GetData(row, col, i16);
				cout << i16;
				break;
			case UShortColType:
				set->GetData(row, col, iu16);
				cout << iu16;
				break;
			case IntColType:
				set->GetData(row, col, i32);
				cout << i32;
				break;
			case UIntColType:
				set->GetData(row, col, iu32);
				cout << iu32;
				break;
			case FloatColType:
				set->GetData(row, col, f32);
				cout << f32;
				break;
			case ASCIICharColType:
				set->GetData(row, col, s);
				cout << s;
				break;
			case UnicodeCharColType:
				set->GetData(row, col, ws);
				wcout << ws;
				break;
			default:
				cout << " ";
				break;
			}
			cout << "\t";
		}
		cout << endl;
	}
	set->Close();
	set->Delete();
}

/*
 * Output the file header parameters.
 */
void OutputFileHeaderParameters(GenericData &gdata)
{
	cout << "#File Header Information" << endl;

	// Output the magic and version numbers.
	cout << "Magic=" << (int) gdata.Header().GetMagicNumber() << endl;
	cout << "Version=" << (int) gdata.Header().GetVersion() << endl;
}

/*
 * Output the header parameters.
 */
void OutputParameters(ParameterNameValueTypeIt &begin, ParameterNameValueTypeIt &end)
{
	ParameterNameValueType param;
	ParameterNameValueTypeIt it;
	for (it=begin; it!=end; ++it)
	{
		param = *it;
		wcout << param.GetName() << "=" << param.ToString() << endl;
	}
}

/*
 * Output the header parameters.
 */
void OutputParameters(ParameterNameValueTypeConstIt &begin, ParameterNameValueTypeConstIt &end)
{
	ParameterNameValueType param;
	ParameterNameValueTypeConstIt it;
	for (it=begin; it!=end; ++it)
	{
		param = *it;
		wcout << param.GetName() << "=" << param.ToString() << endl;
	}
}

/*
 * Output the data header parameters.
 */
void OutputDataHeaderParameters(GenericDataHeader *hdr)
{
	cout << "#Data Header Information" << endl;

	// Add the file id's.
	if (hdr->GetFileId().length() > 0)
	{
		cout << "File Identifier=" << hdr->GetFileId() << endl;
	}
	if (hdr->GetFileTypeId().length() > 0)
	{
		cout << "File Type Identifier=" << hdr->GetFileTypeId() << endl;
	}

	// Add the parameters.
	ParameterNameValueTypeIt begin;
	ParameterNameValueTypeIt end;
	hdr->GetNameValIterators(begin, end);
	OutputParameters(begin, end);

	int nParents = hdr->GetParentCnt();
	for (int iParent=0; iParent<nParents; iParent++)
	{
		OutputDataHeaderParameters(&hdr->GetParent(iParent));
	}
}

/*
 * Output the set information, only the header.
 */
void OutputSetInfo(GenericData &gdata, int groupIndex, int setIndex)
{
	DataSet *pSet = gdata.DataSet(groupIndex, setIndex);
	wcout << "Set Name=" << pSet->Header().GetName() << endl;

	// Add the row and column count.
	cout << "Columns=" << pSet->Cols() << endl;
	cout << "Rows=" << pSet->Rows() << endl;

	// Add the parameters.
	ParameterNameValueTypeConstIt begin;
	ParameterNameValueTypeConstIt end;
	pSet->Header().GetNameValIterators(begin, end);
	OutputParameters(begin, end);
	pSet->Delete();
}

/*
 * Output the group information.
 */
void OutputGroupInfo(GenericData &gdata, const std::wstring &groupName, int groupIndex)
{
	wcout << "Group Name=" << groupName << endl;

	// Add each set.
	int n = gdata.DataSetCnt(groupIndex);
	for (int i=0; i<n; i++)
	{
		OutputSetInfo(gdata, groupIndex, i);
	}
}

/*
 * Output the groups and sets.
 */
void OutputGroupsAndSets(GenericData &gdata)
{
	cout << "#Groups/Sets" << endl;

	// Add each group.
	WStringVector groupNames;
	gdata.DataGroupNames(groupNames);
	int nGroups = (int)groupNames.size();
	for (int i=0; i<nGroups; i++)
	{
		OutputGroupInfo(gdata, groupNames[i], i);
	}
}

/*
 * Output the header information.
 */
void OutputHeader(const string &fileName)
{
	GenericData gdata;

	// Read the file.
	if (ReadFile(fileName, gdata) == false)
	{
		cout << "Error reading the file." << endl;
		return;
	}

	OutputFileHeaderParameters(gdata);

	OutputDataHeaderParameters(gdata.Header().GetGenericDataHdr());

	OutputGroupsAndSets(gdata);
}

/*
 * Extract the information from the Calvin data file and dump it to the command line.
 */
int main(int argc, char* argv[])
{
	CCmdLine cmdLine;

	// parse argc,argv 
	// no switches were given on the command line, abort
	if (cmdLine.SplitLine(argc, argv) < 1)
	{
		ShowHelp();
		return 0;
	}

	// test for the 'help' case
	if (cmdLine.HasSwitch(HELP_ARG))
	{
		ShowHelp();
		return 0;
	}

	// get the required arguments
	string fileName;
	string typeArg;
	string groupArg;
	string setArg;
	int groupNumber=-1;
	int setNumber=-1;
	int startRow=-1;
	int endRow=-1;
	try
	{
		fileName = cmdLine.GetArgument(INPUT_FILE_ARG, 0);
		typeArg = cmdLine.GetSafeArgument(DATA_TYPE_ARG, 0, OUTPUT_DATA);
		groupArg = cmdLine.GetSafeArgument(GROUP_ARG, 0, DEFAULT_GROUP);
		groupNumber = atoi(groupArg.c_str());
		setArg = cmdLine.GetSafeArgument(SET_ARG, 0, DEFAULT_SET);
		setNumber = atoi(setArg.c_str());
		startRow = atoi(cmdLine.GetSafeArgument(ROW_START_ARG, 0, DEFAULT_ROW_START).c_str());
		endRow = atoi(cmdLine.GetSafeArgument(ROW_END_ARG, 0, DEFAULT_ROW_END).c_str());
	}
	catch (...)
	{
		// one of the required arguments was missing, abort
		ShowHelp();
		return -1;
	}

	// Output the data.
	if (typeArg == OUTPUT_DATA)
	{
		OutputData(fileName, groupNumber, setNumber, startRow, endRow);
	}
	else if (typeArg == OUTPUT_HEADER)
	{
		OutputHeader(fileName);
	}

	return 0;
}

