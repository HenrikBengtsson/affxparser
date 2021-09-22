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
 * @file   SnpTable.h
 * @author Chuck Sugnet
 * @date   Thu Feb  9 16:29:28 2006
 * 
 * @brief Big table of genotype data. Rows are snp probesets, columns are
 * experiments. Entries are -1 = NN, 0 = AA, 1 = AB, 2 = BB 
 */

#ifndef SNPTABLE_H
#define SNPTABLE_H

#include "file/TsvFile/TsvFile.h"
//
#include <cassert>
//

using namespace affx;
class SnpTable {

public:

  const static int NN = -1;
  const static int AA = 0;
  const static int AB = 1;
  const static int BB = 2;

  /** 
   * Read in genotypes from a file that looks like
   * probeset_id [tab] chip1 [tab] chip2 [tab]... chipN
   * SNP_A-1889420 [tab] -1 [tab] 0 [tab] 2 [tab] ... 1
   *
   * Entries are -1 = NN, 0 = AA, 1 = AB, 2 = BB 
   * @param fileName - name of file to open.
   */
  void open(const std::string& fileName);

  /** @brief     Return the number of rows in the file
   *  @return    rows
   */
  int getNumRows();

  /** @brief     Return the number of columns in the file.
   *  @return    columns
   */
  int getNumCols();


  /** @brief     Get the name of row rowIx
   *  @param     rowIx     
   *  @return    the name
   */
  const std::string &getRowName(int rowIx);


  /** @brief  Get the name of column colIx
   *  @param  colIx
   *  @return the name
   */
  const std::string &getColName(int colIx);

  /** 
   * Find the row index associated with a particular identified, -1 if
   * not found.
   * 
   * @param s - row identifier.
   * @return - index if found, -1 otherwise.
   */
  int getRowIndex(const std::string &s);

  /** 
   * Find the col index associated with a particular identified, -1 if
   * not found.
   * 
   * @param s - col identifier.
   * @return - index if found, -1 otherwise.
   */
  int getColIndex(const std::string &s);

  /** 
   * Get the genotype for the particular row and index of the genotype matrix.
   * 
   * @param rowIndex - Row of interest.
   * @param colIndex - Column of interest.
   * @return the genotype requested.
   */
  int getGenotypeForSnp(int rowIndex, int colIndex);

private:
  /// Unique identifiers (usually probeset ids) associated with each row.
  std::vector<std::string> m_RowNames;        
  /// Unique identifiers (usually chip names) associated with each column.
  std::vector<std::string> m_ColNames;
  /// Map of probeset ids to the index of the row that contains the
  /// data for that snp
  std::map<std::string, int> m_RowNameMap;
  /// Map of probeset ids to the index of the column that contains the
  /// data for that chip
  std::map<std::string, int> m_ColNameMap;
  /// Matrix of genotypes rows are snps, columns are the chips.
  std::vector<std::vector<int> > m_GTypes;

};

#endif /* SNPTABLE_H */
