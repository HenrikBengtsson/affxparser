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


#include "CalvinCHPFileUpdater.h"
#include "FileOutput.h"
#include "util/Err.h"
#include "util/Util.h"

using namespace std;
using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_parameter;

/*! The data set index. */
#define ENTRY_DATA_SET 0

/*! The data group index. */
#define ENTRY_DATA_GROUP 0

/*! Probetset name and signal columns. */
#define NAME_COLUMN 0
#define ENTRY_COLUMN 1

/*
 * Initialize any needed members.
 */
CalvinCHPFileUpdater::CalvinCHPFileUpdater() : DataSetUpdater()
{
	m_CHPFile = NULL;
}

/*
 * Clean up.
 */
CalvinCHPFileUpdater::~CalvinCHPFileUpdater()
{
	CloseCHPFile();
}

void CalvinCHPFileUpdater::Initialize(const char *file)
{
    DataSetUpdater::Initialize(file);
    dataSetIndexMap.clear();
    int nds=(int)dataSetNames[0].size();
    int ndt=sizeof(MultiDataDataTypes) / sizeof(MultiDataType);
    for (int ids=0; ids<nds; ids++)
    {
        for (int idt=0; idt<ndt; idt++)
        {
            if (dataSetNames[0][ids] == MultiDataDataSetNames[idt])
            dataSetIndexMap[MultiDataDataTypes[idt]] = ids;
        }
    }
}

void CalvinCHPFileUpdater::OpenCHPFile(const char *fileName)
{
	CloseCHPFile();
	Initialize(fileName);
	m_CHPFile = new ofstream(fileName, ios::out | ios::in | ios::binary);
	if (!m_CHPFile) { Err::errAbort("Error: CalvinCHPFileUpdater::OpenCHPFile() - Unable to open CHP file for updating: " + ToStr(fileName)); }
}

void CalvinCHPFileUpdater::UpdateExpressionQuantification(int row, float quantification)
{
	SeekToPosition(*m_CHPFile, ENTRY_DATA_GROUP, ENTRY_DATA_SET, row, ENTRY_COLUMN);
	FileOutput::WriteFloat(*m_CHPFile, quantification);
}

void CalvinCHPFileUpdater::UpdateExpressionQuantificationBuffer(int row_start, std::vector<float> &expressionSignalBuffer)
{
	// seek to start of update row (note NAME_COLUMN is 0)
	SeekToPosition(*m_CHPFile, ENTRY_DATA_GROUP, ENTRY_DATA_SET, row_start, NAME_COLUMN);
	int iProbeSetNameColumnSize = colsizes[ENTRY_DATA_GROUP][ENTRY_DATA_SET][NAME_COLUMN];
	for (int i=0; i<(int)expressionSignalBuffer.size(); i++) 
	{
		m_CHPFile->seekp(iProbeSetNameColumnSize, std::ios::cur);
		FileOutput::WriteFloat(*m_CHPFile, expressionSignalBuffer[i]);
	}
}

void CalvinCHPFileUpdater::UpdateGenotypeEntry(int row, u_int8_t call, float confidence, float RAS1, float RAS2, float aaCall, float abCall, float bbCall, float noCall)
{
	SeekToPosition(*m_CHPFile, ENTRY_DATA_GROUP, ENTRY_DATA_SET, row, ENTRY_COLUMN);
	FileOutput::WriteUInt8(*m_CHPFile, call);
	FileOutput::WriteFloat(*m_CHPFile, confidence);
	FileOutput::WriteFloat(*m_CHPFile, RAS1);
	FileOutput::WriteFloat(*m_CHPFile, RAS2);
	FileOutput::WriteFloat(*m_CHPFile, aaCall);
	FileOutput::WriteFloat(*m_CHPFile, abCall);
	FileOutput::WriteFloat(*m_CHPFile, bbCall);
	FileOutput::WriteFloat(*m_CHPFile, noCall);
}

void CalvinCHPFileUpdater::UpdateGenotypeEntryBuffer(int row_start, const std::vector<CHPFileBufferWriter::GenotypeBufferEntry> &genotypeEntryBuffer)
{
	// seek to start of update row (note NAME_COLUMN is 0)
	SeekToPosition(*m_CHPFile, ENTRY_DATA_GROUP, ENTRY_DATA_SET, row_start, NAME_COLUMN);
	int iProbeSetNameColumnSize = colsizes[ENTRY_DATA_GROUP][ENTRY_DATA_SET][NAME_COLUMN];
	for (int i=0; i<(int)genotypeEntryBuffer.size(); i++) 
	{
		m_CHPFile->seekp(iProbeSetNameColumnSize, std::ios::cur);
		FileOutput::WriteUInt8(*m_CHPFile, genotypeEntryBuffer[i].call);
		FileOutput::WriteFloat(*m_CHPFile, genotypeEntryBuffer[i].confidence);
		FileOutput::WriteFloat(*m_CHPFile, genotypeEntryBuffer[i].RAS1);
		FileOutput::WriteFloat(*m_CHPFile, genotypeEntryBuffer[i].RAS2);
		FileOutput::WriteFloat(*m_CHPFile, genotypeEntryBuffer[i].aaCall);
		FileOutput::WriteFloat(*m_CHPFile, genotypeEntryBuffer[i].abCall);
		FileOutput::WriteFloat(*m_CHPFile, genotypeEntryBuffer[i].bbCall);
		FileOutput::WriteFloat(*m_CHPFile, genotypeEntryBuffer[i].noCall);
	}
}

void CalvinCHPFileUpdater::UpdateMultiDataGenotypeEntry(MultiDataType dataType, int row, const affymetrix_calvin_data::ProbeSetMultiDataGenotypeData &entry)
{
    int dsIndex = dataSetIndexMap[dataType];
	SeekToPosition(*m_CHPFile, ENTRY_DATA_GROUP, dsIndex, row, ENTRY_COLUMN);
	FileOutput::WriteUInt8(*m_CHPFile, entry.call);
	FileOutput::WriteFloat(*m_CHPFile, entry.confidence);
    UpdateMetrics(entry.metrics);
}

void CalvinCHPFileUpdater::UpdateMultiDataGenotypeEntryBuffer(MultiDataType dataType, int row_start, const std::vector<affymetrix_calvin_data::ProbeSetMultiDataGenotypeData> &genotypeEntryBuffer)
{
	// seek to start of update row (note NAME_COLUMN is 0)
    int dsIndex = dataSetIndexMap[dataType];
	SeekToPosition(*m_CHPFile, ENTRY_DATA_GROUP, dsIndex, row_start, NAME_COLUMN);
	int iProbeSetNameColumnSize = colsizes[ENTRY_DATA_GROUP][ENTRY_DATA_SET][NAME_COLUMN];
	for (int i=0; i<(int)genotypeEntryBuffer.size(); i++) 
	{
		m_CHPFile->seekp(iProbeSetNameColumnSize, std::ios::cur);
		FileOutput::WriteUInt8(*m_CHPFile, genotypeEntryBuffer[i].call);
		FileOutput::WriteFloat(*m_CHPFile, genotypeEntryBuffer[i].confidence);
        UpdateMetrics(genotypeEntryBuffer[i].metrics);
    }
}

void CalvinCHPFileUpdater::UpdateMultiDataExpressionEntry(MultiDataType dataType, int row, const affymetrix_calvin_data::ProbeSetMultiDataExpressionData &entry)
{
    int dsIndex = dataSetIndexMap[dataType];
	SeekToPosition(*m_CHPFile, ENTRY_DATA_GROUP, dsIndex, row, ENTRY_COLUMN);
	FileOutput::WriteFloat(*m_CHPFile, entry.quantification);
    UpdateMetrics(entry.metrics);
}

void CalvinCHPFileUpdater::UpdateMultiDataExpressionEntryBuffer(MultiDataType dataType, int row_start, const std::vector<affymetrix_calvin_data::ProbeSetMultiDataExpressionData> &expressionEntryBuffer)
{
	// seek to start of update row (note NAME_COLUMN is 0)
    int dsIndex = dataSetIndexMap[dataType];
	SeekToPosition(*m_CHPFile, ENTRY_DATA_GROUP, dsIndex, row_start, NAME_COLUMN);
	int iProbeSetNameColumnSize = colsizes[ENTRY_DATA_GROUP][ENTRY_DATA_SET][NAME_COLUMN];
	for (int i=0; i<(int)expressionEntryBuffer.size(); i++) 
	{
		m_CHPFile->seekp(iProbeSetNameColumnSize, std::ios::cur);
		FileOutput::WriteFloat(*m_CHPFile, expressionEntryBuffer[i].quantification);
        UpdateMetrics(expressionEntryBuffer[i].metrics);
    }
}

void CalvinCHPFileUpdater::UpdateMetrics(const std::vector<affymetrix_calvin_parameter::ParameterNameValueType> &metrics)
{
	int ncols = (int) metrics.size();
	for (int icol=0; icol<ncols; icol++)
	{
        const ParameterNameValueType &nv = metrics[icol];
		switch (nv.GetParameterType())
		{
			case ParameterNameValueType::Int8Type:
                FileOutput::WriteInt8(*m_CHPFile, nv.GetValueInt8());
				break;

			case ParameterNameValueType::UInt8Type:
				FileOutput::WriteUInt8(*m_CHPFile, nv.GetValueUInt8());
				break;

			case ParameterNameValueType::Int16Type:
				FileOutput::WriteInt16(*m_CHPFile, nv.GetValueInt16());
				break;

			case ParameterNameValueType::UInt16Type:
				FileOutput::WriteUInt16(*m_CHPFile, nv.GetValueUInt16());
				break;

			case ParameterNameValueType::Int32Type:
				FileOutput::WriteInt32(*m_CHPFile, nv.GetValueInt32());
				break;

			case ParameterNameValueType::UInt32Type:
				FileOutput::WriteUInt32(*m_CHPFile, nv.GetValueUInt32());
				break;

			case ParameterNameValueType::FloatType:
				FileOutput::WriteFloat(*m_CHPFile, nv.GetValueFloat());
				break;

			case ParameterNameValueType::AsciiType:
				FileOutput::WriteString8(*m_CHPFile, nv.GetValueAscii());
				break;

			case ParameterNameValueType::TextType:
                FileOutput::WriteString16(*m_CHPFile, nv.GetValueText());
				break;
        // @todo Check that this is ok.
    case ParameterNameValueType::UnknownType:
      break;
		}
	}
}

void CalvinCHPFileUpdater::CloseCHPFile()
{
	if (m_CHPFile != NULL)
	{
		if (m_CHPFile->is_open() == true) { m_CHPFile->close(); }
		delete m_CHPFile;
		m_CHPFile = NULL;
	}
}
