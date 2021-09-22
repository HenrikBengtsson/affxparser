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


/*! \file CalvinCHPFileUpdater.h Provides interfaces to update data in a "Calvin" binary "signal" data file. */

#ifndef _CalvinCHPFileUpdater_HEADER_
#define _CalvinCHPFileUpdater_HEADER_

#include "calvin_files/data/src/CHPMultiDataData.h"
#include "calvin_files/data/src/ProbeSetMultiDataData.h"
#include "calvin_files/writers/src/CalvinCHPFileBufferWriter.h"
#include "calvin_files/writers/src/DataSetUpdater.h"
//
#include <fstream>
#include <map>
//

namespace affymetrix_calvin_io
{

/*! Provides interfaces to update data in a "Calvin" binary "signal" data file.
 * This class assumes that the file has been completely written and that
 * only existing data groups/sets/rows/cols are being modified.
 */
class CalvinCHPFileUpdater : public DataSetUpdater
{
public:
	/*! Constructor */
	CalvinCHPFileUpdater();

	/*! Destructor */
	~CalvinCHPFileUpdater();

	/*! Initialize the class given a "calvin" data file.
	 * @param file The name of the file.
	 * @exception affymetrix_calvin_exceptions::FileNotFoundException The file does not exist.
	 * @exception affymetrix_calvin_exceptions::InvalidVersionException The file version does not match.
	 * @exception affymetrix_calvin_exceptions::InvalidFileTypeException The file is not of the right type.
	 */
	void Initialize(const char *file);

	/*! Open CHP signal file and initialize it.
	 * @param fileName The name of the CHP signal file.
	 */
	void OpenCHPFile(const char *fileName);

	/*! Seek to appropriate file position and update expression signal
	 * @param row The row index.
	 * @param quantification The new quantification value.
	 */
	void UpdateExpressionQuantification(int row, float quantification);

	/*! Seek to appropriate file position and update expression signal vector
	 * @param row_start The start row for updating.
	 * @param expressionSignalBuffer The vector contain all buffered signals to be updated.
	 */
	void UpdateExpressionQuantificationBuffer(int row_start, std::vector<float> &expressionSignalBuffer);

	/*! Seek to appropriate file position and update genotype entry
	 * @param row The row index.
	 * @param call CHP call representation.
	 * @param confidence CHP confidence value.
	 * @param RAS1 CHP RAS1 value.
	 * @param RAS2 CHP RAS2 value.
	 * @param aaCall CHP aaCall value.
	 * @param abCall CHP abCall value.
	 * @param bbCall CHP bbCall value.
	 * @param noCall CHP noCall value.
	 */
	void UpdateGenotypeEntry(int row, u_int8_t call, float confidence, float RAS1, float RAS2, float aaCall, float abCall, float bbCall, float noCall);

	/*! Seek to appropriate file position and update genotype entry vector
	 * @param row_start The start row for updating.
	 * @param genotypeBufferEntry The vector contain all buffered entries to be updated.
	 */
	void UpdateGenotypeEntryBuffer(int row_start, const std::vector<CHPFileBufferWriter::GenotypeBufferEntry> &genotypeEntryBuffer);

	/*! Seek to appropriate file position and update genotype entry
     * @param dataType The data type.
	 * @param row The row index.
	 * @param call CHP call representation.
	 * @param confidence CHP confidence value.
	 */
	void UpdateMultiDataGenotypeEntry(MultiDataType dataType, int row, const affymetrix_calvin_data::ProbeSetMultiDataGenotypeData &entry);

	/*! Seek to appropriate file position and update genotype entry vector
     * @param dataType The data type.
	 * @param row_start The start row for updating.
	 * @param genotypeBufferEntry The vector contain all buffered entries to be updated.
	 */
	void UpdateMultiDataGenotypeEntryBuffer(MultiDataType dataType, int row_start, const std::vector<affymetrix_calvin_data::ProbeSetMultiDataGenotypeData> &genotypeEntryBuffer);

	/*! Seek to appropriate file position and update genotype entry vector
     * @param dataType The data type.
	 * @param row_start The start row for updating.
     * @param bufferEntrySize The size of the buffer for a given element in the vector
	 * @param genotypeBufferEntry The vector contain all buffered entries to be updated.
	 */
	void UpdateMultiDataGenotypeEntryBuffer(MultiDataType dataType, int row_start, int bufferEntrySize, const std::vector<char *> &genotypeEntryBuffer);

	/*! Seek to appropriate file position and update copy number entry vector
     * @param dataType The data type.
	 * @param row_start The start row for updating.
     * @param bufferEntrySize The size of the buffer for a given element in the vector
	 * @param copyNumberEntryBuffer The vector contain all buffered entries to be updated.
	 */
	void UpdateMultiDataCopyNumberEntryBuffer(MultiDataType dataType, int row_start, int bufferEntrySize, const std::vector<char *> &copyNumberEntryBuffer);

	/*! Seek to appropriate file position and update cyto entry vector
     * @param dataType The data type.
	 * @param row_start The start row for updating.
     * @param bufferEntrySize The size of the buffer for a given element in the vector
	 * @param cytoEntryBuffer The vector contain all buffered entries to be updated.
	 */
	void UpdateMultiDataCytoRegionEntryBuffer(MultiDataType dataType, int row_start, int bufferEntrySize, const std::vector<char *> &cytoEntryBuffer);

    /*! Seek to appropriate file position and update cyto entry vector
     * @param dataType The data type.
	 * @param row_start The start row for updating.
     * @param bufferEntrySize The size of the buffer for a given element in the vector
	 * @param cytoEntryBuffer The vector contain all buffered entries to be updated.
	 */
	void UpdateMultiDataCopyNumberVariationRegionEntryBuffer(MultiDataType dataType, int row_start, int bufferEntrySize, const std::vector<char *> &cnVariationEntryBuffer);

	/*! Seek to appropriate file position and update expression entry
     * @param dataType The data type.
	 * @param row The row index.
	 * @param call CHP call representation.
	 * @param confidence CHP confidence value.
	 */
	void UpdateMultiDataExpressionEntry(MultiDataType dataType, int row, const affymetrix_calvin_data::ProbeSetMultiDataExpressionData &entry);

	/*! Seek to appropriate file position and update expression entry vector
     * @param dataType The data type.
	 * @param row_start The start row for updating.
	 * @param expressionBufferEntry The vector contain all buffered entries to be updated.
	 */
	void UpdateMultiDataExpressionEntryBuffer(MultiDataType dataType, int row_start, const std::vector<affymetrix_calvin_data::ProbeSetMultiDataExpressionData> &expressionEntryBuffer);

	/*! Seek to appropriate file position and update expression entry vector
     * @param dataType The data type.
	 * @param row_start The start row for updating.
     * @param bufferEntrySize The size of the buffer for a given element in the vector
	 * @param expressionBufferEntry The vector contain all buffered entries to be updated.
	 */
	void UpdateMultiDataExpressionEntryBuffer(MultiDataType dataType, int row_start, int bufferEntrySize, const std::vector<char *> &expressionEntryBuffer);

	/*! Seek to appropriate file position and update genotype entry
   * @param dataType The data type.
	 * @param row The row index.
	 * @param call CHP call representation.
	 * @param confidence CHP confidence value.
	 */
	void UpdateDmetBiAllelicEntry(MultiDataType dataType, int row, const affymetrix_calvin_data::DmetBiAllelicData &entry);

	/*! Seek to appropriate file position and update genotype entry vector
   * @param dataType The data type.
	 * @param row_start The start row for updating.
	 * @param genotypeBufferEntry The vector contain all buffered entries to be updated.
	 */
	void UpdateDmetBiAllelicEntryBuffer(MultiDataType dataType, int row_start, const std::vector<affymetrix_calvin_data::DmetBiAllelicData> &entryBuffer);

	/*! Seek to appropriate file position and update genotype entry vector
   * @param dataType The data type.
	 * @param row_start The start row for updating.
   * @param bufferEntrySize The size of the buffer for a given element in the vector
	 * @param genotypeBufferEntry The vector contain all buffered entries to be updated.
	 */
	void UpdateDmetBiAllelicEntryBuffer(MultiDataType dataType, int row_start, int bufferEntrySize, const std::vector<char*> &entryBuffer);

	/*! Seek to appropriate file position and update genotype entry
   * @param dataType The data type.
	 * @param row The row index.
	 * @param call CHP call representation.
	 * @param confidence CHP confidence value.
	 */
	void UpdateDmetCopyNumberEntry(MultiDataType dataType, int row, const affymetrix_calvin_data::DmetCopyNumberData &entry);

	/*! Seek to appropriate file position and update genotype entry vector
   * @param dataType The data type.
	 * @param row_start The start row for updating.
	 * @param genotypeBufferEntry The vector contain all buffered entries to be updated.
	 */
	void UpdateDmetCopyNumberEntryBuffer(MultiDataType dataType, int row_start, const std::vector<affymetrix_calvin_data::DmetCopyNumberData> &entryBuffer);

	/*! Seek to appropriate file position and update genotype entry vector
   * @param dataType The data type.
	 * @param row_start The start row for updating.
   * @param bufferEntrySize The size of the buffer for a given element in the vector
	 * @param genotypeBufferEntry The vector contain all buffered entries to be updated.
	 */
	void UpdateDmetCopyNumberEntryBuffer(MultiDataType dataType, int row_start, int bufferEntrySize, const std::vector<char*> &entryBuffer);

	/*! Seek to appropriate file position and update genotype entry
   * @param dataType The data type.
	 * @param row The row index.
	 * @param call CHP call representation.
	 * @param confidence CHP confidence value.
	 */
	void UpdateDmetMultiAllelicEntry(MultiDataType dataType, int row, const affymetrix_calvin_data::DmetMultiAllelicData &entry);

	/*! Seek to appropriate file position and update genotype entry vector
   * @param dataType The data type.
	 * @param row_start The start row for updating.
	 * @param genotypeBufferEntry The vector contain all buffered entries to be updated.
	 */
	void UpdateDmetMultiAllelicEntryBuffer(MultiDataType dataType, int row_start, const std::vector<affymetrix_calvin_data::DmetMultiAllelicData> &entryBuffer);

	/*! Seek to appropriate file position and update genotype entry vector
   * @param dataType The data type.
	 * @param row_start The start row for updating.
   * @param bufferEntrySize The size of the buffer for a given element in the vector
	 * @param genotypeBufferEntry The vector contain all buffered entries to be updated.
	 */
	void UpdateDmetMultiAllelicEntryBuffer(MultiDataType dataType, int row_start, int bufferEntrySize, const std::vector<char*> &entryBuffer);

	/*! Seek to appropriate file position and update the segment entry vector
     * @param dataType The data type.
     * @param row_start The start row for updating.
     * @param bufferEntrySize The size of the buffer for a given element in the vector
     * @param entryBuffer The vector contain all buffered entries to be updated.
	 */
    void UpdateChromosomeSegmentEntryBuffer(MultiDataType dataType, int row_start, int bufferEntrySize, const std::vector<char*> &entryBuffer);

	/*! Seek to appropriate file position and update the summary entry vector
     * @param dataType The data type.
     * @param row_start The start row for updating.
     * @param bufferEntrySize The size of the buffer for a given element in the vector
     * @param entryBuffer The vector contain all buffered entries to be updated.
	 */
    void UpdateChromosomeSummaryEntryBuffer(MultiDataType dataType, int row_start, int bufferEntrySize, const std::vector<char*> &entryBuffer);

	/*! Seek to appropriate file position and update the summary entry vector
     * @param dataType The data type.
     * @param row_start The start row for updating.
     * @param bufferEntrySize The size of the buffer for a given element in the vector
     * @param entryBuffer The vector contain all buffered entries to be updated.
	 */
    void UpdateFamilialSegmentOverlapEntryBuffer(MultiDataType dataType, int row_start, int bufferEntrySize, const std::vector<char*> &entryBuffer);

	/*! Seek to appropriate file position and update the sample entry vector
     * @param dataType The data type.
     * @param row_start The start row for updating.
     * @param bufferEntrySize The size of the buffer for a given element in the vector
     * @param entryBuffer The vector contain all buffered entries to be updated.
	 */
    void UpdateFamilialSampleEntryBuffer(MultiDataType dataType, int row_start, int bufferEntrySize, const std::vector<char*> &entryBuffer);

	/*! Seek to appropriate file position and update allele peak entry vector
     * @param dataType The data type.
	 * @param row_start The start row for updating.
     * @param bufferEntrySize The size of the buffer for a given element in the vector
	 * @param allelePeakEntryBuffer The vector contain all buffered entries to be updated.
	 */
	void UpdateMultiDataAllelePeaksEntryBuffer(MultiDataType dataType, int row_start, int bufferEntrySize, const std::vector<char *> &allelePeakEntryBuffer);

	/*! Seek to appropriate file position and update AB signal entry vector
     * @param dataType The data type.
	 * @param row_start The start row for updating.
     * @param bufferEntrySize The size of the buffer for a given element in the vector
	 * @param entryBuffer The vector contain all buffered entries to be updated.
	 */
	void UpdateMultiDataMarkerABSignalsEntryBuffer(MultiDataType dataType, int row_start, int bufferEntrySize, const std::vector<char *> &entryBuffer);


	/*! Seek to appropriate file position and update Cyto genotyping call entry vector
     * @param dataType The data type.
	 * @param row_start The start row for updating.
     * @param bufferEntrySize The size of the buffer for a given element in the vector
	 * @param entryBuffer The vector contain all buffered entries to be updated.
	 */
	void UpdateMultiDataCytoGenotypeEntryBuffer(MultiDataType dataType, int rowStart, int bufferEntrySize, const std::vector<char*> &entryBuffer);


	/*! Close CHP signal file. */
	void CloseCHPFile();

private:
	// CHP signal file
	std::ofstream *m_CHPFile;
    
    /*! Map of data type to index. */
    std::map<MultiDataType, int> dataSetIndexMap;
     
    /*! Map of data type to data group index. */
    std::map<MultiDataType, int> dataGroupIndexMap;

    /*! Update the metrics */
    void UpdateMetrics(const std::vector<affymetrix_calvin_parameter::ParameterNameValueType> &metrics);
};

}

#endif // _CalvinCHPFileUpdater_HEADER_
