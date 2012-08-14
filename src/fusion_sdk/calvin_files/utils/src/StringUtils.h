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


#ifndef __STRINGUTILS_H__INCLUDED
#define __STRINGUTILS_H__INCLUDED

/*! \file StringUtils.h This file provides string utilities.
 */

#include <cstring>
#include <string>
#include <vector>
//

#ifdef __MINGW32__
#define USE_OLD_SWPRINTF
#endif

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

    /*! Convert an integer to a string.
     * @param value The integer
     * @param digits The number of digits for the resulting string
     * @param fill The value to fill in empty spaces.
     */
    static std::wstring ToString(int value, int digits, wchar_t fill=L'0');

    /*
     * Split a string using the separator as the delimiter
     * @param inputString The string to split
     * @param separator The delimiter
     */
    static std::vector<std::string> Split(const std::string &inputString, const std::string &separator);
};

}

    /*! Formats the data into a string.
     * @param buffer The buffer to fill.
     * @param count The size of the buffer.
     * @param format The format string.
     * @param value1 The first value to add.
     */
#ifdef USE_OLD_SWPRINTF
#define FormatString1(buffer, count, format, value1) swprintf(buffer, format, value1)
#else
#define FormatString1(buffer, count, format, value1) swprintf(buffer, count, format, value1)
#endif

    /*! Formats the data into a string.
     * @param buffer The buffer to fill.
     * @param count The size of the buffer.
     * @param format The format string.
     * @param value1 The first value to add.
     * @param value2 The second value to add.
     */
#ifdef USE_OLD_SWPRINTF
#define FormatString2(buffer, count, format, value1, value2) swprintf(buffer, format, value1, value2)
#else
#define FormatString2(buffer, count, format, value1, value2) swprintf(buffer, count, format, value1, value2)
#endif

    /*! Formats the data into a string.
     * @param buffer The buffer to fill.
     * @param count The size of the buffer.
     * @param format The format string.
     * @param value1 The first value to add.
     * @param value2 The second value to add.
     * @param value3 The third value to add.
     */
#ifdef USE_OLD_SWPRINTF
#define FormatString3(buffer, count, format, value1, value2, value3) swprintf(buffer, format, value1, value2, value3)
#else
#define FormatString3(buffer, count, format, value1, value2, value3) swprintf(buffer, count, format, value1, value2, value3)
#endif

    /*! Formats the data into a string.
     * @param buffer The buffer to fill.
     * @param count The size of the buffer.
     * @param format The format string.
     * @param value1 The first value to add.
     * @param value2 The second value to add.
     * @param value3 The third value to add.
     * @param value4 The fourth value to add.
     */
#ifdef USE_OLD_SWPRINTF
#define FormatString4(buffer, count, format, value1, value2, value3, value4) swprintf(buffer, format, value1, value2, value3, value4)
#else
#define FormatString4(buffer, count, format, value1, value2, value3, value4) swprintf(buffer, count, format, value1, value2, value3, value4)
#endif

    /*! Formats the data into a string.
     * @param buffer The buffer to fill.
     * @param count The size of the buffer.
     * @param format The format string.
     * @param value1 The first value to add.
     * @param value2 The second value to add.
     * @param value3 The third value to add.
     * @param value4 The fourth value to add.
     * @param value5 The fifth value to add.
     */
#ifdef USE_OLD_SWPRINTF
#define FormatString5(buffer, count, format, value1, value2, value3, value4, value5) swprintf(buffer, format, value1, value2, value3, value4, value5)
#else
#define FormatString5(buffer, count, format, value1, value2, value3, value4, value5) swprintf(buffer, count, format, value1, value2, value3, value4, value5)
#endif


#endif //__STRINGUTIL_H__INCLUDED
