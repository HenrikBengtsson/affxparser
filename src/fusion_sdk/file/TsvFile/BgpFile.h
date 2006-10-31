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



/// @file   BgpFile.h
/// @brief  Definition of the BGP file profile

#ifndef _BGP_FILE_H
#define _BGP_FILE_H

//
#include <assert.h>
//
#include "TsvFile.h"

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
  BgpFile() {
    makeBgpBindings();
  }
  /// @brief     Destroy a BgpFile object
  ~BgpFile() {
  }

  /// @brief     Make the bindings which define a BGP file
  void makeBgpBindings() {
    m_tsv.unbindAll();
    //
    m_tsv.bind(0,"probe_id",&probe_id,TSV_BIND_REQUIRED);
    m_tsv.bind(0,"probeset_id",&probeset_id_1);
    m_tsv.bind(0,"atom_id",&atom_id);
    m_tsv.bind(0,"exon_position",&exon_position);
    m_tsv.bind(0,"gc_count",&gc_count);
    m_tsv.bind(0,"interrogation_position",&interrogation_position);
    m_tsv.bind(0,"probe_length",&probe_length);
    m_tsv.bind(0,"probe_sequence",&probe_sequence);
    //m_tsv.bind(0,8,&probeset_id_2);
    m_tsv.bind(0,"probeset_name",&probeset_name);
    m_tsv.bind(0,"type",&type);
    m_tsv.bind(0,"x",&x);
    m_tsv.bind(0,"y",&y);
  }

  /// @brief     Open a BGP file
  /// @param     filename     the pathname of the file
  /// @returns   tsv_return_t
  int open(std::string filename) {
    return m_tsv.open(filename);
  }
  /// @brief     Start the write to a file
  /// @param     filename
  /// @returns   tsv_return_t
  int write(std::string filename) {
    return m_tsv.writeTsv(filename);
  }
  /// @brief     Close a PGF file
  /// @returns   tsv_return_t
  int close() {
    return m_tsv.close();
  }
  /// @brief     Rewind back to the start of the BGP data
  /// @returns   tsv_return_t
  int rewind() {
    return m_tsv.rewind();
  }
  /// @brief     Skip to the next probe
  /// @returns   TSV_LEVEL_LAST when out of probesets
  int next_bgprobe() {
    return m_tsv.nextLevel(0);
  }
  /// @brief     Define a tsv file as a BGP file.
  /// @param     tsv       pointer to tsv to define
  /// @returns   tsv_return_t
  int defineFileBgp(TsvFile* tsv) {
    assert(tsv!=NULL);
    return tsv->defineFile("probe_id\tprobeset_id\tatom_id\texon_position\t"
                           "gc_count\tinterrogation_position\tprobe_length\t"
                           "probe_sequence\tprobeset_id\tprobeset_name\ttype\tx\ty");
  }
  /// @brief     Define this file as a pgf file
  /// @returns   tsv_return_t
  int defineFileBgp() {
    return defineFileBgp(&m_tsv);
  }
};

#endif // TsvFile
