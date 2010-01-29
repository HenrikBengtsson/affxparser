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

/*! \file CalvinCHPMultiDataFileUpdater.h Provides interfaces to update data in a "Calvin" binary "MultiData" data file. */

#ifndef _CalvinCHPMultiDataFileUpdater_HEADER_
#define _CalvinCHPMultiDataFileUpdater_HEADER_

#include "calvin_files/data/src/CHPMultiDataData.h"
#include "calvin_files/data/src/ColumnInfo.h"
#include "calvin_files/data/src/ProbeSetMultiDataData.h"
#include "calvin_files/writers/src/DataSetUpdater.h"
//
#include <map>
#include <vector>
//

namespace affymetrix_calvin_io
{

/*! Provides interfaces to update data in a "Calvin" binary "MultiData" data file.
 * This class assumes that the file has been completely written and that
 * only existing data groups/sets/rows/cols are being modified.
 */
class CalvinCHPMultiDataFileUpdater : public DataSetUpdater
{
public:
	/*! Constructor */
	CalvinCHPMultiDataFileUpdater();

	/*! Destructor */
	~CalvinCHPMultiDataFileUpdater();

	/*! Initialize the class given a "calvin" data file.
	 * @param file The name of the file.
	 * @exception affymetrix_calvin_exceptions::FileNotFoundException The file does not exist.
	 * @exception affymetrix_calvin_exceptions::InvalidVersionException The file version does not match.
	 * @exception affymetrix_calvin_exceptions::InvalidFileTypeException The file is not of the right type.
	 */
	void Initialize(const char *file);

	/*! Update the entry for the given row
	 * @param row The row index.
	 * @param entry The new MultiData value.
     * @param metricColumns The column information.
	 */
	void UpdateMultiData(MultiDataType dataType, int row, const affymetrix_calvin_data::ProbeSetMultiDataGenotypeData &entry, const std::vector<ColumnInfo> &metricColumns);

	/*! Update only the call and confidence for the given row
	 * @param row The row index.
	 * @param entry The new MultiData value.
	 */
	void UpdateMultiData(MultiDataType dataType, int row, const affymetrix_calvin_data::ProbeSetMultiDataGenotypeData &entry);

	/*! Update the entry for the given row
	 * @param row The row index.
	 * @param entry The new MultiData value.
     * @param metricColumns The column information.
	 */
	void UpdateMultiData(MultiDataType dataType, int row, const affymetrix_calvin_data::ProbeSetMultiDataCopyNumberData &entry, const std::vector<ColumnInfo> &metricColumns);

	/*! Update the entry for the given row
	 * @param row The row index.
	 * @param entry The new MultiData value.
	 */
	void UpdateMultiData(MultiDataType dataType, int row, const affymetrix_calvin_data::ProbeSetMultiDataCytoRegionData &entry, const std::vector<ColumnInfo> &metricColumns);

	/*! Update the entry for the given row
	 * @param row The row index.
	 * @param entry The new MultiData value.
     * @param metricColumns The column information.
	 */
	void UpdateMultiData(MultiDataType dataType, int row, const affymetrix_calvin_data::ProbeSetMultiDataExpressionData &entry, const std::vector<ColumnInfo> &metricColumns);

	/*! Update only the call and confidence for the given row
	 * @param row The row index.
	 * @param entry The new MultiData value.
	 */
	void UpdateMultiData(MultiDataType dataType, int row, const affymetrix_calvin_data::ProbeSetMultiDataExpressionData &entry);

    /*! Update the entry for the given row
	 * @param row The row index.
	 * @param entry The new MultiData value.
	 */
	void UpdateMultiData(MultiDataType dataType, int row, const affymetrix_calvin_data::ProbeSetMultiDataCopyNumberVariationRegionData
         &entry, const std::vector<ColumnInfo> &metricColumns);

	/*! Update the entry for the given row
	 * @param row The row index.
	 * @param entry The new MultiData value.
     * @param metricColumns The column information.
	 */
	void UpdateMultiData(MultiDataType dataType, int row, const affymetrix_calvin_data::DmetBiAllelicData &entry, const std::vector<ColumnInfo> &metricColumns);

	/*! Update only the call and confidence for the given row
	 * @param row The row index.
	 * @param entry The new MultiData value.
	 */
	void UpdateMultiData(MultiDataType dataType, int row, const affymetrix_calvin_data::DmetBiAllelicData &entry);

	/*! Update the entry for the given row
	 * @param row The row index.
	 * @param entry The new MultiData value.
     * @param metricColumns The column information.
	 */
	void UpdateMultiData(MultiDataType dataType, int row, const affymetrix_calvin_data::DmetCopyNumberData &entry, const std::vector<ColumnInfo> &metricColumns);

	/*! Update only the call and confidence for the given row
	 * @param row The row index.
	 * @param entry The new MultiData value.
	 */
	void UpdateMultiData(MultiDataType dataType, int row, const affymetrix_calvin_data::DmetCopyNumberData &entry);

	/*! Update the entry for the given row
	 * @param row The row index.
	 * @param entry The new MultiData value.
     * @param metricColumns The column information.
	 */
	void UpdateMultiData(MultiDataType dataType, int row, const affymetrix_calvin_data::DmetMultiAllelicData &entry, const std::vector<ColumnInfo> &metricColumns);

	/*! Update only the call and confidence for the given row
	 * @param row The row index.
	 * @param entry The new MultiData value.
	 */
	void UpdateMultiData(MultiDataType dataType, int row, const affymetrix_calvin_data::DmetMultiAllelicData &entry);

private:
    std::map<MultiDataType, int> dataSetIndexMap;

  	/*! Update the metrics for the given row
	 * @param row The row index.
     * @param dataSetIndex The data set index
     * @param startColIndex The starting column
     * @param metricColumns The column information.
	 */
  void UpdateMetrics(int dataSetIndex, int row, int startColIndex, const std::vector<affymetrix_calvin_parameter::ParameterNameValueType> &metrics, const std::vector<ColumnInfo> &metricColumns);
};

}

#endif // _CalvinCHPMultiDataFileUpdater_HEADER_
