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
#ifndef _CHPMultiDataFileWriter_HEADER_
#define _CHPMultiDataFileWriter_HEADER_

/*! \file CalvinCHPMultiDataFileWriter.h Contains classes to write a multi data CHP file. */

#include "calvin_files/data/src/CHPMultiDataData.h"
#include "calvin_files/data/src/ProbeSetMultiDataData.h"
#include "calvin_files/writers/src/GenericFileWriter.h"
//
#include <fstream>
#include <map>
#include <vector>
//

namespace affymetrix_calvin_io
{

/*! A class to write a multi data CHP file. */
class CHPMultiDataFileWriter
{
private:
	/*! The file writer. */
	GenericFileWriter* writer;

	/*! The data set writer. */
	DataSetWriter* dataSetWriter;

	/*! The file position of the entry for each data set. */
    std::map<MultiDataType, int32_t> entryPos;

    /*! A map of data type to data set index. */
    std::map<MultiDataType, int> dataTypeToIndex; 

    /*! A map of data set index to data type */
    std::map<int, MultiDataType> indexToDataType; 

    /*! The maximum length of the name column. */
    int maxName;

    /*! The maximum length of the familial seg type column. */
    int maxSegmentType;

    /*! The maximum length of the familial ref seg id column. */
    int maxReferenceSegmentID;

    /*! The maximum length of the familial seg id column. */
    int maxFamilialSegmentID;

	/*! The maximum length of the familial sample ARR id */
	int maxFamilialARRID;

	/*! The maximum length of the familial sample CHP id */
	int maxFamilialCHPID;

	/*! The maximum length of the familial sample CHP file name */
	int maxFamilialCHPFile;

	/*! The maximum length of the familial sample role */
	int maxFamilialRole;

    /*! The data type being written. */
    MultiDataType currentDataType;

    /*! The data. */
    CHPMultiDataData *data;

    /*! Write the metrics to the file.
     * @param metrics Other metrics associated with the probe set
     */
    void WriteMetrics(const std::vector<affymetrix_calvin_parameter::ParameterNameValueType> &metrics);

public:

	/*! Constructor
	 * @param p Pointer to the multi data CHP data object.
	 */
	CHPMultiDataFileWriter(CHPMultiDataData& p);

	/*! Destructor */
	~CHPMultiDataFileWriter();

	/*! Write an entry.
	 * @param p A pointer to a multi data entry.
	 */
	void WriteEntry(const affymetrix_calvin_data::ProbeSetMultiDataGenotypeData & p);

	/*! Write an entry.
	 * @param p A pointer to a multi data entry.
	 */
	void WriteEntry(const affymetrix_calvin_data::ProbeSetMultiDataCopyNumberData & p);

	/*! Write an entry.
	 * @param p A pointer to a multi data entry.
	 */
	void WriteEntry(const affymetrix_calvin_data::ProbeSetMultiDataCytoRegionData & p);

	/*! Write an entry.
	 * @param p A pointer to a multi data entry.
	 */
	void WriteEntry(const affymetrix_calvin_data::ProbeSetMultiDataExpressionData & p);

    /*! Write an entry.
	 * @param p A pointer to a multi data entry.
	 */
	void WriteEntry(const affymetrix_calvin_data::ProbeSetMultiDataCopyNumberVariationRegionData & p);

	/*! Write an entry.
	 * @param p A pointer to a multi data entry.
	 */
	void WriteEntry(const affymetrix_calvin_data::DmetBiAllelicData & p);

	/*! Write an entry.
	 * @param p A pointer to a multi data entry.
	 */
	void WriteEntry(const affymetrix_calvin_data::DmetMultiAllelicData & p);

	/*! Write an entry.
	 * @param p A pointer to a multi data entry.
	 */
	void WriteEntry(const affymetrix_calvin_data::DmetCopyNumberData & p);

	/*! Write an entry.
	 * @param p A pointer to a multi data entry.
	 */
    void WriteEntry(const affymetrix_calvin_data::ChromosomeMultiDataSummaryData & p);

	/*! Write an entry.
	 * @param p A pointer to a multi data entry.
	 */
    void WriteEntry(const affymetrix_calvin_data::ChromosomeSegmentData & p);

	/*! Write an entry.
	 * @param p A pointer to a multi data entry.
	 */
    void WriteEntry(const affymetrix_calvin_data::ChromosomeSegmentDataEx & p);

	/*! Write an entry.
	 * @param p A pointer to a multi data entry.
	 */
    void WriteEntry(const affymetrix_calvin_data::FamilialSample & p);

	/*! Write an entry.
	 * @param p A pointer to a multi data entry.
	 */
    void WriteEntry(const affymetrix_calvin_data::FamilialSegmentOverlap & p);

	/*! Write an entry.
	 * @param p A pointer to a multi data entry.
	 */
	void WriteEntry(const affymetrix_calvin_data::AllelePeaks & p);

	/*! Write an entry.
	 * @param p A pointer to a multi data entry.
	 */
	void WriteEntry(const affymetrix_calvin_data::MarkerABSignals & p);

	/*! Write an entry.
	 * @param p A pointer to a multi data entry.
	 */
	void WriteEntry(const affymetrix_calvin_data::CytoGenotypeCallData & p);

	/*! Seeks to the data set.
     * @param dataType The data type
     */
	void SeekToDataSet(MultiDataType dataType);

private:

	/*! Writes the headers to the file. */
	void WriteHeaders();

	/*! Sets the file positions.
	 * @return The current file position.
	 */
	int32_t SetFilePositions();
};

}

#endif // _CHPMultiDataFileWriter_HEADER_
