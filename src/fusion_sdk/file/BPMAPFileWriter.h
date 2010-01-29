////////////////////////////////////////////////////////////////
//
// Copyright (C) 2004 Affymetrix, Inc.
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

#ifndef _BPMAPFILEWRITER_H_
#define _BPMAPFILEWRITER_H_

/*! \file BPMAPFileWriter.h This file provides BPMAP file writing capabilities.
 */

#include "file/BPMAPFileData.h"
//
#include <fstream>
//

namespace affxbpmapwriter
{

#define BPMAP_VERSION 3.0

///////////////////////////////////////////////////////////////////////////////
///  affxcel::CBPMAPFileWriter
///
///  \brief BPMAP file writer object
///
/// Input: tpmap file or if unset, standard input
/*
Outut is a bpmap file; cannot be piped to standard out

INTRODUCTION
 
The BPMAP (binary probe map) file format is used to store the relationship
between (PM,MM) probe pairs and positions on a set of sequences of interest.
The format is binary with data stored in big-endian format.

OVERVIEW OF FILE FORMAT

The following lists the sections and their order and placement in the file.
The definition of each section is detailed below.  We assume there are N sequences
and M_i probe pairs mapping to sequence i (for i = 1 to N).

  1 File Header
  2 Sequence descriptions:
    + Sequence Description for sequence #1
    + Sequence Description for sequence #2
    + ...
    + Sequence Description for sequence #N 
  3 Probe-sequence mappings:
    + Probe-sequence mapping for sequence #1:
      - Sequence ID for sequence #1
      - Position Information for probe pair #1 of sequence #1
      - Position Information for probe pair #2 of sequence #1
      - ...
      - Position Information for probe pair #M_1 of sequence #1 
    + Probe-sequence mapping for sequence #2:
      - Sequence ID for sequence #2
      - Position Information for probe pair #1 of sequence #2
      - Position Information for probe pair #2 of sequence #2
      - ...
      - Position Information for probe pair #M_2 of sequence #2 
    + ...
    + Probe-sequence mapping for sequence #N:
      - Sequence ID for sequence #2
      - Position Information for probe pair #1 of sequence #N
      - Position Information for probe pair #2 of sequence #N
      - ...
      - Position Information for probe pair #M_N of sequence #N
  4 File footer


DETAILS OF FILE FORMAT

  1 File Header
    1.1 The first 8 bytes always contain the following values, to identify the file:
        (octal)                120 110 124 067 015 012 032 012
        (hexadecimal)           50  48  54  37  0d  0a  1a  0a
        (ASCII C notation)       P   H   T   7  \r  \n \032 \n
    1.2 Version number, float. Valid versions are 1.0, 2.0 and 3.0. (4 bytes)
    1.3 N, the number of sequences, stored as an uint32_t (4 bytes).

  2 Sequence Descriptions - the next N blocks (one for each sequence) respect the following format:
    Note that the N blocks are required to be in a specific order, to enable fast searches.
    The ordering of sequences is defined by:
         Lex sort on sequence group name (version >= 2.0 only),
         Lex sort on sequence version (version >= 2.0 only),
         Lex sort on Sequence name
    2.1  Length of the sequence name, uint32_t (4 bytes) 
    2.2  Sequence name stored as a sequence of chars, length specified by 2.1 above.
    ---- Items 2.3-2.4 are only present in versions >= 3.0 of the format ----
    2.3  Probe Mapping type, unsigned int (4 bytes)
         0 indicates a (PM,MM) probe pair tiling across the sequence.
         1 indicates a PM-only tiling across the sequence.
    2.4  Sequence file offset, unsigned int (4 bytes)
         The offset (in bytes) of the probe-sequence mapping info in section 3, after the
         end of the sequence description section.  Intended to enable fast look-up ability.
    ---- End version >= 3.0 specific section ----
    2.5  Number of probes/probe pairs in the sequence, uint32_t (4 bytes)
    ---- Items 2.6 through 2.10 are only included if the version is >= 2.0 ----
    2.6  Length of the sequence group name, uint32_t (4 bytes)
    2.7  Sequence group name (such as the organism to which the sequence belongs) stored as chars, length specified by 2.6
    2.8  Length of the sequence version, uint32_t (4 bytes)
    2.9  Sequence version stored as chars, length specified by 2.8
    2.10 Number of meta-data tag-value pairs, uint32_t (4 bytes)
        For each meta-data tag-value pair we store the following:
        2.10.1 Length of key in meta data block, uint32_t (4 bytes)
        2.10.2 Key in meta data block stored as chars, length specified by 2.10.1
        2.10.3 Length of value in meta data block, uint32_t (4 bytes)
        2.10.4 Value in meta data block stored as chars, length specified by 2.10.3
    ---- End version >= 2.0 specific section ----
  3 Probe-sequence mappings - the next N blocks (one for each sequence) respect the following format:
    3.1 Header info specific to the sequence
        3.1.1 Sequence ID, uint32_t (4 bytes)
    3.2 The next M_i blocks (one for each probe or probe pair aligning to the sequence, from 2.3) respect the format below.
        Note that the N blocks are required to be in a specific order, to enable fast searches.
        The ordering is defined as follows:
          Numeric sort on position
          Numeric sort on strand indicator
          Numeric sort on X coordinate of PM probe
          Numeric sort on Y coordinate of PM probe
          Numeric sort on X coordinate of MM probe (if there is an MM)
          Numeric sort on Y coordinate of MM probe (if there is an MM)
        3.2.1 X coordinate (0-based) on array of the PM probe as uint32_t (4 bytes)
        3.2.2 Y coordinate (0-based) on array of the PM probe as uint32_t (4 bytes)
        --- in versions >= 3.0 the next two sections are skipped if the mapping type is PM-only, as indicated in 3.2.1 ---
        3.2.3 X coordinate (0-based) on array of the MM probe as uint32_t (4 bytes)
        3.2.4 Y coordinate (0-based) on array of the MM probe as uint32_t (4 bytes)
        3.2.5 Length of the PM (and MM if a probe pair) as unsigned char (1 byte)
        3.2.6 DNA Probe sequence. The probe sequence is packed into a 7 byte sequence of chars.
              Each byte represents up to 4 bases (so the format can handle probes of length up to 27bp).
              The first byte contains the first 4 bases of the probe.
              The first base of the probe is encoded in the two most significant bits of the first byte.
              The fourth base of the probe is encoded in the two least significant bits of the first byte.
              The conversion from each pair of bits to a DNA base is as follows: (0,1,2,3) -> (A,C,G,T)
        3.2.7 Match score as float (4 bytes).
              The current BPMAP files are based on perfect 25-mer alignment so the scores are 1. (4 bytes)
        3.2.8 Position of PM probe within the sequence as uint32_t (4 bytes).
              The position is the 0-based position of the lower coordinate of the 25-mer aligned to the target.
        3.2.9 Strand indicator as unsigned char (1 byte).
              Value is 1 if the matching target (not the probe) is on the forward strand, 0 if on the reverse.
*/
///////////////////////////////////////////////////////////////////////////////
 class CGDACSequenceItemWriter : public affxbpmap::CGDACSequenceItem
   {
   public:
     /*! Constructor */
     CGDACSequenceItemWriter();
     
     /*! Destructor */
     ~CGDACSequenceItemWriter();

     /*! Gets an error string for file read errors.
      * @return An error string for file read errors.
      */
     std::string GetError() const { return m_strError; }

     /*! Make a copy with specific fields set */
     CGDACSequenceItemWriter copyMe();

   protected:

     /*! A string to hold error messages. */
     std::string m_strError;

     /*! offset into the BPMAP file for writing position of data blocks of hits */
     std::streampos m_offsetPosition;
     
     /*! flag to see whether this instance needs to be redone */
     bool m_HitNeedsRedoing;

     /*! int for PM_ONLY or PM_MM in sequence */
     int m_ProbePairsValue;

   public: //dont want to make this public but can't figure how

     /*! Comparison operator used by sort */
     int operator<(const CGDACSequenceItemWriter &rhs) const;

   protected:
     /*! List of hits for efficiency in file processing */
     std::list<affxbpmap::GDACSequenceHitItemType> m_HitList;

     /*! Code to process a text line beginning with the Group Name tag */
     bool processGroupName(const std::string&, const std::string&);

     /*! Code to process a text line beginning with the Version tag */
     bool processVersion(const std::string&, const std::string&);

     /*! Code to process a text line beginning with any other tag */
     bool processGenericTagValue(const std::string&, const std::string&);

     /*! Code to process a Sequence Hit text line */
     bool processHitItem(std::list<std::string>& );

     friend class CBPMAPFileWriter;
   };

class CBPMAPFileWriter : public affxbpmap::CBPMAPFileData  
{
 public:
	/*! Constructor */
	CBPMAPFileWriter();

	/*! Destructor */
	~CBPMAPFileWriter();

 protected:
	/*! The full path to the input TPMAP file */
	std::string m_TpmapFileName;

	/*! List of sequence items */
	std::list<CGDACSequenceItemWriter> m_SequenceItemList;

	/*! Read succeeded */
	bool m_ReadSucceeded;

 public:
	/*! Gets the full path of the TPMAP file.
	 * @return The full path of the TPMAP file.
	 */
	std::string GetTpmapFileName() const { return m_TpmapFileName; }

	/*! Sets the input TPMAP file name, if not set ReadTpmap uses stdin
	 * @param str The input TPMAP file name (full path).
	 */
	void SetTpmapFileName(const char *str);

	/*! Writes a version 3 binary BPMAP file.
	 * @return True if successful
	 */
	bool WriteBpmap();

	/*! Tests whether the input TPMAP file exists
	 * @return True if successful
	 */
	bool TpmapExists();

	/*! Reads the TPMAP file
	 * @return True if successful
	 */
	bool ReadTpmap();
 
};


//////////////////////////////////////////////////////////////////////

} // namespace

////////////////////////////////////////////////////////////////////
#endif // _BPMAPFILEWRITER_H_
