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

#include "calvin_files/utils/src/StringUtils.h"
//
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <vector>
//

#ifdef _MSC_VER
#pragma warning(disable: 4996) // ignore deprecated functions warning
#endif

using namespace affymetrix_calvin_utilities;

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

/*
 * Convert an integer to a string.
 */
std::wstring StringUtils::ToString(int value, int digits, wchar_t fill)
{
#if defined(_GLIBCPP_USE_WCHAR_T) || defined(_GLIBCXX_USE_WCHAR_T) || defined(_MSC_VER)
	std::wostringstream str;
	str << std::setw(digits) << std::setfill(fill) << value;
	return str.str();
#else
    throw "No implementation";
#endif
}

/*
 * Split a string using the separator as the delimiter
 */
std::vector<std::string> StringUtils::Split(const std::string &inputString, const std::string &separator)
{
	  std::vector<std::string> tokens;
      size_t substrBegin = 0;
      for (;;)
      {
            size_t substrEnd = inputString.find (separator, substrBegin);
			if (substrEnd == std::string::npos)
            {
                  // No more ',' - save what's left, quit.
				  std::string subString = inputString.substr (substrBegin);
                  // Avoid returning a null string from a terminating ',' or an empty inputString.
                  if (! subString.empty())
                        tokens.push_back (subString);
                  break;
            }
            // Avoid null strings from an initial ',' or ',,'.
            if (substrEnd != substrBegin)
                  tokens.push_back (inputString.substr (substrBegin, substrEnd - substrBegin) );
            // Continue following the ','
            substrBegin = substrEnd + 1;
      }
      return tokens;
}

