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
 * @file   SnpTable.cpp
 * @author Chuck Sugnet
 * @date   Thu Feb  9 16:29:28 2006
 *
 * @brief Big table of genotype data. Rows are snp probesets, columns are
 * experiments. Entries are -1 = NN, 0 = AA, 1 = AB, 2 = BB
 */


#include "file/TsvFile/SnpTable.h"

using namespace affx;

/**
 * Read in genotypes from a file that looks like
 * probeset_id [tab] chip1 [tab] chip2 [tab]... chipN
 * SNP_A-1889420 [tab] -1 [tab] 0 [tab] 2 [tab] ... 1
 *
 * Entries are -1 = NN, 0 = AA, 1 = AB, 2 = BB
 * @param fileName - name of file to open.
 */
void SnpTable::open(const std::string& fileName)
{
  affx::TsvFile tsv;
  std::string snpName;
  std::vector<int> gTypes;
  tsv.open(fileName);
  int colCount = tsv.getColumnCount(0);
  tsv.bind(0, "probeset_id", &snpName);
  gTypes.resize(colCount - 1);
  /* Bind up the variables. */
  for (int i = 1; i < colCount; i++) {
    std::string s;
    tsv.cidx2cname(0, i, s);
    m_ColNames.push_back(s);
    tsv.bind(0, i, &gTypes[i-1]);
    m_ColNameMap[s] = i - 1;
  }
  int count = 0;
  while (tsv.nextLevel(0) == TSV_OK) {
    m_RowNames.push_back(snpName);
    m_GTypes.push_back(gTypes);
    m_RowNameMap[snpName] = count++;
  }
  tsv.close();
}

/** @brief     Return the number of rows in the file
 *  @return    rows
 */
int SnpTable::getNumRows()
{
  return (int)m_GTypes.size();
}

/** @brief     Return the number of columns in the file.
 *  @return    columns
 */
int SnpTable::getNumCols()
{
  return m_GTypes.empty() ? 0 : (int)m_GTypes[0].size();
}


/** @brief     Get the name of row rowIx
 *  @param     rowIx
 *  @return    the name
 */
const std::string &SnpTable::getRowName(int rowIx)
{
  assert(rowIx < (int)m_RowNames.size());
  return m_RowNames[rowIx];
}


/** @brief  Get the name of column colIx
 *  @param  colIx
 *  @return the name
 */
const std::string &SnpTable::getColName(int colIx)
{
  assert(colIx < (int)m_ColNames.size());
  return m_ColNames[colIx];
}

/**
 * Find the row index associated with a particular identified, -1 if
 * not found.
 *
 * @param s - row identifier.
 * @return - index if found, -1 otherwise.
 */
int SnpTable::getRowIndex(const std::string &s)
{
  std::map<std::string, int>::iterator iter = m_RowNameMap.find(s);
  if (m_RowNameMap.end() == iter)
    return -1;
  else
    return iter->second;
}

/**
 * Find the col index associated with a particular identified, -1 if
 * not found.
 *
 * @param s - col identifier.
 * @return - index if found, -1 otherwise.
 */
int SnpTable::getColIndex(const std::string &s)
{
  std::map<std::string, int>::iterator iter = m_ColNameMap.find(s);
  if (m_ColNameMap.end() == iter)
    return -1;
  else
    return iter->second;
}

/**
 * Get the genotype for the particular row and index of the genotype matrix.
 *
 * @param rowIndex - Row of interest.
 * @param colIndex - Column of interest.
 * @return the genotype requested.
 */
int SnpTable::getGenotypeForSnp(int rowIndex, int colIndex)
{
  assert(rowIndex < (int)m_GTypes.size());
  assert(colIndex < (int)m_GTypes[0].size());
  return m_GTypes[rowIndex][colIndex];
}

