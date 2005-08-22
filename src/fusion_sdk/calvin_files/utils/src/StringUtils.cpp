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

#include "StringUtils.h"

using namespace affymetrix_calvin_utilities;

#pragma warning(disable: 4996) // ignore deprecated functions warning

/* 
 * Trim chTarget from the left of string s.
 */
void StringUtils::STLTrimLeft(std::string& s, char chTarget)
{
	std::string::size_type n = s.find_first_not_of( chTarget );
	if( n == std::string::npos )
		return;
	s = s.substr( n );
}

/* 
 * Trim space characters (' ') from the left of string s.
 */
void StringUtils::STLTrimLeft(std::string& s)
{
	STLTrimLeft( s, ' ' );
}

/* 
 * Trim chTarget from the right of string s.
 */
void StringUtils::STLTrimRight(std::string& s, char chTarget)
{
	std::string::size_type n = s.find_last_not_of( chTarget );
	if( n == std::string::npos )
		s.clear();
	else
		s = s.substr( 0, n+1 );
}

/* 
 * Trim space characters (' ') from the right of string s.
 */
void StringUtils::STLTrimRight(std::string& s)
{
	STLTrimRight( s, ' ' );
}

/* 
 * Trim chTarget from the left of string s.
 */
void StringUtils::STLTrimLeft(std::wstring& s, wchar_t chTarget)
{
	std::wstring::size_type n = s.find_first_not_of( chTarget );
	if( n == std::wstring::npos )
		return;
	s = s.substr( n );
}

/* 
 * Trim space characters (' ') from the left of string s.
 */
void StringUtils::STLTrimLeft(std::wstring& s)
{
	STLTrimLeft( s, ' ' );
}

/* 
 * Trim chTarget from the right of string s.
 */
void StringUtils::STLTrimRight(std::wstring& s, wchar_t chTarget)
{
	std::wstring::size_type n = s.find_last_not_of( chTarget );
	if( n == std::wstring::npos )
		s.clear();
	else
		s = s.substr( 0, n+1 );
}

/* 
 * Trim space characters (' ') from the right of string s.
 */
void StringUtils::STLTrimRight(std::wstring& s)
{
	STLTrimRight( s, ' ' );
}

/* 
 * Convert wide character string to a multi-byte character string.
 */
std::string StringUtils::ConvertWCSToMBS(const std::wstring& source)
{
	char* szSource = new char[ source.length()+1 ];
	wcstombs( szSource, source.c_str(), source.length()+1 );
	std::string result = szSource;
	delete [] szSource;
	return result;
}

/* 
 * Convert multi-byte character string to a wide character character string.
 */
std::wstring StringUtils::ConvertMBSToWCS(const std::string& source)
{
	wchar_t* szSource = new wchar_t[ source.length()+1 ];
	mbstowcs( szSource, source.c_str(), source.length()+1 );
	std::wstring result = szSource;
	delete [] szSource;
	return result;
}

