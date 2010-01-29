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
#ifndef _CHPGenotypeEntry_HEADER_
#define _CHPGenotypeEntry_HEADER_

#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//
#include <cstring>
#include <string>
#include <vector>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{

/*! This class stores a genotype result */
class CHPGenotypeEntry
{

private:

	std::string probeSetName;
	u_int8_t call;
	float confidence;
	float RAS1;
	float RAS2;
	float aaCall;
	float abCall;
	float bbCall;
	float noCall;

public:

	CHPGenotypeEntry();
	CHPGenotypeEntry(const std::string& probeSetname,
									u_int8_t call,
									float confidence,
									float RAS1,
									float RAS2,
									float AACall,
									float ABCall,
									float BBCall,
									float NoCall);
	~CHPGenotypeEntry();

	void Clear();

	/*! Assignment operator
	 * @param zn The zone to copy
	 * @return The new zone object
	 */
	CHPGenotypeEntry operator=(CHPGenotypeEntry zn);

	std::string GetProbeSetName() const;

	u_int8_t GetCall() const;

	float GetConfidence() const;

	float GetRAS1() const;

	float GetRAS2() const;

	float GetAACall() const;

	float GetABCall() const;

	float GetBBCall() const;

	float GetNoCall() const;

	void SetProbeSetName(const std::string& p);

	void SetCall(u_int8_t p);

	void SetConfidence(float p);

	void SetRAS1(float p);

	void SetRAS2(float p);

	void SetAACall(float p);

	void SetABCall(float p);

	void SetBBCall(float p);

	void SetNoCall(float p);
};

/*! An STL list of zones */
typedef std::vector<CHPGenotypeEntry> CHPGenotypeEntryVector;

/*! iterator of CHPGenotypeEntrys */
typedef std::vector<CHPGenotypeEntry>::iterator CHPGenotypeEntryVectorIt;

}

#endif // _CHPGenotypeEntry_HEADER_
