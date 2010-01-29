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


/**
 * @file   CalvinCHPQuantificationFileBufferWriter.h
 * @author David Le
 * @date   Mon May 15 12:09:42 2006
 * 
 * @brief  Class for writing Quantifications to a buffer before writing to CHP files.
 */

#ifndef _CHPQUANTIFICATIONFILEBUFFERWRITER_HEADER_
#define _CHPQUANTIFICATIONFILEBUFFERWRITER_HEADER_

#include <cstring>
#include <string>
#include <vector>
//

#define MAX_BUFFER_SIZE				5242880		// 5 MB

namespace affymetrix_calvin_io
{
class CHPQuantificationFileBufferWriter
{
public:
	/*! Constructor */
	CHPQuantificationFileBufferWriter();

	/*! Destructor */
	~CHPQuantificationFileBufferWriter();

	/*! Set maximum buffer size */
	void SetMaxBufferSize(int MaxBufferSize) { m_MaxBufferSize = MaxBufferSize; }

	/*! Cleans up memory */
	void Cleanup();

	/*! Initialize quantification buffer writer
	 * @param CHPFileNames Reference to a list of CHP file names.
	 */
	void Initialize(std::vector<std::string> *CHPFileNames);

	/*! Write a quantification entry to buffer. If the buffer is full, flush it.
	 * @param target Target for the quantification entry.
	 * @param quantification Value for the quantification entry.
	 */
	void WriteQuantificationEntry(int target, float quantification);

	/*! Write the content of the buffer to CHP files. */
	void FlushBuffer();

private:
	// Pointer to list of CHP file names.
	std::vector<std::string> *m_CHPFileNames;

	// List of targets used for storing quantification entries.
	std::vector< std::vector<float> > m_TargetQuantificationBuffers;

	// Buffer for storing quantification row indexes.
	std::vector<int> m_TargetQuantificationRowIndexes;

	// Size of the current buffer in bytes.
	int m_BufferSize;

	// Maximum size of the buffer before it gets flushed
	int m_MaxBufferSize;
};

}

#endif // _CHPQUANTIFICATIONFILEBUFFERWRITER_HEADER_
