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

#ifndef _TemplateAuditActionTypes_HEADER_
#define _TemplateAuditActionTypes_HEADER_

/*! \file TemplateAuditActionTypes.h Defines constants for various template file audit actions.
 */

#include "AffymetrixBaseTypes.h"

namespace affymetrix_calvin_array
{

/*! Initial creation of a template file. */
static const int32_t TEMPLATE_FILE_CREATEION_ACTION = 1001;

/*! Attribute update action. */
static const int32_t TEMPLATE_FILE_ATTRIBUTE_UPDATE_ACTION = 1002;

};

#endif // _TemplateAuditActionTypes_HEADER_
