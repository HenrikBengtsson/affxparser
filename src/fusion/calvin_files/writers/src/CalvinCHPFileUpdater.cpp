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

#include "calvin_files/writers/src/CalvinCHPFileUpdater.h"
//
#include "calvin_files/parsers/src/CHPMultiDataFileReader.h"
#include "calvin_files/writers/src/FileOutput.h"
//
#include "file/FileIO.h"
#include "util/Err.h"
#include "util/Util.h"
//
#include <cstring>
#include <string.h>
//

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
    dataGroupIndexMap.clear();
    std::map<std::wstring, MultiDataType> nameTypeMap;
    int nnames = sizeof(MultiDataDataSetNames) / sizeof(std::wstring);
    for (int iname=0; iname<nnames; iname++)
        nameTypeMap[MultiDataDataSetNames[iname]] = MultiDataDataTypes[iname];
    int ng = (int)dataSetNames.size();
    for (int ig=0; ig<ng; ig++)
    {
        int ns = (int)dataSetNames[ig].size();
        for (int is=0; is<ns; is++)
        {
            wstring &name = dataSetNames[ig][is];
            dataSetIndexMap[nameTypeMap[name]] = is;
            dataGroupIndexMap[nameTypeMap[name]] = ig;
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
    int dgIndex = dataGroupIndexMap[dataType];
	SeekToPosition(*m_CHPFile, dgIndex, dsIndex, row, ENTRY_COLUMN);
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

void CalvinCHPFileUpdater::UpdateDmetBiAllelicEntry(MultiDataType dataType, int row, const affymetrix_calvin_data::DmetBiAllelicData &entry)
{
	int dsIndex = dataSetIndexMap[dataType];
    int dgIndex = dataGroupIndexMap[dataType];
	SeekToPosition(*m_CHPFile, dgIndex, dsIndex, row, ENTRY_COLUMN);
	FileOutput::WriteUInt8(*m_CHPFile, entry.call);
	FileOutput::WriteFloat(*m_CHPFile, entry.confidence);
	FileOutput::WriteUInt8(*m_CHPFile, entry.force);
	FileOutput::WriteFloat(*m_CHPFile, entry.signalA);
	FileOutput::WriteFloat(*m_CHPFile, entry.signalB);
	FileOutput::WriteUInt8(*m_CHPFile, entry.contextA);
	FileOutput::WriteUInt8(*m_CHPFile, entry.contextB);
	UpdateMetrics(entry.metrics);
}

void CalvinCHPFileUpdater::UpdateDmetBiAllelicEntryBuffer(MultiDataType dataType, int rowStart, const std::vector<affymetrix_calvin_data::DmetBiAllelicData> &entryBuffer)
{
	if (entryBuffer.size() == 0)
	{
		return;
	}
	// seek to start of update row (note NAME_COLUMN is 0)
	int dsIndex = dataSetIndexMap[dataType];
    int dgIndex = dataGroupIndexMap[dataType];
	SeekToPosition(*m_CHPFile, dgIndex, dsIndex, rowStart, NAME_COLUMN);

	// Create a buffer for writing.
	int iProbeSetNameColumnSize = colsizes[dgIndex][dsIndex][NAME_COLUMN];
	int len = iProbeSetNameColumnSize - sizeof(int);
	int bufferSize = iProbeSetNameColumnSize + 
		sizeof(entryBuffer[0].call) + 
		sizeof(entryBuffer[0].confidence) + 
		sizeof(entryBuffer[0].force) + 
		sizeof(entryBuffer[0].signalA) + 
		sizeof(entryBuffer[0].signalB) + 
		sizeof(entryBuffer[0].contextA) + 
		sizeof(entryBuffer[0].contextB) + 
		GetMetricBufferSize(entryBuffer[0].metrics);
	bufferSize *= (int)entryBuffer.size();
	char *buffer = new char[bufferSize];
	memset(buffer, 0, bufferSize);

	// Copy the data to the buffer
	char *pbuffer = buffer;
	for (int i = 0; i<(int)entryBuffer.size(); i++) 
	{
		MmSetUInt32_N((uint32_t*)pbuffer, len);
		pbuffer += sizeof(int);
		memcpy(pbuffer, entryBuffer[i].name.c_str(), entryBuffer[i].name.length());
		pbuffer += len;
		MmSetUInt8((uint8_t*)pbuffer, entryBuffer[i].call);
		pbuffer += sizeof(uint8_t);
		MmSetFloat_N((float *)pbuffer, entryBuffer[i].confidence);
		pbuffer += sizeof(float);
		MmSetUInt8((uint8_t*)pbuffer, entryBuffer[i].force);
		pbuffer += sizeof(uint8_t);
		MmSetFloat_N((float *)pbuffer, entryBuffer[i].signalA);
		pbuffer += sizeof(float);
		MmSetFloat_N((float *)pbuffer, entryBuffer[i].signalB);
		pbuffer += sizeof(float);
		MmSetUInt8((uint8_t*)pbuffer, entryBuffer[i].contextA);
		pbuffer += sizeof(uint8_t);
		MmSetUInt8((uint8_t*)pbuffer, entryBuffer[i].contextB);
		pbuffer += sizeof(uint8_t);
		CopyMetricToBuffer(entryBuffer[i].metrics, pbuffer);
	}
	// Write the buffer.
	m_CHPFile->write(buffer, bufferSize);
	delete[] buffer;
	buffer = NULL;
}

void CalvinCHPFileUpdater::UpdateDmetBiAllelicEntryBuffer(MultiDataType dataType, int rowStart, int bufferEntrySize, const std::vector<char*> &entryBuffer)
{
	if (entryBuffer.size() == 0)
	{
		return;
	}
	// Copy the data to the buffer
	int bufferSize = bufferEntrySize * (int)entryBuffer.size();
	char *buffer = new char[bufferSize];
	memset(buffer, 0, bufferSize);
	char *pbuffer = buffer;
	for (int i=0; i<(int)entryBuffer.size(); i++) 
	{
		memcpy(pbuffer, entryBuffer[i], bufferEntrySize);
		pbuffer += bufferEntrySize;
	}

	// seek to start of update row (note NAME_COLUMN is 0)
	int dsIndex = dataSetIndexMap[dataType];
    int dgIndex = dataGroupIndexMap[dataType];
	SeekToPosition(*m_CHPFile, dgIndex, dsIndex, rowStart, NAME_COLUMN);

	// Write the buffer.
	m_CHPFile->write(buffer, bufferSize);
	delete[] buffer;
	buffer = NULL;
}

void CalvinCHPFileUpdater::UpdateDmetCopyNumberEntry(MultiDataType dataType, int row, const affymetrix_calvin_data::DmetCopyNumberData &entry)
{
	int dsIndex = dataSetIndexMap[dataType];
    int dgIndex = dataGroupIndexMap[dataType];
	SeekToPosition(*m_CHPFile, dgIndex, dsIndex, row, ENTRY_COLUMN);
	FileOutput::WriteInt16(*m_CHPFile, entry.call);
	FileOutput::WriteFloat(*m_CHPFile, entry.confidence);
	FileOutput::WriteInt16(*m_CHPFile, entry.force);
	FileOutput::WriteFloat(*m_CHPFile, entry.estimate);
	FileOutput::WriteFloat(*m_CHPFile, entry.lower);
	FileOutput::WriteFloat(*m_CHPFile, entry.upper);
	UpdateMetrics(entry.metrics);
}

void CalvinCHPFileUpdater::UpdateDmetCopyNumberEntryBuffer(MultiDataType dataType, int rowStart, const std::vector<affymetrix_calvin_data::DmetCopyNumberData> &entryBuffer)
{
	if (entryBuffer.size() == 0)
	{
		return;
	}
	// seek to start of update row (note NAME_COLUMN is 0)
	int dsIndex = dataSetIndexMap[dataType];
    int dgIndex = dataGroupIndexMap[dataType];
	SeekToPosition(*m_CHPFile, dgIndex, dsIndex, rowStart, NAME_COLUMN);

	// Create a buffer for writing.
	int iProbeSetNameColumnSize = colsizes[dgIndex][dsIndex][NAME_COLUMN];
	int len = iProbeSetNameColumnSize - sizeof(int);
	int bufferSize = iProbeSetNameColumnSize + 
		sizeof(entryBuffer[0].call) + 
		sizeof(entryBuffer[0].confidence) + 
		sizeof(entryBuffer[0].force) + 
		sizeof(entryBuffer[0].estimate) + 
		sizeof(entryBuffer[0].lower) + 
		sizeof(entryBuffer[0].upper) + 
		GetMetricBufferSize(entryBuffer[0].metrics);
	bufferSize *= (int)entryBuffer.size();
	char *buffer = new char[bufferSize];
	memset(buffer, 0, bufferSize);

	// Copy the data to the buffer
	char *pbuffer = buffer;
	for (int i = 0; i<(int)entryBuffer.size(); i++) 
	{
		MmSetUInt32_N((uint32_t*)pbuffer, len);
		pbuffer += sizeof(int);
		memcpy(pbuffer, entryBuffer[i].name.c_str(), entryBuffer[i].name.length());
		pbuffer += len;
		MmSetUInt16_N((uint16_t*)pbuffer, entryBuffer[i].call);
		pbuffer += sizeof(uint16_t);
		MmSetFloat_N((float*)pbuffer, entryBuffer[i].confidence);
		pbuffer += sizeof(float);
		MmSetUInt16_N((uint16_t*)pbuffer, entryBuffer[i].force);
		pbuffer += sizeof(uint16_t);
		MmSetFloat_N((float*)pbuffer, entryBuffer[i].estimate);
		pbuffer += sizeof(float);
		MmSetFloat_N((float*)pbuffer, entryBuffer[i].lower);
		pbuffer += sizeof(float);
		MmSetFloat_N((float*)pbuffer, entryBuffer[i].upper);
		pbuffer += sizeof(float);
		CopyMetricToBuffer(entryBuffer[i].metrics, pbuffer);
	}
	// Write the buffer.
	m_CHPFile->write(buffer, bufferSize);
	delete[] buffer;
	buffer = NULL;
}

void CalvinCHPFileUpdater::UpdateDmetCopyNumberEntryBuffer(MultiDataType dataType, int rowStart, int bufferEntrySize, const std::vector<char*> &entryBuffer)
{
	if (entryBuffer.size() == 0)
	{
		return;
	}
	// Copy the data to the buffer
	int bufferSize = bufferEntrySize * (int)entryBuffer.size();
	char *buffer = new char[bufferSize];
	memset(buffer, 0, bufferSize);
	char *pbuffer = buffer;
	for (int i=0; i<(int)entryBuffer.size(); i++) 
	{
		memcpy(pbuffer, entryBuffer[i], bufferEntrySize);
		pbuffer += bufferEntrySize;
	}

	// seek to start of update row (note NAME_COLUMN is 0)
	int dsIndex = dataSetIndexMap[dataType];
    int dgIndex = dataGroupIndexMap[dataType];
	SeekToPosition(*m_CHPFile, dgIndex, dsIndex, rowStart, NAME_COLUMN);

	// Write the buffer.
	m_CHPFile->write(buffer, bufferSize);
	delete[] buffer;
	buffer = NULL;
}

void CalvinCHPFileUpdater::UpdateDmetMultiAllelicEntry(MultiDataType dataType, int row, const affymetrix_calvin_data::DmetMultiAllelicData &entry)
{
	int dsIndex = dataSetIndexMap[dataType];
    int dgIndex = dataGroupIndexMap[dataType];
	SeekToPosition(*m_CHPFile, dgIndex, dsIndex, row, ENTRY_COLUMN);
	FileOutput::WriteUInt8(*m_CHPFile, entry.call);
	FileOutput::WriteFloat(*m_CHPFile, entry.confidence);
	FileOutput::WriteUInt8(*m_CHPFile, entry.force);
	FileOutput::WriteUInt8(*m_CHPFile, entry.alleleCount);
	FileOutput::WriteFloat(*m_CHPFile, entry.signalA);
	FileOutput::WriteFloat(*m_CHPFile, entry.signalB);
	FileOutput::WriteFloat(*m_CHPFile, entry.signalC);
	FileOutput::WriteFloat(*m_CHPFile, entry.signalD);
	FileOutput::WriteFloat(*m_CHPFile, entry.signalE);
	FileOutput::WriteFloat(*m_CHPFile, entry.signalF);
	FileOutput::WriteUInt8(*m_CHPFile, entry.contextA);
	FileOutput::WriteUInt8(*m_CHPFile, entry.contextB);
	FileOutput::WriteUInt8(*m_CHPFile, entry.contextC);
	FileOutput::WriteUInt8(*m_CHPFile, entry.contextD);
	FileOutput::WriteUInt8(*m_CHPFile, entry.contextE);
	FileOutput::WriteUInt8(*m_CHPFile, entry.contextF);
	UpdateMetrics(entry.metrics);
}

void CalvinCHPFileUpdater::UpdateDmetMultiAllelicEntryBuffer(MultiDataType dataType, int rowStart, const std::vector<affymetrix_calvin_data::DmetMultiAllelicData> &entryBuffer)
{
	if (entryBuffer.size() == 0)
	{
		return;
	}
	// seek to start of update row (note NAME_COLUMN is 0)
	int dsIndex = dataSetIndexMap[dataType];
    int dgIndex = dataGroupIndexMap[dataType];
	SeekToPosition(*m_CHPFile, dgIndex, dsIndex, rowStart, NAME_COLUMN);

	// Create a buffer for writing.
	int iProbeSetNameColumnSize = colsizes[dgIndex][dsIndex][NAME_COLUMN];
	int len = iProbeSetNameColumnSize - sizeof(int);
	int bufferSize = iProbeSetNameColumnSize + 
		sizeof(entryBuffer[0].call) + 
		sizeof(entryBuffer[0].confidence) + 
		sizeof(entryBuffer[0].force) + 
		sizeof(entryBuffer[0].alleleCount) + 
		sizeof(entryBuffer[0].signalA) + 
		sizeof(entryBuffer[0].signalB) + 
		sizeof(entryBuffer[0].signalC) + 
		sizeof(entryBuffer[0].signalD) + 
		sizeof(entryBuffer[0].signalE) + 
		sizeof(entryBuffer[0].signalF) + 
		sizeof(entryBuffer[0].contextA) + 
		sizeof(entryBuffer[0].contextB) + 
		sizeof(entryBuffer[0].contextC) + 
		sizeof(entryBuffer[0].contextD) + 
		sizeof(entryBuffer[0].contextE) + 
		sizeof(entryBuffer[0].contextF) + 
		GetMetricBufferSize(entryBuffer[0].metrics);
	bufferSize *= (int)entryBuffer.size();
	char *buffer = new char[bufferSize];
	memset(buffer, 0, bufferSize);

	// Copy the data to the buffer
	char *pbuffer = buffer;
	for (int i = 0; i<(int)entryBuffer.size(); i++) 
	{
		MmSetUInt32_N((uint32_t*)pbuffer, len);
		pbuffer += sizeof(int);
		memcpy(pbuffer, entryBuffer[i].name.c_str(), entryBuffer[i].name.length());
		pbuffer += len;
		MmSetUInt8((uint8_t*)pbuffer, entryBuffer[i].call);
		pbuffer += sizeof(uint8_t);
		MmSetFloat_N((float*)pbuffer, entryBuffer[i].confidence);
		pbuffer += sizeof(float);
		MmSetUInt8((uint8_t*)pbuffer, entryBuffer[i].force);
		pbuffer += sizeof(uint8_t);
		MmSetUInt8((uint8_t*)pbuffer, entryBuffer[i].alleleCount);
		pbuffer += sizeof(uint8_t);
		MmSetFloat_N((float*)pbuffer, entryBuffer[i].signalA);
		pbuffer += sizeof(float);
		MmSetFloat_N((float*)pbuffer, entryBuffer[i].signalB);
		pbuffer += sizeof(float);
		MmSetFloat_N((float*)pbuffer, entryBuffer[i].signalC);
		pbuffer += sizeof(float);
		MmSetFloat_N((float*)pbuffer, entryBuffer[i].signalD);
		pbuffer += sizeof(float);
		MmSetFloat_N((float*)pbuffer, entryBuffer[i].signalE);
		pbuffer += sizeof(float);
		MmSetFloat_N((float*)pbuffer, entryBuffer[i].signalF);
		pbuffer += sizeof(float);
		MmSetUInt8((uint8_t*)pbuffer, entryBuffer[i].contextA);
		pbuffer += sizeof(uint8_t);
		MmSetUInt8((uint8_t*)pbuffer, entryBuffer[i].contextB);
		pbuffer += sizeof(uint8_t);
		MmSetUInt8((uint8_t*)pbuffer, entryBuffer[i].contextC);
		pbuffer += sizeof(uint8_t);
		MmSetUInt8((uint8_t*)pbuffer, entryBuffer[i].contextD);
		pbuffer += sizeof(uint8_t);
		MmSetUInt8((uint8_t*)pbuffer, entryBuffer[i].contextE);
		pbuffer += sizeof(uint8_t);
		MmSetUInt8((uint8_t*)pbuffer, entryBuffer[i].contextF);
		pbuffer += sizeof(uint8_t);
		CopyMetricToBuffer(entryBuffer[i].metrics, pbuffer);
	}
	// Write the buffer.
	m_CHPFile->write(buffer, bufferSize);
	delete[] buffer;
	buffer = NULL;
}

void CalvinCHPFileUpdater::UpdateDmetMultiAllelicEntryBuffer(MultiDataType dataType, int rowStart, int bufferEntrySize, const std::vector<char*> &entryBuffer)
{
	if (entryBuffer.size() == 0)
		return;

    // Copy the data to the buffer
	int bufferSize = bufferEntrySize * (int)entryBuffer.size();
	char *buffer = new char[bufferSize];
	memset(buffer, 0, bufferSize);
	char *pbuffer = buffer;
	for (int i=0; i<(int)entryBuffer.size(); i++) 
	{
		memcpy(pbuffer, entryBuffer[i], bufferEntrySize);
		pbuffer += bufferEntrySize;
	}

	// seek to start of update row (note NAME_COLUMN is 0)
	int dsIndex = dataSetIndexMap[dataType];
    int dgIndex = dataGroupIndexMap[dataType];
	SeekToPosition(*m_CHPFile, dgIndex, dsIndex, rowStart, NAME_COLUMN);

	// Write the buffer.
	m_CHPFile->write(buffer, bufferSize);
	delete[] buffer;
	buffer = NULL;
}

void CalvinCHPFileUpdater::UpdateMultiDataGenotypeEntryBuffer(MultiDataType dataType, int row_start, const std::vector<affymetrix_calvin_data::ProbeSetMultiDataGenotypeData> &genotypeEntryBuffer)
{
    if (genotypeEntryBuffer.size() == 0)
        return;

	// seek to start of update row (note NAME_COLUMN is 0)
    int dsIndex = dataSetIndexMap[dataType];
    int dgIndex = dataGroupIndexMap[dataType];
	SeekToPosition(*m_CHPFile, dgIndex, dsIndex, row_start, NAME_COLUMN);

    // Create a buffer for writing.
	int iProbeSetNameColumnSize = colsizes[dgIndex][dsIndex][NAME_COLUMN];
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
    int dgIndex = dataGroupIndexMap[dataType];
	SeekToPosition(*m_CHPFile, dgIndex, dsIndex, row_start, NAME_COLUMN);

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
    int dgIndex = dataGroupIndexMap[dataType];
	SeekToPosition(*m_CHPFile, dgIndex, dsIndex, row_start, NAME_COLUMN);

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
    int dgIndex = dataGroupIndexMap[dataType];
	SeekToPosition(*m_CHPFile, dgIndex, dsIndex, row_start, NAME_COLUMN);

    // Write the buffer.
    m_CHPFile->write(buffer, bufferSize);
    delete[] buffer;
    buffer = NULL;
}

void CalvinCHPFileUpdater::UpdateMultiDataCopyNumberVariationRegionEntryBuffer(MultiDataType dataType, int row_start, int bufferEntrySize, 
                                                                               const std::vector<char *> &cnVariationEntryBuffer)

{
    if (cnVariationEntryBuffer.size() == 0)
        return;

    // Copy the data to the buffer
    int bufferSize = bufferEntrySize * (int)cnVariationEntryBuffer.size();
    char *buffer = new char[bufferSize];
    memset(buffer, 0, bufferSize);
    char *pbuffer = buffer;
    for (int i=0; i<(int)cnVariationEntryBuffer.size(); i++) 
    {
        memcpy(pbuffer, cnVariationEntryBuffer[i], bufferEntrySize);
        pbuffer += bufferEntrySize;
    }

	// seek to start of update row (note NAME_COLUMN is 0)
    int dsIndex = dataSetIndexMap[dataType];
    int dgIndex = dataGroupIndexMap[dataType];
	SeekToPosition(*m_CHPFile, dgIndex, dsIndex, row_start, NAME_COLUMN);

    // Write the buffer.
    m_CHPFile->write(buffer, bufferSize);
    delete[] buffer;
    buffer = NULL;
}

void CalvinCHPFileUpdater::UpdateMultiDataExpressionEntry(MultiDataType dataType, int row, const affymetrix_calvin_data::ProbeSetMultiDataExpressionData &entry)
{
    int dsIndex = dataSetIndexMap[dataType];
    int dgIndex = dataGroupIndexMap[dataType];
	SeekToPosition(*m_CHPFile, dgIndex, dsIndex, row, ENTRY_COLUMN);
	FileOutput::WriteFloat(*m_CHPFile, entry.quantification);
    UpdateMetrics(entry.metrics);
}

void CalvinCHPFileUpdater::UpdateMultiDataExpressionEntryBuffer(MultiDataType dataType, int row_start, const std::vector<affymetrix_calvin_data::ProbeSetMultiDataExpressionData> &expressionEntryBuffer)
{
	// seek to start of update row (note NAME_COLUMN is 0)
    int dsIndex = dataSetIndexMap[dataType];
    int dgIndex = dataGroupIndexMap[dataType];
	SeekToPosition(*m_CHPFile, dgIndex, dsIndex, row_start, NAME_COLUMN);
	int iProbeSetNameColumnSize = colsizes[dgIndex][dsIndex][NAME_COLUMN];
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
    int dgIndex = dataGroupIndexMap[dataType];
	SeekToPosition(*m_CHPFile, dgIndex, dsIndex, row_start, NAME_COLUMN);

    // Write the buffer.
    m_CHPFile->write(buffer, bufferSize);
    delete[] buffer;
    buffer = NULL;
}

void CalvinCHPFileUpdater::UpdateChromosomeSegmentEntryBuffer(MultiDataType dataType, int row_start, int bufferEntrySize, const std::vector<char*> &entryBuffer)
{
    if (entryBuffer.size() == 0)
        return;

    // Copy the data to the buffer
    int bufferSize = bufferEntrySize * (int)entryBuffer.size();
    char *buffer = new char[bufferSize];
    memset(buffer, 0, bufferSize);
    char *pbuffer = buffer;
    for (int i=0; i<(int)entryBuffer.size(); i++) 
    {
        memcpy(pbuffer, entryBuffer[i], bufferEntrySize);
        pbuffer += bufferEntrySize;
    }

	// seek to start of update row
    int dsIndex = dataSetIndexMap[dataType];
    int dgIndex = dataGroupIndexMap[dataType];
	SeekToPosition(*m_CHPFile, dgIndex, dsIndex, row_start, 0);

    // Write the buffer.
    m_CHPFile->write(buffer, bufferSize);
    delete[] buffer;
    buffer = NULL;
}

void CalvinCHPFileUpdater::UpdateChromosomeSummaryEntryBuffer(MultiDataType dataType, int row_start, int bufferEntrySize, const std::vector<char*> &entryBuffer)
{
    if (entryBuffer.size() == 0)
        return;

    // Copy the data to the buffer
    int bufferSize = bufferEntrySize * (int)entryBuffer.size();
    char *buffer = new char[bufferSize];
    memset(buffer, 0, bufferSize);
    char *pbuffer = buffer;
    for (int i=0; i<(int)entryBuffer.size(); i++) 
    {
        memcpy(pbuffer, entryBuffer[i], bufferEntrySize);
        pbuffer += bufferEntrySize;
    }

	// seek to start of update row
    int dsIndex = dataSetIndexMap[dataType];
    int dgIndex = dataGroupIndexMap[dataType];
	SeekToPosition(*m_CHPFile, dgIndex, dsIndex, row_start, 0);

    // Write the buffer.
    m_CHPFile->write(buffer, bufferSize);
    delete[] buffer;
    buffer = NULL;
}

void CalvinCHPFileUpdater::UpdateFamilialSegmentOverlapEntryBuffer(MultiDataType dataType, int row_start, int bufferEntrySize, const std::vector<char*> &entryBuffer)
{
    if (entryBuffer.size() == 0)
        return;

    // Copy the data to the buffer
    int bufferSize = bufferEntrySize * (int)entryBuffer.size();
    char *buffer = new char[bufferSize];
    memset(buffer, 0, bufferSize);
    char *pbuffer = buffer;
    for (int i=0; i<(int)entryBuffer.size(); i++) 
    {
        memcpy(pbuffer, entryBuffer[i], bufferEntrySize);
        pbuffer += bufferEntrySize;
    }

	// seek to start of update row
    int dsIndex = dataSetIndexMap[dataType];
    int dgIndex = dataGroupIndexMap[dataType];
	SeekToPosition(*m_CHPFile, dgIndex, dsIndex, row_start, 0);

    // Write the buffer.
    m_CHPFile->write(buffer, bufferSize);
    delete[] buffer;
    buffer = NULL;
}

void CalvinCHPFileUpdater::UpdateFamilialSampleEntryBuffer(MultiDataType dataType, int row_start, int bufferEntrySize, const std::vector<char*> &entryBuffer)
{
    if (entryBuffer.size() == 0)
        return;

    // Copy the data to the buffer
    int bufferSize = bufferEntrySize * (int)entryBuffer.size();
    char *buffer = new char[bufferSize];
    memset(buffer, 0, bufferSize);
    char *pbuffer = buffer;
    for (int i=0; i<(int)entryBuffer.size(); i++) 
    {
        memcpy(pbuffer, entryBuffer[i], bufferEntrySize);
        pbuffer += bufferEntrySize;
    }

	// seek to start of update row
    int dsIndex = dataSetIndexMap[dataType];
    int dgIndex = dataGroupIndexMap[dataType];
	SeekToPosition(*m_CHPFile, dgIndex, dsIndex, row_start, 0);

    // Write the buffer.
    m_CHPFile->write(buffer, bufferSize);
    delete[] buffer;
    buffer = NULL;
}

void CalvinCHPFileUpdater::UpdateMultiDataAllelePeaksEntryBuffer(MultiDataType dataType, int rowStart, int bufferEntrySize, const std::vector<char*> &allelePeakEntryBuffer)
{
	if (allelePeakEntryBuffer.size() == 0)
	{
		return;
	}
	// Copy the data to the buffer
	int bufferSize = bufferEntrySize * (int)allelePeakEntryBuffer.size();
	char *buffer = new char[bufferSize];
	memset(buffer, 0, bufferSize);
	char *pbuffer = buffer;
	for (int i=0; i<(int)allelePeakEntryBuffer.size(); i++) 
	{
		memcpy(pbuffer, allelePeakEntryBuffer[i], bufferEntrySize);
		pbuffer += bufferEntrySize;
	}

	// seek to start of update row (note NAME_COLUMN is 0)
	int dsIndex = dataSetIndexMap[dataType];
    int dgIndex = dataGroupIndexMap[dataType];
	SeekToPosition(*m_CHPFile, dgIndex, dsIndex, rowStart, NAME_COLUMN);

	// Write the buffer.
	m_CHPFile->write(buffer, bufferSize);
	delete[] buffer;
	buffer = NULL;
}

void CalvinCHPFileUpdater::UpdateMultiDataMarkerABSignalsEntryBuffer(MultiDataType dataType, int rowStart, int bufferEntrySize, const std::vector<char*> &entryBuffer)
{
	if (entryBuffer.size() == 0)
	{
		return;
	}
	// Copy the data to the buffer
	int bufferSize = bufferEntrySize * (int)entryBuffer.size();
	char *buffer = new char[bufferSize];
	memset(buffer, 0, bufferSize);
	char *pbuffer = buffer;
	for (int i=0; i<(int)entryBuffer.size(); i++) 
	{
		memcpy(pbuffer, entryBuffer[i], bufferEntrySize);
		pbuffer += bufferEntrySize;
	}

	// seek to start of update row (note NAME_COLUMN is 0)
	int dsIndex = dataSetIndexMap[dataType];
    int dgIndex = dataGroupIndexMap[dataType];
	SeekToPosition(*m_CHPFile, dgIndex, dsIndex, rowStart, NAME_COLUMN);

	// Write the buffer.
	m_CHPFile->write(buffer, bufferSize);
	delete[] buffer;
	buffer = NULL;
}

void CalvinCHPFileUpdater::UpdateMultiDataCytoGenotypeEntryBuffer(MultiDataType dataType, int rowStart, int bufferEntrySize, const std::vector<char*> &entryBuffer)
{
    if (entryBuffer.size() == 0)
	{
		return;
	}
	// Copy the data to the buffer
	int bufferSize = bufferEntrySize * (int)entryBuffer.size();
	char *buffer = new char[bufferSize];
	memset(buffer, 0, bufferSize);
	char *pbuffer = buffer;
	for (int i=0; i<(int)entryBuffer.size(); i++) 
	{
		memcpy(pbuffer, entryBuffer[i], bufferEntrySize);
		pbuffer += bufferEntrySize;
	}
	// seek to start of update row (note NAME_COLUMN is 0)
	int dsIndex = dataSetIndexMap[dataType];
    int dgIndex = dataGroupIndexMap[dataType];
	SeekToPosition(*m_CHPFile, dgIndex, dsIndex, rowStart, NAME_COLUMN);

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
    dataSetIndexMap.clear();
    dataGroupIndexMap.clear();
}
