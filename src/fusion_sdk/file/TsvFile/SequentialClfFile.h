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
 * @file   SequentialClfFile.h
 * @brief  Class for coordinate calculation from a sequential clf file.
 */
#ifndef SEQUENTIAL_CLF_FILE_H
#define SEQUENTIAL_CLF_FILE_H

#include "file/TsvFile/TsvFile.h"
//
#include "util/Convert.h"
//
#include <cassert>
//

/**
 * Class for calculating probe coordinates using a sequential
 * clf file.
 */
class SequentialClfFile
{
public:
  /**
   * Constructor.
   */
  SequentialClfFile (void);

  /**
   * Check whether a clf file is sequential.
   *
   * @param clfTsv Clf file tsv object.
   * @return bool Returns true if the file is sequential, else false.
   */
  bool isSequential (affx::TsvFile& clfTsv);

  /**
   * Calculate the coordinates of a probe, given its id.
   *
   * @param probeId Probe id.
   * @param x Calculated x value.
   * @param y Calculated y value.
   */
  void getProbeCoordinates (const int probeId, int& x, int& y);

private:
  /// Number of rows.
  unsigned int m_Rows;
  /// Number of columns.
  unsigned int m_Cols;
};

#endif /* SEQUENTIAL_CLF_FILE_H */
