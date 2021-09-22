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
 * @file   ClfFile.cpp
 * @brief
 *         Read \link file-format-clf the ClfFile format \endlink for an overview.
 *         Read \link file-format-tsv the TsvFile docs \endlink more about TsvFile.
 */

#include "file/TsvFile/ClfFile.h"

/**
\page file-format-clf File Format: CLF (NON-OFFICIAL-RELEASE)

<center><b>DRAFT</b></center>

The CLF (cel layout file) maps probe IDs to a particular location
in the CEL file. The CLF file is based on version 2 of the
<a href="./file-format-tsv.html">TSV file format</a>.

The CLF file is based on version 1 of the \link file-format-tsv TSV file format \endlink.

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
  - clf_format_version: currently the only documented and supported version is 1.0.
  - rows: the number of rows in the CEL file (1 base)
  - cols: the number of cols in the CEL file (1 base)
  - header0: indicates the values in the CLF file. A CLF file must contain
    probe_id, x, and y columns. The probe_id column must be unique. Other
    columns may be present.
- Optional headers
  - sequential: When present, this header indicates that the relationship between
    probe IDs and x/y location is deterministic. The value of this key is the
    starting ID. See note below about Deterministic IDs. If sequential is
    specified, then the order header must also be supplied. The value must be
    an integer >= 0.
  - order: Indicates whether the IDs are enumerated in a column or row major order.
    Valid values are "col_major" and "row_major".  If order is specified, then
    sequential must also be specified.
  - create_date: timestamp for when the file was created
  - guid: a unique identifier for the fileA
  - other headers may be present
- Required columns
  - per TSV format, order of columns is not guarateed
  - per TSV format, additional columns may be present
  - probe_id: A positive integer ID for the probe. Must be unique within the CLF file.
  - x: the x position of the probe in the CEL file. (0 base)
  - y: the y position of the probe in the CEL file. (0 base)

\section deterministic Deterministic IDs

In many cases, probe IDs are simply enumerated starting with a fixed number by walking
through the CEL file in a column or row major order. When this is the case, hints may
be provided in the headers which allow the location to be computed from the probe_id.
This saves the work of reading the file.
When sequential is specified along with col_major order, the the following determines
probe IDs:

\code
probe_id = y*cols + x + sequential
\endcode

And for row_major:

\code
probe_id = x*rows + y + sequential
\endcode

\section implementation Parsing and Writing

The official C++ parser used by affy can be found in APT under
sdk/file/TsvFile/ClfFile.h. When possible, parsing and
writing of CLF files should be done using this code.

\section notes Notes

ChipLayout used by APT (in sdk/chipstream) will currently only
accept CLF files for which:

- sequential=1
- order=col_major 
- order=row_major

In fact all the coordinates are done as row_major and the library files
with col_major in the header are due to a bug. Please use row_major going
forward.


\section clfExample1 Example 1

\verbatim
#%chip_type=HuEx-1_0-st-v2
#%chip_type=HuEx-1_0-st-v1
#%chip_type=HuEx-1_0-st-ta1
#%lib_set_name=HuEx-1_0-st
#%lib_set_version=r2
#%create_date=Tue Sep 19 15:18:05 PDT 2006
#%guid=0000008635-1158704285-0732263232-1857033251-0689718480
#%clf_format_version=1.0
#%rows=2560
#%cols=2560
#%sequential=1
#%order=col_major
#%header0=probe_id x y
1 0 0
2 1 0
3 2 0
4 3 0
5 4 0
6 5 0
7 6 0
...
\endverbatim

\section related Related Pages

- <a href="./file-format-tsv-notes.html">TsvFile Design Notes</a>
- <a href="./file-format-tsv.html">TSV File Format</a>

*/

namespace affx
{
/// @brief     Create the ClfFile object
ClfFile::ClfFile()
{
  makeClfBindings();
}
/// @brief     Destroy a ClfFile object
ClfFile::~ClfFile()
{
}

///
int ClfFile::getXMax()
{
  int val;
  int rv;

  rv = m_tsv.getHeader("rows", val);
  Err::check(rv == TSV_OK, "No 'rows' header found in clf file");
  return val -1; // 1-Based to 0-based
}

int ClfFile::getYMax()
{
  int val;
  int rv;

  rv = m_tsv.getHeader("cols", val);
  Err::check(rv == TSV_OK, "No 'cols' header found in clf file");
  return val -1; // 1-Based to 0-based
}

/// @brief     Return the value of the sequential header
/// @return    int value of the header or -1 if not present
int ClfFile::getSequential()
{
  int val;
  int rv;

  rv = m_tsv.getHeader("sequential", val);
  if (rv == TSV_OK) {
    Err::check(val >= 0, "sequential header in clf file must be a value >= 0");
    return val;
  } else
    return -1;
}

/// @brief     Return the value of the order header
/// @return    the value of the order header if present
std::string ClfFile::getOrder()
{
  std::string val;
  int rv;

  rv = m_tsv.getHeader("order", val);
  if (rv == TSV_OK) {
    Err::check(val.compare("row_major") == 0 || val.compare("col_major") == 0, 
               "order header in clf file must be set to 'row_major' (old mislabeled 'col_major' is tolerated due to old bug)");
    return val;
  } else {
    val.erase();
    return val;
  }
}

/// @brief     Make the bindings which define a CLF file
void ClfFile::makeClfBindings()
{
  m_tsv.unbindAll();
  //
  m_tsv.bind(0, "probe_id", &probe_id, TSV_BIND_REQUIRED);
  m_tsv.bind(0, "x", &x, TSV_BIND_REQUIRED);
  m_tsv.bind(0, "y", &y, TSV_BIND_REQUIRED);
}

/// @brief     Open a CLF file
/// @param     filename     the pathname of the file
/// @return    tsv_return_t
int ClfFile::open(const std::string& filename)
{
  std::string val;
  int rc = m_tsv.open(filename);
  if (rc == TSV_OK) {
    int rv = m_tsv.getHeader("clf_format_version", val);
    Err::check(rv == TSV_OK, "No clf_format_version header in clf file");
    Err::check(val.compare("1.0") == 0, "Unable to handle clf file format");
  }
  return rc;
}
/// @brief     Start the write to a file
/// @param     filename
/// @return    tsv_return_t
int ClfFile::write(const std::string& filename)
{
  return m_tsv.writeTsv(filename);
}
/// @brief     Close a PGF file
/// @return    tsv_return_t
int ClfFile::close()
{
  return m_tsv.close();
}
/// @brief     Rewind back to the start of the CLF data
/// @return    tsv_return_t
int ClfFile::rewind()
{
  return m_tsv.rewind();
}
/// @brief     Skip to the next probe
/// @return    TSV_LEVEL_LAST when out of probesets
int ClfFile::next_probe()
{
  return m_tsv.nextLevel(0);
}
/// @brief     Define a tsv file as a CLF file.
/// @param     tsv       pointer to tsv to define
/// @return    tsv_return_t
int ClfFile::defineFileClf(TsvFile* tsv)
{
  Err::check(tsv != NULL, "affx::ClfFile - no tsv file provided to defineFileClf");
  return tsv->defineFile("probe_id\tx\ty");
}
/// @brief     Define this file as a pgf file
/// @return    tsv_return_t
int ClfFile::defineFileClf()
{
  return defineFileClf(&m_tsv);
}
};

