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
 * @file   CHPFileBufferWriter.h
 * @author David Le
 * @date   Mon May 15 12:09:42 2006
 * 
 * @brief  Class for writing signals to a buffer before writing to CHP files.
 */

#ifndef _CCHPFILEBUFFERWRITER_HEADER_
#define _CCHPFILEBUFFERWRITER_HEADER_

//
#include "file/CHPFileData.h"
//
#include "portability/affy-base-types.h"
//
#include <cstring>
#include <string>
#include <vector>
//

#define MAX_BUFFER_SIZE				5242880		// 5 MB

namespace affxchpwriter
{
class CCHPFileBufferWriter
{
	class GenotypeBufferEntry
	{
	public:
		uint8_t call;
		float confidence;
		float RAS1;
		float RAS2;
		float aaCall;
		float abCall;
		float bbCall;
		float noCall;
	};

	class ExpressionBufferEntry
	{
	public:
		unsigned char Detection;
		float DetectionPValue;
		float Signal; 
		unsigned short NumPairs;
		unsigned short NumUsedPairs;
	};

public:
	/*! Constructor */
	CCHPFileBufferWriter();

	/*! Destructor */
	~CCHPFileBufferWriter();

	/*! Set maximum buffer size */
	void SetMaxBufferSize(int MaxBufferSize) { m_MaxBufferSize = MaxBufferSize; }

	/*! Cleans up memory */
	void Cleanup();

	/*! Initialize entry buffer writer
	 * @param CHPFileNames Reference to a list of CHP file names.
	 * @param IsGenotype Genotype or Expression CHP file.
	 */
	void Initialize(std::vector<std::string> *CHPFileNames, bool IsGenotype);

	/*! Write an entry to buffer. If the buffer is full, flush it.
	 * @param target Target for the Signal entry.
	 * @param entry CHP genotype entry.
	 */
	void WriteGenotypeEntry(int target, affxchp::CGenotypeProbeSetResults &entry);

	/*! Write an entry to buffer. If the buffer is full, flush it.
	 * @param target Target for the Signal entry.
	 * @param entry CHP expression entry.
	 */
	void WriteExpressionEntry(int target, affxchp::CExpressionProbeSetResults &entry);

	/*! Write the content of the buffer to CHP files. */
	void FlushBuffer();

private:
	// Pointer to list of CHP file names.
	std::vector<std::string> *m_CHPFileNames;

	// List of targets used for storing genotype entries.
	std::vector< std::vector<GenotypeBufferEntry> > m_TargetGenotypeEntryBuffers;

	// List of targets used for storing expression entries.
	std::vector< std::vector<ExpressionBufferEntry> > m_TargetExpressionEntryBuffers;

	// Buffer for storing genotype entry row indexes.
	std::vector<int> m_TargetEntryRowIndexes;

	// Size of the current buffer in bytes.
	int m_BufferSize;

	// Maximum size of the buffer before it gets flushed
	int m_MaxBufferSize;

	// Genotype or Expression
	bool m_IsGenotype;
};

}

#endif // _CHPFILEBUFFERWRITER_HEADER_
