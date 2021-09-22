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

#if !defined(AFX_CDFFILEDATA_H__BDEC0B48_58F6_480C_A988_05355DCF0BA4__INCLUDED_)
#define AFX_CDFFILEDATA_H__BDEC0B48_58F6_480C_A988_05355DCF0BA4__INCLUDED_


/*! \file CDFFileData.h This file provides CDF file reading and storage capabilities.
 */

//////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning(disable: 4786) // identifier was truncated in the debug information
#include <windows.h>
#endif

//////////////////////////////////////////////////////////////////////

//
#include "portability/affy-base-types.h"
//
#include <cstring>
#include <fstream>
#include <istream>
#include <string>
#include <vector>
//

//////////////////////////////////////////////////////////////////////

namespace affxcdf
{

//////////////////////////////////////////////////////////////////////

/*! The maximum storage length for a probe set name in the CDF file. */
#define MAX_PROBE_SET_NAME_LENGTH 64

/*! The storage length for integrity md5 (hex) in the CDF file. */
#define INTEGRITY_MD5_LENGTH 32

////////////////////////////////////////////////////////////////////

/*! Storage for the header in a CDF file. */
class CCDFFileHeader
{
protected:
    /*! The magic number in an XDA file */
    int m_Magic;

    /*! The version number */
    int m_Version;

	/*! The GUID */
	std::string m_GUID;

	/*! The integrity md5 */
	std::string m_IntegrityMd5;

	/*! The chip type - array type name without version (only apply to XDA version 4 or above */
	std::string m_ChipType;

	/*! The array of chip types - all array type names with and without versions (only apply to XDA version 4 or above */
	std::vector<std::string> m_ChipTypes;

    /*! The number of probe sets defined in the file */
    int m_NumProbeSets;

    /*! The number of QC probe sets in the file */
    int m_NumQCProbeSets;

    /*! The reference sequence (used for resequencing arrays only) */
    std::string m_Reference;

    /*! The number of feature columns in the array */
    unsigned short m_Cols;

    /*! The number of feature rows in the array */
    unsigned short m_Rows;

    /*! Friend to the parent class */
    friend class CCDFFileData;

public:
	/*! Get CDF Format Version
	 * @return CDF Format Version
	 */
	int GetFormatVersion() const { return m_Version; }

	/*! Get GUID
	 * @return GUID
	 */
	std::string GetGUID() const { return m_GUID; }

	/*! Get integrity md5
	 * @return The integrity md5
	 */
	std::string GetIntegrityMd5() const { return m_IntegrityMd5; }

	/*! Get chip type - array type name without version
	 * @return The chip type.
	 */
	std::string GetChipType() const { return m_ChipType; }

	/*! Get chip types - array type names with and without version
	 * @return The chip types.
	 */
	std::vector<std::string> GetChipTypes() const { return m_ChipTypes; }

	/*! Gets the number of feature columns in the array.
     * @return The number of columns.
     */
    int GetCols() const { return m_Cols; }

    /*! Gets the number of feature rows in the array.
     * @return The number of rows.
     */
    int GetRows() const { return m_Rows; }

    /*! Gets the number of probe sets.
     * @return The number of probe sets.
     */
    int GetNumProbeSets() const { return m_NumProbeSets; }

    /*! Gets the number of QC probe sets.
     * @return The number of QC probe sets.
     */
    int GetNumQCProbeSets() const { return m_NumQCProbeSets; }

    /*! Gets the reference sequence (for resequencing arrays only).
     * @return The reference sequence.
     */
    std::string &GetReference() { return m_Reference; }

    /*! Constructor */
    CCDFFileHeader();
};

////////////////////////////////////////////////////////////////////

/*! Defines the type of probe sets stored in the CDF file */
enum GeneChipProbeSetType
{
    /*! Unknown probe set */
    UnknownProbeSetType,

    /*! Expression probe set */
    ExpressionProbeSetType,

    /*! Genotyping probe set */
    GenotypingProbeSetType,

    /*! Resequencing probe set */
    ResequencingProbeSetType,

    /*! Tag probe set */
    TagProbeSetType,

    /*! Copy number probe set. */
    CopyNumberProbeSetType,

    /*! Genotype control probe set. */
    GenotypeControlProbeSetType,

    /*! Expression control probe set. */
    ExpressionControlProbeSetType,

    /*! Polymorphic marker probe set. */
    MarkerProbeSetType,

    /*! Multichannel marker probe set. */
    MultichannelMarkerProbeSetType
};

////////////////////////////////////////////////////////////////////

/*! Defines the direction of a probe set or group */
enum DirectionType
{
    /*! No direction specified */
    NoDirection,

    /*! Sense
     * All probes in the probe set (or probe group) have the sense direction
     */
    SenseDirection,

    /*! Anti sense
     * All probes in the probe set (or probe group) have the antisense direction
     */
    AntiSenseDirection,

    /* !Either
     * Some probes in the probe set (or probe group) have antisense direction,
     * others have sense direction
     */
    EitherDirection
};

////////////////////////////////////////////////////////////////////

/*! Defines the type of probe replication of a group   */
enum ReplicationType
{
    /*! Unspecified replication type */
    UnknownRepType,

    /*! Different
     * All probes in the probe group have different sequences
     */
    DifferentRepType,

    /*! Mixed
     * Some probes in the probe group have identical sequences
     */
    MixedRepType,

    /* Identical
     * All probes in the probe group have identical sequences
     */
    IdenticalRepType
};

////////////////////////////////////////////////////////////////////

/*! This class provides storage for an individual probe in a CDF file */
class CCDFProbeInformation
{
protected:
    /*! The index of the probes probe pair or quartet (probe list). Also known as the atom position. */
    int m_ListIndex;

    /*! The expos value in the CDF file, this can either be a zero based index equal to the
     * list index or the exon position. */
    int m_Expos;

    /*! The X coordinate */
    unsigned short m_X;

    /*! The Y coordinate */
    unsigned short m_Y;

    /*! The probes base at the interrogation position */
    char m_PBase;

    /*! The targets base at the interrogation position */
    char m_TBase;

    /*! The probe length */
    unsigned short m_ProbeLength;

    /*! The probe grouping */
    unsigned short m_ProbeGrouping;

    /*! Friend to the parent */
    friend class CCDFProbeGroupInformation;

    /*! Friend to the probe set */
    friend class CCDFProbeSetInformation;

    /*! Friend to the top level class */
    friend class CCDFFileData;

public:
    /*! Returns the list index.
     * @return The list index.
     * This is an internal index used by Affymetrix chip design.
     */
    int GetListIndex() const { return m_ListIndex; }

    /*! Returns the expos value.
     * @return The expos value.
     * This is the position of the probe sequence in a longer sequence.  (Note
     * in some arrays this value may instead correspond to the GetListIndex value).
     * In expression arrays the longer sequence can be the sequence from which
     * the probe is selected.  In genotyping arrays the longer sequence can be
     * the sequence containing a polymorphic SNP.  More than one probe can have
     * the same expos value, e.g., in arrays containing perfect match/mismatch
     * pairs, both probes in the pair will have the same expos value.
     */
    int GetExpos() const { return m_Expos; }

    /*! Returns the X coordinate of the probe in the physical array.
     * @return The X coordinate.
     */
    int GetX() const { return m_X; }

    /*! Returns the Y coordinate of the probe in the physical array.
     * @return The Y coordinate.
     */
    int GetY() const { return m_Y; }

    /*! Returns the probes base at the interrogation position.
     * @return The probes base at the interrogation position.
     */
    char GetPBase() const { return m_PBase; }

    /*! Returns the targets base at the interrogation position.
     * @return The targets base at the interrogation position.
     */
    char GetTBase() const { return m_TBase; }

    /*! Returns the length of the probe.
     * @return The probe length.
     * This is not applicable to XDA CDF file version 1.
     */
    unsigned short GetProbeLength() const { return m_ProbeLength; }

    /*! Returns the grouping of the probe in the physical array.
     * @return The probe grouping.
     * This is not applicable to XDA CDF file version 1.
     */
    unsigned short GetProbeGrouping() const { return m_ProbeGrouping; }

    /*! Constructor */
    CCDFProbeInformation();
};

/*! The size of the probe object as stored in the XDA CDF file version 1. */
#define PROBE_SIZE (4+4+2+2+1+1)

/*! The size of the probe object as stored in the XDA CDF file version 2, 3 and 4. */
#define PROBE_SIZE_V2 (4+4+2+2+1+1+2+2)

////////////////////////////////////////////////////////////////////

/*! This class provides storage for a group of probes, also known as a block. */
class CCDFProbeGroupInformation
{
protected:
    /*! The number of probe pairs or probe quartets in the group. */
    int m_NumLists;

    /*! The number of probes in the group. */
    int m_NumCells;

    /*! The first probes list index value. */
    int m_Start;

    /*! The last probes list index value. */
    int m_Stop;

    /*! The probe set index associated with the group. */
    int m_ProbeSetIndex;

    /*! The group index. */
    int m_GroupIndex;

    /*! The name of the group. */
    std::string m_Name;

    /*! The wobble situation. */
    unsigned short m_WobbleSituation;

    /*! The allele code. */
    unsigned short m_AlleleCode;

    /*! The channel. */
    unsigned char m_Channel;

    /*! The probe replication type. */
    unsigned char m_RepType;

    /*! The number of cells per list (1 or 2 for expression and genotyping, 4 for resequencing). */
    unsigned char m_NumCellsPerList;

    /*! The direction of the target that the probes are interrogating. */
    unsigned char m_Direction;

    /*! The probes in the group */
    std::vector<CCDFProbeInformation> m_Cells;

    /*! A pointer to the probes. This is used when memory mapping is used. */
    std::vector<CCDFProbeInformation> *m_pCells;

    /*! Friend to the parent class. */
    friend class CCDFProbeSetInformation;

    /*! Friend to the top level class. */
    friend class CCDFFileData;

    /*! Copies the data in the input object to the member variables.
     * @param orig The group to copy.
     */
    void MakeShallowCopy(CCDFProbeGroupInformation &orig);

public:
    /*! Gets the groups direction.
     * @return The groups direction.
     * See the DirectionType enumeration for details
     */
    DirectionType GetDirection() const { return (DirectionType)m_Direction; }

    /*! Gets the number of lists (atoms) in the group.
     * @return The number of lists (atoms) in the group.
     * In expression and genotyping arrays probes may be arranged in singletons or paired
     * with a mismatch probe. In resequencing arrays probes may be arranged in quartes.
     * Note that multiple arrangements may be present on the same
     * array, but not within a probe group (or probe set).
     * Each such singleton or pair or quartet is a "list" or "atom."
     */
    int GetNumLists() const { return m_NumLists; }

    /*! Gets the number of probes in the group.
     * @return The number of probes in the group.
     */
    int GetNumCells() const { return m_NumCells; }

    /*! Gets the number of probes per list.
     * @return The number of probes per list.
     * Always constant within a probe group or probe set.
     */
    int GetNumCellsPerList() const { return (int)m_NumCellsPerList; }

    /*! Gets the start list index value.
     * @return The start list index value.
     * This is the minimum value returned by GetListIndex()
     * This is an internal index used by Affymetrix chip design.
     */
    int GetStart() const { return m_Start; }

    /*! Gets the stop list index value.
     * @return The stop list index value.
     * This is the maximum value returned by GetListIndex()
     * This is an internal index used by Affymetrix chip design.
     */
    int GetStop() const { return m_Stop; }

    /*! Gets the group name.
     * @return The group name.
     */
    std::string GetName() { return m_Name; }

    /*! Gets the wobble situation.
     * @return The wobble situation.
     * This is not applicable to XDA CDF file version 1.
     */
    unsigned short GetWobbleSituation() const { return m_WobbleSituation; }

    /*! Gets the allele code.
     * @return The allele code.
     * This is not applicable to XDA CDF file version 1.
     */
    unsigned short GetAlleleCode() const { return m_AlleleCode; }

    /*! Gets the channel.
     * @return The channel.
     * This is not applicable to XDA CDF file version 1 and 2.
     */
    unsigned char GetChannel() const { return m_Channel; }

    /*! Gets the probe replication type.
     * @return The probe replication type.
     * See the ReplicationType enumeration for details
     * This is not applicable to XDA CDF file version 1 and 2.
     */
    ReplicationType GetRepType() const { return (ReplicationType) m_RepType; }

    /*! Retrieves the probe object given a zero-based index.
     * @param cell_index zero-based index in the probe group to the probe of interest.
     * @param info The returned probe data.
     * Note: this has nothing to do with list index returned by GetListIndex
     */
    void GetCell(int cell_index, CCDFProbeInformation & info);

    /*! Constructor */
    CCDFProbeGroupInformation();

    /*! Destructor */
    ~CCDFProbeGroupInformation();
};

/*! This is the size of the object in a binary CDF file version 1. */
#define PROBE_GROUP_SIZE (MAX_PROBE_SET_NAME_LENGTH+4+4+4+4+1+1)

/*! This is the size of the object in a binary CDF file version 2. */
#define PROBE_GROUP_SIZE_V2 (MAX_PROBE_SET_NAME_LENGTH+4+4+4+4+1+1+2+2)

/*! This is the size of the object in a binary CDF file version 3 and 4. */
#define PROBE_GROUP_SIZE_V3 (MAX_PROBE_SET_NAME_LENGTH+4+4+4+4+1+1+2+2+1+1)

////////////////////////////////////////////////////////////////////

/*! This class provides storage for a probe set. */
class CCDFProbeSetInformation
{
protected:
    /*! The number of lists (atoms) in the probe set. */
    int m_NumLists;

    /*! The number of groups (blocks) in the probe set. */
    int m_NumGroups;

    /*! The number of probes in the set. */
    int m_NumCells;

    /*! An index for the probe set.*/
    int m_Index;

    /*! An arbitrary number assigned to the probe set. */
    int m_ProbeSetNumber;

    /*! The type of probe set. */
    unsigned short m_ProbeSetType;

    /*! The direction of the target that the probes are interrogating. */
    unsigned char m_Direction;

    /*! The number of probes per list. */
    unsigned char m_NumCellsPerList;

    /*! The groups in the set. */
    std::vector<CCDFProbeGroupInformation> m_Groups;

    /*! A pointer to the groups. This is used when memory mapping is used. */
    std::vector<CCDFProbeGroupInformation> *m_pGroups;

    /*! Friend to the top level class. */
    friend class CCDFFileData;

    /*! Copies the data in the input object to the member variables.
     * @param orig The probe set to copy.
     */
    void MakeShallowCopy(CCDFProbeSetInformation &orig);

public:
    /*! Gets the probe set type.
     * @return The probe set type.
     * see GeneChipProbeSetType for details
     */
    GeneChipProbeSetType GetProbeSetType() const { return (GeneChipProbeSetType)m_ProbeSetType; }

    /*! Gets the probe sets direction.
     * @return The probe sets direction.
     * See the DirectionType enumeration for details
     */
    DirectionType GetDirection() const { return (DirectionType)m_Direction; }

    /*! Gets the number of lists (atoms) in the set.
     * @return The number of lists (atoms) in the set.
     * See GetNumLists for probe group for details
     */
    int GetNumLists() const { return m_NumLists; }

    /*! The number of groups in the set.
     * @return The number of groups in the set.
     * In genotyping arrays it is convenient to group organize probes that interrogate
     * a polymorphic SNP into subsets that interrogate a particular allele and have
     * the same direction (i.e., sense or antisense).  Each grouping is a "probe group."
     * Affymetrix also uses the term "block" for a "probe group."
     */
    int GetNumGroups() const { return m_NumGroups; }

    /*! The number of probes in the set.
     * @return The number of probes in the set.
     */
    int GetNumCells() const { return m_NumCells; }

    /*! Gets the number of probes per list.
     * @return The number of probes per list.
     * Always constant within a probe group or probe set.
     */
    int GetNumCellsPerList() const { return (int)m_NumCellsPerList; }

    /*! Gets the probe set number.
     * @return The probe set number.
     * This is an internal value used by Affymetrix chip design.
     */
    int GetProbeSetNumber() const { return m_ProbeSetNumber; }

    /*! Gets a group object.
     * @param index The index to the group of interest.
     * @param info The returned group data.
     */
    void GetGroupInformation(int index, CCDFProbeGroupInformation & info);

    /*! Constructor */
    CCDFProbeSetInformation();

    /*! Destructor */
    ~CCDFProbeSetInformation();
};

/*! This is the size of the object in a binary CDF file. */
#define PROBE_SET_SIZE (4+4+4+4+2+1+1)

////////////////////////////////////////////////////////////////////

/*! This class provides storage for the list of probe set names. */
class CCDFProbeSetNames
{
public:
    /*! Friend to the top level class. */
    friend class CCDFFileData;

    /*! Constructor */
    CCDFProbeSetNames();

    /*! Destructor */
    ~CCDFProbeSetNames();

    /*! Deallocates memory used by the class. */
    void Clear();

protected:

    /*! Array of probe set names, used if not memory mapping. */
    std::vector<std::string> m_ProbeSetNames;

    /*! Resizes the probe set name array. */
    void Resize(int size);

    /*! Stores the name of the probe set in the internal array. */
    void SetName(int index, std::string name);

public:
    /*! Gets the probe set name.
     * @param index The zero-based index to the probe set name of interest.
     * @return The name of the probe set.
     */
    std::string GetName(int index) const;
};

////////////////////////////////////////////////////////////////////

/*! This class provides storage for QC probes */
class CCDFQCProbeInformation
{
public:
    /*! Constructor */
    CCDFQCProbeInformation();

protected:
    /*! The X coordinate of the probe */
    unsigned short m_X;

    /*! The Y coordinate of the probe */
    unsigned short m_Y;

    /*! The probe length. This value may be 1 for non-synthesized features */
    unsigned char m_PLen;

    /*! Flag indicating if the probe is a perfect match probe. */
    unsigned char m_PMProbe;

    /*! Flag indicating if the probe is used for background calculations (blank feature). */
    unsigned char m_Background;

    /*! Friend to the parent class */
    friend class CCDFQCProbeSetInformation;

    /*! Friend to the top level CDF class */
    friend class CCDFFileData;

public:
    /*! Gets the X coordinate of the probe.
     * @return The X coordinate.
     */
    int GetX() const { return m_X; }

    /*! Gets the Y cooridnate of the probe.
     * @return The Y coordinate.
     */
    int GetY() const { return m_Y; }

    /*! Gets the probe length.
     * @return The probe length. This value may be 1 for non-synthesized features.
     */
    int GetPLen() const { return m_PLen; }

    /*! Gets the flag indicating if the probe is a perfect match probe.
     * @return The flag indicating if the probe is a perfect match probe
     */
    bool IsPerfectMatchProbe() const { return (m_PMProbe == 1 ? true : false); }

    /*! Gets a flag indicating if the probe is used for background calculations (blank feature).
     * @return Flag indicating if the probe is used for background calculations (blank feature).
     */
    bool IsBackgroundProbe() const { return (m_Background == 1 ? true : false); }
};

/*! This is the size of the object in a binary CDF file. */
#define QC_PROBE_SIZE (2+2+1+1+1)

////////////////////////////////////////////////////////////////////

/*! The type of QC probe set */
enum GeneChipQCProbeSetType
{
    /*! Unknown probe set */
    UnknownQCProbeSetType,

    /*! Probes used for the checker board patterns for antisense arrays. */
    CheckerboardNegativeQCProbeSetType,

    /*! Probes used for the checker board patterns for sense arrays. */
    CheckerboardPositiveQCProbeSetType,

    /*! Hybridization control probes for antisense arrays. */
    HybNegativeQCProbeSetType,

    /*! Hybridization control probes for sense arrays. */
    HybPositiveQCProbeSetType,

    /*! Probes used for text patterns for antisense arrays. */
    TextFeaturesNegativeQCProbeSetType,

    /*! Probes used for text patterns for sense arrays. */
    TextFeaturesPositiveQCProbeSetType,

    /*! Central probes for antisense arrays. */
    CentralNegativeQCProbeSetType,

    /*! Central probes for sense arrays. */
    CentralPositiveQCProbeSetType,

    /*! Gene expression control probes for antisense arrays. */
    GeneExpNegativeQCProbeSetType,

    /*! Gene expression control probes for sense arrays. */
    GeneExpPositiveQCProbeSetType,

    /*! Cycle fidelity probes for antisense arrays. */
    CycleFidelityNegativeQCProbeSetType,

    /*! Cycle fidelity probes for sense arrays. */
    CycleFidelityPositiveQCProbeSetType,

    /*! Central cross probes for antisense arrays. */
    CentralCrossNegativeQCProbeSetType,

    /*! Central cross probes for sense arrays. */
    CentralCrossPositiveQCProbeSetType,

    /*! X-hyb control probes for antisense arrays. */
    CrossHybNegativeQCProbeSetType,

    /*! X-hyb control probes for sense arrays. */
    CrossHybPositiveQCProbeSetType,

    /*! Space normalization probes for antisense arrays. */
    SpatialNormalizationNegativeQCProbeSetType,

    /*! Space normalization probes for sense arrays. */
    SpatialNormalizationPositiveQCProbeSetType
};

////////////////////////////////////////////////////////////////////

/*! This class provides storage for the probes in a QC probe set. */
class CCDFQCProbeSetInformation
{
protected:
    /*! The number of probes in the set. */
    int m_NumCells;

    /*! The type of QC probes. */
    unsigned short m_QCProbeSetType;

    /*! The array of probes. */
    std::vector<CCDFQCProbeInformation> m_Cells;

    /*! The array of probes. */
    std::vector<CCDFQCProbeInformation> *m_pCells;

    /*! Friend to the top level CDF class. */
    friend class CCDFFileData;

    /*! Copies the data in the input object to the member variables.
     * @param orig The probe set to copy.
     */
    void MakeShallowCopy(CCDFQCProbeSetInformation &orig);

public:
    /*! Gets the probe set type.
     * @return The probe set type.
     */
    GeneChipQCProbeSetType GetQCProbeSetType() const { return (GeneChipQCProbeSetType)m_QCProbeSetType; }

    /*! Gets the number of probes in the set.
     * @return The number of probes in the set.
     */
    int GetNumCells() const { return m_NumCells; }

    /*! Gets the information about a single probe in the set.
     * @param index The zero-based index to the probe of interest.
     * @param info The information about the probe.
     */
    void GetProbeInformation(int index, CCDFQCProbeInformation & info);

    /*! Constructor */
    CCDFQCProbeSetInformation();

    /*! Destructor */
    ~CCDFQCProbeSetInformation();
};

/*! This is the size of the QC probe set object in a binary CDF file. */
#define QC_PROBE_SET_SIZE (4+2)

////////////////////////////////////////////////////////////////////

/*! This class provides storage and reading capabilities for a CDF file. */
class CCDFFileData
{
protected:
    /*! The position of the probe set names in the XDA file. */
    std::ios::pos_type probeSetNamePos;

    /*! The position of the QC probe set index array in the XDA file. */
    std::ios::pos_type qcSetIndexPos;

    /*! The position of the probe set index array in the XDA file. */
    std::ios::pos_type probeSetIndexPos;

    /*! What was the last probesetindex read?
     *  This state allows us to handle sequential reads quickly. (no seeking.)
     */
    uint32_t m_probeSetIndex_last;

    /*! 1 if like m_probeSetIndex_last is valid.
     *  The seekg method clears this.
     */
    uint32_t m_probeSetIndex_last_valid;

    /*! Use this seekg method when seeking with the iteratorReader.
     *  It updates the state needed for sequenial reads.
     */
    void seekg(uint32_t pos, const std::ios_base::seekdir& dir)
    {
        // invalidate the last probe state
        m_probeSetIndex_last_valid=0;
        // m_probeSetIndex_last=0; // we dont need to clear this as we just cleared "_valid"
        // and do the seek
        iteratorReader.seekg(pos,dir);
    }

    /*! The file stream for the probe set information iterator. */
    std::ifstream iteratorReader;

    /*! Flag to indicate that only the header part of the file is to be read. */
    bool readHeaderOnly;

    /*! The file header object. */
    CCDFFileHeader m_Header;

    /*! The list of probe set names. */
    CCDFProbeSetNames m_ProbeSetNames;

    /*! An array of probe sets. */
    std::vector<CCDFProbeSetInformation> m_ProbeSets;

    /*! An array of QC probe sets. */
    std::vector<CCDFQCProbeSetInformation> m_QCProbeSets;

    /*! The CDF file name (full path). */
    std::string m_FileName;

    /*! A string to hold an error message upon read failures. */
    std::string m_strError;

    /*! Opens the file for reading.
     * @return True if successful.
     */
    bool Open();

    /*! Reads a text format CDF file.
     * @return True if successful.
     */
    bool ReadTextFormat();

    /*! Reads an XDA format CDF file.
     * @return True if successful.
     */
    bool ReadXDAFormat();

    /*! Reads the header of an XDA format CDF file.
     * @return True if successful.
     */
    bool ReadXDAHeader();

public:
    /*! Sets the name of the file.
     * @param name The full path of the CDF file.
     */
    void SetFileName(const char *name) { m_FileName = name; }

    /*! Gets the name of the file.
     * @return The full path of the CDF file.
     */
    std::string GetFileName() const { return m_FileName; }

    /*! Gets the header object.
     * @return The CDF file header object.
     */
    CCDFFileHeader &GetHeader() { return m_Header; }

	/*! Get GUID
	 * @return GUID
	 */
	std::string GetGUID();

	/*! Get integrity md5
	 * @return The integrity md5
	 */
	std::string GetIntegrityMd5();

	/*! Gets the error string.
     * @return A string describing the last read error.
     */
    std::string GetError() const { return m_strError; }

    /*! Gets the name of a probe set.
     * @param index The zero-based index to the probe set name of interest.
     * @return The probe set name.
     */
    std::string GetProbeSetName(int index);

    /*! Gets the chip type (probe array type) of the CDF file.
     * @return The chip type. This is just the name (without extension) of the CDF file.
     */
    std::string GetChipType();

    /*! Gets the chip types (probe array type) of the CDF file. Allow substrings deliminated by '.'
     * @return vector of chip types
     */
    std::vector<std::string> GetChipTypes();

    /*! Reads the entire file.
     * @return True if successful.
     */
    bool Read();

    /*! Reads the header of the file only.
     * @return True if successful.
     */
    bool ReadHeader();

    /*! Checks if the file exists.
     * @return True if the file exists.
     */
    bool Exists();

    /*! Deallocates memory and closes any file handles. */
    void Close();

    /*! Determines if a CDF file is of the XDA (binary) format.
     * @return True if XDA format.
     */
    bool IsXDACompatibleFile();

    /*! Gets the probe set type for non-qc probe sets.
     * @param index The zero-based index to the probe set of interest.
     * @return The type of probe set.
     */
    GeneChipProbeSetType GetProbeSetType(int index);

    /*! Gets the probe set information.
     * @param index The zero-based index to the probe set of interest.
     * @param info The probe set information.
     * @return The probe set information.
     */
    void GetProbeSetInformation(int index, CCDFProbeSetInformation & info);

    /*! Gets the QC probe set information by index.
     * @param index The zero-based index to the QC probe set of interest.
     * @param info The QC probe set information.
     * @return The QC probe set information.
     */
    void GetQCProbeSetInformation(int index, CCDFQCProbeSetInformation & info);

    /*! Gets the QC probe set information by type.
     * @param qcType The type of QC probe set to retrieve.
     * @param info The QC probe set information.
     * @return The QC probe set information.
     */
    void GetQCProbeSetInformation(GeneChipQCProbeSetType qcType, CCDFQCProbeSetInformation & info);

    /*! Constructor */
    CCDFFileData();

    /*! Destructor */
    ~CCDFFileData();
};

////////////////////////////////////////////////////////////////////

} // namespace

////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_CDFFILEDATA_H__BDEC0B48_58F6_480C_A988_05355DCF0BA4__INCLUDED_)
