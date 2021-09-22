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
#include "calvin_files/writers/src/CalvinCHPFileWriter.h"
//
#include "calvin_files/data/src/CHPData.h"
//

using namespace affymetrix_calvin_io;

CHPFileWriter::CHPFileWriter(CHPData &p) 
{
	writer = new GenericFileWriter(p.GetFileHeader());
	entryPos = 0;
	bgZonePos = 0;
	forcePos = 0;
	origPos = 0;
	maxProbeSetName = p.GetMaxProbeSetName();
	WriteHeaders();
}

CHPFileWriter::~CHPFileWriter() 
{
	delete writer;
}

void CHPFileWriter::WriteHeaders()
{
	writer->WriteHeader();
	DataGroupWriterIt beginGrp;
	DataGroupWriterIt endGrp;
	writer->GetDataGroupWriters(beginGrp, endGrp);
	while(beginGrp != endGrp)
	{
		DataGroupWriter* dataGroupWriter = &(*beginGrp);
		dataGroupWriter->WriteHeader();

		DataSetWriterIt beginSet;
		DataSetWriterIt endSet;
		dataGroupWriter->GetDataSetWriters(beginSet, endSet);
		while(beginSet != endSet)
		{
			dataSetWriter = &(*beginSet);
			beginSet->WriteHeader();
			SetFilePositions();
			beginSet++;
		}
		dataGroupWriter->UpdateNextDataGroupPos();
		beginGrp++;
	}
}

void CHPFileWriter::SeekToDataSet()
{
	writer->SeekFromBeginPos(entryPos);
}

void CHPFileWriter::WriteExpressionEntry(const CHPExpressionEntry& p)
{
	dataSetWriter->Write(p.GetProbeSetName(), maxProbeSetName);
	dataSetWriter->Write(p.GetDetection());
	dataSetWriter->Write(p.GetDetectionPValue());
	dataSetWriter->Write(p.GetSignal());
	dataSetWriter->Write(p.GetNumPairs());
	dataSetWriter->Write(p.GetNumPairsUsed());
	if (p.GetHasComparisonData())
	{
		dataSetWriter->Write(p.GetChange());
		dataSetWriter->Write(p.GetChangePValue());
		dataSetWriter->Write(p.GetSigLogRatio());
		dataSetWriter->Write(p.GetSigLogRatioLo());
		dataSetWriter->Write(p.GetSigLogRatioHi());
		dataSetWriter->Write(p.GetCommonPairs());
	}
}

void CHPFileWriter::WriteGenotypeEntry(const CHPGenotypeEntry& p)
{
	dataSetWriter->Write(p.GetProbeSetName(), maxProbeSetName);
	dataSetWriter->Write(p.GetCall());
	dataSetWriter->Write(p.GetConfidence());
	dataSetWriter->Write(p.GetRAS1());
	dataSetWriter->Write(p.GetRAS2());
	dataSetWriter->Write(p.GetAACall());
	dataSetWriter->Write(p.GetABCall());
	dataSetWriter->Write(p.GetBBCall());
	dataSetWriter->Write(p.GetNoCall());
}

void CHPFileWriter::WriteUniversalEntry(const CHPUniversalEntry& p)
{
	dataSetWriter->Write(p.GetBackground());
}

void CHPFileWriter::WriteReseqEntry(const CHPReseqEntry& p)
{
	dataSetWriter->Write((int8_t)p.call);
	dataSetWriter->Write(p.score);
}

void CHPFileWriter::SeekToBgSet()
{
	writer->SeekFromBeginPos(bgZonePos);
}

void CHPFileWriter::WriteBackgroundZone(const CHPBackgroundZone& zone)
{
	dataSetWriter->Write(zone.GetCenterX());
	dataSetWriter->Write(zone.GetCenterY());
	dataSetWriter->Write(zone.GetBackground());
	dataSetWriter->Write(zone.GetSmoothFactor());
}

void CHPFileWriter::SeekToForceSet()
{
	writer->SeekFromBeginPos(forcePos);
}

void CHPFileWriter::WriteForceCall(const CHPReseqForceCall& force)
{
	dataSetWriter->Write(force.position);
	dataSetWriter->Write((int8_t)force.call);
	dataSetWriter->Write((int8_t)force.reason);
}

void CHPFileWriter::SeekToOrigCallSet()
{
	writer->SeekFromBeginPos(origPos);
}

void CHPFileWriter::WriteOrigCall(const CHPReseqOrigCall& orig)
{
	dataSetWriter->Write(orig.position);
	dataSetWriter->Write((int8_t)orig.call);
}

void CHPFileWriter::SetFilePositions()
{
	const std::wstring &name = dataSetWriter->GetDataSetName();
	int32_t dataSetSz = dataSetWriter->GetDataSetSize();
	if(name == CHP_BG_ZONE_GROUP)
	{
		bgZonePos = writer->GetFilePos();
	}
	else if (name == CHP_RESEQ_FORCE_CALL_GROUP)
	{
		forcePos = writer->GetFilePos();
	}
	else if (name == CHP_RESEQ_ORIG_CALL_GROUP)
	{
		origPos = writer->GetFilePos();
	}
	else
	{
		entryPos = writer->GetFilePos();
	}

	writer->SeekFromCurrentPos(dataSetSz + 1);
	dataSetWriter->UpdateNextDataSetOffset();
}
