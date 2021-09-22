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

//
#include "util/AffxTime.h"
//
#include <cmath>
#include <cstdio>
#include <cstring>
#include <stdio.h>
#include <string>

#ifdef _MSC_VER
// dont warn about some funcs...
#define _CRT_SECURE_NO_WARNINGS
#endif

#define MIN_DATE                (-657434L)  // about year 100
#define MAX_DATE                2958465L    // about year 9999

// Half a second, expressed in days
#define HALF_SECOND  (1.0/172800.0)

// One-based array of days in year at month start
static int rgMonthDays[13] =
	{0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

long AffxTime::m_lStartTime = 0;
long AffxTime::m_lStopTime = 0;

AffxTime::AffxTime()
	{empty();}

AffxTime::AffxTime(const AffxTime& timeSrc)
	{ m_time = timeSrc.m_time;}

AffxTimeSpan::AffxTimeSpan()
	{ }

/////////////////////////////////////////////////////////////////////////////
// AffxTime - absolute time

AffxTime::AffxTime(time_t time)
{
	double dtSrc = (double)time / 24.0 / 60.0 / 60.0;
	
	tm tm;
	memset((void*)&tm, 0, sizeof(tm)); tm.tm_isdst = -1;

	// The legal range does not actually span year 0 to 9999.
	if (dtSrc > MAX_DATE || dtSrc < MIN_DATE) // about year 100 to about 9999
		return;

	int nDays;             // Number of days since Dec. 30, 1899
	int nDaysAbsolute;     // Number of days since 1/1/0
	int nSecsInDay;        // Time in seconds since midnight
	int nMinutesInDay;     // Minutes in day

	int n400Years;         // Number of 400 year increments since 1/1/0
	int n400Century;       // Century within 400 year block (0,1,2 or 3)
	int n4Years;           // Number of 4 year increments since 1/1/0
	int n4Day;             // Day within 4 year block
							//  (0 is 1/1/yr1, 1460 is 12/31/yr4)
	int n4Yr;              // Year within 4 year block (0,1,2 or 3)
	bool bLeap4 = true;     // TRUE if 4 year block includes leap year

	double dblDate = dtSrc; // tempory serial date

	// If a valid date, then this conversion should not overflow
	nDays = (int)dblDate;

	// Round to the second
	dblDate += ((dtSrc > 0.0) ? HALF_SECOND : -HALF_SECOND);

	nDaysAbsolute = (int)dblDate + 693959L; // Add days from 1/1/0 to 12/30/1899

	dblDate = fabs(dblDate);
	nSecsInDay = (int)((dblDate - floor(dblDate)) * 86400.);

	// Calculate the day of week (sun=1, mon=2...)
	//   -1 because 1/1/0 is Sat.  +1 because we want 1-based
	tm.tm_wday = (int)((nDaysAbsolute - 1) % 7L) + 1;

	// Leap years every 4 yrs except centuries not multiples of 400.
	n400Years = (int)(nDaysAbsolute / 146097L);

	// Set nDaysAbsolute to day within 400-year block
	nDaysAbsolute %= 146097L;

	// -1 because first century has extra day
	n400Century = (int)((nDaysAbsolute - 1) / 36524L);

	// Non-leap century
	if (n400Century != 0)
	{
		// Set nDaysAbsolute to day within century
		nDaysAbsolute = (nDaysAbsolute - 1) % 36524L;

		// +1 because 1st 4 year increment has 1460 days
		n4Years = (int)((nDaysAbsolute + 1) / 1461L);

		if (n4Years != 0)
			n4Day = (int)((nDaysAbsolute + 1) % 1461L);
		else
		{
			bLeap4 = false;
			n4Day = (int)nDaysAbsolute;
		}
	}
	else
	{
		// Leap century - not special case!
		n4Years = (int)(nDaysAbsolute / 1461L);
		n4Day = (int)(nDaysAbsolute % 1461L);
	}

	if (bLeap4)
	{
		// -1 because first year has 366 days
		n4Yr = (n4Day - 1) / 365;

		if (n4Yr != 0)
			n4Day = (n4Day - 1) % 365;
	}
	else
	{
		n4Yr = n4Day / 365;
		n4Day %= 365;
	}

	// n4Day is now 0-based day of year. Save 1-based day of year, year number
	tm.tm_yday = (int)n4Day + 1;
	tm.tm_year = n400Years * 400 + n400Century * 100 + n4Years * 4 + n4Yr;

	// Handle leap year: before, on, and after Feb. 29.
	if (n4Yr == 0 && bLeap4)
	{
		// Leap Year
		if (n4Day == 59)
		{
			/* Feb. 29 */
			tm.tm_mon = 2;
			tm.tm_mday = 29;
			goto DoTime;
		}

		// Pretend it's not a leap year for month/day comp.
		if (n4Day >= 60)
			--n4Day;
	}

	// Make n4DaY a 1-based day of non-leap year and compute
	//  month/day for everything but Feb. 29.
	++n4Day;

	// Month number always >= n/32, so save some loop time */
	for (tm.tm_mon = (n4Day >> 5) + 1;
		n4Day > rgMonthDays[tm.tm_mon]; tm.tm_mon++);

	tm.tm_mday = (int)(n4Day - rgMonthDays[tm.tm_mon-1]);

DoTime:
	if (nSecsInDay == 0)
		tm.tm_hour = tm.tm_min = tm.tm_sec = 0;
	else
	{
		tm.tm_sec = (int)nSecsInDay % 60L;
		nMinutesInDay = nSecsInDay / 60L;
		tm.tm_min = (int)nMinutesInDay % 60;
		tm.tm_hour = (int)nMinutesInDay / 60;
	}

	m_time = tm;
}
AffxTime::AffxTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec, int nDST)
{
	struct tm atm;
	memset((void*)&atm, 0, sizeof(tm)); atm.tm_isdst = -1;
	atm.tm_sec = nSec;
	atm.tm_min = nMin;
	atm.tm_hour = nHour;
	if (nDay < 1 && nDay > 31) {Err::errAbort("AffxTime, invalid day value.");}
	atm.tm_mday = nDay;
//	_ASSERTE(nMonth >= 1 && nMonth <= 12);
	atm.tm_mon = nMonth;  
	atm.tm_year = nYear;  
	atm.tm_isdst = nDST;

	m_time = atm;

//	_ASSERTE(m_time != -1);       // indicates an illegal input time
}

AffxTime AffxTime::getCurrentTime()
// return the current system time
{
	AffxTime time;
	tm tm;
	
	time_t lTime = ::time(NULL);
	tm = *localtime(&lTime);
	time.m_time = tm; 
	// Do not free(ptm) as subsequent calls to this function will then fail.
	
	time.m_time.tm_mon++;
	if (time.m_time.tm_year < 50)
		time.m_time.tm_year = time.m_time.tm_year + 2000;
	else if (time.m_time.tm_year < 100)
		time.m_time.tm_year = time.m_time.tm_year + 1900;
	else if (time.m_time.tm_year > 100)
		time.m_time.tm_year = time.m_time.tm_year -100 + 2000;


	return time;
}

// return runtime string -- timeElapsed = clock() - lStart
AffxString AffxTime::getRuntime(AffxString strAction, double timeElapsed)
{
	double milliElapsed = timeElapsed / CLOCKS_PER_SEC * 1000;
	double MILLISECONDS_PER_HOUR   = 60 * 60 * 1000;
	double MILLISECONDS_PER_MINUTE = 60 * 1000;
	double MILLISECONDS_PER_SECOND = 1000;

	AffxString strTime;
	if (timeElapsed > MILLISECONDS_PER_HOUR)
	{
		strTime.sprintf("%.1lf hours", milliElapsed / MILLISECONDS_PER_HOUR);
	}
	else if (timeElapsed > MILLISECONDS_PER_MINUTE)
	{
		strTime.sprintf("%.1lf minutes", milliElapsed / MILLISECONDS_PER_MINUTE);
	}
	else
	{
		strTime.sprintf("%.1lf seconds", milliElapsed / MILLISECONDS_PER_SECOND);
	}

	return (strAction + " = " + strTime);
}

void AffxTime::startTime()
{
#ifdef WIN32
    m_lStartTime = clock();
#else
    m_lStartTime = time(NULL);
#endif
}

AffxString AffxTime::getRuntime(AffxString strAction)
{
#ifdef WIN32
    m_lStopTime = clock();
    return AffxTime::getRuntime(strAction, m_lStopTime - m_lStartTime);
#else
    m_lStopTime = time(NULL);
    double timeElapsed = m_lStopTime - m_lStartTime;
	double SECONDS_PER_HOUR   = 60 * 60;
	double SECONDS_PER_MINUTE = 60;

	AffxString strTime;
	if (timeElapsed > SECONDS_PER_HOUR)
	{
		strTime.sprintf("%.1lf hours", timeElapsed / SECONDS_PER_HOUR);
	}
	else if (timeElapsed > SECONDS_PER_MINUTE)
	{
		strTime.sprintf("%.1lf minutes", timeElapsed / SECONDS_PER_MINUTE);
	}
	else
	{
		strTime.sprintf("%.1lf seconds", timeElapsed);
	}

	return (strAction + " = " + strTime);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// AffxTimeSpan - relative time



/////////////////////////////////////////////////////////////////////////////
// String formatting

#define maxTimeBufferSize       128
	// Verifies will fail if the needed buffer size is too large

AffxString AffxTimeSpan::format(char* pFormat) const
// formatting timespans is a little trickier than formatting AffxTimes
//  * we are only interested in relative time formats, ie. it is illegal
//      to format anything dealing with absolute time (i.e. years, months,
//         day of week, day of year, timezones, ...)
//  * the only valid formats:
//      %D - # of days -- NEW !!!
//      %H - hour in 24 hour format
//      %M - minute (0-59)
//      %S - seconds (0-59)
//      %% - percent sign
{
	char szBuffer[maxTimeBufferSize];
	char ch;
	char* pch = szBuffer;

	while ((ch = *pFormat++) != '\0')
	{
		if (ch == '%')
		{
			switch (ch = *pFormat++)
			{
			default:
				Err::errAbort("AffxTimeSpan, Bad format character.");      // probably a bad format character
			case '%':
				*pch++ = ch;
				break;
			case 'D':
				pch += sprintf(pch, "%d", getDays());
				break;
			case 'H':
				pch += sprintf(pch, "%02d", getHours());
				break;
			case 'M':
				pch += sprintf(pch, "%02d", getMinutes());
				break;
			case 'S':
				pch += sprintf(pch, "%02d", getSeconds());
				break;
			}
		}
		else
		{
			*pch++ = ch;
		}
	}

	*pch = '\0';
	return szBuffer;
}

void AffxTime::empty()
{
	struct tm atm;
	memset((void*)&atm, 0, sizeof(tm)); atm.tm_isdst = -1;
	m_time = atm;
}

void AffxTimeSpan::empty()
{
	m_timeSpan = 0;
}

// Calculate the DATE value.
AffxTimeSpan::AffxTimeSpan(double dt)
{
	// Convert from days to total seconds.
	m_timeSpan = (int)(dt * 24 * 60 * 60);
}

// Calculate the DATE value.
double AffxTimeSpan::getDATE(void)
{
	double dt = 0;

	int lDays = getDays();
	int nHours = getHours();
	int nMinutes = getMinutes();
	int nSeconds = getSeconds();

	// Set date span by breaking into fractional days (all input ranges valid)
	dt = lDays + ((double)nHours)/24 + ((double)nMinutes)/(24*60) +
		((double)nSeconds)/(24*60*60);

	return dt;
}

// Calculate the DATE value.
double AffxTime::getDATE(void)
{
	double dt = 0;

	int wYear = getYear();
	int wMonth = getMonth();
	int wDay = getDay();
	int wHour = getHour();
	int wMinute = getMinute();
	int wSecond = getSecond();

	// Validate year and month (ignore day of week and milliseconds)
	if (wYear > 9999 || wMonth < 1 || wMonth > 12)
		return 0;

	//  Check for leap year and set the number of days in the month
	bool bLeapYear = ((wYear & 3) == 0) &&
		((wYear % 100) != 0 || (wYear % 400) == 0);

	int nDaysInMonth =
		rgMonthDays[wMonth] - rgMonthDays[wMonth-1] +
		((bLeapYear && wDay == 29 && wMonth == 2) ? 1 : 0);

	// Finish validating the date
	if (wDay < 1 || wDay > nDaysInMonth ||
		wHour > 23 || wMinute > 59 ||
		wSecond > 59)
	{
		return 0;
	}

	// Cache the date in days and time in fractional days
	int nDate;
	double dblTime;

	//It is a valid date; make Jan 1, 1AD be 1
	nDate = wYear*365L + wYear/4 - wYear/100 + wYear/400 +
		rgMonthDays[wMonth-1] + wDay;

	//  If leap year and it's before March, subtract 1:
	if (wMonth <= 2 && bLeapYear)
		--nDate;

	//  Offset so that 12/30/1899 is 0
	nDate -= 693959L;

	dblTime = (((int)wHour * 3600L) +  // hrs in seconds
		((int)wMinute * 60L) +  // mins in seconds
		((int)wSecond)) / 86400.;

	dt = (double) nDate + ((nDate >= 0) ? dblTime : -dblTime);

	return dt;
}

// Constructor for DATE.
AffxTime::AffxTime(double dtSrc)
{
	tm tm;	
	memset((void*)&tm, 0, sizeof(tm)); tm.tm_isdst = -1;

	// The legal range does not actually span year 0 to 9999.
	if (dtSrc > MAX_DATE || dtSrc < MIN_DATE) // about year 100 to about 9999
		return;

	int nDays;             // Number of days since Dec. 30, 1899
	int nDaysAbsolute;     // Number of days since 1/1/0
	int nSecsInDay;        // Time in seconds since midnight
	int nMinutesInDay;     // Minutes in day

	int n400Years;         // Number of 400 year increments since 1/1/0
	int n400Century;       // Century within 400 year block (0,1,2 or 3)
	int n4Years;           // Number of 4 year increments since 1/1/0
	int n4Day;             // Day within 4 year block
							//  (0 is 1/1/yr1, 1460 is 12/31/yr4)
	int n4Yr;              // Year within 4 year block (0,1,2 or 3)
	bool bLeap4 = true;     // TRUE if 4 year block includes leap year

	double dblDate = dtSrc; // tempory serial date

	// If a valid date, then this conversion should not overflow
	nDays = (int)dblDate;

	// Round to the second
	dblDate += ((dtSrc > 0.0) ? HALF_SECOND : -HALF_SECOND);

	nDaysAbsolute = (int)dblDate + 693959L; // Add days from 1/1/0 to 12/30/1899

	dblDate = fabs(dblDate);
	nSecsInDay = (int)((dblDate - floor(dblDate)) * 86400.);

	// Calculate the day of week (sun=1, mon=2...)
	//   -1 because 1/1/0 is Sat.  +1 because we want 1-based
	tm.tm_wday = (int)((nDaysAbsolute - 1) % 7L) + 1;

	// Leap years every 4 yrs except centuries not multiples of 400.
	n400Years = (int)(nDaysAbsolute / 146097L);

	// Set nDaysAbsolute to day within 400-year block
	nDaysAbsolute %= 146097L;

	// -1 because first century has extra day
	n400Century = (int)((nDaysAbsolute - 1) / 36524L);

	// Non-leap century
	if (n400Century != 0)
	{
		// Set nDaysAbsolute to day within century
		nDaysAbsolute = (nDaysAbsolute - 1) % 36524L;

		// +1 because 1st 4 year increment has 1460 days
		n4Years = (int)((nDaysAbsolute + 1) / 1461L);

		if (n4Years != 0)
			n4Day = (int)((nDaysAbsolute + 1) % 1461L);
		else
		{
			bLeap4 = false;
			n4Day = (int)nDaysAbsolute;
		}
	}
	else
	{
		// Leap century - not special case!
		n4Years = (int)(nDaysAbsolute / 1461L);
		n4Day = (int)(nDaysAbsolute % 1461L);
	}

	if (bLeap4)
	{
		// -1 because first year has 366 days
		n4Yr = (n4Day - 1) / 365;

		if (n4Yr != 0)
			n4Day = (n4Day - 1) % 365;
	}
	else
	{
		n4Yr = n4Day / 365;
		n4Day %= 365;
	}

	// n4Day is now 0-based day of year. Save 1-based day of year, year number
	tm.tm_yday = (int)n4Day + 1;
	tm.tm_year = n400Years * 400 + n400Century * 100 + n4Years * 4 + n4Yr;

	// Handle leap year: before, on, and after Feb. 29.
	if (n4Yr == 0 && bLeap4)
	{
		// Leap Year
		if (n4Day == 59)
		{
			/* Feb. 29 */
			tm.tm_mon = 2;
			tm.tm_mday = 29;
			goto DoTime;
		}

		// Pretend it's not a leap year for month/day comp.
		if (n4Day >= 60)
			--n4Day;
	}

	// Make n4DaY a 1-based day of non-leap year and compute
	//  month/day for everything but Feb. 29.
	++n4Day;

	// Month number always >= n/32, so save some loop time */
	for (tm.tm_mon = (n4Day >> 5) + 1;
		n4Day > rgMonthDays[tm.tm_mon]; tm.tm_mon++);

	tm.tm_mday = (int)(n4Day - rgMonthDays[tm.tm_mon-1]);

DoTime:
	if (nSecsInDay == 0)
		tm.tm_hour = tm.tm_min = tm.tm_sec = 0;
	else
	{
		tm.tm_sec = (int)nSecsInDay % 60L;
		nMinutesInDay = nSecsInDay / 60L;
		tm.tm_min = (int)nMinutesInDay % 60;
		tm.tm_hour = (int)nMinutesInDay / 60;
	}

	m_time = tm;
}

// Format the date into a string.    
AffxString AffxTime::getTimeStampString(void)
{
	AffxString str;

	if (!isEmpty())
	{
		int iMonth = getMonth();
		int iDay = getDay();
		int iYear = getYear();
		int iHour = getHour();
		int iMinute = getMinute();
		int iSecond = getSecond();

		char szBuffer[64];
		sprintf(szBuffer, "%02d/%02d/%d %02d:%02d:%02d", iMonth, iDay, iYear, iHour, iMinute, iSecond);
		str = szBuffer;
	}

	return str;
}
 
// Format the date into a string.    
AffxString AffxTime::getDateString(void)
{
	AffxString str;

	if (!isEmpty())
	{
		int iMonth = getMonth();
		int iDay = getDay();
		int iYear = getYear();

		char szBuffer[64];
		sprintf(szBuffer, "%d/%d/%d", iMonth, iDay, iYear);
		str = szBuffer;
	}

	return str;
}
 
// Format the time into a string.    
AffxString AffxTime::getHourMinuteString(void)
{
	AffxString str;
	char szBuffer[64];
	AffxString strAmPm = " AM";

	if (!isEmpty())
	{
		int iHour = getHour();
		int iMinute = getMinute();

		if (iHour >= 12)	//pjo 13-Jan-97 equals sign added
			strAmPm = " PM";
		
		if (iHour >= 13)	//pjo 13-Jan-97 hour adjusted at 1 PM
			iHour -= 12;

		if (iHour == 0)
			iHour = 12;

		sprintf(szBuffer, "%d:", iHour);
		str = szBuffer;
		sprintf(szBuffer, "%2d", iMinute);
		if (szBuffer[0] == ' ')
			szBuffer[0] = '0';
		str += szBuffer;
		str += strAmPm;

		return str;
	}

	return str;
}

