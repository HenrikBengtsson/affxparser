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
 * \file  PgfFile.h
 * \brief Headers for the PgfFile class.
 *        Read \link file-format-pgf the PgfFile format \endlink for an overview.
 *        Read \link file-format-tsv the TsvFile docs \endlink more about TsvFile.
 */


#ifndef _PGF_FILE_H
#define _PGF_FILE_H

#include "file/TsvFile/TsvFile.h"
//
#include "util/Err.h"
//

namespace affx {
  class PgfFile;
};

/// This defines an object which has fields and some methods
/// to make working with PGF files a snap.  Basicly we define
/// user friendly method which map to TsvFile mothods.
class affx::PgfFile {
public:
#ifndef SWIG
  /// If you want to work with the underling tsv: "pgf.m_tsv.method()"
  affx::TsvFile m_tsv;
#endif
  // There isnt a "m_" prefix on these, as I want to seperate them out
  // from the "real" member variables. This will make it
  // easy for people to write "pgf.atom_id".
  // (No annoying prefix.)

  //#%header0=
  int probeset_id;            ///< pgf
  std::string probeset_type;  ///< pgf
  std::string probeset_name;  ///< pgf
  //#%header1=
  int atom_id;                ///< pgf
  //#%header2=
  int probe_id;               ///< pgf
  std::string probe_type;     ///< pgf
  int gc_count;               ///< pgf
  int probe_length;           ///< pgf
  int interrogation_position; ///< pgf
  std::string probe_sequence; ///< pgf
  int exon_position;          ///< pgf

  /// @brief     Create the PgfFile object
  PgfFile();

  /// @brief     Destroy a PgfFile object
  ~PgfFile();

  /// @brief     Make the normal bindings to the standard PGF slots
  void makePgfBindings();
  /// @brief     Open a PGF file
  /// @param     filename the pathname of the file
  /// @return    tsv_return_t
  int open(const std::string& filename);
  /// @brief     start the write of a PGF file.
  /// @param     filename
  /// @return    tsv_return_t
  int write(const std::string& filename);
  /// @brief     Close a PGF file
  /// @return    tsv_return_t
  int close();
  /// @brief     Rewind back to the start of the PGF data
  /// @return    tsv_return_t
  int rewind();
  /// @brief     Skip to the next probeset
  /// @return    TSV_LEVEL_LAST when out of probesets
  int next_probeset();
  /// @brief     Skip to the next atom, but not beyond
  /// @return    TSV_LEVEL_LAST when out of atoms
  int next_atom();
  /// @brief     Skip to the next probe, but not beyond
  /// @return    TSV_LEVEL_LAST when out of probes
  int next_probe();
  /// @brief     Define a tsv file as a PGF file.
  /// @param     tsv       pointer to tsv to define
  /// @return    tsv_return_t
  int defineFilePgf(TsvFile* tsv);
  /// @brief     Define this file as a pgf file
  /// @return    tsv_return_t
  int defineFilePgf();

  /// @brief     Get a ptr for SWIG
  /// @return    a pointer to the internal TSV file object
  affx::TsvFile* tsv_ptr();
};

#endif // TsvFile
