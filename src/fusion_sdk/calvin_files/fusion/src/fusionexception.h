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


#ifndef _FusionException_HEADER_
#define _FusionException_HEADER_

#include "calvin_files/exception/src/ExceptionBase.h"
//

/*! \file fusionexception.h This file defines exceptions from the fusion layer.
 */

namespace affymetrix_fusion_exceptions
{

/*! System did not recognize the requested fusion writer */
class UnknownFusionWriterException : public affymetrix_calvin_exceptions::CalvinException
{
};

/*! System did not recognize the requested fusion DAT data class */
class UnknownFusionDatDataException : public affymetrix_calvin_exceptions::CalvinException
{
};

}

#endif
