////////////////////////////////////////////////////////////////
//
// Copyright (C) 2008 Affymetrix, Inc.
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

#include "calvin_files/writers/src/DataSetHeaderAppend.h"
//
#include "calvin_files/parsers/src/GenericFileReader.h"
#include "calvin_files/utils/src/StringUtils.h"
#include "calvin_files/writers/src/GenericFileUpdater.h"
//

#ifdef _HAS_APT_VERBOSE_
#include "util/Err.h"
#include "util/Verbose.h"
#endif

using namespace std;
using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_utilities;
using namespace affymetrix_calvin_parameter;

#ifdef _HAS_APT_VERBOSE_
#define VERBOSE(level, msg) { Verbose::out(level, msg); }
#define ERR(msg) { Err::errAbort(msg); }
#else
#define VERBOSE(level, msg) { }
#define ERR(msg) { throw msg; }
#endif

static void AddDataSetHeaderToGroup(GenericFileWriter *gwf, int groupIndex, int nsets, FileHeader &fileHdr, const wstring &setName, const ParameterNameValueTypeList &params)
{
	DataSetHeader hdr;
	hdr.SetName(setName);
	hdr.SetRowCnt(0);
	for (ParameterNameValueTypeList::const_iterator it=params.begin(); it!=params.end(); it++)
	{
		hdr.AddNameValParam(*it);
	}

	// Write the file
	DataSetWriter* writer = NULL;
	try
	{
		int groupPos = fileHdr.GetFirstDataGroupFilePos();
		if (groupIndex > 0)
			groupPos = fileHdr.GetDataGroup(groupIndex-1).GetNextGroupPos();
		if (nsets == 0)
		{
			gwf->SeekFromEndPos(0);
			int setPos = gwf->GetFilePos();
			gwf->SeekFromBeginPos(groupPos + 4);
			gwf->Write(setPos);
		}
		gwf->SeekFromBeginPos(groupPos + 8);
		gwf->Write(nsets + 1);

		writer = gwf->GetDataGroupWriter(groupIndex).CreateDataSetWriter(hdr);
		gwf->SeekFromEndPos(0);
		writer->WriteHeader();
		writer->UpdateNextDataSetOffset();
		delete writer;
	}
	catch (...)
	{
		delete writer;
		writer = NULL;
		ERR("Failed to update the file");
	}
}

void DataSetHeaderAppend::AddDataSetHeader(const string &file, const wstring &groupName, const list<DataSetHeaderInformation> &params)
{
	GenericData gData;
	GenericFileReader reader;
	reader.SetFilename(file);
	reader.ReadHeader(gData);
	gData.Close();
	try
	{
		FileHeader &fileHdr = gData.Header();
		int32_t ng = fileHdr.GetDataGroupCnt();
		bool groupFound = false;
		int groupIndex = 0;
		for (int ig=0; ig<ng; ig++)
		{
			DataGroupHeader &dataGrpHdr = fileHdr.GetDataGroup(ig);
			if (dataGrpHdr.GetName() != groupName)
				continue;

			groupFound = true;
			groupIndex = ig;
			int ns = dataGrpHdr.GetDataSetCnt();
			for (int is=0; is<ns; is++)
			{
				DataSetHeader &dataSetHdr = dataGrpHdr.GetDataSet(is);
				for (list<DataSetHeaderInformation>::const_iterator it=params.begin(); it!=params.end(); it++)
				{
					//bool lastSet = (is == (ns-1) && ig == (ng-1));
					//if (params.size() > 1)
					//	lastSet = false;
					if (dataSetHdr.GetName() == it->dataSetName/* && lastSet == false*/)
					{
						u_int32_t offset = dataSetHdr.GetHeaderStartFilePos();
						GenericFileWriter* writer = new GenericFileWriter(&fileHdr, false);
						writer->SeekFromBeginPos(offset);
						wstring name = dataSetHdr.GetName();
						for (int i=0; i<(int)name.length(); i++)
							name[i] = '-';
						dataSetHdr.SetName(name);
						DataSetWriter& setWriter = writer->GetDataGroupWriter(ig).GetDataSetWriter(is);
						setWriter.WriteHeader();
						setWriter.UpdateNextDataSetOffset();
						delete writer;
						break;
					}
				}
			}
		}

		if (groupFound == false)
		{
			groupIndex = ng;
			DataGroupHeader dgh;
			dgh.SetName(groupName);
			dgh.SetNextGroupPos(0);
			dgh.SetDataSetPos(0);
			fileHdr.AddDataGroupHdr(dgh);

			GenericFileWriter* writer = new GenericFileWriter(&fileHdr, false);
			writer->WriteHeader();
			for (int ig=0; ig<ng; ig++)
			{
				DataGroupHeader& currentHdr = fileHdr.GetDataGroup(ig);
				int nextGroupPos = (ig < ng - 1 ? currentHdr.GetNextGroupPos() : 0);
				if (nextGroupPos == 0)
				{
					int currentPos = writer->GetFilePos();
					writer->SeekFromEndPos(0);
					int endPos = writer->GetFilePos();
					currentHdr.SetNextGroupPos(endPos);
					writer->SeekFromBeginPos(currentPos);
					DataGroupWriter &groupWriter = writer->GetDataGroupWriter(ig);
					groupWriter.WriteHeader();
					writer->SeekFromBeginPos(currentHdr.GetNextGroupPos());
					groupWriter.UpdateNextDataGroupPos();
				}
				writer->SeekFromBeginPos(currentHdr.GetNextGroupPos());
			}
			DataGroupWriter &groupWriter = writer->GetDataGroupWriter(ng);
			groupWriter.WriteHeader();
			writer->SeekFromBeginPos(0);
			groupWriter.UpdateNextDataGroupPos();
			delete writer;
		}

		VERBOSE(1, "Merging data into the file");
		GenericFileWriter *gwf = new GenericFileWriter(&fileHdr, false);
		DataGroupHeader &dgh = fileHdr.GetDataGroup(groupIndex);
		int nsets = dgh.GetDataSetCnt();
		for (list<DataSetHeaderInformation>::const_iterator it=params.begin(); it!=params.end(); it++)
		{
			AddDataSetHeaderToGroup(gwf, groupIndex, nsets, fileHdr, it->dataSetName, it->params);
			++nsets;
		}
		delete gwf;
	}
	catch (...)
	{
		ERR("Error updating the file");
	}

}

