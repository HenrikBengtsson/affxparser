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


#include "calvin_files/data/src/CHPGenotypeEntry.h"
//

using namespace affymetrix_calvin_io;

CHPGenotypeEntry::CHPGenotypeEntry() 
{
	Clear();
}

CHPGenotypeEntry::CHPGenotypeEntry(const std::string& psName,
																	u_int8_t c,
																	float conf,
																	float ras1,
																	float ras2,
																	float aa,
																	float ab,
																	float bb,
																	float no)
{
	probeSetName = psName;
	call = c;
	confidence = conf;
	RAS1 = ras1;
	RAS2 = ras2;
	aaCall = aa;
	abCall = ab;
	bbCall = bb;
	noCall = no;
}

CHPGenotypeEntry::~CHPGenotypeEntry() {}

void CHPGenotypeEntry::Clear() 
{
	probeSetName.clear();
	call = 0;
	confidence = 0;
	RAS1 = 0;
	RAS2 = 0;
	aaCall = 0;
	abCall = 0;
	bbCall = 0;
	noCall = 0;
}

CHPGenotypeEntry CHPGenotypeEntry::operator=(CHPGenotypeEntry zn)
	{
		probeSetName = zn.GetProbeSetName();
		call = zn.GetCall();
		confidence = zn.GetConfidence();
		RAS1 = zn.GetRAS1();
		RAS2 = zn.GetRAS2();
		aaCall = zn.GetAACall();
		abCall = zn.GetABCall();
		bbCall = zn.GetBBCall();
		noCall = zn.GetNoCall();
		return *this; 
	}

	std::string CHPGenotypeEntry::GetProbeSetName() const
	{
		return probeSetName;
	}

	u_int8_t CHPGenotypeEntry::GetCall() const
	{
		return call;
	}

	float CHPGenotypeEntry::GetConfidence() const
	{
		return confidence;
	}

	float CHPGenotypeEntry::GetRAS1() const
	{
		return RAS1;
	}

	float CHPGenotypeEntry::GetRAS2() const
	{
		return RAS2;
	}

	float CHPGenotypeEntry::GetAACall() const
	{
		return aaCall;
	}

	float CHPGenotypeEntry::GetABCall() const
	{
		return abCall;
	}

	float CHPGenotypeEntry::GetBBCall() const
	{
		return bbCall;
	}

	float CHPGenotypeEntry::GetNoCall() const
	{
		return noCall;
	}

	void CHPGenotypeEntry::SetProbeSetName(const std::string& p)
	{
		probeSetName = p;
	}

	void CHPGenotypeEntry::SetCall(u_int8_t p)
	{
		call = p;
	}

	void CHPGenotypeEntry::SetConfidence(float p)
	{
		confidence = p;
	}

	void CHPGenotypeEntry::SetRAS1(float p)
	{
		RAS1 = p;
	}

	void CHPGenotypeEntry::SetRAS2(float p)
	{
		RAS2 = p;
	}

	void CHPGenotypeEntry::SetAACall(float p)
	{
		aaCall = p;
	}

	void CHPGenotypeEntry::SetABCall(float p)
	{
		abCall = p;
	}

	void CHPGenotypeEntry::SetBBCall(float p)
	{
		bbCall = p;
	}

	void CHPGenotypeEntry::SetNoCall(float p)
	{
		noCall = p;
	}
