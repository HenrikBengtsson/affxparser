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

#ifndef _AffxTime_h_
#define _AffxTime_h_

#include "util/AffxString.h"
//
#include <ctime>
//

/////////////////////////////////////////////////////////////////////////////
// AffxTimeSpan and AffxTime

class AffxTime;

class AffxTimeSpan
{
public:

// Constructors
	AffxTimeSpan();
	AffxTimeSpan(time_t time);
	AffxTimeSpan(int lDays, int nHours, int nMins, int nSecs);
	AffxTimeSpan(double dt);

	AffxTimeSpan(const AffxTimeSpan& timeSpanSrc);
	const AffxTimeSpan& operator=(const AffxTimeSpan& timeSpanSrc);

// Attributes
	// extract parts
	void empty(void);
	double getDATE(void);

	int getDays() const;   // total # of days
	int getTotalHours() const;
	int getHours() const;
	int getTotalMinutes() const;
	int getMinutes() const;
	int getTotalSeconds() const;
	int getSeconds() const;

// Operations
	// time math
	AffxTimeSpan operator-(AffxTimeSpan timeSpan) const;
	AffxTimeSpan operator+(AffxTimeSpan timeSpan) const;
	const AffxTimeSpan& operator+=(AffxTimeSpan timeSpan);
	const AffxTimeSpan& operator-=(AffxTimeSpan timeSpan);
	bool operator==(AffxTimeSpan timeSpan) const;
	bool operator!=(AffxTimeSpan timeSpan) const;
	bool operator<(AffxTimeSpan timeSpan) const;
	bool operator>(AffxTimeSpan timeSpan) const;
	bool operator<=(AffxTimeSpan timeSpan) const;
	bool operator>=(AffxTimeSpan timeSpan) const;

	AffxString format(char* pFormat) const;

private:
	time_t m_timeSpan;
	friend class AffxTime;
};

class AffxTime
{
public:
	static AffxTime getCurrentTime();
	static AffxString getRuntime(AffxString strAction, double timeElapsed);
	static void startTime();
	static AffxString getRuntime(AffxString strAction);

    static long m_lStartTime;
    static long m_lStopTime;

public:
// Constructors
	AffxTime();
	AffxTime(time_t time);
	AffxTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec, int nDST = -1);
	AffxTime(double dtSrc);
	AffxTime(const AffxTime& timeSrc);
	const AffxTime& operator=(const AffxTime& timeSrc);

	void clear(void) {empty();} // No memory allocation to worry about.
	void empty(void);
	bool isEmpty(void) {return getDATE() == 0;}
	bool isClear(void) {return isEmpty();}

// Attributes
	double getDATE(void);
	time_t getTime();
	int getYear() const;
	int getMonth() const;       // month of year (1 = Jan)
	int getDay() const;         // day of month
	int getHour() const;
	int getMinute() const;
	int getSecond() const;
	int getDayOfWeek() const;   // 1=Sun, 2=Mon, ..., 7=Sat

// Operations
	// time math
	AffxTimeSpan operator-(AffxTime time);
	AffxTime operator-(AffxTimeSpan timeSpan);
	AffxTime operator+(AffxTimeSpan timeSpan);
	const AffxTime& operator+=(AffxTimeSpan timeSpan);
	const AffxTime& operator-=(AffxTimeSpan timeSpan);
	bool operator==(AffxTime time);
	bool operator!=(AffxTime time);
	bool operator<(AffxTime time);
	bool operator>(AffxTime time);
	bool operator<=(AffxTime time);
	bool operator>=(AffxTime time);

	// formatting using "C" strftime
	AffxString format(char* pFormat);
	AffxString formatGmt(char* pFormat);
	
	AffxString getDateString(void);
	AffxString getHourMinuteString(void);
	AffxString getTimeStampString(void);

private:
	tm m_time;
};

// AffxTime and AffxTimeSpan
inline AffxTimeSpan::AffxTimeSpan(time_t time)
	{ m_timeSpan = time; }
inline AffxTimeSpan::AffxTimeSpan(int lDays, int nHours, int nMins, int nSecs)
	{ m_timeSpan = nSecs + 60* (nMins + 60* (nHours + 24* lDays)); }
inline AffxTimeSpan::AffxTimeSpan(const AffxTimeSpan& timeSpanSrc)
	{ m_timeSpan = timeSpanSrc.m_timeSpan; }
inline const AffxTimeSpan& AffxTimeSpan::operator=(const AffxTimeSpan& timeSpanSrc)
	{ m_timeSpan = timeSpanSrc.m_timeSpan; return *this; }
inline int AffxTimeSpan::getDays() const
	{ return (int)m_timeSpan / (24*3600L); }
inline int AffxTimeSpan::getTotalHours() const
	{ return (int)m_timeSpan/3600; }
inline int AffxTimeSpan::getHours() const
	{ return (int)(getTotalHours() - getDays()*24); }
inline int AffxTimeSpan::getTotalMinutes() const
	{ return (int)m_timeSpan/60; }
inline int AffxTimeSpan::getMinutes() const
	{ return (int)(getTotalMinutes() - getTotalHours()*60); }
inline int AffxTimeSpan::getTotalSeconds() const
	{ return (int)m_timeSpan; }
inline int AffxTimeSpan::getSeconds() const
	{ return (int)(getTotalSeconds() - getTotalMinutes()*60); }
inline AffxTimeSpan AffxTimeSpan::operator-(AffxTimeSpan timeSpan) const
	{ return AffxTimeSpan(m_timeSpan - timeSpan.m_timeSpan); }
inline AffxTimeSpan AffxTimeSpan::operator+(AffxTimeSpan timeSpan) const
	{ return AffxTimeSpan(m_timeSpan + timeSpan.m_timeSpan); }
inline const AffxTimeSpan& AffxTimeSpan::operator+=(AffxTimeSpan timeSpan)
	{ m_timeSpan += timeSpan.m_timeSpan; return *this; }
inline const AffxTimeSpan& AffxTimeSpan::operator-=(AffxTimeSpan timeSpan)
	{ m_timeSpan -= timeSpan.m_timeSpan; return *this; }
inline bool AffxTimeSpan::operator==(AffxTimeSpan timeSpan) const
	{ return m_timeSpan == timeSpan.m_timeSpan; }
inline bool AffxTimeSpan::operator!=(AffxTimeSpan timeSpan) const
	{ return m_timeSpan != timeSpan.m_timeSpan; }
inline bool AffxTimeSpan::operator<(AffxTimeSpan timeSpan) const
	{ return m_timeSpan < timeSpan.m_timeSpan; }
inline bool AffxTimeSpan::operator>(AffxTimeSpan timeSpan) const
	{ return m_timeSpan > timeSpan.m_timeSpan; }
inline bool AffxTimeSpan::operator<=(AffxTimeSpan timeSpan) const
	{ return m_timeSpan <= timeSpan.m_timeSpan; }
inline bool AffxTimeSpan::operator>=(AffxTimeSpan timeSpan) const
	{ return m_timeSpan >= timeSpan.m_timeSpan; }

inline const AffxTime& AffxTime::operator=(const AffxTime& timeSrc)
	{ m_time = timeSrc.m_time; return *this; }
inline int AffxTime::getYear() const
	{ return (m_time.tm_year); }
inline int AffxTime::getMonth() const
	{ return m_time.tm_mon; }
inline int AffxTime::getDay() const
	{ return m_time.tm_mday; }
inline int AffxTime::getHour() const
	{ return m_time.tm_hour; }
inline int AffxTime::getMinute() const
	{ return m_time.tm_min; }
inline int AffxTime::getSecond() const
	{ return m_time.tm_sec; }
inline int AffxTime::getDayOfWeek() const
	{ return m_time.tm_wday + 1; }
inline AffxTimeSpan AffxTime::operator-(AffxTime time) 
	{ return getDATE() - time.getDATE(); }
inline AffxTime AffxTime::operator-(AffxTimeSpan timeSpan)
	{ return AffxTime(getDATE() - timeSpan.getDATE()); }
inline AffxTime AffxTime::operator+(AffxTimeSpan timeSpan)
	{ return AffxTime(getDATE() + timeSpan.getDATE()); }
inline const AffxTime& AffxTime::operator+=(AffxTimeSpan timeSpan)
	{ *this = getDATE() + timeSpan.getDATE(); return *this; }
inline const AffxTime& AffxTime::operator-=(AffxTimeSpan timeSpan)
	{ *this = getDATE() - timeSpan.getDATE(); return *this; }
inline bool AffxTime::operator==(AffxTime time) 
	{ return getDATE() == time.getDATE(); }
inline bool AffxTime::operator!=(AffxTime time) 
	{ return getDATE() != time.getDATE(); }
inline bool AffxTime::operator<(AffxTime time) 
	{ return getDATE() < time.getDATE(); }
inline bool AffxTime::operator>(AffxTime time)
	{ return getDATE() > time.getDATE(); }
inline bool AffxTime::operator<=(AffxTime time) 
	{ return getDATE() <= time.getDATE(); }
inline bool AffxTime::operator>=(AffxTime time) 
	{ return getDATE() >= time.getDATE(); }


#endif // _AffxTime_h_

