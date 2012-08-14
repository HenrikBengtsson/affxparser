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


#include "calvin_files/utils/src/DateTime.h"
//
#include "calvin_files/exception/src/InterpretationException.h"
#include "calvin_files/utils/src/StringUtils.h"
//
#include <ctime>
//

using namespace affymetrix_calvin_utilities;
using namespace affymetrix_calvin_exceptions;

#define DATE_PRINTF_FORMAT std::wstring(L"%02d-%02d-%02d")
#define TIME_PRINTF_FORMAT std::wstring(L"%02d:%02d:%02d")

#ifdef _MSC_VER
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
	StringUtils::STLTrimRight(value);
	StringUtils::STLTrimLeft(value);
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
	StringUtils::STLTrimRight(value);
	StringUtils::STLTrimLeft(value);
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

	std::wstring datetime = this->Date() + DATETIME_SEPERATOR_FORMAT_SPECIFIER + this->Time();
	if (coordinateduniversaltime)
		datetime += ZULU_FORMAT_SPECIFIER;
	return datetime;
}

/*
 * Converts the string into a DateTime.
 */
DateTime DateTime::Parse(std::wstring value)
{
	DateTime result;
	int32_t posT = (int32_t) value.find(DATETIME_SEPERATOR_FORMAT_SPECIFIER);
	if (posT == std::wstring::npos)
	{
		FormatException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}

	std::wstring date = value.substr(0, posT);

	CheckDateFormat(date);

	bool utc = false;
	int32_t posZ = (int32_t) value.find(ZULU_FORMAT_SPECIFIER);
	if (posZ != std::wstring::npos)
	{
		if (posZ < posT)
		{
			FormatException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
			throw e;
		}
		utc = true;
		value = value.substr(0,posZ);	// remove the ZULU specifier
	}

	std::wstring time = value.substr(posT+1);

	// Check the time format
	CheckTimeFormat(time);

	// Looks good
	result.Time(time);
	result.Date(date);
	result.coordinateduniversaltime = utc;
	return result;
}

/*
 * Get a properly formatted Date string.
 */
std::wstring DateTime::FormatDate(u_int32_t year, u_int32_t month, u_int32_t day)
{
	wchar_t buf[50];
	FormatString3(buf, 50, DATE_PRINTF_FORMAT.c_str(), year, month, day);
	return buf;
}

/*
 * Get a properly formatted Time string.
 */
std::wstring DateTime::FormatTime(u_int32_t hour, u_int32_t minute, u_int32_t second)
{
	wchar_t buf[50];
	FormatString3(buf, 50, TIME_PRINTF_FORMAT.c_str(), hour, minute, second);
	return buf;
}

/*
 * Get a properly formatted date-time string.
 */
std::wstring DateTime::FormatDateTime(u_int32_t year, u_int32_t month, u_int32_t day, u_int32_t hour, u_int32_t minute, u_int32_t second, bool utc)
{
	std::wstring datetime = FormatDate(year, month, day) + DATETIME_SEPERATOR_FORMAT_SPECIFIER + FormatTime(hour, minute, second);
	if (utc)
		datetime += ZULU_FORMAT_SPECIFIER;
	return datetime;
}

/* 
 * Check the date format and modify it if needed and possible.
 */
void DateTime::CheckDateFormat(std::wstring& date)
{
	// Check the format
	int32_t y, m, d;
	if (swscanf(date.c_str(), L"%d-%d-%d", &y, &m, &d) != 3)
	{
		if (swscanf(date.c_str(), L"%d/%d/%d", &y, &m, &d) == 3 && date.length() < 50)
		{
			date = FormatDate(y, m, d);
		}
		else
		{
			FormatException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
			throw e;
		}
	}
}

/*
 * Check the time format.
 */
void DateTime::CheckTimeFormat(std::wstring& time)
{
	int32_t h, M, s;
	if (swscanf(time.c_str(), L"%d:%d:%d", &h, &M, &s) != 3)
	{
		FormatException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
}
