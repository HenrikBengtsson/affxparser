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

#include "DateTime.h"
#include "InterpretationException.h"
#include <time.h>

using namespace affymetrix_calvin_utilities;
using namespace affymetrix_calvin_exceptions;

#ifdef WIN32
#pragma warning(disable: 4996) // Turn off warnings for depricated VC++ functions.
#endif

/*
 * Initialize the class.
 */
DateTime::DateTime()
{
	Clear();
}

/*
 * Clean up the class.
 */
DateTime::~DateTime()
{
}

/*
 * Erase the date and time values.
 */
void DateTime::Clear()
{
	coordinateduniversaltime = true;
	affydate = L"";
	affytime = L"";
}

/*! The date.
 *
 * @return The date.
 */
std::wstring DateTime::Date() const
{
	return affydate; 
}

/*! Set the date.
 *
 */
void DateTime::Date(std::wstring value)
{
	affydate = value;
}

/*! The time.
 *
 * @return The time.
 */
std::wstring DateTime::Time() const
{
	return affytime;
}

/*! Set the time.
 *
 */
void DateTime::Time(std::wstring value)
{
	affytime = value;
}

/*
 * Get the current date and time.
 *
 * @return DateTime
 */
DateTime DateTime::GetCurrentDateTime()
{
	DateTime currentDateTime;
	size_t stringLength = 0;
	const size_t MAX_DATETIME_BUFFER_SIZE = 100;
	wchar_t strDest[MAX_DATETIME_BUFFER_SIZE];

	// Get the current local time.
	time_t long_time = 0;
	time( &long_time );
	struct tm* newtime = gmtime( &long_time );

	// Conver the date to a string.
	stringLength = wcsftime(strDest, MAX_DATETIME_BUFFER_SIZE, DATEFORMAT.c_str(), newtime);
	wcsftime(strDest, stringLength+1, DATEFORMAT.c_str(), newtime);
	currentDateTime.Date(strDest);

	// Convert the time to a string.
	stringLength = wcsftime(strDest, MAX_DATETIME_BUFFER_SIZE, TIMEFORMAT.c_str(), newtime);
	wcsftime(strDest, stringLength+1, TIMEFORMAT.c_str(), newtime);
	currentDateTime.Time(strDest);

	return currentDateTime;
}

/*! Gets whether or not the DateTime object time value is UTC or not.
 *
 * @return UTC or not.
 */
bool DateTime::IsUTC()
{
	return coordinateduniversaltime;
}


std::wstring DateTime::ToString()
{
	if(affydate == L"" || affytime == L"")
	{
		DateTime dt = GetCurrentDateTime();
		this->Date(dt.Date());
		this->Time(dt.Time());
	}

	return this->Date() + DATETIME_SEPERATOR_FORMAT_SPECIFIER + this->Time() + ZULU_FORMAT_SPECIFIER;
}

/*
 * Converts the string into a DateTime.  This method is not bullet-proof.
 */
DateTime DateTime::Parse(std::wstring value)
{
	DateTime result;
	int32_t posT = (int32_t) value.find(DATETIME_SEPERATOR_FORMAT_SPECIFIER);
	if (posT == std::wstring::npos)
	{
		FormatException e;
		throw e;
	}

	std::wstring date = value.substr(0, posT);

	// Check the format
	int32_t y, m, d;
	if (swscanf(date.c_str(), L"%d-%d-%d", &y, &m, &d) != 3)
	{
		FormatException e;
		throw e;
	}

	bool utc = false;
	int32_t posZ = (int32_t) value.find(ZULU_FORMAT_SPECIFIER);
	if (posZ != std::wstring::npos)
	{
		if (posZ < posT)
		{
			FormatException e;
			throw e;
		}
		utc = true;
		value = value.substr(0,posZ);	// remove the ZULU specifier
	}

	std::wstring time = value.substr(posT+1);

	// Check the format
	int32_t h, M, s;
	if (swscanf(time.c_str(), L"%d:%d:%d", &h, &M, &s) != 3)
	{
		FormatException e;
		throw e;
	}

	// Looks good
	result.Time(time);
	result.Date(date);
	result.coordinateduniversaltime = utc;
	return result;
}