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

#ifndef _CopyNumberResultWriter_HEADER_
#define _CopyNumberResultWriter_HEADER_

/*! \file CopyNumberResultWriter.h Defines classes which provides a wrapper to writing a single copy number file of the Command Console format. */

#include "calvin_files/data/src/ColumnInfo.h"
#include "calvin_files/data/src/ProbeSetMultiDataData.h"
#include "calvin_files/fusion/src/FusionCELData.h"
#include "calvin_files/parameter/src/ParameterNameValueType.h"
#include "calvin_files/writers/src/CalvinCHPMultiDataFileBufferWriter.h"
#include "calvin_files/writers/src/CalvinCHPMultiDataFileWriter.h"
//
#include <cstring>
#include <map>
#include <string>
#include <vector>
//

/*! This class provides a wrapper to writing a single copy number file of the Command Console format. */
class CopyNumberResultWriter
{
private:

    /*! The maximum probe set name. */
    std::map<affymetrix_calvin_io::MultiDataType, int> maxProbeSetNameLength;

    /*! The number of probe sets */
    int numberProbeSets;

    /*! The number of cyto regions */
    int numberCytoRegions;

    /*! The number of genotype probe sets */
    int numberGenotypeProbeSets;

    /*! The name of the algorithm. */
    std::string algName;

	/*! The algorithm version. */
    std::string algVersion;

	/*! The program name. */
    std::string programName;

	/*! The program version. */
    std::string programVersion;

	/*! The company name. */
    std::string programCompany;

    /*! The columns of data to store. */
    std::vector<affymetrix_calvin_io::ColumnInfo> columns;

    /*! The columns of genotype data to store. */
    std::vector<affymetrix_calvin_io::ColumnInfo> genotypeColumns;

	/*! The columns of cytoregion data to store. */
    std::vector<affymetrix_calvin_io::ColumnInfo> cytoRegionColumns;

    /*! Algorithm parameters. */
    affymetrix_calvin_parameter::ParameterNameValueTypeList algParams;

    /*! Summary parameters. */
    affymetrix_calvin_parameter::ParameterNameValueTypeList summaryParams;

    /*! A list of start/stop indicies for the probesets in a chromosome. */
    affymetrix_calvin_parameter::ParameterNameValueTypeList chrStartStop;

    /*! The writer for the results file. */
    affymetrix_calvin_io::CHPMultiDataFileBufferWriter *bufferWriter;

    /*! Used by the buffer writing. The list of output files. */
    std::vector<std::string> outputFiles;

    /*! Initialize the class */
    void Init();

public:
    /*! Initialize the class */
    CopyNumberResultWriter() { Init(); }

    /*! Clean up */
    ~CopyNumberResultWriter() { Clear(); }

    /*! Clean up */
    void Clear();

    /*! Set the max probe set name length.
     * @param len The maximum length.
     */
    void MaximumProbeSetNameLength(int len) { maxProbeSetNameLength[affymetrix_calvin_io::CopyNumberMultiDataType] = len; }

    /*! Set the max cyto region name length.
     * @param len The maximum length.
     */
    void MaximumCytoRegionNameLength(int len) { maxProbeSetNameLength[affymetrix_calvin_io::CytoMultiDataType] = len; }

    /*! Set the max genotype probe set name length.
     * @param len The maximum length.
     */
    void MaximumGenotypeProbeSetNameLength(int len) { maxProbeSetNameLength[affymetrix_calvin_io::GenotypeMultiDataType] = len; }

	/*! The name of the algorithm.
	 * @return The algorithm name.
	 */
    std::string& AlgName() { return algName; }

	/*! Sets the algorithm version.
	 * @return The version.
	 */
    std::string& AlgVersion() { return algVersion; }

	/*! The program name. */
    std::string& ProgramName() { return programName; }

	/*! The program version. */
    std::string& ProgramVersion() { return programVersion; }

	/*! The company name. */
    std::string& ProgramCompany() { return programCompany; }

    /*! The number of probe sets
     * @return The number of probe sets
     */
    int& NumberProbeSets() { return numberProbeSets; }

    /*! The number of cyto regions
     * @return The number of cyto regions
     */
    int& NumberCytoRegions() { return numberCytoRegions; }

	/*! The number of genotype probe sets
	 * return The number of genotype probe sets.
	 */
	int& NumberGenotypeProbeSets() { return numberGenotypeProbeSets; }

    /*! The columns of data to store. */
    std::vector<affymetrix_calvin_io::ColumnInfo>& Columns() { return columns; }

	/*! The columns of genotype data to store. */
    std::vector<affymetrix_calvin_io::ColumnInfo>& GenotypeColumns() { return genotypeColumns; }

    /*! The columns of cyto data to store. */
    std::vector<affymetrix_calvin_io::ColumnInfo>& CytoRegionColumns() { return cytoRegionColumns; }

    /*! Algorithm parameters. This should include the genome name, version, etc. */
    affymetrix_calvin_parameter::ParameterNameValueTypeList& AlgParams() { return algParams; }

    /*! Summary parameters. This includes items such as IQR, etc. */
    affymetrix_calvin_parameter::ParameterNameValueTypeList& SummaryParams() { return summaryParams; }

    /*!  Store the starting probe set index and count of probe sets for the given chromosome.
     * @param chr The chromosome value
     * @param startIndex The index of the probe sets that the given chromosome starts at.
     * @param count The number of probe sets for the given chromosome.
     */
    void SetChromosomeProbeSetIndexInformation(u_int8_t chr, int startIndex, int count);

    /*! Create the results file with the header information (no data is written). This function
     * will throw a std::string exception on errors.
     * @param cel The CEL file data object.
     * @param fileName The name of the results file to create.
     * @return True if successful.
     */
    void CreateResultFile(affymetrix_fusion_io::FusionCELData& cel, const std::string& fileName);

    /*! Write the probe set result to the file.
     * @param entry The results.
     */
    void WriteProbeSetResult(const affymetrix_calvin_data::ProbeSetMultiDataCopyNumberData& entry);

    /*! Write the cyto region result to the file.
     * @param entry The results.
     */
    void WriteCytoRegionResult(const affymetrix_calvin_data::ProbeSetMultiDataCytoRegionData& entry);

    /*! Write the genotype probe set result to the file.
     * @param entry The results.
     */
    void WriteGenotypeProbeSetResult(const affymetrix_calvin_data::ProbeSetMultiDataGenotypeData& entry);

    /*! Close the output file. */
    void CloseResultsFile();
};

#endif

