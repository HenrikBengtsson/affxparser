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


#ifndef _FusionCHPGenericData_HEADER_
#define _FusionCHPGenericData_HEADER_


#include "calvin_files/data/src/GenericData.h"
#include "calvin_files/fusion/src/FusionCHPData.h"
//
#include <cstring>
#include <list>
#include <string>
//

//////////////////////////////////////////////////////////////////////

namespace affymetrix_fusion_io
{

/*! This class provides storage and reading capabilities for CHP files */
class FusionCHPGenericData : public FusionCHPData
{
public:

	/*! Destructor */
	~FusionCHPGenericData();

	/*! Get the id of the file (only valid for Command Console "calvin" files)
	 * @return The unique file id.
	 */
	affymetrix_calvin_utilities::AffymetrixGuidType FileId();

	/*! Returns the GenericData object associated with a Calvin file, NULL for GCOS files. */
	affymetrix_calvin_io::GenericData *GetGenericData();

	/*! Gets the class name. */
	affymetrix_calvin_utilities::AffymetrixGuidType GetObjectName();

	/*! Deallocates any memory used by the class object */
	void Clear();

	/*! Gets the generic data object.
	 * @return The generic data object.
	 */
	affymetrix_calvin_io::GenericData &GetData() { return genericData; }

private:
	
	/*! Constructor */
	FusionCHPGenericData();

	/*! The generic file data object. */
	affymetrix_calvin_io::GenericData genericData;

	/*! Reads the CHP file.
	 * @return True if successful.
	 */
	bool Read();

	/*! Reads the header of the CHP file
	 * @return True if successful
	 */
	bool ReadHeader();

	/*! A class to register a generic CHP reader. */
	class Reg : public FusionCHPDataReg
	{
	public:
		/*! Constructor - register the file type. */
		Reg() : FusionCHPDataReg()
		{
			std::list<std::string> ids;
			SetFileTypeIds(ids);
		}

		/*! Creates a generic CHP object.
		 * @return The generic CHP object.
		 */
		FusionCHPData *MakeObject() { return new FusionCHPGenericData; }
	};

	/*! The one and only registration object. This registers the class as a CHP reader. */
	static Reg reg;

public:
	/*! Converts the type to the generic CHP type.
	 * @param chip The pointer to the CHP data object.
	 * @return The generic CHP data type or NULL if not compatible.
	 */
	static FusionCHPGenericData * FromBase(FusionCHPData *chip);

};

}

#endif
