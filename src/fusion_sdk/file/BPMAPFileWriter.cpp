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

#include "file/BPMAPFileWriter.h"
//
#include "file/FileIO.h"
#include "file/FileWriter.h"
//
#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>
#include <list>
#include <sstream>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <vector>
//

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif

using namespace affxbpmap;
using namespace affxbpmapwriter;

#define BPMAP_TAG_VALUE_START "#"
#define BPMAP_GROUP_NAME "#seq_group_name"
#define BPMAP_VERSION_NAME "#version"

///////////////////////////////////////////////////////////////////////////////
CGDACSequenceItemWriter::CGDACSequenceItemWriter()
{
  m_ProbePairsValue = -1;
  m_HitNeedsRedoing = false;
}

///////////////////////////////////////////////////////////////////////////////
CGDACSequenceItemWriter::~CGDACSequenceItemWriter()
{
	m_Hits.erase(m_Hits.begin(), m_Hits.end());
	m_HitList.erase(m_HitList.begin(), m_HitList.end());
}

///////////////////////////////////////////////////////////////////////////////
CGDACSequenceItemWriter CGDACSequenceItemWriter::copyMe()
{
  CGDACSequenceItemWriter copy = CGDACSequenceItemWriter();
  // copy over relevant items
  copy.m_GroupName = m_GroupName;
  copy.m_SeqVersion = m_SeqVersion;
  copy.m_Parameters = m_Parameters;
  return(copy);
}

///////////////////////////////////////////////////////////////////////////////
CBPMAPFileWriter::CBPMAPFileWriter() : CBPMAPFileData()
{
  m_Version = BPMAP_VERSION;
  m_TpmapFileName ="";
}

///////////////////////////////////////////////////////////////////////////////
CBPMAPFileWriter::~CBPMAPFileWriter()
{
	Close();

	m_TpmapFileName="";
}

///////////////////////////////////////////////////////////////////////////////
void CBPMAPFileWriter::SetTpmapFileName(const char *str)
{
	m_TpmapFileName = str;
}

///////////////////////////////////////////////////////////////////////////////
bool CBPMAPFileWriter::TpmapExists()
{
	// Find the file stats.
	struct stat st;
	return (stat(m_TpmapFileName.c_str(), &st) == 0);
}

///////////////////////////////////////////////////////////////////////////////
int tokenize(const std::string& lineBuffer, std::list<std::string>& tokens){
  // gets tokens  delimited by white space from lineBuffer into a list of tokens
  // returns the number of tokens found
  std::string token;
  std::stringstream ss(lineBuffer);

  tokens.clear();
  while (ss >> token)
    tokens.push_back(token);

  return( (int)tokens.size());
}

///////////////////////////////////////////////////////////////////////////////
int CGDACSequenceItemWriter::operator<(const CGDACSequenceItemWriter &rhs) const
{
  // sort on m_GroupName, then on m_SeqVersion, then on m_Name
   if( this->m_GroupName == rhs.m_GroupName && this->m_SeqVersion == rhs.m_SeqVersion && this->m_Name < rhs.m_Name) return 1;
   if( this->m_GroupName == rhs.m_GroupName && this->m_SeqVersion < rhs.m_SeqVersion) return 1;
   if( this->m_GroupName < rhs.m_GroupName ) return 1;
   return 0;
}

//////////////////////////////////////////////////////////////////////////////
bool CGDACSequenceItemWriter::processGroupName(const std::string& tag, const std::string& value)
{
  if( tag == BPMAP_GROUP_NAME) {
    m_GroupName = value;
    m_Name = "";
    m_SeqVersion = "";
    return (true);
  }
      return(false);
}

///////////////////////////////////////////////////////////////////////////////
bool CGDACSequenceItemWriter::processVersion(const std::string& tag, const std::string& value)
{
  if( tag == BPMAP_VERSION_NAME) { 
    m_SeqVersion = value;
    return (true);
  }
      return(false);
}

///////////////////////////////////////////////////////////////////////////////
bool CGDACSequenceItemWriter::processGenericTagValue(const std::string& tag, const std::string& value)
{
  TagValuePairType tagValue;
  if (tag.length() <=1){
    // tag is a singleton # character, not permissible
    return( false );
  }
  std::string nTag = tag.substr(1, tag.length() -1); // strip the leading "#"
  tagValue.Tag = nTag;
  tagValue.Value = value;
  m_Parameters.push_back(tagValue);
  return(true);
}

///////////////////////////////////////////////////////////////////////////////
#define DNA_BASE_A     0
#define DNA_BASE_C     1
#define DNA_BASE_G     2
#define DNA_BASE_T     3
#define DNA_BASE_OTHER 4

int acgt_to_int(char c, unsigned int *i) {
  switch(c) {
    case 'a':
    case 'A':
      *i = DNA_BASE_A;
      break;
    case 'c':
    case 'C':
      *i = DNA_BASE_C;
      break;
    case 'g':
    case 'G':
      *i = DNA_BASE_G;
      break;
    case 't':
    case 'T':
      *i = DNA_BASE_T;
      break;
    default:
      *i = DNA_BASE_OTHER;
      break;
  }

  if(*i == DNA_BASE_OTHER)
    return(EXIT_FAILURE);
  else
    return(EXIT_SUCCESS);
}

///////////////////////////////////////////////////////////////////////////////
/*
** d is a char array, should consist only of [acgtACGT]
** n_d is the length of d
** b is the array to which the byte form of the dna will be written
** n_b is the available number of bytes (so we can check if there is enough space)
*/
int dna_to_byte(const std::string& sequence, char *packedSequence) {
  unsigned short i,j,k,n_bytes;
  unsigned int base;
  
  int n_d = (int)sequence.length();

  /* Number of bytes required - we pack 4 bases per byte */
  n_bytes = (unsigned short) ceil(n_d/(float)4);

  for(i=0,j=0; i<PROBE_STORAGE_BUFFER_LENGTH; i++) {
    for(k=0; (k<4 && j<n_d); k++, j++) {
      if(acgt_to_int(sequence[j],&base)==EXIT_FAILURE)
	return(0);
      else {
        packedSequence[i] = ((packedSequence[i] << 2) | base);
        /* printf("dna_to_byte: writing base %c as %d, b[%d] is now %u\n",d[j],base,i,(unsigned char) b[i]); */
      }
    }
    /* the last byte may not be filled, in which case pad with zeros (effectively pads with 'A') */
    if(k < 4)
      packedSequence[i] = packedSequence[i] << 2*(4-k);
    /* printf("dna_to_byte: b[%d]=%u\n",i,(unsigned char)b[i]); */
  }

  return(n_d);
}


///////////////////////////////////////////////////////////////////////////////
bool CGDACSequenceItemWriter::processHitItem(std::list<std::string>& tokens)
{
  unsigned int val;
  float fval;

  GDACSequenceHitItemType hit;
  if (!(6 <= (int)tokens.size()) && ((int)tokens.size() <= 9)){
    return( false );
  }
  std::string sequence = tokens.front(); tokens.pop_front();
  std::string strand = tokens.front(); tokens.pop_front();
  std::string seqName = tokens.front(); tokens.pop_front();
  std::string hitPosition = tokens.front(); tokens.pop_front();
  std::string pmX = tokens.front(); tokens.pop_front();
  std::string pmY = tokens.front(); tokens.pop_front();
  std::string mmX;
  std::string mmY;
  // no match score in current TPMAPS
  std::string matchScore = "1.0";
  // in the future a match score can be tacked onto the end of the line
  if (tokens.size() == 1) {
    matchScore = tokens.front(); tokens.pop_front();
  }

  // check the seqName for consistency
  if (m_Name.size() == 0 ) m_Name = seqName;
  if (m_Name != seqName) { // this SequenceItemWriter instance is finished
    m_HitNeedsRedoing = true;
    return(true);
  }
  
  if (m_ProbePairsValue < 0) { // initialize
    if (tokens.size() >= 2) { // PM and MM present
      m_ProbePairsValue = PM_MM;
    } else { // PM only present
      m_ProbePairsValue = PM_ONLY;
    }
  } else { // test for consistency presence or absence of mismatch
    if ((m_ProbePairsValue == PM_MM) && (tokens.size() < 2)){
      m_strError = "Too few tokens to process MM probes in PM/MM line";
      return (false);
    }
    if ((m_ProbePairsValue == PM_ONLY) && (tokens.size() >= 2)){
      m_strError = "Too many tokens left to process in PMOnly line";
      return (false);
    }
  }
  if(m_ProbePairsValue == PM_MM){
    mmX = tokens.front(); tokens.pop_front();
    mmY = tokens.front(); tokens.pop_front();
  }

  if (tokens.size() == 1) {
    matchScore = tokens.front(); tokens.pop_front();
  }

  // 1. process the sequence
  memset(&hit.PackedPMProbe[0], 0, PROBE_STORAGE_BUFFER_LENGTH+1);
  char *packedSequence = &hit.PackedPMProbe[0];
  int len = dna_to_byte(sequence, packedSequence);
  if (len == 0){
    m_strError = "failed to pack sequence token: " + sequence;
    return(false);
  }

  // 2. process the probe length
  hit.ProbeLength = (int)sequence.size();

  // 3. process the strand
  const char ch = strand[0];
  if (strand.size() != 1){
    m_strError = "Unrecognized strand token: " + strand;
    return(false);
  }
  switch(ch) {
  case 't':
  case 'T':
  case '1':
  case '+':
    hit.TopStrand = 1;
    break;
  case 'f':
  case 'F':
  case '0':
  case '-':
    hit.TopStrand = 0;
    break;
  default:
    m_strError = "Unrecognized strand token: " + strand;
    return( false );
  }
  
  // 4. process the hitPosition
  if( sscanf(hitPosition.c_str(), "%u", &val) == EOF){ //error
    m_strError = "Unrecognized hitPosition: " + hitPosition;
    return(false);
  }
  hit.Position = val;

  // 5. process the PM X position
  if( sscanf(pmX.c_str(), "%u", &val) == EOF){ //error
    m_strError = "Unrecognized PM X: " + pmX;
    return(false);
  }
  hit.PMX = val;

  // 6. process the PM Y position
  if( sscanf(pmY.c_str(), "%u", &val) == EOF){ //error
    m_strError = "Unrecognized PM Y: " + pmY;
    return(false);
  }
  hit.PMY = val;

  // 7. process the match score
  if( sscanf(matchScore.c_str(), "%f", &fval) == EOF){ //error
    m_strError = "Unrecognized Match Score: " + matchScore;
    return(false);
  }
  hit.MatchScore = fval;
  if ( !( (0.0 <= hit.MatchScore) && (hit.MatchScore <= 1.0)) ) { //error
    m_strError = "Match Score must be at least 0.0 and no greater than 1.0: " + matchScore;
    return (false);
  }

  if ( m_ProbePairsValue == PM_MM ){
    // 8. process the MM X position
    if( sscanf(mmX.c_str(), "%u", &val) == EOF){ //error
      m_strError = "Unrecognized MM X: " + mmX;
      return(false);
    }
    hit.MMX = val;
  
    // 9. process the PM Y position
    if( sscanf(mmY.c_str(), "%u", &val) == EOF){ //error
    m_strError = "Unrecognized MM Y: " + mmY;
      return(false);
    }
    hit.MMY = val;
  }
    
  m_HitList.push_front(hit);

  return(true);
}

///////////////////////////////////////////////////////////////////////////////

bool CBPMAPFileWriter::ReadTpmap()
{
	// pseudo code
	// Make a GDACSequenceItem
	// Read in the first line
	// expect a format:
	// #group_name\tString
	// Read in second line
	// expect format:
	// #version\tString
	// other parameters have the form:
	// #name\tString
	// and must occur after the #version line.
	// Read in subsequent lines: Sequence\tStrand\tName\tPos\tX\tY\tX\tY
	// and make a GDACSequenceHitItemType and add to the array within
	// the GDACSequenceItem until we see the next
	// #group_name\tString
	// or EOF

	// sort by Group Name then by Version then by Name
	// within each set sort by position and then by strand

	m_ReadSucceeded = false;

	bool processingHitItems = true; //enter the while loop looking for a groupName tag
	// bool processingGenericTagValues = false; // unused
	bool processingTagValues = false;
	bool processingGroupName = true;
	bool processingVersion = true;

	std::string tag;
	std::string value;

	std::string lineBuffer;
	std::string token;
	

	m_strError = "";
	std::ifstream tpMapFileStr;
	bool use_cin = (m_TpmapFileName.size() == 0);
	if(!use_cin) { // Open the file if set
	  tpMapFileStr.open(m_TpmapFileName.c_str(), std::ios::in);
	  if (!tpMapFileStr)
	    {
	      m_strError = "Unable to open " + m_TpmapFileName;
	      return( false );
	    }
	}
	try {
	  while (1)
	  {
		  if (use_cin)
		  {
			  if (!getline(std::cin, lineBuffer))
				  break;
		  }
		  else
		  {
			  if (!getline(tpMapFileStr, lineBuffer))
				  break;
		  }

	      std::list<std::string> tokens;
	      int ntokens = tokenize(lineBuffer, tokens);
	      if (ntokens == 0) continue;  // ignore lines with 0 tokens
	      // ignore lines beginning with "#" followed by any white space
	      if (tokens.
		  front() == BPMAP_TAG_VALUE_START) continue;

	      // set up the logic to process the line
	      if (lineBuffer.substr(0,1) == BPMAP_TAG_VALUE_START){ // tag is present
		tag = tokens.front();
		tokens.pop_front();
		std::transform(tag.begin(), tag.end(), tag.begin(), tolower);
		if (ntokens == 1) {  // tag is present with no value
		  value = "";
		}
		else {
		  value = tokens.front();
		}

		bool tagDone = false;
		if (tag == BPMAP_GROUP_NAME) {
		  if (processingHitItems) {
		    // should have just finished a block of probes (unless first time through)
		    processingGroupName = true;
		    processingVersion = false;
		    processingTagValues = false;
		    processingHitItems = false;

		    tagDone = true;
		  }
		  else { //error
		    m_strError = "Saw #group_name tag not at beginning of file or not after a set of probes in " + m_TpmapFileName;
		    return(false);
		  }		
		}
		if (tag == BPMAP_VERSION_NAME){
		  // should have just processed the #seq_group_name tag
		  if (processingGroupName) {
		    processingVersion = true;
		    processingGroupName = false;
		    processingTagValues = false;
		    processingHitItems = false;
		  
		    tagDone = true;
		  }
		  else { // error out
		    m_strError = "Saw #version tag not following #group_name tag in " + m_TpmapFileName;
		    return(false);
		  }
		}
	      
		if (!tagDone) {
		  //should have previously processed the #version tag
		  if (processingVersion) {
		    processingTagValues = true;
		    processingGroupName = false;
		    processingHitItems = false;
		  }
		  else { //error out
		    m_strError = "Saw a generic tag in line not following #version tag in " + m_TpmapFileName + ": " + lineBuffer;
		    return(false);	  
		  }
		}
	      }
	      else {  // no tag was present, must be processing probe hit items
		processingHitItems = true;
		processingTagValues = false;
		processingGroupName = false;
		processingVersion = false;
	      }

	      if ( !(processingHitItems || processingTagValues ||
		     processingGroupName || processingVersion) ) {
		// error out
		m_strError = "Invalid input line: " + lineBuffer + " in "  + m_TpmapFileName;
		return(false);
	      }
	    
	      if (processingGroupName) {
		// make a new GDACSequenceItem
		m_SequenceItemList.push_front(CGDACSequenceItemWriter());
		if (!m_SequenceItemList.front().processGroupName(tag, value)) {
		  m_strError = "Expected to see #group_name, saw: " + lineBuffer + " in " + m_TpmapFileName;
		  return( false );
		}
		continue;
	      }
		
	      if (processingVersion && !processingTagValues) {
		if (!m_SequenceItemList.front().processVersion(tag, value)) {
		  m_strError = "Expected to see #version, saw: " + lineBuffer + " in " + m_TpmapFileName;
		  return( false );
		}
		continue;
	      }

	      if (processingTagValues) { // processing tag/value pairs
		if (!m_SequenceItemList.front().processGenericTagValue(tag, value)) {
		  m_strError = "Expected #tag/value pair, saw: " + lineBuffer + " in " + m_TpmapFileName;
		  return( false );
		}
		continue;
	      }

	      if (processingHitItems) {
		if (!m_SequenceItemList.front().processHitItem(tokens)){
		  m_strError = "Error processing sequence hit, line: " + lineBuffer + " in " + m_TpmapFileName + "\n" + m_SequenceItemList.front().GetError();
		  return( false );
		}
		if( m_SequenceItemList.front().m_HitNeedsRedoing ){
		  // test to see whether this m_SequenceItemList.front() should be finished
		  // this covers the case when a new SeqName needs to be created with
		  // an ongoing Group Name and Version
		  // make a new CGDACSequenceItemWriter
		  ntokens = tokenize(lineBuffer, tokens); // remake the token list
		  m_SequenceItemList.push_front(m_SequenceItemList.front().copyMe());
		  if (!m_SequenceItemList.front().processHitItem(tokens)){ //retry
		    m_strError = "Error processing sequence hit, line: " + lineBuffer + " in " + m_TpmapFileName + "\n" + m_SequenceItemList.front().GetError();
		    return( false );
		  }
		}
		continue;
	      }

	    } //end while

	  // now everything is in memory and we do the big sort
	  m_SequenceItemList.sort();

	  std::list<CGDACSequenceItemWriter>::iterator i;
	  for(i=m_SequenceItemList.begin(); i != m_SequenceItemList.end(); ++i) {
	    (*i).m_HitList.sort();
	  }
	
	  m_ReadSucceeded = true;
	  return( true );
	}
	catch (std::string exceptionString) {
	  m_strError = "Exception: " + exceptionString + " raised while reading input";
	  tpMapFileStr.close();
	}
	return( false );
}

bool CBPMAPFileWriter::WriteBpmap()
{
	// write the BPMAP file header
	// iterate through all the GDACSequenceItem
	// write each Sequence Description
	// iterate through all the GDACSequenceItem and then GDACSequenceItemHitType
	// write position information until the Group name/Version changes

	// no BPMAP footer

	if(!m_ReadSucceeded) {
	  m_strError = "No data to write.";
	  return ( false );
	}

	std::ofstream of;
	of.open(m_FileName.c_str(), std::ios::out | std::ios::binary);

	// check if open
	if(!of) {
	  m_strError = "Unable to open file " + m_FileName + " for output.";
	  return ( false );
	}
	
	try {
	  // This file is written in network (big-endian) format
	  // Version is 3.0
	  // Write File Header (Section 1)
	  std::string magic = BPMAP_FILE_HEADER_BYTES;
	  WriteFixedString(of, magic, BPMAP_FILE_HEADER_LEN); // 1.1
	  WriteFloat_N(of, m_Version);                        // 1.2
	  WriteUInt32_N(of, (uint32_t)m_SequenceItemList.size());       // 1.3

	  // Write blocks of Sequence description (Section 2)
	  unsigned int offsetPlaceHolder = 0;
	  std::list<CGDACSequenceItemWriter>::iterator i;
	  for(i=m_SequenceItemList.begin(); i != m_SequenceItemList.end(); ++i) {
	    WriteString_N(of, (*i).m_Name);           // 2.1, 2.2
	    WriteUInt32_N(of, (*i).m_ProbePairsValue);// 2.3
	    (*i).m_offsetPosition = of.tellp();
	    WriteUInt32_N(of, offsetPlaceHolder);     // 2.4
	    WriteUInt32_N(of, (uint32_t)(*i).m_HitList.size()); // 2.5
	    WriteString_N(of, (*i).m_GroupName);      // 2.6, 2.7
	    WriteString_N(of, (*i).m_SeqVersion);     // 2.8, 2.9
	    WriteUInt32_N(of, (uint32_t)(*i).m_Parameters.size());// 2.10
	    TagValuePairTypeVector::iterator j;
	    for(j=(*i).m_Parameters.begin(); j != (*i).m_Parameters.end(); ++j) {
	      WriteString_N(of, (*j).Tag);            // 2.10.1, 2.10.2
	      WriteString_N(of, (*j).Value);          // 2.10.3, 2.10.4
	    }
	  }
	  // Write blocks of Probe-sequence mappings (Section 3)
	  unsigned int sequenceIndex = 0;
	  for(i=m_SequenceItemList.begin(); i != m_SequenceItemList.end(); ++i) {
	    (*i).m_HitStartPosition = of.tellp();

	    WriteUInt32_N(of, sequenceIndex);    // 3.1.1 SequenceID (really index)
	    sequenceIndex++;
	    	  
	    std::list<GDACSequenceHitItemType>::iterator j;
	    for(j=(*i).m_HitList.begin(); j != (*i).m_HitList.end(); ++j) {	  
	      WriteUInt32_N(of, (*j).PMX);       // 3.2.1
	      WriteUInt32_N(of, (*j).PMY);       // 3.2.2
	      if((*i).m_ProbePairsValue == PM_MM){   
		WriteUInt32_N(of, (*j).MMX);     // 3.2.3
		WriteUInt32_N(of, (*j).MMY);     // 3.2.4
	      }
	      WriteUInt8(of, (*j).ProbeLength);  // 3.2.5
	      WriteCharacterArray(of, (*j).PackedPMProbe, PROBE_STORAGE_BUFFER_LENGTH); // 3.2.6
	      WriteFloat_N(of, (*j).MatchScore); // 3.2.7
	      WriteUInt32_N(of, (*j).Position);  // 3.2.8
	      WriteUInt8(of, (*j).TopStrand);    // 3.2.9
	    }
	  }
	  // Write offsets into file
	  for(i=m_SequenceItemList.begin(); i != m_SequenceItemList.end(); ++i) {
	    of.seekp((*i).m_offsetPosition);
	    WriteUInt32_N(of, (*i).m_HitStartPosition);   // 2.4 fixup
	  }
	  of.close();
	  return( true );
	}
	catch (std::string exceptionString) {
	  m_strError = "Exception: " + exceptionString + " raised while writing output file " + m_FileName;
	  of.close();
	}
	return( false );
}

