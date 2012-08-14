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

/**
 * @file   Except.h
 * @author Chuck Sugnet
 * @date   Tue Nov  8 10:55:45 2005
 * 
 * @brief  General purpose exception for error handling.
 */

#ifndef EXCEPT_H
#define EXCEPT_H

#include <cstring>
#include<stdexcept>
#include<string>
//

/**
 * @brief General purpose exception error for error handling.
 */
class Except : public std::exception {

public:
  /** 
   * @brief Constructor.
   * @param msg Message for the user/programmer.
   */
  Except(const std::string &msg) : m_Msg(msg) {}

  /** 
   * @brief virtual destructor for a base class.
   * @return 
   */
  virtual ~Except() throw() {}

  /** 
   * @brief Standard what() call, just returns error message. 
   * @return error message.
   */
  virtual const char* what() const throw() {
    return m_Msg.c_str();
  }

private:
  /// Our error message.
  std::string m_Msg;
};

#endif /* EXCEPT_H */
