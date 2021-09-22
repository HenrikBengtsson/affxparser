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

#ifndef _1LQ_FILE_DATA_HEADER_
#define _1LQ_FILE_DATA_HEADER_

/*! \file 1LQFileData.h This file provides 1LQ file reading and storage capabilities.
 */

//////////////////////////////////////////////////////////////////////

#include "portability/affy-base-types.h"
//
#include <cstring>
#include <fstream>
#include <list>
#include <string>
//

//////////////////////////////////////////////////////////////////////

namespace affx1lq
{

/*! The number of columns in the file */
#define NUM_1LQ_COLUMNS 16

/*! Data for a line in the 1LQ file. */
typedef struct _DataEntry
{
	/*! The x coodinate */
	int32_t x;

	/*! The y coodinate */
	int32_t y;

	/*! The probe sequence */
	std::string probe;

	/*! The design type */
	int32_t destype;

	/*! The feature */
	std::string feature;

	/*! The qualifier */
	std::string qualifier;

	/*! The expos */
	int32_t expos;

	/*! The probe length */
	int32_t plength;

	/*! The position */
	int32_t position;

	/*! The cbase */
	char cbase;

	/*! The probe base at the substitution position. */
	char pbase;

	/*! The target base being interrogated by the probe. */
	char tbase;

	/*! The ipbase */
	char ipbase;

	/*! The unit */
	int32_t unit;

	/*! The block */
	int32_t block;

	/*! The atom */
	int32_t atom;


} DataEntry;



/*! The class provides storage and reading capabilities for 1LQ files. */
class C1LQFileData
{
public:
	/*! Constructor */
	C1LQFileData();

	/*! Destructor */
	~C1LQFileData();

protected:
	/*! The full path to the BAR file. */
	std::string m_FileName;

	/*! The number of rows of features on the array. */
	int		m_NumberRows;

	/*! The number of columns of features on the array. */
	int		m_NumberColumns;

	/*! The sequence results. */
	std::list<DataEntry> m_Entries;

	/*! The number of rotations that have been done on the data. */
	int m_NumberRotates;

	/*! Reads the file data
	 * @param instr The input file stream.
	 * @return True if successful.
	 */
	bool Read(std::ifstream &instr);

public:
	/*! Sets the full path to the BAR file.
	 * @param name The file name.
	 */
	void SetFileName (const char *name) { m_FileName = name; }

	/*! Gets the full path of the file.
	 * @return The full path of the file.
	 */
	std::string GetFileName() const { return m_FileName; }

	/*! Reads the contents of the file.
	 * @return True if successful.
	 */
	bool Read();

	/*! Checks if the file exists.
	 * @return True if exists.
	 */
	bool Exists();

	/*! Deallocate memory. */
	void Clear();

	/*! Gets the number of columns of features on the array.
	 * @return The number of columns of features on the array.
	 */
	int	GetNumberColumns() const { return m_NumberColumns; }

	/*! Gets the number of rows of features on the array.
	 * @return The number of rows of features on the array.
	 */
	int	GetNumberRows() const { return m_NumberRows; }

	/*! Return the number of times this data has been rotate.
	 *  @return The number of times this data has been rotate.
	 *          In normal condition, it should return 0 for no-rotation
	 *          or 1 for one time rotate.
	 */
	int GetNumberRotates() { return m_NumberRotates; }

	/*! Rotate the data one time. */
	void Rotate();

	/*! Gets the data list.
	 * @return The data list.
	 */
	const std::list<DataEntry>& GetEntries() const { return m_Entries; }
};

////////////////////////////////////////////////////////////////////

} // namespace

//////////////////////////////////////////////////////////////////////

#endif // !defined(_1LQ_FILE_DATA_HEADER_)
