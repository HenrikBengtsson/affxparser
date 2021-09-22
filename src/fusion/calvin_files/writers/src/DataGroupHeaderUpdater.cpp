////////////////////////////////////////////////////////////////
//
// Copyright (C) 2006 Affymetrix, Inc.
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


#include "calvin_files/writers/src/DataGroupHeaderUpdater.h"
//
#include "calvin_files/writers/src/FileOutput.h"
//

using namespace std;
using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_parameter;

#define WCHAR_T_SIZE 2

/*
 * Constructor
 */

DataGroupHeaderUpdater::DataGroupHeaderUpdater(std::ofstream& fileStream, DataGroupHeader &hdr)
{
	os = &fileStream;
	grpHdr = &hdr;
}

/*
* Destructor
*/
DataGroupHeaderUpdater::~DataGroupHeaderUpdater()
{
}

void DataGroupHeaderUpdater::SeekNextDataGrpPosition()
{
	u_int32_t pos = grpHdr->GetHeaderStartFilePos();
	os->seekp(pos, std::ios::beg);
}

void DataGroupHeaderUpdater::SeekLeadDataSetPosition()
{
	u_int32_t pos = grpHdr->GetHeaderStartFilePos();
	pos += sizeof(u_int32_t); // next data group position
	os->seekp(pos, std::ios::beg);
}

void DataGroupHeaderUpdater::SeekDataSetCntPosition()
{
	u_int32_t pos = grpHdr->GetHeaderStartFilePos();
	pos += sizeof(u_int32_t); // next data group position
	pos += sizeof(u_int32_t); // lead data set position
	os->seekp(pos, std::ios::beg);
}

void DataGroupHeaderUpdater::UpdateNextDataGroupOffset(u_int32_t offset)
{
	u_int32_t pos = os->tellp();
	SeekNextDataGrpPosition();
	FileOutput::WriteUInt32(*os, offset);
	os->seekp(pos, std::ios::beg);
}

void DataGroupHeaderUpdater::UpdateLeadDataSetOffset(u_int32_t offset)
{
	u_int32_t pos = os->tellp();
	SeekLeadDataSetPosition();
	FileOutput::WriteUInt32(*os, offset);
	os->seekp(pos, std::ios::beg);
}

void DataGroupHeaderUpdater::UpdateDataSetCount(u_int32_t count)
{
	u_int32_t pos = os->tellp();
	SeekDataSetCntPosition();
	FileOutput::WriteUInt32(*os, count);
	os->seekp(pos, std::ios::beg);
}

void DataGroupHeaderUpdater::AppendDataSetHeader(DataSetHeader& setHdr)
{
	os->seekp(0, std::ios::end);
	int sz = grpHdr->GetDataSetCnt();
	u_int32_t setPos = os->tellp();
	setHdr.SetHeaderStartFilePos(setPos);
	if(sz > 0)
	{
		//update previous data set with new position
		DataSetHeader lastHdr = grpHdr->GetDataSet(sz - 1);
		DataSetHeaderUpdater lastSetUpdater(*os, lastHdr);
		lastSetUpdater.UpdateNextDataSetPosition(setPos);
	}
	else
	{
		UpdateLeadDataSetOffset(setPos);
	}
	//increment the number of data sets in the group
	UpdateDataSetCount(sz + 1);
	DataSetWriter setWriter(os, &setHdr);
	setWriter.WriteHeader();
	setHdr.SetDataStartFilePos(os->tellp());
	os->seekp(setWriter.GetDataSetSize(), std::ios::cur);
	setWriter.UpdateNextDataSetOffset();
	setHdr.SetNextSetFilePos(os->tellp());
	grpHdr->AddDataSetHdr(setHdr);
}

void DataGroupHeaderUpdater::ReplaceDataSetHeader(std::wstring targetName, DataSetHeader& newHdr)
{
	os->seekp(0, std::ios::end);
	u_int32_t setPos = os->tellp();
	u_int32_t nextSetPos = 0;
	newHdr.SetHeaderStartFilePos(setPos);

	DataSetHdrIt prev, current, end;
	grpHdr->GetDataSetIterators(current,end);
	prev = current;
	if(current == end)
	{
		//no sets. we're done
		return;
	}
	current++;
	if(prev->GetName() == targetName)
	{
		//first set is the target
		UpdateLeadDataSetOffset(setPos);
		if(current != end)
		{
			//save next set offset
			nextSetPos = current->GetHeaderStartFilePos();
		}
	}
	else
	{
		while(current != end)
		{
			if(current->GetName() == targetName)
			{

				//OPTIMIZATION: if the new data set size is <= the size of the target set then
				//we should add it in place of the target and NOT orphan the target set.
				//NOTE: it is not easy to calculate the new data set size.  ParamaterNameValueType
				//should be modified to return it's size.
				//int currentSz = (current->GetDataStartFilePos() - current->GetHeaderStartFilePos()) + current->GetDataSize();

				//found the target
				DataSetHeaderUpdater prevSetUpdater(*os, *prev);
				prevSetUpdater.UpdateNextDataSetPosition(setPos);
				current++;
				if(current != end)
				{
					nextSetPos = current->GetHeaderStartFilePos();
				}
				break;
			}
			else
			{
				prev = current;
				current++;
			}
		}
	}
	DataSetWriter setWriter(os, &newHdr);
	setWriter.WriteHeader();
	newHdr.SetDataStartFilePos(os->tellp());
	os->seekp(setWriter.GetDataSetSize(), std::ios::cur);
	if(nextSetPos > 0)
	{
		setWriter.UpdateNextDataSetOffset(nextSetPos);
		newHdr.SetNextSetFilePos(nextSetPos);
	}
	else
	{
		setWriter.UpdateNextDataSetOffset();
		newHdr.SetNextSetFilePos(os->tellp());
	}
	grpHdr->ReplaceDataSetHdr(newHdr);
}

