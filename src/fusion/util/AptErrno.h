////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011 Affymetrix, Inc.
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
// k/apt-1/affy/sdk/util/AptErrno.h ---
//
// $Id$
//

#ifndef _UTIL_APTERRNO_H_
#define _UTIL_APTERRNO_H_

/// Return codes for methods.
enum AptErr_t {
  // dont use negative values as some systems mess up the signs.
  APT_OK=0,
  // a generic error, for when there isnt a more specific one.
  APT_ERR=1000,
  //
  APT_ERR_ISNULL,
  APT_ERR_NOTFOUND,
  APT_ERR_OUTOFBOUNDS,
  //
  APT_ERR_WRONGTYPE,
  //
  APT_ERR_ACCESS,
  APT_ERR_EXISTS,
  APT_ERR_FS_PERM,
  APT_ERR_FS_STAT,
  APT_ERR_NOTEXISTS,
  APT_ERR_NOTREMOVED,
};

#endif // _UTIL_APTERRNO_H_
