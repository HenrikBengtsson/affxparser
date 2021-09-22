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

#include "calvin_files/data/src/CHPMultiDataData.h"
#include "calvin_files/data/src/ProbeSetMultiDataData.h"
//
#include <cassert>
#include <cstring>
#include <map>
#include <string>
#include <vector>
//

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
   * @param maxProbeSetNmLn The maximum probe set name length
	 */
  void Initialize(std::vector<std::string> *CHPFileNames, std::vector<MultiDataType> &dataTypes, std::map<MultiDataType, int> &maxProbeSetNmLn);

	/*! Initialize the buffer writer
	 * @param CHPFileNames Reference to a list of CHP file names.
   * @param dataTypes The data types.
   * @param maxSegmentTypeLn The maximum segment type length
   * @param maxReferenceSegmentIDLn The maximum ref sample key length
   * @param maxFamilialSegmentIDLn The maximum familial seg id length
   * @param maxFamilialARRIDLn The maximum familial arr id length
   * @param maxFamilialCHPIDLn The maximum familial sample chp id length
   * @param maxFamilialCHPFilenameLn The maximum familial sample chp file length
   * @param maxFamilialRoleLn The maximum familial sample role length
   */
  void Initialize(std::vector<std::string> *CHPFileNames, std::vector<MultiDataType> &dataTypes,
      std::map<MultiDataType, int> &maxSegmentTypeLn,
      std::map<MultiDataType, int> &maxReferenceSegmentIDLn,
      std::map<MultiDataType, int> &maxFamilialSegmentIDLn,	  
	  std::map<MultiDataType, int> &maxFamilialARRIDLn,
	  std::map<MultiDataType, int> &maxFamilialCHPIDLn,
	  std::map<MultiDataType, int> &maxFamilialCHPFilenameLn,
	  std::map<MultiDataType, int> &maxFamilialRoleLn);

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

  /*! Write a copy number variation region entry to buffer. If the buffer is full, flush it.
   * @param dataType The data type.
	 * @param target Target for the entry.
	 * @param entry Value for the copy number entry.
	 */
	void WriteMultiDataCopyNumberVariationRegionEntry(MultiDataType dataType, int target, const affymetrix_calvin_data::ProbeSetMultiDataCopyNumberVariationRegionData &entry);

	/*! Write a DMET copy number entry to buffer. If the buffer is full, flush it.
   * @param dataType The data type.
	 * @param target Target for the entry.
	 * @param entry Value for the copy number entry.
	 */
	void WriteEntry(MultiDataType dataType, int target, const affymetrix_calvin_data::DmetCopyNumberData &entry);

	/*! Write a DMET multi-allelic entry to buffer. If the buffer is full, flush it.
   * @param dataType The data type.
	 * @param target Target for the entry.
	 * @param entry Value for the copy number entry.
	 */
	void WriteEntry(MultiDataType dataType, int target, const affymetrix_calvin_data::DmetMultiAllelicData &entry);

	/*! Write a DMET copy number entry to buffer. If the buffer is full, flush it.
     * @param dataType The data type.
	 * @param target Target for the entry.
	 * @param entry Value for the copy number entry.
	 */
	void WriteEntry(MultiDataType dataType, int target, const affymetrix_calvin_data::DmetBiAllelicData &entry);
    
	/*! Write an entry.
	 * @param entry A pointer to a multi data entry.
	 */
    void WriteEntry(MultiDataType dataType, int target, const affymetrix_calvin_data::ChromosomeMultiDataSummaryData & entry);

	/*! Write an entry.
	 * @param entry A pointer to a multi data entry.
	 */
	void WriteEntry(MultiDataType dataType, int target, const affymetrix_calvin_data::ChromosomeSegmentData & entry);

	/*! Write an entry.
	 * @param entry A pointer to a multi data entry.
	 */
	void WriteEntry(MultiDataType dataType, int target, const affymetrix_calvin_data::ChromosomeSegmentDataEx & entry);

	/*! Write an entry.
	 * @param entry A pointer to a multi data entry.
	 */
    void WriteEntry(MultiDataType dataType, int target, const affymetrix_calvin_data::FamilialSegmentOverlap & entry);

	/*! Write an entry.
	 * @param entry A pointer to a multi data entry.
	 */
	void WriteEntry(MultiDataType dataType, int target, const affymetrix_calvin_data::FamilialSample & entry);

	/*! Write an entry.
	 * @param entry A pointer to a multi data entry.
	 */
	void WriteEntry(MultiDataType dataType, int target, const affymetrix_calvin_data::AllelePeaks &entry);

	/*! Write an entry.
	 * @param entry A pointer to a multi data entry.
	 */
	void WriteEntry(MultiDataType dataType, int target, const affymetrix_calvin_data::MarkerABSignals &entry);

	/*! Write an entry.
	 * @param entry A pointer to a multi data entry.
	 */
	void WriteEntry(MultiDataType dataType, int target, const affymetrix_calvin_data::CytoGenotypeCallData &entry);

	/*! Write the content of the buffer to CHP files. */
	void FlushBuffer();

private:

	/*! Initialize the buffer writer
	 * @param CHPFileNames Reference to a list of CHP file names.
     * @param dataTypes The data types.
     * @param maxProbeSetNmLn The maximum probe set name length
     * @param maxSegmentTypeLn The maximum segment type length
     * @param maxReferenceSegmentIDLn The maximum ref sample key length
     * @param maxFamilialSegmentIDLn The maximum familial seg id length
     * @param maxFamilialARRIDLn The maximum familial arr id length
     * @param maxFamilialCHPIDLn The maximum familial sample chp id length
     * @param maxFamilialCHPFilenameLn The maximum familial sample chp file length
     * @param maxFamilialRoleLn The maximum familial sample role length
     */
    void Initialize(std::vector<std::string> *CHPFileNames, std::vector<MultiDataType> &dataTypes,
      std::map<MultiDataType, int> &maxProbeSetNmLn,
      std::map<MultiDataType, int> &maxSegmentTypeLn,
      std::map<MultiDataType, int> &maxReferenceSegmentIDLn,
      std::map<MultiDataType, int> &maxFamilialSegmentIDLn,	  
	  std::map<MultiDataType, int> &maxFamilialARRIDLn,
	  std::map<MultiDataType, int> &maxFamilialCHPIDLn,
	  std::map<MultiDataType, int> &maxFamilialCHPFilenameLn,
	  std::map<MultiDataType, int> &maxFamilialRoleLn);

	/*! Pointer to list of CHP file names. */
	std::vector<std::string> *chpFileNames;

	/*! List of targets used for storing entries. */
    std::map<MultiDataType, std::vector< std::vector<char *> > > dataBuffers;

	/*! Buffer for storing row indexes. */
	std::map<MultiDataType, std::vector<int> > rowIndexes;

	/*! Size of the current buffer in bytes. */
	int m_BufferSize;

	/*! Maximum size of the buffer before it gets flushed */
	int m_MaxBufferSize;

	/*! The maximum probe set name length */
	std::map<MultiDataType, int> maxProbeSetNameLength;

	/*! The maximum segment type length */
	std::map<MultiDataType, int> maxSegmentType;

	/*! The maximum ref sample key length */
	std::map<MultiDataType, int> maxReferenceSegmentID;

	/*! The maximum familial seg id length */
	std::map<MultiDataType, int> maxFamilialSegmentID;

	/*! The maximum familial sample arr id length */
	std::map<MultiDataType, int> maxFamilialARRID;

	/*! The maximum familial sample chp id length */
	std::map<MultiDataType, int> maxFamilialCHPID;

	/*! The maximum familial sample chp file length */
	std::map<MultiDataType, int> maxFamilialCHPFilename;

	/*! The maximum familial sample role length */
	std::map<MultiDataType, int> maxFamilialRole;

    /*! The size of the buffer for each data type. */
    std::map<MultiDataType, int> dataBufferSz;
};

}

#endif // _CalvinCHPMultiDataFileBufferWriter_HEADER_
