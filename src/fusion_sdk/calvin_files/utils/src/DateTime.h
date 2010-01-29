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


#ifndef _DateTime_HEADER_
#define _DateTime_HEADER_

/*! \file DateTime.h This file provides data structures for holding a date and time.
 */

#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//
#include <cstring>
#include <string>
//

namespace affymetrix_calvin_utilities
{
/*! The zulu format. */
#define ZULU_FORMAT_SPECIFIER std::wstring(L"Z")

/*! The datetime separator format specifier.*/
#define DATETIME_SEPERATOR_FORMAT_SPECIFIER std::wstring(L"T")

/*! The date format string. */
#define DATEFORMAT std::wstring(L"%Y-%m-%d") // YYYY-MM-DD

/*! The time format string. */
#define TIMEFORMAT std::wstring(L"%H:%M:%S") // hh:mm:ss

/*! This class provides storage for a date and time object. */
class DateTime
{
public:
	/*! Constructor */
	DateTime();

	/*! Destructor */
	~DateTime();

protected:
	/*! The date */
	std::wstring affydate;

	/*! The time */
	std::wstring affytime;

	/*! indicates if the time falue is UTC */
	bool coordinateduniversaltime;

public:
	/*! The date.
	 *
	 * @return The date.
	 */
	std::wstring Date() const;
	/*! Set the date.
	 *
	 * @param value The new date.
	 */
	 void Date(std::wstring value);

	/*! The time.
	 *
	 * @return The time.
	 */
	std::wstring Time() const;
	/*! Set the time.
	 *
	 * @param value The new time.
	 */
	void Time(std::wstring value);

	/*! Clears the members. */
	void Clear();

	/*! Gets the current date and time.
	 *
	 * @return The current date and time.
	 */
	static DateTime GetCurrentDateTime();

	/*! Gets whether or not the DateTime object time value is UTC or not.
	 *
	 * @return UTC or not.
	 */
	bool IsUTC();

	/*! Sets if the DateTime is UTC or not.
	 *	@param value True if the DateTime should be considered UTC.
	 */
	void UTC(bool value) { coordinateduniversaltime = value; }

	/*! Gets the current date and time and concatinates both together.
	 *
	 * @return The string representation of the date and time.
	 */
	std::wstring ToString();

	/*! Converts the string into a DateTime.
	 *	@param value A wstring containing the date and time to convert. The format must match that returned by ToString().
	 *	@return A DateTime equivalent to the date and time in value.
	 *	@exception FormatException
	 */
	static DateTime Parse(std::wstring value);

	/*! Get a properly formatted Date string.
	 *	@param year A four digit year.
	 *	@param month A one-based month of the year.
	 *	@param day A one-based day of month
	 *	@return A properly formatted date string.
	 */
	static std::wstring FormatDate(u_int32_t year, u_int32_t month, u_int32_t day);

	/*! Get a properly formatted Time string.
	 *	@param hour Zero-based hour of day.
	 *	@param minute Zero-based minute of hour.
	 *	@param second Zero-based second of minute.
	 *	@return A properly formatted time string.
	 */
	static std::wstring FormatTime(u_int32_t hour, u_int32_t minute, u_int32_t second);

	/*! Get a properly formatted date-time string.
	 *	@param year A four digit year.
	 *	@param month A one-based month of the year.
	 *	@param day A one-based day of month
	 *	@param hour Zero-based hour of day.
	 *	@param minute Zero-based minute of hour.
	 *	@param second Zero-based second of minute.
	 *	@param utc Indicates if the time is Universal Coordinated Time.
	 *	@return A properly formatted date-time string.
	 */
	static std::wstring FormatDateTime(u_int32_t year, u_int32_t month, u_int32_t day, u_int32_t hour, u_int32_t minute, u_int32_t second, bool utc);


protected:
	/*! Check the date format and may modify it.
	 *	@param date The formatted date string.
	 *	@exception FormatException
	 */
	static void CheckDateFormat(std::wstring& date);

	/*! Check the itme format.
	 *	@param time The formatted time string.
	 *	@exception FormatException
	 */
	static void CheckTimeFormat(std::wstring& time);
};

}

#endif // _DateTime_HEADER_
