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
#ifndef _CHPFileWriter_HEADER_
#define _CHPFileWriter_HEADER_

/*! \file CalvinCHPFileWriter.h Provides classes to write CHP files with expression, genotyping, tag and resequencing data. */

#include "calvin_files/data/src/CHPBackgroundZone.h"
#include "calvin_files/data/src/CHPData.h"
#include "calvin_files/data/src/CHPExpressionEntry.h"
#include "calvin_files/data/src/CHPGenotypeEntry.h"
#include "calvin_files/data/src/CHPReseqEntry.h"
#include "calvin_files/data/src/CHPUniversalEntry.h"
#include "calvin_files/writers/src/GenericFileWriter.h"
//
#include <fstream>
//


namespace affymetrix_calvin_io
{

/*! Provides write capabilities for expression, genotyping, resequencing and tag arrays. */
class CHPFileWriter
{

private:

	/*! The maximum length of a probe set name. */
	int maxProbeSetName;

	/*! The file writer. */
	GenericFileWriter* writer;

	/*! The data set writer. */
	DataSetWriter* dataSetWriter;

	/*! The position of the data entry. */
	int32_t entryPos;

	/*! The position of the bg zone entry. */
	int32_t bgZonePos;

	/*! The position of the force entry. */
	int32_t forcePos;

	/*! The position of the orig entry. */
	int32_t origPos;

public:

	/*! Constructor.
	 * @param p The CHP file object.
	 */
	CHPFileWriter(CHPData& p);

	/*! Destructor */
	~CHPFileWriter();

	/*! Seeks to the data set. */
	void SeekToDataSet();

	/*! Writes an expression result to the file.
	 * @param p The result to write.
	 */
	void WriteExpressionEntry(const CHPExpressionEntry& p);

	/*! Writes a genotype result to the file.
	 * @param p The result to write.
	 */
	void WriteGenotypeEntry(const CHPGenotypeEntry& p);

	/*! Writes a universal (tag) result to the file.
	 * @param p The result to write.
	 */
	void WriteUniversalEntry(const CHPUniversalEntry& p);

	/*! Writes a resequencing result to the file.
	 * @param p The result to write.
	 */
	void WriteReseqEntry(const CHPReseqEntry& p);

	/*! Seeks to the data set. */
	void SeekToBgSet();

	/*! Writes a background zone to the file.
	 * @param zone The result to write.
	 */
	void WriteBackgroundZone(const CHPBackgroundZone& zone);

	/*! Seeks to the data set. */
	void SeekToForceSet();

	/*! Writes a force call to the file.
	 * @param force The result to write.
	 */
	void WriteForceCall(const CHPReseqForceCall& force);

	/*! Seeks to the data set. */
	void SeekToOrigCallSet();

	/*! Writes original call to the file.
	 * @param orig The result to write.
	 */
	void WriteOrigCall(const CHPReseqOrigCall& orig);

private:

	/*! Writes the header to the file. */
	void WriteHeaders();

	/*! Updates the file positions after a data set is added to the file. */
	void SetFilePositions();
};

}

#endif // _CHPFileWriter_HEADER_
