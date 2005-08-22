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

#ifndef __STRINGUTILS_H__INCLUDED
#define __STRINGUTILS_H__INCLUDED

/*! \file StringUtils.h This file provides string utilities.
 */

#include <string>

namespace affymetrix_calvin_utilities
{

/*! This class provides utility functions for strings. */
class StringUtils
{
public:
	/*! Trim chTarget from the left of string s.
	 *  @param s String from which to remove characters.
	 *  @param chTarget Character to trim.
	 */
	static void STLTrimLeft(std::string& s, char chTarget);

	/*! Trim space characters (' ') from the left of string s.
	 *  @param s String from which to remove characters.
	 */
	static void STLTrimLeft(std::string& s);

	/*! Trim chTarget from the right of string s.
	 *  @param s String from which to remove characters.
	 *  @param chTarget Character to trim.
	 */
	static void STLTrimRight(std::string& s, char chTarget);

	/*! Trim space characters (' ') from the right of string s.
	 *  @param s String from which to remove characters.
	 */
	static void STLTrimRight(std::string& s);

	/*! Trim chTarget from the left of string s.
	 *  @param s String from which to remove characters.
	 *  @param chTarget Character to trim.
	 */
	static void STLTrimLeft(std::wstring& s, wchar_t chTarget);

	/*! Trim space characters (' ') from the left of string s.
	 *  @param s String from which to remove characters.
	 */
	static void STLTrimLeft(std::wstring& s);

	/*! Trim chTarget from the right of string s.
	 *  @param s String from which to remove characters.
	 *  @param chTarget Character to trim.
	 */
	static void STLTrimRight(std::wstring& s, wchar_t chTarget);

	/*! Trim space characters (' ') from the right of string s.
	 *  @param s String from which to remove characters.
	 */
	static void STLTrimRight(std::wstring& s);

	/*! Convert wide character string to a multi-byte character string.
	 *  @param source Wide character string to convert.
	 *  @return Converted multi-byte character string.
	 */
	static std::string ConvertWCSToMBS(const std::wstring& source);

	/*! Convert multi-byte character string to a wide character character string.
	 *  @param source Multi-byte character string to convert.
	 *  @return Converted wide character character string.
	 */
	static std::wstring ConvertMBSToWCS(const std::string& source);
};

}

#endif //__STRINGUTIL_H__INCLUDED