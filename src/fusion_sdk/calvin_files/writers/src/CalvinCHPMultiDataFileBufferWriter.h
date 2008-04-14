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


/**
 * @file   CalvinCHPMultiDataFileBufferWriter.h
 * @brief  Class for writing multi-data data to a buffer before writing to CHP files.
 */

#ifndef _CalvinCHPMultiDataFileBufferWriter_HEADER_
#define _CalvinCHPMultiDataFileBufferWriter_HEADER_

#include <string>
#include <vector>
#include <map>
#include "ProbeSetMultiDataData.h"
#include <assert.h>

#define MAX_BUFFER_SIZE				5242880		// 5 MB

namespace affymetrix_calvin_io
{
class CHPMultiDataFileBufferWriter
{
public:
	/*! Constructor */
	CHPMultiDataFileBufferWriter();

	/*! Destructor */
	~CHPMultiDataFileBufferWriter();

	/*! Set maximum buffer size */
	void SetMaxBufferSize(int MaxBufferSize) { m_MaxBufferSize = MaxBufferSize; }

	/*! Cleans up memory */
	void Cleanup();

	/*! Initialize the buffer writer
	 * @param CHPFileNames Reference to a list of CHP file names.
     * @param dataTypes The data types.
     * @param maxProbeSetNameLength The maximum probe set name length
	 */
    void Initialize(std::vector<std::string> *CHPFileNames, std::vector<MultiDataType> &dataTypes, std::map<MultiDataType, int> &maxProbeSetNameLength);

	/*! Write a genotype entry to buffer. If the buffer is full, flush it.
     * @param dataType The data type.
	 * @param target Target for the entry.
	 * @param entry Value for the genotype entry.
	 */
	void WriteMultiDataGenotypeEntry(MultiDataType dataType, int target, const affymetrix_calvin_data::ProbeSetMultiDataGenotypeData &entry);

	/*! Write a copy number entry to buffer. If the buffer is full, flush it.
     * @param dataType The data type.
	 * @param target Target for the entry.
	 * @param entry Value for the copy number entry.
	 */
	void WriteMultiDataCopyNumberEntry(MultiDataType dataType, int target, const affymetrix_calvin_data::ProbeSetMultiDataCopyNumberData &entry);

	/*! Write a cyto region entry to buffer. If the buffer is full, flush it.
     * @param dataType The data type.
	 * @param target Target for the entry.
	 * @param entry Value for the copy number entry.
	 */
	void WriteMultiDataCytoRegionEntry(MultiDataType dataType, int target, const affymetrix_calvin_data::ProbeSetMultiDataCytoRegionData &entry);

	/*! Write an expression entry to buffer. If the buffer is full, flush it.
     * @param dataType The data type.
	 * @param target Target for the entry.
	 * @param entry Value for the expression entry.
	 */
	void WriteMultiDataExpressionEntry(MultiDataType dataType, int target, const affymetrix_calvin_data::ProbeSetMultiDataExpressionData &entry);

	/*! Write the content of the buffer to CHP files. */
	void FlushBuffer();

private:
    /*! Map of data type to data set index. */
    std::map<MultiDataType, int> dataSetIndexMap;

	/*! Pointer to list of CHP file names. */
	std::vector<std::string> *m_CHPFileNames;

	/*! List of targets used for storing entries. */
	std::vector< std::vector<char *> > m_TargetMultiDataGenotypeBuffers;

	/*! List of targets used for storing entries. */
	std::vector< std::vector<char *> > m_TargetMultiDataCopyNumberBuffers;

	/*! List of targets used for storing entries. */
	std::vector< std::vector<char *> > m_TargetMultiDataCytoRegionBuffers;

	/*! List of targets used for storing entries. */
    std::vector< std::vector<char *> > m_TargetMultiDataExpressionBuffers;

	/*! Buffer for storing row indexes. */
	std::vector<int> m_TargetMultiDataGenotypeRowIndexes;

	/*! Buffer for storing row indexes. */
	std::vector<int> m_TargetMultiDataCopyNumberRowIndexes;

	/*! Buffer for storing row indexes. */
	std::vector<int> m_TargetMultiDataCytoRegionRowIndexes;

	/*! Buffer for storing row indexes. */
	std::vector<int> m_TargetMultiDataExpressionRowIndexes;

	/*! Size of the current buffer in bytes. */
	int m_BufferSize;

	/*! Maximum size of the buffer before it gets flushed */
	int m_MaxBufferSize;

    /*! The maximum probe set name length */
    std::map<MultiDataType, int> m_maxProbeSetNameLength;

    /*! The buffer size of a genotype entry */
    int genoBufferSize;

    /*! The buffer size of a genotype entry */
    int exprBufferSize;

    /*! The buffer size of a genotype entry */
    int cnBufferSize;

    /*! The buffer size of a cyto entry */
    int cyBufferSize;
};

}

#endif // _CalvinCHPMultiDataFileBufferWriter_HEADER_
