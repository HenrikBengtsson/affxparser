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


#include "file/TsvFile/SequentialClfFile.h"
//
#include <cassert>
//


/**
 * Constructor.
 */
SequentialClfFile::SequentialClfFile(void)
    : m_Rows(0), m_Cols(0)
{}

/**
 * Check whether a clf file is sequential.
 *
 * @param clfTsv Clf file tsv object.
 * @return bool Returns true if the file is sequential, else false.
 */
bool SequentialClfFile::isSequential(affx::TsvFile& clfTsv)
{
  std::string rowsValue, colsValue, sequentialValue;
  clfTsv.headersFindNext("rows", rowsValue);
  clfTsv.headersFindNext("cols", colsValue);
  clfTsv.headersFindNext("sequential", sequentialValue);
  // Require rows, cols, and a sequential value of 1.
  if (rowsValue.empty() || colsValue.empty() || sequentialValue != "1")
    return false;
  // Save rows, cols.
  m_Rows = Convert::toInt(rowsValue);
  m_Cols = Convert::toInt(colsValue);
  return true;
}

/**
 * Calculate the coordinates of a probe, given its id.
 *
 * @param probeId Probe id.
 * @param x Calculated x value.
 * @param y Calculated y value.
 */
void SequentialClfFile::getProbeCoordinates(const int probeId, int& x, int& y)
{
  // Probe id is one based, x and y are zero based.
  const int id = probeId - 1;
  x = id % m_Rows;
  assert(m_Cols > 0);
  y = id / m_Cols;
}

