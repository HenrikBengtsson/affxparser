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


/*! \file DataSetUpdater.h Provides interfaces to update data in a "Calvin" binary data file. */

#ifndef _DataSetUpdater_HEADER_
#define _DataSetUpdater_HEADER_

#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//
#include <cstring>
#include <fstream>
#include <string>
#include <vector>
//

namespace affymetrix_calvin_io
{

/*! Provides interfaces to update data in a "Calvin" binary data file.
 * This class assumes that the file has been completely written and that
 * only existing data groups/sets/rows/cols are being modified.
 */
class DataSetUpdater
{
protected:
	/*! The name of the file being updated. */
	std::string fileName;

	/*! The position of a data set for all data sets in the file. */
	std::vector<std::vector<int> > positions;

	/*! The size of a row for all data sets in the file. */
	std::vector<std::vector<int> > rowsizes;

	/*! The size of a column for all data sets in the file. */
	std::vector<std::vector<std::vector<int> > > colsizes;

    /*! The data set names. */
    std::vector<std::vector<std::wstring> > dataSetNames; 

	/*! Seek to the position of the data.
	 * @param groupIndex The index to the data groups.
	 * @param setIndex The index to the data sets.
	 * @param row The index to the rows.
	 * @param col The index to the columns.
	 */
	void SeekToPosition(std::ofstream &os, int groupIndex, int setIndex, int row, int col);

public:
	/*! Constructor */
	DataSetUpdater();

	/*! Destructor */
	~DataSetUpdater();

	/*! Initialize the class given a "calvin" data file.
	 * @param file The name of the file.
	 * @exception affymetrix_calvin_exceptions::FileNotFoundException The file does not exist.
	 * @exception affymetrix_calvin_exceptions::InvalidVersionException The file version does not match.
	 * @exception affymetrix_calvin_exceptions::InvalidFileTypeException The file is not of the right type.
	 */
	virtual void Initialize(const char *file);

	/*! Updates a string value in the data set
	 * @param groupIndex The index to the data groups.
	 * @param setIndex The index to the data sets.
	 * @param row The index to the rows.
	 * @param col The index to the columns.
	 * @param value The new value.
	 */
	void Update(int groupIndex, int setIndex, int row, int col, const std::string &value);

	/*! Updates a wstring value in the data set
	 * @param groupIndex The index to the data groups.
	 * @param setIndex The index to the data sets.
	 * @param row The index to the rows.
	 * @param col The index to the columns.
	 * @param value The new value.
	 */
	void Update(int groupIndex, int setIndex, int row, int col, const std::wstring &value);

	/*! Updates an integer value in the data set
	 * @param groupIndex The index to the data groups.
	 * @param setIndex The index to the data sets.
	 * @param row The index to the rows.
	 * @param col The index to the columns.
	 * @param value The new value.
	 */
	void Update(int groupIndex, int setIndex, int row, int col, int8_t value);

	/*! Updates an integer value in the data set
	 * @param groupIndex The index to the data groups.
	 * @param setIndex The index to the data sets.
	 * @param row The index to the rows.
	 * @param col The index to the columns.
	 * @param value The new value.
	 */
	void Update(int groupIndex, int setIndex, int row, int col, int16_t value);

	/*! Updates an integer value in the data set
	 * @param groupIndex The index to the data groups.
	 * @param setIndex The index to the data sets.
	 * @param row The index to the rows.
	 * @param col The index to the columns.
	 * @param value The new value.
	 */
	void Update(int groupIndex, int setIndex, int row, int col, int32_t value);

	/*! Updates an integer value in the data set
	 * @param groupIndex The index to the data groups.
	 * @param setIndex The index to the data sets.
	 * @param row The index to the rows.
	 * @param col The index to the columns.
	 * @param value The new value.
	 */
	void Update(int groupIndex, int setIndex, int row, int col, u_int8_t value);

	/*! Updates an integer value in the data set
	 * @param groupIndex The index to the data groups.
	 * @param setIndex The index to the data sets.
	 * @param row The index to the rows.
	 * @param col The index to the columns.
	 * @param value The new value.
	 */
	void Update(int groupIndex, int setIndex, int row, int col, u_int16_t value);

	/*! Updates an integer value in the data set
	 * @param groupIndex The index to the data groups.
	 * @param setIndex The index to the data sets.
	 * @param row The index to the rows.
	 * @param col The index to the columns.
	 * @param value The new value.
	 */
	void Update(int groupIndex, int setIndex, int row, int col, u_int32_t value);

	/*! Updates a floating point value in the data set
	 * @param groupIndex The index to the data groups.
	 * @param setIndex The index to the data sets.
	 * @param row The index to the rows.
	 * @param col The index to the columns.
	 * @param value The new value.
	 */
	void Update(int groupIndex, int setIndex, int row, int col, float value);
};

}

#endif // _DataSetUpdater_HEADER_
