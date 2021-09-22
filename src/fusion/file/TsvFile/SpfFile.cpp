////////////////////////////////////////////////////////////////
//
// Copyright (C) 2008 Affymetrix, Inc.
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

#include "file/TsvFile/SpfFile.h"

affx::SpfFile::SpfFile() {
  clearOptions();
  clearHeaderInfo();
  clearCidx();
}

void
affx::SpfFile::clearOptions()
{
  m_spf_format=affx::SpfFile::FMT_UNSET;
  //
  m_has_allele_info=0;
  m_has_context_info=0;
  m_has_channel_info=0;
  m_has_rep_type_info=0;
}

void
affx::SpfFile::clearHeaderInfo()
{
  m_header_format=-1;
  m_header_chipTypes.resize(0);
  m_header_numCols=-1;
  m_header_numRows=-1;
  m_header_numProbesets=-1;
  m_header_numChannels=-1;
}

void
affx::SpfFile::clearCidx()
{
  // all formats v1, v2, v3, v4
  m_name_cidx=-1;
  m_type_cidx=-1;
  m_num_match_cidx=-1;

  // format v1 and v2
  // all level 0
  m_v2_num_blocks_cidx=-1;
  m_v2_block_sizes_cidx=-1;
  m_v2_block_annotations_cidx=-1;
  m_v2_num_probes_cidx=-1;
  m_v2_probes_cidx=-1;
  m_v2_block_alleles_cidx=-1;
  m_v2_block_contexts_cidx=-1;
  m_v2_block_channels_cidx=-1;
  m_v2_block_rep_types_cidx=-1;

  // format v3
  // level 1
  m_v3_allele_lbl_cidx=-1;
  m_v3_allele_code_cidx=-1;
  // level 2
  m_v3_context_lbl_cidx=-1;
  m_v3_context_code_cidx=-1;
  m_v3_channel_code_cidx=-1;
  m_v3_rep_type_cidx=-1;
  m_v3_annotation_code_cidx=-1;
  // level 3
  m_v3_probe_id_cidx=-1;

  // format v4
  // level 1
  m_v4_block_lbl_cidx=-1;
  m_v4_annotation_code_cidx=-1;
  m_v4_allele_code_cidx=-1;
  m_v4_context_code_cidx=-1;
  m_v4_channel_code_cidx=-1;
  m_v4_rep_type_cidx=-1;
  // level 2
  m_v4_probe_id_cidx=-1;
}

#define DUMP_CIDX_HEAD(_head) { printf("== %-30s\n",_head); }
#define DUMP_CIDX(_cidx) { printf("  %-30s = %3d\n",#_cidx,_cidx); }

void affx::SpfFile::dumpCidx()
{
  DUMP_CIDX_HEAD("features");
  DUMP_CIDX(m_spf_format);
  DUMP_CIDX(m_has_allele_info);
  DUMP_CIDX(m_has_context_info);
  DUMP_CIDX(m_has_channel_info);
  DUMP_CIDX(m_has_rep_type_info);

  DUMP_CIDX_HEAD("required");
  DUMP_CIDX(m_name_cidx);
  DUMP_CIDX(m_type_cidx);
  DUMP_CIDX(m_num_match_cidx);
 
  DUMP_CIDX_HEAD("v2");
  DUMP_CIDX(m_v2_num_blocks_cidx);
  DUMP_CIDX(m_v2_block_sizes_cidx);
  DUMP_CIDX(m_v2_block_annotations_cidx);
  DUMP_CIDX(m_v2_num_probes_cidx);
  DUMP_CIDX(m_v2_probes_cidx);
  DUMP_CIDX(m_v2_block_alleles_cidx);
  DUMP_CIDX(m_v2_block_contexts_cidx);
  DUMP_CIDX(m_v2_block_channels_cidx);
  DUMP_CIDX(m_v2_block_rep_types_cidx);

  DUMP_CIDX_HEAD("v3");
  DUMP_CIDX(m_v3_allele_lbl_cidx);
  DUMP_CIDX(m_v3_allele_code_cidx);
  DUMP_CIDX(m_v3_context_lbl_cidx);
  DUMP_CIDX(m_v3_context_code_cidx);
  DUMP_CIDX(m_v3_channel_code_cidx);
  DUMP_CIDX(m_v3_annotation_code_cidx);
  DUMP_CIDX(m_v3_probe_id_cidx);

  DUMP_CIDX_HEAD("v4");
  DUMP_CIDX(m_v4_block_lbl_cidx);
  DUMP_CIDX(m_v4_annotation_code_cidx);
  DUMP_CIDX(m_v4_allele_code_cidx);
  DUMP_CIDX(m_v4_context_code_cidx);
  DUMP_CIDX(m_v4_channel_code_cidx);
  DUMP_CIDX(m_v4_rep_type_cidx);
  DUMP_CIDX(m_v4_probe_id_cidx);
}

#undef DUMP_CIDX_HEAD
#undef DUMP_CIDX

void affx::SpfFile::setFormat(int fmt)
{
  m_spf_format=fmt;
}

//////////


void affx::SpfFile::define_v2()
{
  clearCidx();

  setFormat(2);

  //
  int cidx=0;
  m_name_cidx=cidx++;
  defineColumn(0,m_name_cidx,"name");
  m_type_cidx=cidx++;
  defineColumn(0,m_type_cidx,"type");
  //
  m_v2_num_blocks_cidx=cidx++;
  defineColumn(0,m_v2_num_blocks_cidx,"num_blocks");
  m_v2_block_sizes_cidx=cidx++;
  defineColumn(0,m_v2_block_sizes_cidx,"block_sizes");
  m_v2_block_annotations_cidx=cidx++;
  defineColumn(0,m_v2_block_annotations_cidx,"block_annotations");
  m_num_match_cidx=cidx++;
  defineColumn(0,m_num_match_cidx,"num_match");
  m_v2_num_probes_cidx=cidx++;
  defineColumn(0,m_v2_num_probes_cidx,"num_probes");
  m_v2_probes_cidx=cidx++;
  defineColumn(0,m_v2_probes_cidx,"probes");
  // optional columns
  if (m_has_allele_info) {
    m_v2_block_alleles_cidx=cidx++;
    defineColumn(0,m_v2_block_alleles_cidx,"block_alleles");
  }
  if (m_has_context_info) {
    m_v2_block_contexts_cidx=cidx++;
    defineColumn(0,m_v2_block_contexts_cidx,"block_contexts");
  }
  if (m_has_channel_info) {
    m_v2_block_channels_cidx=cidx++;
    defineColumn(0,m_v2_block_channels_cidx,"block_channels");
  }
  if (m_has_rep_type_info) {
    m_v2_block_rep_types_cidx=cidx++;
    defineColumn(0,m_v2_block_rep_types_cidx,"block_rep_types");
  }
}

void affx::SpfFile::define_v3()
{
  clearCidx();

  setFormat(3);

  // level0
  m_name_cidx=0;
  defineColumn(0,m_name_cidx,"name");
  m_type_cidx=1;
  defineColumn(0,m_type_cidx,"type");
  m_num_match_cidx=2;
  defineColumn(0,m_num_match_cidx,"num_match");
  // level1
  int cidx1=0;
  m_v3_allele_lbl_cidx=cidx1++;
  defineColumn(1,m_v3_allele_lbl_cidx,"allele");
  if (m_has_allele_info) {
    m_v3_allele_code_cidx=cidx1++;
    defineColumn(1,m_v3_allele_code_cidx,"allele_code");
  }
  //
  int cidx2=0;
  m_v3_context_lbl_cidx=cidx2++;
  defineColumn(2,m_v3_context_lbl_cidx,"context");
  //
  m_v3_annotation_code_cidx=cidx2++;
  defineColumn(2,m_v3_annotation_code_cidx,"annotation");
  //
  if (m_has_context_info) {
    m_v3_context_code_cidx=cidx2++;
    defineColumn(2,m_v3_context_code_cidx,"context_code");
  }
  //
  if (m_has_channel_info) {
    m_v3_channel_code_cidx=cidx2++;
    defineColumn(2,m_v3_channel_code_cidx,"channel_code");
  }
  // level 3
  m_v3_probe_id_cidx=0;
  defineColumn(3,m_v3_probe_id_cidx,"probe_id");
}

void affx::SpfFile::define_v4()
{
  clearCidx();

  setFormat(4);

  // level0
  m_name_cidx=0;
  defineColumn(0,m_name_cidx,"name");
  m_type_cidx=1;
  defineColumn(0,m_type_cidx,"type");
  m_num_match_cidx=2;
  defineColumn(0,m_num_match_cidx,"num_match");
  // level1
  int cidx1=0;
  m_v4_block_lbl_cidx=cidx1++;
  defineColumn(1,m_v4_block_lbl_cidx,"block");
  //
  m_v4_annotation_code_cidx=cidx1++;
  defineColumn(1,m_v4_annotation_code_cidx,"annotation");
  if (m_has_allele_info==1) {
    m_v4_allele_code_cidx=cidx1++;
    defineColumn(1,m_v4_allele_code_cidx,"allele_code");
  }
  if (m_has_context_info==1) {
    m_v4_context_code_cidx=cidx1++;
    defineColumn(1,m_v4_context_code_cidx,"context_code");
  }
  if (m_has_channel_info==1) {
    m_v4_channel_code_cidx=cidx1++;
    defineColumn(1,m_v4_channel_code_cidx,"channel_code");
  }
  if (m_has_rep_type_info==1) {
    m_v4_rep_type_cidx=cidx1++;
    defineColumn(1,m_v4_rep_type_cidx,"rep_type");
  }
  // level 2
  m_v4_probe_id_cidx=0;
  defineColumn(2,m_v4_probe_id_cidx,"probe_id");
}

void affx::SpfFile::define_file(int version)
{
  if ((version==1)||(version==2)) {
    define_v2();
  }
  else if (version==3) {
    define_v3();
  } 
  else if (version==4) {
    define_v4();
  }
  else {
    APT_ERR_ABORT("bad spf version");
  }
}

int affx::SpfFile::writeSpf(const std::string& fileName)
{
  //
  define_file(m_spf_format);
  addStandardHeaders();
  //
  if ((m_spf_format==affx::SpfFile::FMT_1)||(m_spf_format==affx::SpfFile::FMT_2)) {
    return writeTsv_v1(fileName);
  }
  else if ((m_spf_format==affx::SpfFile::FMT_3)||(m_spf_format==affx::SpfFile::FMT_4)) {
    return writeTsv_v2(fileName);
  }
  else {
    APT_ERR_ABORT("affx::SpfFile::writeSpf(): unknown format.");
    return -1;
  }
}    

int affx::SpfFile::writeSpf(const std::string& fileName,int format)
{
  m_spf_format=format;
  return writeSpf(fileName);
}

//////////

int
affx::SpfFile::determineFormat()
{
  // The "_lbl_cidx" columns are just there for the humans.
  // we done really care if they arent there.

  if ((m_name_cidx<0)||(m_type_cidx<0)) {
    m_spf_format=0;
    return m_spf_format;
  }
  
  if ((m_v2_num_blocks_cidx>=0)        &&
      (m_v2_block_sizes_cidx>=0)       &&
      (m_v2_block_annotations_cidx>=0) &&
      (m_num_match_cidx           >=0) &&
      (m_v2_num_probes_cidx       >=0) &&
      (m_v2_probes_cidx           >=0)) {
    m_spf_format=2;
    return m_spf_format;
  }
  
  // allele_code and context are required for v3
  if ((m_v3_annotation_code_cidx>=0) &&
      (m_v3_allele_code_cidx>=0) &&
      (m_v3_context_code_cidx>=0) &&
      (m_v3_probe_id_cidx>=0)) {
    m_spf_format=3;
    return m_spf_format;
  }
  
  // allele_code, context_code and channel_code are optional for v4
  if ((m_v4_annotation_code_cidx>=0) &&
      (m_v4_probe_id_cidx>=0)) {
    m_spf_format=4;
    return m_spf_format;
  }

  // punt!
  m_spf_format=0;
  return m_spf_format;
}

// on second thought, the headers arent really required.
// but the return value should still be checked.
// #define getRequiredHeader(_key,_val) { if (getHeader(_key,_val)!=TSV_OK) { APT_ERR_ABORT("SpfFile::readSpf(): missing header: '" _key "'") } }

int 
affx::SpfFile::openSpf(const std::string& fileName)
{
  if (open(fileName)!=affx::TSV_OK) {
    APT_ERR_ABORT("SpfFile::readSpf: Couldn't open '" + ToStr(fileName) + "' to read.");
  }
  //
  clearHeaderInfo();
  //
  getHeaderAppend("chip-type",m_header_chipTypes);
  getHeaderAppend("chip_type",m_header_chipTypes);
  //
  getHeader("spf-format",m_header_format);
  getHeader("chip_type",m_header_chipTypes);
  getHeader("num-cols",m_header_numCols);
  getHeader("num-rows",m_header_numRows);
  getHeader("num-probesets",m_header_numProbesets);
  getHeader("num-channels",m_header_numChannels);

  // zero out the cidx info first.
  clearCidx();
   
  // common to v1,v2,v3
  m_name_cidx              = cname2cidx(0,"name");
  m_type_cidx              = cname2cidx(0,"type");
  m_num_match_cidx         = cname2cidx(0,"num_match");

  // v1,v2
  m_v2_num_blocks_cidx = cname2cidx(0,"num_blocks");
  m_v2_block_sizes_cidx = cname2cidx(0,"block_sizes");
  m_v2_block_annotations_cidx = cname2cidx(0,"block_annotations");
  m_v2_num_probes_cidx = cname2cidx(0,"num_probes");
  m_v2_probes_cidx = cname2cidx(0,"probes");
  m_v2_block_alleles_cidx = cname2cidx(0,"block_alleles");
  m_v2_block_contexts_cidx = cname2cidx(0,"block_contexts");
  m_v2_block_channels_cidx = cname2cidx(0,"block_channels");
  m_v2_block_rep_types_cidx = cname2cidx(0,"block_rep_types");

  // v3
  // level 1
  m_v3_allele_lbl_cidx=cname2cidx(1,"allele");  // always the string "allele" for readablity
  m_v3_allele_code_cidx=cname2cidx(1,"allele_code");
  // level 2
  m_v3_context_lbl_cidx=cname2cidx(2,"context"); // always the string "context" for readablity
  m_v3_context_code_cidx=cname2cidx(2,"context_code");
  m_v3_annotation_code_cidx=cname2cidx(2,"annotation");
  m_v3_channel_code_cidx=cname2cidx(2,"channel_code");
  // level 3
  m_v3_probe_id_cidx=cname2cidx(3,"probe_id");

  // v4
  // level 1
  m_v4_block_lbl_cidx=cname2cidx(1,"block");  // always the string "block" for readablity
  m_v4_annotation_code_cidx=cname2cidx(1,"annotation");
  m_v4_allele_code_cidx=cname2cidx(1,"allele_code");
  m_v4_context_code_cidx=cname2cidx(1,"context_code");
  m_v4_channel_code_cidx=cname2cidx(1,"channel_code");
  m_v4_rep_type_cidx=cname2cidx(1,"rep_type");
  // level 2
  m_v4_probe_id_cidx=cname2cidx(2,"probe_id");

  // what does this file look like?
  determineFormat();

  //
  return affx::TSV_OK;
}

const char* spf_standard_headers[]={
  "spf-format",
  "chip_type", // @todo: we should only have "chip-type"
  "chip-type",
  "num-cols",
  "num-rows",
  "num-channels",
  NULL
};

void 
affx::SpfFile::addStandardHeaders()
{
  deleteHeaders(spf_standard_headers);
  //
  addHeader("spf-format",m_spf_format);
  addHeader("chip_type",m_header_chipTypes);
  addHeader("num-cols",m_header_numCols);
  addHeader("num-rows",m_header_numRows);
  addHeader("num-probesets",m_header_numProbesets);
  addHeader("num-channels",m_header_numChannels);
}
