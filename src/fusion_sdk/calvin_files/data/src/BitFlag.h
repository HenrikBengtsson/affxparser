/////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License,
// or (at your option) any later version.
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
/////////////////////////////////////////////////////////////////

#ifndef _BitFlag_HEADER_
#define _BitFlag_HEADER_

#include "AffymetrixBaseTypes.h"
#include <bitset>


#ifdef WIN32
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{

static u_int8_t DEFAULT_DATA_SET_HDR_FLAG = 0;

class BitFlag
{
private:

	std::bitset<16> flags;

public:

	BitFlag();

	BitFlag(u_int16_t p);

	~BitFlag();

	/*! Clear all flags. */
	void Clear();

	/*! Get all flags.
	 * @return number with flags set.
	 */
	u_int16_t GetFlags() const;

	/*! Set all flags.
	 * @param p number with flags set.
	 */
	void SetFlags(u_int16_t p);

	/*! True if the default data set header flag has been set.
	 * @return true if the default data set header flag has been set.
	 */
	bool HasDefaultDataSetHdr() const;

	/*! Set the default data set header flag.
	 * @param p true or false.
	 */
	void SetDefaultDataSetHdr(bool p);

private:

	bool GetFlag(u_int32_t index) const;

	void SetFlag(u_int32_t index, bool p);

};

}

#endif // _BitFlag_HEADER_
