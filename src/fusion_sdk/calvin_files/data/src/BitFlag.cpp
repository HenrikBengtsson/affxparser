#include "BitFlag.h"

using namespace affymetrix_calvin_io;

BitFlag::BitFlag()
{
	Clear();
}

BitFlag::BitFlag(u_int16_t p)
{
	SetFlags(p);
}

BitFlag::~BitFlag() {}

void BitFlag::Clear()
{
	flags.reset();
}

u_int16_t BitFlag::GetFlags() const
{
	return (u_int16_t)flags.to_ulong();
}

void BitFlag::SetFlags(u_int16_t p)
{
	for(int i = 0; i < 8; i++)
	{
		if(p & (1 << i))
		{
			SetFlag(i, true);
		}
		else
		{
			SetFlag(i, false);
		}
	}
}

bool BitFlag::GetFlag(u_int32_t index) const
{
	return flags.test(index);
}

void BitFlag::SetFlag(u_int32_t index, bool p)
{
	flags.set(index, p);
}

bool BitFlag::HasDefaultDataSetHdr() const
{
	return GetFlag(DEFAULT_DATA_SET_HDR_FLAG);
}

void BitFlag::SetDefaultDataSetHdr(bool p)
{
	SetFlag(DEFAULT_DATA_SET_HDR_FLAG, p);
}
