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
 * @file   ClfFile.h
 * @brief  Headers for the CLF file class
 *         Read \link file-format-clf the ClfFile format \endlink for an overview.
 *         Read \link file-format-tsv the TsvFile docs \endlink more about TsvFile.
 */

#ifndef _CLF_FILE_H
#define _CLF_FILE_H

//
#include "file/TsvFile/TsvFile.h"
//
#include "util/Err.h"
//

namespace affx {
  class ClfFile;
};

/// To be called a CLF file, the file requires
class affx::ClfFile {
public:
  affx::TsvFile m_tsv; ///< public so people can work with it

  /// If you want the string forms, use "clf->tsv->get(0,"probe_id",probe_id_string)"
  //#%header0=probe_id	x	y
  int probe_id; ///< The probe id
  int x;        ///< The x position of the probe
  int y;        ///< The y position of the probe

  /// @brief     Create the ClfFile object
  ClfFile();
  /// @brief     Destroy a ClfFile object
  ~ClfFile();

  ///
  int getXMax();

  int getYMax();

  /// @brief     Return the value of the sequential header
  /// @return    int value of the header or -1 if not present
  int getSequential();

  /// @brief     Return the value of the order header
  /// @return    the value of the order header if present
  std::string getOrder();

  /// @brief     Make the bindings which define a CLF file
  void makeClfBindings();

  /// @brief     Open a CLF file
  /// @param     filename     the pathname of the file
  /// @return    tsv_return_t
  int open(const std::string& filename);

  /// @brief     Start the write to a file
  /// @param     filename
  /// @return    tsv_return_t
  int write(const std::string& filename);

  /// @brief     Close a PGF file
  /// @return    tsv_return_t
  int close();

  /// @brief     Rewind back to the start of the CLF data
  /// @return    tsv_return_t
  int rewind();

  /// @brief     Skip to the next probe
  /// @return    TSV_LEVEL_LAST when out of probesets
  int next_probe();
  /// @brief     Define a tsv file as a CLF file.
  /// @param     tsv       pointer to tsv to define
  /// @return    tsv_return_t
  int defineFileClf(TsvFile* tsv);
  /// @brief     Define this file as a pgf file
  /// @return    tsv_return_t
  int defineFileClf();

};

#endif // TsvFile
