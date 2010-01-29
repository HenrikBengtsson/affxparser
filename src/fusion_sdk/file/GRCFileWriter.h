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

#ifndef _GRCFileWriter_HEADER_
#define _GRCFileWriter_HEADER_

/*! \file GRCFileWriter.h This file provides writing capaibilities for GRC files.
 */

//////////////////////////////////////////////////////////////////////

#include "file/1LQFileData.h"
#include "file/GRCFileData.h"
#include "file/GridControlData.h"
//

//////////////////////////////////////////////////////////////////////

namespace affxgrc
{

////////////////////////////////////////////////////////////////////

/*! This class holds the coordinates of those probes used by the GCAST algorithm. */
class CGRCFileWriter : public CGRCFileData
{
public:
	/*! Constructor
	 */
	CGRCFileWriter();

	/*! Destructor
	 */
	~CGRCFileWriter();

	/*! Writes the contents of the file.
	 * @param libData The data from a 1LQ file.
	 * @param opposite The generated GRC File contains the reverse direction.
	 * @return True if successful.
	 */
	bool Write(const affx1lq::C1LQFileData &libData, bool opposite = false);

	/*! Writes the existing 1lq data to the grc file.
	 * This method requires that this class be populated with 1LQ File Data before it can be
	 *    invoked (To poluate 1LQ file, you must first call PopulateData or 
	 *    Write(const affx1lq::C1LQFileData &libData,..) method).
	 * @param opposite The generated GRC File contains the reverse direction.
	 * @return True if successful.
	 */
	bool Write(bool opposite = false);

	/*! Populate the class with 1LQ file data.
	 * @param libData The data from a 1LQ file.
	 */
	void PopulateData(const affx1lq::C1LQFileData &libData);

	/*! Sets the number of rows.
	 * @param rows The number of rows.
	 */
	void SetRows(int rows) { m_Grid.SetRows(rows); }

	/*! Sets the number of columns.
	 * @param columns The number of columns.
	 */
	void SetColumns(int columns) { m_Grid.SetColumns(columns); }

	/*! Resizes the B1 array.
	 * @param size The size of the array.
	 */
	void ResizeB1(int size) { m_Grid.ResizeB1(size); }

	/*! Resizes the B2 array.
	 * @param size The size of the array.
	 */
	void ResizeB2(int size) { m_Grid.ResizeB2(size); }

	/*! Resizes the NS array.
	 * @param size The size of the array.
	 */
	void ResizeNS(int size) { m_Grid.ResizeNS(size); }

	/*! Sets the B1 probe at the given index.
	 * @param index The index of the array.
	 * @param coord The new coordinate.
	 */
	void SetB1(int index, int x, int y);

	/*! Sets the B2 probe at the given index.
	 * @param index The index of the array.
	 * @param coord The new coordinate.
	 */
	void SetB2(int index, int x, int y);

	/*! Sets the NS probe at the given index.
	 * @param index The index of the array.
	 * @param coord The new coordinate.
	 */
	void SetNS(int index, int x, int y);

protected:
	/*! The GRC file object. */
	affymetrix_grid_control::GridControlData m_Grid;

	/*! Parses the data from the 1LQ object.
	 * @param libData The data from a 1LQ file.
	 */
	void ParseLibData(const affx1lq::C1LQFileData &libData);

	/*! Writes the existing data to the GRC file
	 * @param opposite The generated GRC File contains the reverse direction.
	 * @return True if successful.
	 */
	bool WriteFile(bool opposite = false);

};

////////////////////////////////////////////////////////////////////

} // namespace

//////////////////////////////////////////////////////////////////////

#endif
