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

#if !defined(AFX_BPMAPFILEDATA_H__601D7E2E_5897_4CAA_ABF0_D6002698CB78__INCLUDED_)
#define AFX_BPMAPFILEDATA_H__601D7E2E_5897_4CAA_ABF0_D6002698CB78__INCLUDED_

/*! \file BPMAPFileData.h This file provides BPMAP file reading capabilities.
 */

//////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning(disable: 4786) // identifier was truncated in the debug information
#include <windows.h>
#endif

//////////////////////////////////////////////////////////////////////

#include "file/TagValuePair.h"
//
#include <cstring>
#include <list>
#include <string>
#include <vector>
//

//////////////////////////////////////////////////////////////////////

#define BPMAP_FILE_HEADER_BYTES "PHT7\r\n\032\n"
#define BPMAP_FILE_HEADER_LEN 8

#define PM_ONLY 1
#define PM_MM 0

//////////////////////////////////////////////////////////////////////
namespace affxbpmap
{

////////////////////////////////////////////////////////////////////

/*! The number of bytes used to store the probe sequence in the BPMAP file. */
#define PROBE_STORAGE_BUFFER_LENGTH 7

/*! This class provides storage for the array information for a hit (probe or probe pair interrogating a genomic position */
typedef struct _GDACSequenceHitItemType
{
	/*! The X coordinate of the PM probe. */
	unsigned int	PMX;

	/*! The Y coordinate of the PM probe. */
	unsigned int	PMY;

	/*! The X coordinate of the MM probe. */
	unsigned int	MMX;

	/*! The Y coordinate of the MM probe. */
	unsigned int	MMY;

	/*! The match score for the PM probe. */
	float			MatchScore;

	/*! The genomic position the probe is interrogating. */
	unsigned int	Position;

	/*! The PM probe sequence. */
	std::string		PMProbe;

	/*! The length of the PM probe. */
	unsigned char	ProbeLength;

	/*! A flag indicating if the probe is designed to interrogate the top strand.
	 * A value of 1 indicates top, 0 indicates bottom
	 */
	unsigned char	TopStrand;

	/*! Comparison operator used by sort */
	bool operator<(const _GDACSequenceHitItemType &rhs) const;

	/*! Packed PM probe sequence */
	char PackedPMProbe[PROBE_STORAGE_BUFFER_LENGTH+1];

	/*! Returns the genomic position relative to the center of the probe.
	 * @return The genomic position.
	 */
	unsigned int getCenterPosition() { return Position + (ProbeLength-1)/2; }

	/*! Returns the genomic position relative to the start of the probe.
	 * @return The genomic position.
	 */
	unsigned int getStartPosition() { return Position; }

} GDACSequenceHitItemType;

/*! The size of the hit item in the BPMAP file when both PM and MM are present. */
#define HIT_ITEM_SIZE_WITH_PROBE_PAIRS (4+4+4+4+1+PROBE_STORAGE_BUFFER_LENGTH+4+4+1)

/*! The size of the hit item in the BPMAP file when only PM are present. */
#define HIT_ITEM_SIZE_WITH_PM_ONLY (4+4+1+PROBE_STORAGE_BUFFER_LENGTH+4+4+1)

//////////////////////////////////////////////////////////////////////

/*! This class provides storage for the probes interrogating a sequence. */
class CGDACSequenceItem
{
public:
	/*! Constructor */
	CGDACSequenceItem();

	/*! Destructor */
	~CGDACSequenceItem();

protected:
	/*! The name of the sequence, e.g., chromosome. */
	std::string m_Name;

	/*! The group name for the sequence, e.g., species or organism. */
	std::string m_GroupName;

	/*! The version associated to the sequence, e.g., build version of the genomic database. */
	std::string m_SeqVersion;

	/*! The sequence id. */
	int m_Number;

	/*! Flag indicating if PM/MM pairs. */
	int m_ProbePairs;

	/* The number of probes or probe pairs (hits). */
	int m_NumberHits;

	/*! An array of hits. */
	std::vector<GDACSequenceHitItemType> m_Hits;

	/*! A pointer to the array of hits. This is used in memory mapping files. */
	std::vector<GDACSequenceHitItemType> *m_pHits;

	/*! The parameter name/value array. */
	TagValuePairTypeVector m_Parameters;

	/*! Flag indicating if the file is memory mapped. */
	bool m_bMapped;

	/*! Pointer to the memory mapped file. */
	char *m_lpData;

	/*! The file position of the start of the data. */
	int m_HitStartPosition;

	/*! Makes a copy of the object. The results are not copied to the new object, just
	 * the pointer back to the originating object.
	 * @param orig The object to copy.
	 */
	void MakeShallowCopy(CGDACSequenceItem &orig);

public:
	/*! Gets the sequence name.
	 * @return The sequence name.
	 */
	std::string GetName() const { return m_Name; }

	/*! Gets the group name.
	 * @return The group name.
	 */
	std::string GroupName() const { return m_GroupName; }

	/*! Returns the full name (name, group, version).
	 * @return The full name.
	 */
	std::string FullName() const;

	/*! Gets the version number.
	 * @return The version number.
	 */
	std::string GetSeqVersion() const { return m_SeqVersion; }

	/*! Gets the probe mapping type in the sequence
	 * @return 0 if PM-MM probe pairs, 1 if PM-Only probes.
	 */
	int GetProbeMapping () const { return m_ProbePairs; }

	/*! Gets the sequence number.
	 * @return The sequence number.
	 */
	int GetNumber() const { return m_Number; }

	/*! Gets the number of hits (probes or probe pairs).
	 * @return The number of hits (probes or probe pairs).
	 */
	int GetNumberHits() const { return m_NumberHits; }

	/*! Gets the number of tag/value parameters.
	 * @return The number of tag/value parameters.
	 */
	int GetNumberParameters() const { return (int)m_Parameters.size(); }

	/*! Gets the parameter at the given index.
	 * @param index The index to the parameter array.
	 * @return The parameter.
	 */
	TagValuePairType GetParameter(int index);

	/*! Gets the hit data.
	 * @param index The index to the hit of interest.
	 * @param hit The hit (probe/probe pair) information.
	 * @param readProbe A flag indicating if the probe sequence should be copied to the hit information.
	 */
	void GetHitItem(int index, GDACSequenceHitItemType &hit, bool readProbe=false);

	/*! Friend to the top level class. */
	friend class CBPMAPFileData;
};

//////////////////////////////////////////////////////////////////////

/*! This class provides storage and reading capabilities for the BPMAP file. */
class CBPMAPFileData
{
public:
	/*! Constructor */
	CBPMAPFileData();

	/*! Destructor */
	~CBPMAPFileData();

protected:
	/*! The full path to the BPMAP file. */
	std::string m_FileName;

	/*! The number of sequences stored in the BPMAP file. */
	int	 m_NumberSequences;

	/*! The version number of the BPMAP file. */
	float m_Version;

	/*! The file position of the start of the data. */
	int m_DataStartPosition;

	/*! A string to hold error messages. */
	std::string m_strError;

	/*! An array of sequence items - used when not  memory mapping. */
	std::vector<CGDACSequenceItem> m_SequenceItems;

	/*! Reads the file.
	 * @param bReadHeaderOnly Flag to indicate if the header should only be read.
	 * @return True if successful.
	 */
	bool ReadFile(bool bReadHeaderOnly=false);

	/*! Reads the file header
	 * @return True if successful.
	 */
	bool ReadHeaderSection();

	/*! Reads the file data (after the header).
	 * @return True if successful.
	 */
	bool ReadDataSection();

	/*! Pointer to a memory mapped file. */
	void  *m_lpFileMap;

	/*! Pointer to the data in the memory mapped file. */
	char  *m_lpData;
#ifdef _MSC_VER

	/*! Windows handle to the file. */
	HANDLE m_hFileMap;

	/*! Windows handle to the file. */
	HANDLE m_hFile;
#else
	/*! File pointer for memory mapping. */
	FILE *m_fp;

	/*! The size of the file. */
	int m_MapLen;
#endif

	/*! Flag indicating if the file is open. */
	bool m_bFileOpen;

	/*! Flag indicating if the file is memory mapped. */
	bool m_bFileMapped;

public:
	/*! Sets the full path to the BPMAP file.
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

	/*! Reads the header of the file.
	 * @return True if successful.
	 */
	bool ReadHeader();

	/*! Checks if the file exists.
	 * @return True if exists.
	 */
	bool Exists();

	/*! Closes and deallocate memory. */
	void Close();

	/*! Gets an error string for file read errors.
	 * @return An error string for file read errors.
	 */
	std::string GetError() const { return m_strError; }

	/*! Gets the number of sequences stored in the BPMAP file.
	 * @return The number of sequences stored in the BPMAP file.
	 */
	int GetNumberSequences() const { return m_NumberSequences; }

	/*! The BPMAP file version number.
	 * @return The BPMAP file version number.
	 */
	float GetVersion() const { return m_Version; }

	/*! Gets the sequence data.
	 * @param index The index to the sequence of interest.
	 * @param seq The information about a sequence.
	 */
	void GetSequenceItem(int index, CGDACSequenceItem &seq);
};

////////////////////////////////////////////////////////////////////

} // namespace

//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_BPMAPFILEDATA_H__601D7E2E_5897_4CAA_ABF0_D6002698CB78__INCLUDED_)
