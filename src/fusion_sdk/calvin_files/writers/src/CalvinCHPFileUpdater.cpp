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
#include "FileIO.h"
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
	if (!m_CHPFile) { Err::errAbort("CalvinCHPFileUpdater::OpenCHPFile() - Unable to open CHP file for updating: " + ToStr(fileName)); }
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

static int GetMetricBufferSize(const std::vector<affymetrix_calvin_parameter::ParameterNameValueType> &metrics)
{
    int bufferSize = 0;
	int ncols = (int) metrics.size();
	for (int icol=0; icol<ncols; icol++)
	{
		const ParameterNameValueType &nv = metrics[icol];
		switch (nv.GetParameterType())
		{
			case ParameterNameValueType::Int8Type:
				bufferSize += sizeof(nv.GetValueInt8());
				break;

			case ParameterNameValueType::UInt8Type:
				bufferSize += sizeof(nv.GetValueUInt8());
				break;

			case ParameterNameValueType::Int16Type:
				bufferSize += sizeof(nv.GetValueInt16());
				break;

			case ParameterNameValueType::UInt16Type:
				bufferSize += sizeof(nv.GetValueUInt16());
				break;

			case ParameterNameValueType::Int32Type:
				bufferSize += sizeof(nv.GetValueInt32());
				break;

			case ParameterNameValueType::UInt32Type:
				bufferSize += sizeof(nv.GetValueUInt32());
				break;

			case ParameterNameValueType::FloatType:
				bufferSize += sizeof(nv.GetValueFloat());
				break;

			case ParameterNameValueType::AsciiType:
                bufferSize += sizeof(int);
				bufferSize += (int)(nv.GetValueAscii().length() * sizeof(char));
				break;

			case ParameterNameValueType::TextType:
                bufferSize += sizeof(int);
				bufferSize += (int)(nv.GetValueText().length() * sizeof(wchar_t));
				break;
            
            case ParameterNameValueType::UnknownType:
                break;
		}
	}
    return bufferSize;
}

static void CopyMetricToBuffer(const std::vector<affymetrix_calvin_parameter::ParameterNameValueType> &metrics, char * &pbuffer)
{
	int ncols = (int) metrics.size();
	for (int icol=0; icol<ncols; icol++)
	{
		const ParameterNameValueType &nv = metrics[icol];
		switch (nv.GetParameterType())
		{
			case ParameterNameValueType::Int8Type:
				*pbuffer = nv.GetValueInt8();
                pbuffer += sizeof(int8_t);
				break;

			case ParameterNameValueType::UInt8Type:
				*pbuffer = nv.GetValueUInt8();
                pbuffer += sizeof(u_int8_t);
				break;

			case ParameterNameValueType::Int16Type:
				MmSetUInt16_N((uint16_t *)pbuffer, nv.GetValueInt16());
                pbuffer += sizeof(int16_t);
				break;

			case ParameterNameValueType::UInt16Type:
				MmSetUInt16_N((uint16_t *)pbuffer, nv.GetValueUInt16());
                pbuffer += sizeof(u_int16_t);
				break;

			case ParameterNameValueType::Int32Type:
				MmSetUInt32_N((uint32_t *)pbuffer, nv.GetValueInt32());
                pbuffer += sizeof(int32_t);
				break;

			case ParameterNameValueType::UInt32Type:
				MmSetUInt32_N((uint32_t *)pbuffer, nv.GetValueUInt32());
                pbuffer += sizeof(u_int32_t);
				break;

			case ParameterNameValueType::FloatType:
				MmSetFloat_N((float *)pbuffer, nv.GetValueFloat());
                pbuffer += sizeof(float);
				break;

			case ParameterNameValueType::AsciiType:
			case ParameterNameValueType::TextType:
            case ParameterNameValueType::UnknownType:
                break;
		}
	}
}

void CalvinCHPFileUpdater::UpdateMultiDataGenotypeEntryBuffer(MultiDataType dataType, int row_start, const std::vector<affymetrix_calvin_data::ProbeSetMultiDataGenotypeData> &genotypeEntryBuffer)
{
    if (genotypeEntryBuffer.size() == 0)
        return;

	// seek to start of update row (note NAME_COLUMN is 0)
    int dsIndex = dataSetIndexMap[dataType];
	SeekToPosition(*m_CHPFile, ENTRY_DATA_GROUP, dsIndex, row_start, NAME_COLUMN);

    // Create a buffer for writing.
	int iProbeSetNameColumnSize = colsizes[ENTRY_DATA_GROUP][ENTRY_DATA_SET][NAME_COLUMN];
    int len = iProbeSetNameColumnSize - sizeof(int);
    int bufferSize = iProbeSetNameColumnSize + sizeof(genotypeEntryBuffer[0].call) + sizeof(genotypeEntryBuffer[0].confidence) + GetMetricBufferSize(genotypeEntryBuffer[0].metrics);
    bufferSize *= (int)genotypeEntryBuffer.size();
    char *buffer = new char[bufferSize];
    memset(buffer, 0, bufferSize);

    // Copy the data to the buffer
    char *pbuffer = buffer;
    for (int i=0; i<(int)genotypeEntryBuffer.size(); i++) 
    {
        MmSetUInt32_N((uint32_t *)pbuffer, len);
        pbuffer += sizeof(int);
        memcpy(pbuffer, genotypeEntryBuffer[i].name.c_str(), genotypeEntryBuffer[i].name.length());
        pbuffer += len;
        *pbuffer = genotypeEntryBuffer[i].call;
        pbuffer += sizeof(char);
        MmSetFloat_N((float *)pbuffer, genotypeEntryBuffer[i].confidence);
        pbuffer += sizeof(float);
        CopyMetricToBuffer(genotypeEntryBuffer[i].metrics, pbuffer);
    }

    // Write the buffer.
    m_CHPFile->write(buffer, bufferSize);
    delete[] buffer;
    buffer = NULL;
}

void CalvinCHPFileUpdater::UpdateMultiDataGenotypeEntryBuffer(MultiDataType dataType, int row_start, int bufferEntrySize, const std::vector<char *> &genotypeEntryBuffer)
{
    if (genotypeEntryBuffer.size() == 0)
        return;

    // Copy the data to the buffer
    int bufferSize = bufferEntrySize * (int)genotypeEntryBuffer.size();
    char *buffer = new char[bufferSize];
    memset(buffer, 0, bufferSize);
    char *pbuffer = buffer;
    for (int i=0; i<(int)genotypeEntryBuffer.size(); i++) 
    {
        memcpy(pbuffer, genotypeEntryBuffer[i], bufferEntrySize);
        pbuffer += bufferEntrySize;
    }

	// seek to start of update row (note NAME_COLUMN is 0)
    int dsIndex = dataSetIndexMap[dataType];
	SeekToPosition(*m_CHPFile, ENTRY_DATA_GROUP, dsIndex, row_start, NAME_COLUMN);

    // Write the buffer.
    m_CHPFile->write(buffer, bufferSize);
    delete[] buffer;
    buffer = NULL;
}

void CalvinCHPFileUpdater::UpdateMultiDataCopyNumberEntryBuffer(MultiDataType dataType, int row_start, int bufferEntrySize, const std::vector<char *> &copyNumberEntryBuffer)
{
    if (copyNumberEntryBuffer.size() == 0)
        return;

    // Copy the data to the buffer
    int bufferSize = bufferEntrySize * (int)copyNumberEntryBuffer.size();
    char *buffer = new char[bufferSize];
    memset(buffer, 0, bufferSize);
    char *pbuffer = buffer;
    for (int i=0; i<(int)copyNumberEntryBuffer.size(); i++) 
    {
        memcpy(pbuffer, copyNumberEntryBuffer[i], bufferEntrySize);
        pbuffer += bufferEntrySize;
    }

	// seek to start of update row (note NAME_COLUMN is 0)
    int dsIndex = dataSetIndexMap[dataType];
	SeekToPosition(*m_CHPFile, ENTRY_DATA_GROUP, dsIndex, row_start, NAME_COLUMN);

    // Write the buffer.
    m_CHPFile->write(buffer, bufferSize);
    delete[] buffer;
    buffer = NULL;
}

void CalvinCHPFileUpdater::UpdateMultiDataCytoRegionEntryBuffer(MultiDataType dataType, int row_start, int bufferEntrySize, const std::vector<char *> &cytoEntryBuffer)
{
    if (cytoEntryBuffer.size() == 0)
        return;

    // Copy the data to the buffer
    int bufferSize = bufferEntrySize * (int)cytoEntryBuffer.size();
    char *buffer = new char[bufferSize];
    memset(buffer, 0, bufferSize);
    char *pbuffer = buffer;
    for (int i=0; i<(int)cytoEntryBuffer.size(); i++) 
    {
        memcpy(pbuffer, cytoEntryBuffer[i], bufferEntrySize);
        pbuffer += bufferEntrySize;
    }

	// seek to start of update row (note NAME_COLUMN is 0)
    int dsIndex = dataSetIndexMap[dataType];
	SeekToPosition(*m_CHPFile, ENTRY_DATA_GROUP, dsIndex, row_start, NAME_COLUMN);

    // Write the buffer.
    m_CHPFile->write(buffer, bufferSize);
    delete[] buffer;
    buffer = NULL;
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

void CalvinCHPFileUpdater::UpdateMultiDataExpressionEntryBuffer(MultiDataType dataType, int row_start, int bufferEntrySize, const std::vector<char *> &expressionEntryBuffer)
{
    if (expressionEntryBuffer.size() == 0)
        return;

    // Copy the data to the buffer
    int bufferSize = bufferEntrySize * (int)expressionEntryBuffer.size();
    char *buffer = new char[bufferSize];
    memset(buffer, 0, bufferSize);
    char *pbuffer = buffer;
    for (int i=0; i<(int)expressionEntryBuffer.size(); i++) 
    {
        memcpy(pbuffer, expressionEntryBuffer[i], bufferEntrySize);
        pbuffer += bufferEntrySize;
    }

	// seek to start of update row (note NAME_COLUMN is 0)
    int dsIndex = dataSetIndexMap[dataType];
	SeekToPosition(*m_CHPFile, ENTRY_DATA_GROUP, dsIndex, row_start, NAME_COLUMN);

    // Write the buffer.
    m_CHPFile->write(buffer, bufferSize);
    delete[] buffer;
    buffer = NULL;
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
