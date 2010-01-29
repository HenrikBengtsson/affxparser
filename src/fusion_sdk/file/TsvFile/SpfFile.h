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

/// @file   SpfFile.h
/// @brief  See the discussion in chipstream/ChipLayout.cpp for the format of the Spf file.

// It should be worth noting that the difference
// between v1 and v2 is the number of columns.
// To get a v1 file, turn off the columns you dont want.
// The difference between v2 and v3 is the v3
// format has probes on seperate lines to make them
// easy to comment out.
// See apt-convert-spf for examples.

//// Required v2 column list:
// name type num_blocks block_sizes block_annotations num_match num_probes probes

//// Required v3 column list:
//
// #%header0=name	type num_match
// #%header1=  allele_code
// #%header2=    annotation
// #%header3=      probe_id

// Optional columns:
//
// Feature                   Enables column
// -------------------       --------------------------
// m_has_allele_info         block_alleles  (v2)
//                           allele_code    (v3)
// m_has_context_info        block_contexts (v2)
//                           context_code   (v3)
// m_has_channel_info        block_channels (v2)
//                           channel_code   (v3)     
// m_has_rep_type_info        block_rep_types (v2)
//                           rep_type       (v3)     

//// Version 2 Example
//
// #%chip_type=GenomeWideSNP_6
// #%num-probesets=1856069
// #%num-cols=2680
// #%num-rows=2572
// #%spf-format=2
// name	type	num_blocks	block_sizes	block_annotations	num_match	num_probes	probes	block_alleles	block_contexts	block_channels block_rep_types
// AFR_010_SB	1	1	15	0	1	15	3662237,3595293,3670285,3614059,3597992,3568454,3678322,3611342,3643542,3675711,3547055,3565800,3549758,3608717,3622070	-1	-1	0	0
// AFR_012_SB	1	1	15	0	1	15	3605997,3568443,3640857,3678315,3600655,3656947,3654195,3667602,3584537,3622059,3659564,3672985,3565768,3573837,3552392	-1	-1	0	0
// AFR_035_SB	1	1	15	0	1	15	3571187,3592558,3584513,3662257,3622042,3632783,3646158,3654250,3595269,3651528,3565776,3659608,3640877,3648853,3579227	-1	-1	0	0
// AFR_054_SB	1	1	15	0	1	15	3549750,3624759,3584540,3573857,3648916,3635446,3614066,3648833,3587229,3646218,3638193,3587217,3646187,3573792,3651512	-1	-1	0	0
// AFR_057_SB	1	1	15	0	1	15	3544345,3664998,3624714,3579207,3675718,3606016,3541672,3670357,3619380,3541704,3654247,3600619,3584588,3547015,3611330	-1	-1	0	0


//// Examples from the meeting of 'Fri Apr 17 14:50:41 PDT 2009'

// an example falcon genotype probe.
// three probes, PM only, no wobble, two color, allele-nonspecific
// three probes on the physical array
// falconprobe1	2	2	3	0	1	6	0,1,2,0,1,2	0,1	0,0	0,1

// allele-specific G=> channel 1
// six probes on the physical array
// falconprobe2	2	2	3	0	1	6	4,5,6,7,8,9	0,1	0,0	1,1

// allele-nonspecific, with context 2 alleles-channels x 2 contexts x 3 probes => 2x2x3 => 12
// six probes on the physical array
// falconprobe3	2	4	3	0	1	12	11,12,13,11,12,13,0,1,2,0,1,2	0,1,0,1	0,0,1,1	0,1,0,1


//// Version 3 Example
//
// #%spf-format=3
// #%chip_type=GenomeWideSNP_6
// #%num-probesets=1856069
// #%num-cols=2680
// #%num-rows=2572
// #%header0=name	type num_match
// #%header1=	allele	allele_code
// #%header2=		context	context_code	channel_code	annotation
// #%header3=			probe_id
// AFFX-5Q-123	1
// 	allele	-1
// 		context	-1	0	0
// 			3466655
// 			3463975
// 			3461295
// 			3458615
// 			3453255
// AFFX-5Q-456	1
// 	allele	-1
// 		context	-1	0	0
// 			3466660
// 			3463980
// 			3461300
// 			3458620
// 			3455940
// AFFX-5Q-789	1
// 	allele	-1
// 		context	-1	0	0
// 			3466665
// 			3463985
// 			3461305
// 			3458625

//// Version 4 Example
//
// #%spf-format=4
// #%chip_type=GenomeWideSNP_6
// #%num-probesets=1856069
// #%num-cols=2680
// #%num-rows=2572
// #%num-FOO=?
// #%num-channels=?
// #%header0=name	type num_match
// #%header1=  block	annotation	allele_code	context_code	channel_code	rep_type
// #%header2=    probe_id
// AFFX-5Q-123	1
//   block	-1	-1	0	0	3
//     3466655
//     3463975
//     3461295

#ifndef _SPFFILE_H_
#define _SPFFILE_H_

//
#include "file/TsvFile/TsvFile.h"

namespace affx {
  class SpfFile;
}

class affx::SpfFile : public affx::TsvFile {
public:
  ///
  enum spf_format_t {
    FMT_UNSET=0,
    FMT_1=1,
    FMT_2=2,
    FMT_3=3,
    FMT_4=4,
  };

  /// 1,2 or 3.
  int m_spf_format;
  
  // in the order they should appear
  std::string m_header_format;
  std::vector<std::string> m_header_chipTypes;
  int m_header_numCols;
  int m_header_numRows;
  int m_header_numProbesets;
  int m_header_numChannels;

  /// enable/disable these on before calling "define_v{2,3}" to get the extra columns.
  int m_has_allele_info;
  int m_has_context_info;
  int m_has_channel_info;
  int m_has_rep_type_info;

  /// When openSpf is called all the "m_XXX_cidx" members are set.

  /// all formats v1,v2,v3,v4
  // level 0
  int m_name_cidx;
  int m_type_cidx;
  int m_num_match_cidx;

  // format v1,v2
  // all level 0
  int m_v2_num_blocks_cidx;
  int m_v2_block_sizes_cidx;
  int m_v2_block_annotations_cidx;
  int m_v2_num_probes_cidx;
  int m_v2_probes_cidx;
  int m_v2_block_alleles_cidx;
  int m_v2_block_contexts_cidx;
  int m_v2_block_channels_cidx;
  int m_v2_block_rep_types_cidx;

  // format v3
  // level 1
  int m_v3_allele_lbl_cidx;
  int m_v3_allele_code_cidx;
  // level 2
  int m_v3_context_lbl_cidx;
  int m_v3_context_code_cidx;
  int m_v3_channel_code_cidx;
  int m_v3_rep_type_cidx;
  int m_v3_annotation_code_cidx;
  // level 3
  int m_v3_probe_id_cidx;

  // format v4
  // level 1
  int m_v4_block_lbl_cidx;
  int m_v4_annotation_code_cidx;
  int m_v4_allele_code_cidx;
  int m_v4_context_code_cidx;
  int m_v4_channel_code_cidx;
  int m_v4_rep_type_cidx;
  // level 2
  int m_v4_probe_id_cidx;

  //
  SpfFile();
  //
  void clearOptions();
  void clearHeaderInfo();
  void clearCidx();
  void dumpCidx();

  //
  void setFormat(int fmt);

  //
  int determineFormat();

  //
  int openSpf(const std::string& fileName);
  // selects one of the below.
  void define_file(int version);
  // v1 is v2 with no "has_XXX_info"s set.
  void define_v2();
  void define_v3();
  void define_v4();

  //
  int writeSpf(const std::string& fileName);
  int writeSpf(const std::string& fileName,int format);

  //
  void addStandardHeaders();

private:
  void define_v2_cols();
};

#endif
