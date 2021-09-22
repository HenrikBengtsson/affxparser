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

#ifndef _AffxStatistics_h_
#define _AffxStatistics_h_
/**
 * @file AffxStatistics.h
 *
 * @brief This header contains the AffxStatistics class definition.
 */

#define PI 3.1415926536

#include "util/AffxMultiDimensionalArray.h"
//

/**
 * @brief  A class for statistical functions.
 *
 */
class AffxStatistics
{
protected:

public:
    AffxStatistics();
    virtual ~AffxStatistics();

    static double factorial(int i);
    static double binocdf(int x, int n, double p);
    static int binoinv(double y, int n, double p);
    static double normcdf(double u);
    static double norminv(double p);
};

#endif
