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
 * \file  BgpFile.h
 * \brief Definition of the BGP file profile
 *        Read \link file-format-tsv the TsvFile docs \endlink more about TsvFile.
 */

#ifndef _BGP_FILE_H
#define _BGP_FILE_H

//
#include "file/TsvFile/TsvFile.h"
//
#include <cassert>
//

// declare the namespace
namespace affx {
  class BgpFile;
};

/// To be called a BGP file, the file requires
class affx::BgpFile {
public:
  affx::TsvFile m_tsv; ///< public so people can work with it

  //#%header0=probset_id probe_id
  int         probe_id;
	int         probeset_id_1;
  int         atom_id;
  int         exon_position;
  int         gc_count;
	int         interrogation_position;
	int         probe_length;
  std::string probe_sequence;
  int         probeset_id_2;
	std::string probeset_name;
	std::string type;
	int         x;
	int         y;

  /// @brief     Create the BgpFile object
  BgpFile();
  /// @brief     Destroy a BgpFile object
  ~BgpFile();

  /// @brief     Clears all the member vars.
  void clear();

  /// @brief     Make the bindings which define a BGP file
  void makeBgpBindings();

  /// @brief     Open a BGP file
  /// @param     filename     the pathname of the file
  /// @returns   tsv_return_t
  int open(const std::string& filename);

  /// @brief     Start the write to a file
  /// @param     filename
  /// @returns   tsv_return_t
  int write(const std::string& filename);
  /// @brief     Close a PGF file
  /// @returns   tsv_return_t
  int close();
  /// @brief     Rewind back to the start of the BGP data
  /// @returns   tsv_return_t
  int rewind();
  /// @brief     Skip to the next probe
  /// @returns   TSV_LEVEL_LAST when out of probesets
  int next_bgprobe();
  /// @brief     Define a tsv file as a BGP file.
  /// @param     tsv       pointer to tsv to define
  /// @returns   tsv_return_t
  int defineFileBgp(TsvFile* tsv);
  /// @brief     Define this file as a pgf file
  /// @returns   tsv_return_t
  int defineFileBgp();
};

#endif // TsvFile
