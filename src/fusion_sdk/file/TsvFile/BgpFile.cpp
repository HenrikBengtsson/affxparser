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
 * \file  BgpFile.cpp
 * \brief Definition of the BGP file profile
 *        Read \link file-format-tsv the TsvFile docs \endlink more about TsvFile.
 */

/**
\page file-format-bgp File Format: BGP (NON-OFFICIAL-RELEASE)

<center><b>DRAFT</b></center>

The BGP (background probe file) lists which probes to use for background correction.
Currently this file is used with the PM-GCBG pm adjustment method implemented in
apt-probeset-summarize.

The BGP file is based on version 1 of the \link file-format-tsv TSV file format \endlink.

\section specs Specs & Features

- Required headers
  - chip_type: indicates the chip type (as stored in the CEL file) which the clf file
    describes. Multiple chip_type headers may be present indicating the clf file
    can be used with any one of the listed chip types.
  - lib_set_name: indicates the name of a collection of related library files for a given
    chip. For example, PGF and CLF files intended to be used together should have
    the same lib_set_name. Only a single lib_set_name is allowed.
  - lib_set_version: indicates the version of a collection of related library files for
    a given chip. For example, PGF and CLF files intended to be used together should have
    the same lib_set_version. Only a single lib_set_version is allowed.
- Optional headers
  - create_date: timestamp for when the file was created
  - guid: a unique identifier for the fileA
  - other headers may be present
- Required columns
  - probe_id: the ID of the probe to use for background correction
- Optional columns
  - probeset_id: The ID of the probeset in the PGF file to which the probe belongs. When
    using a bgp file with a CDF file, the probe ID is in column major order starting at
    1. See \link VIGNETTE-expression-mask-probes \endlink for more details.
  - probeset_type: See type field at probeset level in PGF documentation
  - atom_id: The ID of the atom to which the probe belongs to
  - probe_type: See type field at the probe level in PGF documentation
  - gc_count: The numbers of G's and C's in the probe
  - probe_length: The length of the probe in base pairs
  - probe_sequence: See the probe_sequence info in the PGF documentation
  - x: The column coordinate of the probe in the CEL file
  - y: The row coordinate of the probe in the CEL file
  - other columns may be present

\section implementation Parsing and Writing

The official C++ parser used by affy can be found in APT under
sdk/file/TsvFile/BgpFile.h. When possible, parsing and
writing of BGP files should be done using this code.

\section notes Notes

The optional fields listed above are frequently present. This is in part due to the fact that
apt-dump-pgf is used to generate the BGP files.

\section bgpExample1 Example 1

\verbatim
#%chip_type=HuEx-1_0-st-v2
#%chip_type=HuEx-1_0-st-v1
#%chip_type=HuEx-1_0-st-ta1
#%lib_set_name=HuEx-1_0-st
#%lib_set_version=r2
#%create_date=Tue Sep 19 15:18:05 PDT 2006
#%guid=0000008635-1158704285-2136082293-1007618462-0634896595
probeset_id probeset_type atom_id probe_id probe_type gc_count probe_length probe_sequence x y
4057134 control->bgp->antigenomic 70013 78973 pm:st 4 25 TATTAAAATTAGAGCTATTAACAAT 2172 30
4057134 control->bgp->antigenomic 70013 125043 pm:st 4 25 AACTTATCTAAATGCTATTAAAAAT 2162 48
4057134 control->bgp->antigenomic 70013 148149 pm:st 4 25 TGATATAATAAGCTAATTTGTTTAA 2228 57
4057134 control->bgp->antigenomic 70013 152379 pm:st 4 25 ATTATAATAGTTAGCAACTTAATAT 1338 59
4057134 control->bgp->antigenomic 70013 178557 pm:st 4 25 CATTTATTAGTATTAAAGCAAATAT 1916 69
4057134 control->bgp->antigenomic 70013 178785 pm:st 4 25 AATTATTAAAACAAGCTTATATATC 2144 69
\endverbatim

\section related Related Pages

- <a href="./file-format-tsv-notes.html">TsvFile Design Notes</a>
- <a href="./file-format-tsv.html">TSV File Format</a>

*/

#include "file/TsvFile/BgpFile.h"


// declare the namespacen
namespace affx
{

/// @brief     Create the BgpFile object
BgpFile::BgpFile()
{
  clear();
  makeBgpBindings();
}
/// @brief     Destroy a BgpFile object
BgpFile::~BgpFile()
{
}

void BgpFile::clear()
{
  probe_id=0;
  probeset_id_1=0;
  atom_id=0;
  exon_position=0;
  gc_count=0;
	interrogation_position=0;
	probe_length=0;
  probe_sequence="";
  probeset_id_2=0;
	probeset_name="";
	type="";
	x=0;
	y=0;
}

/// @brief     Make the bindings which define a BGP file
void BgpFile::makeBgpBindings()
{
  m_tsv.unbindAll();
  //
  m_tsv.bind(0, "probe_id", &probe_id, TSV_BIND_REQUIRED);
  m_tsv.bind(0, "probeset_id", &probeset_id_1);
  m_tsv.bind(0, "atom_id", &atom_id);
  m_tsv.bind(0, "exon_position", &exon_position);
  m_tsv.bind(0, "gc_count", &gc_count);
  m_tsv.bind(0, "interrogation_position", &interrogation_position);
  m_tsv.bind(0, "probe_length", &probe_length);
  m_tsv.bind(0, "probe_sequence", &probe_sequence);
  //m_tsv.bind(0,8,&probeset_id_2);
  m_tsv.bind(0, "probeset_name", &probeset_name);
  m_tsv.bind(0, "type", &type);
  m_tsv.bind(0, "x", &x);
  m_tsv.bind(0, "y", &y);
}

/// @brief     Open a BGP file
/// @param     filename     the pathname of the file
/// @returns   tsv_return_t
int BgpFile::open(const std::string& filename)
{
  return m_tsv.open(filename);
}
/// @brief     Start the write to a file
/// @param     filename
/// @returns   tsv_return_t
int BgpFile::write(const std::string& filename)
{
  return m_tsv.writeTsv(filename);
}
/// @brief     Close a PGF file
/// @returns   tsv_return_t
int BgpFile::close()
{
  return m_tsv.close();
}
/// @brief     Rewind back to the start of the BGP data
/// @returns   tsv_return_t
int BgpFile::rewind()
{
  return m_tsv.rewind();
}
/// @brief     Skip to the next probe
/// @returns   TSV_LEVEL_LAST when out of probesets
int BgpFile::next_bgprobe()
{
  return m_tsv.nextLevel(0);
}
/// @brief     Define a tsv file as a BGP file.
/// @param     tsv       pointer to tsv to define
/// @returns   tsv_return_t
int BgpFile::defineFileBgp(TsvFile* tsv)
{
  assert(tsv != NULL);
  return tsv->defineFile("probe_id\tprobeset_id\tatom_id\texon_position\t"
                         "gc_count\tinterrogation_position\tprobe_length\t"
                         "probe_sequence\tprobeset_id\tprobeset_name\ttype\tx\ty");
}
/// @brief     Define this file as a pgf file
/// @returns   tsv_return_t
int BgpFile::defineFileBgp()
{
  return defineFileBgp(&m_tsv);
}

};

