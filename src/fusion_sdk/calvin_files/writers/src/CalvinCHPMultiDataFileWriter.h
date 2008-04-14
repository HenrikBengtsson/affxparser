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

#include <vector>
#include <fstream>
#include <map>
#include "CHPMultiDataData.h"
#include "GenericFileWriter.h"
#include "ProbeSetMultiDataData.h"

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

    /*! The maximum probe set name. */
    int maxProbeSetName;

    /*! The data type being written. */
    MultiDataType currentDataType;

    bool seekPos;

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
