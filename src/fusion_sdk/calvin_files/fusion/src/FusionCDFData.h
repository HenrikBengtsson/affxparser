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


#ifndef _AffymetrixFusionCDFData_HEADER_
#define _AffymetrixFusionCDFData_HEADER_

/*! \file FusionCDFData.h This file defines the Fusion CDF Data classes
 */

#include "calvin_files/data/src/CDFData.h"
#include "calvin_files/data/src/CDFProbeGroupInformation.h"
#include "calvin_files/data/src/CDFProbeInformation.h"
#include "calvin_files/data/src/CDFQCProbeInformation.h"
#include "calvin_files/data/src/CDFQCProbeSetInformation.h"
//
#include "file/CDFFileData.h"
//

namespace affymetrix_fusion_io
{

/*! Storage for the header in a CDF file. */
class FusionCDFFileHeader
{
protected:
	/*! The GCOS CDF file header object. */
	affxcdf::CCDFFileHeader *gcosHeader;

	/*! The Calvin CDF file object. */
	affymetrix_calvin_io::CDFData *calvinData;

	/*! The reference sequence */
	std::string ref;

	/*! Initializes the class based on GCOS information.
	 * @param data The GCOS CDF file object.
	 */
	void Initialize(affxcdf::CCDFFileData *data);

	/*! Initializes the class based on Calvin information.
	 * @param data The Calvin CDF file object.
	 */
	void Initialize(affymetrix_calvin_io::CDFData *data);

	/*! Friend to the parent. */
	friend class FusionCDFData;

public:
	/*! Get CDF Format Version
	 * @return CDF Format Version
	 */
	int GetFormatVersion() const;

	/*! Get GUID
	 * @return GUID
	 */
	std::string GetGUID() const;

	/*! Get integrity md5
	 * @return integrity md5
	 */
	std::string GetIntegrityMd5() const;

	/*! Gets the number of feature columns in the array.
	 * @return The number of columns.
	 */
	int GetCols() const;

	/*! Gets the number of feature rows in the array.
	 * @return The number of rows.
	 */
	int GetRows() const;

	/*! Gets the number of probe sets.
	 * @return The number of probe sets.
	 */
	int GetNumProbeSets() const;

	/*! Gets the number of QC probe sets.
	 * @return The number of columns.
	 */
	int GetNumQCProbeSets() const;

	/*! Gets the reference sequence (for resequencing arrays only).
	 * @return The reference sequence.
	 */
	std::string &GetReference();

	/*! Constructor */
	FusionCDFFileHeader();
};

/*! This class provides storage for an individual probe in a CDF file */
class FusionCDFProbeInformation
{
private:
	/*! The GCOS probe object. */
	affxcdf::CCDFProbeInformation *gcosProbe;

	/*! The Calvin probe object. */
	affymetrix_calvin_io::CDFProbeInformation *calvinProbe;

	/*! Initializes the class for GCOS file reading.
	 * @param index The index to the probe.
	 * @param gcosGroup The GCOS probe group object.
	 */
	void Initialize(int index, affxcdf::CCDFProbeGroupInformation *gcosGroup);

	/*! Initializes the class for Calvin file reading.
	 * @param index The index to the probe.
	 * @param calvinGroup The Calvin probe group object.
	 */
	void Initialize(int index, affymetrix_calvin_io::CDFProbeGroupInformation *calvinGroup);

	/*! Deallocates any used memory. */
	void Clear();

	/*! Friend to the parent class. */
	friend class FusionCDFProbeGroupInformation;

public:
	/*! Returns the list index.
	 * @return The list index.
	 */
	int GetListIndex() const;

	/*! Returns the expos value.
	 * @return The expos value.
	 */
	int GetExpos() const;

	/*! Returns the X coordinate.
	 * @return The X coordinate.
	 */
	int GetX() const;

	/*! Returns the Y coordinate.
	 * @return The Y coordinate.
	 */
	int GetY() const;

	/*! Returns the probes base at the interrogation position.
	 * @return The probes base at the interrogation position.
	 */
	char GetPBase() const;

	/*! Returns the targets base at the interrogation position.
	 * @return The targets base at the interrogation position.
	 */
	char GetTBase() const;

	/*! Returns the probe length.
	 * @return The probe length.
	 */
	unsigned short GetProbeLength() const;

	/*! Returns the probe grouping.
	 * @return The probe grouping.
	 */
	unsigned short GetProbeGrouping() const;

	/*! Constructor */
	FusionCDFProbeInformation();

	/*! Destructor */
	~FusionCDFProbeInformation();
};

/*! This class provides storage for a group of probes, also known as a block. */
class FusionCDFProbeGroupInformation
{
private:
	/*! The GCOS probe group object. */
	affxcdf::CCDFProbeGroupInformation *gcosGroup;

	/*! The Calvin probe group object. */
	affymetrix_calvin_io::CDFProbeGroupInformation *calvinGroup;

	/*! Initializes the class for GCOS file reading.
	 * @param index The index to the probe group.
	 * @param gcosSet The GCOS probe set object.
	 */
	void Initialize(int index, affxcdf::CCDFProbeSetInformation *gcosSet);

	/*! Initializes the class for Calvin file reading.
	 * @param index The index to the probe group.
	 * @param calvinSet The Calvin probe set object.
	 */
	void Initialize(int index, affymetrix_calvin_io::CDFProbeSetInformation *calvinSet);

	/*! Deallocates any used memory. */
	void Clear();

	/*! Friend to the parent class. */
	friend class FusionCDFProbeSetInformation;

public:
	/*! Gets the groups direction.
	 * @return The groups direction.
	 */
	affxcdf::DirectionType GetDirection() const;

	/*! Gets the number of lists (atoms) in the group.
	 * @return The number of lists (atoms) in the group.
	 */
	int GetNumLists() const;

	/*! Gets the number of probes in the group.
	 * @return The number of probes in the group.
	 */
	int GetNumCells() const;

	/*! Gets the number of probes per list.
	 * @return The number of probes per list.
	 */
	int GetNumCellsPerList() const;

	/*! Gets the start list index value.
	 * @return The start list index value.
	 */
	int GetStart() const;

	/*! Gets the stop list index value.
	 * @return The stop list index value.
	 */
	int GetStop() const;

	/*! Gets the group name.
	 * @return The group name.
	 */
	std::string GetName() const;

	/*! Gets the wobble situation.
	 * @return The wobble situation.
	 */
	unsigned short GetWobbleSituation() const;

	/*! Gets the allele code.
	 * @return The allele code.
	 */
	unsigned short GetAlleleCode() const;

	/*! Gets the channel.
	 * @return The channel.
	 */
	unsigned char GetChannel() const;

	/*! Gets the probe replication type.
	 * @return The probe replication type.
	 */
	affxcdf::ReplicationType GetRepType() const;

	/*! Retrieves the probe object given an index.
	 * @param cell_index Index to the probe of interest.
	 * @param info The returned probe data.
	 */
	void GetCell(int cell_index, FusionCDFProbeInformation & info);

	/*! Constructor */
	FusionCDFProbeGroupInformation();

	/*! Destructor */
	~FusionCDFProbeGroupInformation();
};

/*! This class provides storage for a probe set. */
class FusionCDFProbeSetInformation
{
private:
	/*! The GCOS probe set object. */
	affxcdf::CCDFProbeSetInformation *gcosSet;

	/*! The Calvin probe set object. */
	affymetrix_calvin_io::CDFProbeSetInformation *calvinSet;

	/*! Initializes the class for GCOS file reading.
	 * @param index The index to the probe set.
	 * @param cdf The GCOS CDF file object.
	 */
	void Initialize(int index, affxcdf::CCDFFileData *cdf);

	/*! Initializes the class for Calvin file reading.
	 * @param index The index to the probe set.
	 * @param cdf The Calvin CDF file object.
	 */
	void Initialize(int index, affymetrix_calvin_io::CDFData *cdf);

	/*! Deallocates any used memory. */
	void Clear();

	/*! Friend to the parent class. */
	friend class FusionCDFData;

public:
	/*! Gets the probe set type.
	 * @return The probe set type.
	 */
	affxcdf::GeneChipProbeSetType GetProbeSetType();

	/*! Gets the probe sets direction.
	 * @return The probe sets direction.
	 */
	affxcdf::DirectionType GetDirection() const;

	/*! Gets the number of lists (atoms) in the group.
	 * @return The number of lists (atoms) in the group.
	 */
	int GetNumLists() const;

	/*! The number of groups in the set.
	 * @return The number of groups in the set.
	 */
	int GetNumGroups() const;

	/*! The number of probes in the set.
	 * @return The number of probes in the set.
	 */
	int GetNumCells() const;

	/*! Gets the number of probes per list.
	 * @return The number of probes per list.
	 */
	int GetNumCellsPerList() const;

	/*! Gets the probe set number.
	 * @return The probe set number.
	 */
	int GetProbeSetNumber() const;

	/*! Gets a group object.
	 * @param index The index to the group of interest.
	 * @param info The returned group data.
	 */
	void GetGroupInformation(int index, FusionCDFProbeGroupInformation & info);

	/*! Constructor */
	FusionCDFProbeSetInformation();

	/*! Destructor */
	~FusionCDFProbeSetInformation();
};

/*! This class provides storage for QC probes */
class FusionCDFQCProbeInformation
{
private:
	/*! The GCOS probe object. */
	affxcdf::CCDFQCProbeInformation *gcosProbe;

	/*! The Calvin probe object. */
	affymetrix_calvin_io::CDFQCProbeInformation *calvinProbe;

	/*! Initializes the class for GCOS file reading.
	 * @param index The index to the probe.
	 * @param gcosSet The GCOS QC probe set.
	 */
	void Initialize(int index, affxcdf::CCDFQCProbeSetInformation *gcosSet);

	/*! Initializes the class for Calvin file reading.
	 * @param index The index to the probe.
	 * @param calvinSet The Calvin QC probe set.
	 */
	void Initialize(int index, affymetrix_calvin_io::CDFQCProbeSetInformation *calvinSet);

	/*! Deallocates any used memory. */
	void Clear();

	/*! Friend to the parent class. */
	friend class FusionCDFQCProbeSetInformation;

public:
	/*! Constructor */
	FusionCDFQCProbeInformation();

	/*! Destructor */
	~FusionCDFQCProbeInformation();

	/*! Gets the X cooridnate of the probe.
	 * @return The X coordinate.
	 */
	int GetX() const;

	/*! Gets the Y cooridnate of the probe.
	 * @return The Y coordinate.
	 */
	int GetY() const;

	/*! Gets the probe length.
	 * @return The probe length. This value may be 1 for non-synthesized features.
	 */
	int GetPLen() const;

	/*! Gets the flag indicating if the probe is a perfect match probe.
	 * @return The flag indicating if the probe is a perfect match probe
	 */
	bool IsPerfectMatchProbe() const;

	/*! Gets a flag indicating if the probe is used for background calculations (blank feature).
	 * @return Flag indicating if the probe is used for background calculations (blank feature).
	 */
	bool IsBackgroundProbe() const;
};

/*! This class provides storage for the probes in a QC probe set. */
class FusionCDFQCProbeSetInformation
{
private:
	/*! The GCOS probe set object. */
	affxcdf::CCDFQCProbeSetInformation *gcosSet;

	/*! The Calvin probe set object. */
	affymetrix_calvin_io::CDFQCProbeSetInformation *calvinSet;

	/*! Initializes the class for GCOS file reading.
	 * @param index The index to the QC probe set.
	 * @param cdf The GCOS CDF file object.
	 */
	void Initialize(int index, affxcdf::CCDFFileData *cdf);

	/*! Initializes the class for GCOS file reading.
	 * @param qcType The type of QC probe set.
	 * @param cdf The GCOS CDF file object.
	 */
	void Initialize(affxcdf::GeneChipQCProbeSetType qcType, affxcdf::CCDFFileData *cdf);

	/*! Initializes the class for Calvin file reading.
	 * @param index The index to the QC probe set.
	 * @param cdf The Calvin CDF file object.
	 */
	void Initialize(int index, affymetrix_calvin_io::CDFData *cdf);

	/*! Initializes the class for Calvin file reading.
	 * @param qcType The type of QC probe set.
	 * @param cdf The Calvin CDF file object.
	 */
	void Initialize(affxcdf::GeneChipQCProbeSetType qcType, affymetrix_calvin_io::CDFData *cdf);

	/*! Deallocates any used memory. */
	void Clear();

	/*! Friend to the parent class. */
	friend class FusionCDFData;

public:
	/*! Gets the probe set type.
	 * @return The probe set type.
	 */
	affxcdf::GeneChipQCProbeSetType GetQCProbeSetType() const;

	/*! Gets the number of probes in the set.
	 * @return The number of probes in the set.
	 */
	int GetNumCells() const;

	/*! Gets the information about a single probe in the set.
	 * @param index The index to the probe of interest.
	 * @param info The information about the probe.
	 */
	void GetProbeInformation(int index, FusionCDFQCProbeInformation & info);

	/*! Constructor */
	FusionCDFQCProbeSetInformation();

	/*! Destructor */
	~FusionCDFQCProbeSetInformation();
};

/*! This defines the combined GCOS/Calvin CDF data interaction class. */
class FusionCDFData  
{
protected:
	/*! The GCOS CDF file object. */
	affxcdf::CCDFFileData *gcosData;

	/*! The header object. */
	FusionCDFFileHeader header;

	/*! The Calvin CDF file object. */
	affymetrix_calvin_io::CDFData *calvinData;

	/*! The name of the file to read. */
	std::string fileName;

	/*! Creates either the GCOS or Calvin parser object. */
	void CreateObject();

public:
	/*! Constructor */
	FusionCDFData();

	/*! Destructor */
	~FusionCDFData();

	/*! Sets the name of the file.
	 * @param name The full path of the CDF file.
	 */
	void SetFileName(const char *name);

	/*! Gets the name of the file.
	 * @return The full path of the CDF file.
	 */
	std::string GetFileName() const;

	/*! Gets the header object.
	 * @return The CDF file header object.
	 */
	FusionCDFFileHeader &GetHeader();

	/*! Get GUID
	 * @return GUID
	 */
	std::string GetGUID();

	/*! Get integrity md5
	 * @return integrity md5
	 */
	std::string GetIntegrityMd5();

	/*! Gets the error string.
	 * @return A string describing the last read error.
	 */
	std::string GetError() const;

	/*! Gets the name of a probe set.
	 * @param index The index to the probe set name of interest.
	 * @return The probe set name.
	 */
	std::string GetProbeSetName(int index) const;

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
	 * @param fileName The name of the file to test.
	 * @return True if XDA format.
	 */
	static bool IsXDACompatibleFile(const char *fileName);

	/*! Determines if a CDF file is of the Calvin format.
	 * @param fileName The name of the file to test.
	 * @return True if Calvin format.
	 */
	static bool IsCalvinCompatibleFile(const char *fileName);

	/*! Gets the probe set type for non-qc probe sets.
	 * @param index The index to the probe set of interest.
	 * @return The type of probe set.
	 */
	affxcdf::GeneChipProbeSetType GetProbeSetType(int index) const;

	/*! Gets the probe set information.
	 * @param index The index to the probe set of interest.
	 * @param info The probe set information.
	 * @return The probe set information.
	 */
	void GetProbeSetInformation(int index, FusionCDFProbeSetInformation & info);

	/*! Gets the QC probe set information by index.
	 * @param index The index to the QC probe set of interest.
	 * @param info The QC probe set information.
	 * @return The QC probe set information.
	 */
	void GetQCProbeSetInformation(int index, FusionCDFQCProbeSetInformation & info);

	/*! Gets the QC probe set information by type.
	 * @param qcType The type of QC probe set to retrieve.
	 * @param info The QC probe set information.
	 * @return The QC probe set information.
	 */
	void GetQCProbeSetInformation(affxcdf::GeneChipQCProbeSetType qcType, FusionCDFQCProbeSetInformation & info);
};

}

#endif	//_AffymetrixFusionCDFData_HEADER_
