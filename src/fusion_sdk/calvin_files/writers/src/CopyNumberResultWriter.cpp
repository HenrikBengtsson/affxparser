////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007 Affymetrix, Inc.
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

#include "calvin_files/writers/src/CopyNumberResultWriter.h"
//
#include "calvin_files/data/src/CHPMultiDataData.h"
#include "calvin_files/exception/src/ExceptionBase.h"
#include "calvin_files/parameter/src/AffymetrixParameterConsts.h"
#include "calvin_files/utils/src/StringUtils.h"
#include "calvin_files/writers/src/CalvinCHPMultiDataFileWriter.h"
//
#include <iomanip>
#include <sstream>
//

using namespace std;
using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_parameter;
using namespace affymetrix_calvin_data;
using namespace affymetrix_calvin_utilities;
using namespace affymetrix_calvin_exceptions;

/*
 * Initialize the class members.
 */
void CopyNumberResultWriter::Init()
{
    maxProbeSetNameLength.clear();
    numberProbeSets = 0;
    numberCytoRegions = 0;
	numberGenotypeProbeSets = 0;
    bufferWriter = NULL;
}

/*
 * Clear the buffer.
 */
void CopyNumberResultWriter::Clear()
{
    maxProbeSetNameLength.clear();
    numberProbeSets = 0;
    numberCytoRegions = 0;
	numberGenotypeProbeSets = 0;
    algName.clear();
    algVersion.clear();
    programName.clear();
    programVersion.clear();
    programCompany.clear();
    columns.clear();
	genotypeColumns.clear();
	cytoRegionColumns.clear();
    algParams.clear();
    summaryParams.clear();
    chrStartStop.clear();
    outputFiles.clear();
    if (bufferWriter != NULL)
        delete bufferWriter;
    bufferWriter = NULL;
}

/*
 * Create a results file with the CEL file header and other parameters.
 */
void CopyNumberResultWriter::CreateResultFile(affymetrix_fusion_io::FusionCELData& cel, const std::string& fileName)
{
    try
    {
        // Create the results file with the header.
        CHPMultiDataData *data = new CHPMultiDataData(fileName);
        data->SetEntryCount(CopyNumberMultiDataType, numberProbeSets, maxProbeSetNameLength[CopyNumberMultiDataType], columns);
		if (numberCytoRegions > 0)
			data->SetEntryCount(CytoMultiDataType, numberCytoRegions, maxProbeSetNameLength[CytoMultiDataType],cytoRegionColumns);
		if (numberGenotypeProbeSets > 0)
			data->SetEntryCount(GenotypeMultiDataType, numberGenotypeProbeSets, maxProbeSetNameLength[GenotypeMultiDataType], genotypeColumns);
        data->SetAlgName(StringUtils::ConvertMBSToWCS(algName));
        data->SetAlgVersion(StringUtils::ConvertMBSToWCS(algVersion));
        data->SetArrayType(cel.GetChipType());
        GenericDataHeader *gdh = data->GetFileHeader()->GetGenericDataHdr();
        ParameterNameValueType param;
        param.SetName(PROGRAM_NAME);
        param.SetValueText(StringUtils::ConvertMBSToWCS(programName));
        gdh->AddNameValParam(param);
        param.SetName(L"program-version");
        param.SetValueText(StringUtils::ConvertMBSToWCS(programVersion));
        gdh->AddNameValParam(param);
        param.SetName(PROGRAM_COMPANY);
        param.SetValueText(StringUtils::ConvertMBSToWCS(programCompany));
        gdh->AddNameValParam(param);
        ParameterNameValueTypeList params = algParams;
        param.SetName(L"ArraySet");
        param.SetValueText(cel.GetChipType());
        params.push_back(param);
        data->AddAlgParams(params);
        data->AddSummaryParams(summaryParams);
        DataSetHeader *dsh = data->GetDataSetHeader(CopyNumberMultiDataType);
        for (ParameterNameValueTypeList::iterator it=chrStartStop.begin(); it!=chrStartStop.end(); it++)
            dsh->AddNameValParam(*it);
        GenericData *gdata = cel.GetGenericData();
        if (gdata != NULL)
            gdh->AddParent(*gdata->Header().GetGenericDataHdr());
        CHPMultiDataFileWriter *writer = new CHPMultiDataFileWriter(*data);
        delete writer;
        delete data;

        // Create a buffer writer object
        outputFiles.clear();
        outputFiles.push_back(fileName);
        vector<MultiDataType> dataTypes;
        dataTypes.push_back(CopyNumberMultiDataType);
		if (numberCytoRegions > 0)
			dataTypes.push_back(CytoMultiDataType);
		if (numberGenotypeProbeSets > 0)
			dataTypes.push_back(GenotypeMultiDataType);
        bufferWriter = new CHPMultiDataFileBufferWriter();
        bufferWriter->Initialize(&outputFiles, dataTypes, maxProbeSetNameLength);
    }
    catch (CalvinException &ex)
    {
        string err = "Error creating the output file: " + fileName;
        wstring msg = ex.ToString();
        if (msg.empty() == false)
            err += " " + StringUtils::ConvertWCSToMBS(msg);
        throw err;
    }
    catch (...)
    {
        string err = "Error creating the output file: " + fileName;
        throw err;
    }
}

/*
 * Flush the buffer and close the file.
 */
void CopyNumberResultWriter::CloseResultsFile()
{
    if (bufferWriter != NULL)
    {
        bufferWriter->FlushBuffer();
        delete bufferWriter;
    }
    bufferWriter = NULL;
}

/*
 * Write the entry to the buffer.
 */
void CopyNumberResultWriter::WriteProbeSetResult(const ProbeSetMultiDataCopyNumberData& entry)
{
    bufferWriter->WriteMultiDataCopyNumberEntry(CopyNumberMultiDataType, 0, entry);
}

/*
 * Write the entry to the buffer.
 */
void CopyNumberResultWriter::WriteGenotypeProbeSetResult(const ProbeSetMultiDataGenotypeData& entry)
{
    bufferWriter->WriteMultiDataGenotypeEntry(GenotypeMultiDataType, 0, entry);
}

/*
 * Write the entry to the buffer.
 */
void CopyNumberResultWriter::WriteCytoRegionResult(const ProbeSetMultiDataCytoRegionData& entry)
{
    bufferWriter->WriteMultiDataCytoRegionEntry(CytoMultiDataType, 0, entry);
}

/*
 * Store the start index and count of probe sets for the given chromosome.
 */
void CopyNumberResultWriter::SetChromosomeProbeSetIndexInformation(u_int8_t chr, int startIndex, int count)
{
	ostringstream str;
	str << (int) chr;
    wstring schr = StringUtils::ConvertMBSToWCS(str.str());
    ParameterNameValueType param;
    param.SetName(schr + L":start");
    param.SetValueInt32(startIndex);
    chrStartStop.push_back(param);
    param.SetName(schr + L":count");
    param.SetValueInt32(count);
    chrStartStop.push_back(param);
    param.SetName(schr + L":display");
    param.SetValueAscii(ChromosomeToString(chr));
    chrStartStop.push_back(param);
}
