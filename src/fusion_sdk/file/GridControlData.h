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


#ifndef _GridControlData_HEADER_
#define _GridControlData_HEADER_

/*! \file GridControlData.h This file provides interfaces to store information in a GRC (grid control) file.
 */

#include <cstring>
#include <string>
#include <vector>
//

namespace affymetrix_grid_control
{

/*! This structure provides a feature coordinate definition. */
typedef struct _FeatureCoordinate
{
	/*! The X cooridnate. */
	int x;

	/*! The Y cooridnate. */
	int y;

} FeatureCoordinate;

/*! This class provides interfaces to store GRC file information. */
class GridControlData
{
public:
	/*! Constructor */
	GridControlData() {};

	/*! Destructor */
	~GridControlData() { Clear(); };

protected:
	/*! An array of coordinates for the B1 probes. */
	std::vector<FeatureCoordinate> probesB1;

	/*! An array of coordinates for the B2 probes. */
	std::vector<FeatureCoordinate> probesB2;

	/*! An array of coordinates for the non-synthesized features. */
	std::vector<FeatureCoordinate> probesNS;

	/*! The number of columns of features on the array */
	int arrayColumns;

	/*! The number of rows of features on the array */
	int arrayRows;

public:
	/*! Gets the number of rows.
	 * @return The number of rows.
	 */
	int GetRows() const { return arrayRows; }

	/*! Sets the number of rows.
	 * @param rows The number of rows.
	 */
	void SetRows(int rows) { arrayRows = rows; }

	/*! Gets the number of columns.
	 * @return The number of columns.
	 */
	int GetColumns() const { return arrayColumns; }

	/*! Sets the number of columns.
	 * @param columns The number of columns.
	 */
	void SetColumns(int columns) { arrayColumns = columns; }

	/*! Resizes the B1 array.
	 * @param size The size of the array.
	 */
	void ResizeB1(int size) { probesB1.resize(size); }

	/*! Resizes the B2 array.
	 * @param size The size of the array.
	 */
	void ResizeB2(int size) { probesB2.resize(size); }

	/*! Resizes the NS array.
	 * @param size The size of the array.
	 */
	void ResizeNS(int size) { probesNS.resize(size); }

	/*! Sets the B1 probe at the given index.
	 * @param index The index of the array.
	 * @param coord The new coordinate.
	 */
	void SetB1(int index, FeatureCoordinate coord) { probesB1[index] = coord; }

	/*! Gets the B1 probe at the given index.
	 * @param index The index of the array.
	 * @return coord The coordinate.
	 */
	FeatureCoordinate GetB1(int index) const { return probesB1[index]; }

	/*! Gets the number of B1 probes.
	 * @return The number of B1 probes.
	 */
	int GetNumB1Probes() const { return (int) probesB1.size(); }

	/*! Sets the B2 probe at the given index.
	 * @param index The index of the array.
	 * @param coord The new coordinate.
	 */
	void SetB2(int index, FeatureCoordinate coord) { probesB2[index] = coord; }

	/*! Gets the B2 probe at the given index.
	 * @param index The index of the array.
	 * @return coord The coordinate.
	 */
	FeatureCoordinate GetB2(int index) const { return probesB2[index]; }

	/*! Gets the number of B2 probes.
	 * @return The number of B2 probes.
	 */
	int GetNumB2Probes() const { return (int) probesB2.size(); }

	/*! Sets the NS probe at the given index.
	 * @param index The index of the array.
	 * @param coord The new coordinate.
	 */
	void SetNS(int index, FeatureCoordinate coord) { probesNS[index] = coord; }

	/*! Gets the NS probe at the given index.
	 * @param index The index of the array.
	 * @return coord The coordinate.
	 */
	FeatureCoordinate GetNS(int index) const { return probesNS[index]; }

	/*! Gets the number of NS probes.
	 * @return The number of NS probes.
	 */
	int GetNumNSProbes() const { return (int) probesNS.size(); }

	/*! Clears the member objects. */
	void Clear() {
		probesB1.clear();
		probesB2.clear();
		probesNS.clear();
		arrayColumns=0;
		arrayRows=0;
	};

	
};

};

#endif // _GridControlData_HEADER_

