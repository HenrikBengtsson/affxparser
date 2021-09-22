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
 * @file   GenoCallCoder.h
 * @author Ray Wheeler
 * @date   Tue May 27 12:34:42 PDT 2008
 * 
 * @brief  Genotype call en/decoder
 */
#ifndef _CALLCODER_H_
#define _CALLCODER_H_

//
#include <cstring>
#include <map>
#include <string>
#include <vector>


class AlleleRecord {

public:

  /** 
   * @brief Constructor
   * @param probeset_id - string identifying marker
   * @param abstract_allele - string (char) identifying allele independently of bases
   * @param reference_allele - string representing delimited list of design-time allele bases
   * @param report_allele - string representing delimited list of allele bases as they should be reported
   * @param allowable_cn_states = string representing delimited list of allowable copy number states for this marker
   * @param allele_count - number of alleles for this marker
   */
/*   AlleleRecord(const std::string probeset_id,  */
/* 	       const std::string abstract_allele,  */
/* 	       const std::string reference_allele, */
/* 	       const std::string report_allele, */
/* 	       const int allele_count, */
/* 	       const std::vector<unsigned char> allowable_cn_states */
/*    ) : */
/*     m_probeset_id(probeset_id),  */
/*     m_abstract_allele(abstract_allele),  */
/*     m_reference_allele(reference_allele), */
/*     m_report_allele(report_allele), */
/*     m_allele_count(allele_count), */
/*     m_allowable_cn_states(allowable_cn_states) */
/*       {} */

/*   AlleleRecord(const std::string probeset_id,  */
/* 	       const std::string abstract_allele,  */
/* 	       const std::string reference_allele, */
/* 	       const std::string report_allele */
/*    ) : */
/*     m_probeset_id(probeset_id),  */
/*     m_abstract_allele(abstract_allele),  */
/*     m_reference_allele(reference_allele), */
/*     m_report_allele(report_allele) */
/*       { */
/* 	m_allele_count = 2; */
/* 	m_allowable_cn_states.push_back(2); */
/*       } */

/*   std::string m_probeset_id; */
/*   std::string m_abstract_allele; */
/*   std::string m_reference_allele; */
/*   std::string m_report_allele; */
/*   int m_allele_count; */
/*   std::vector<unsigned char> m_allowable_cn_states; */
  
  AlleleRecord(const std::string probeset_id, 
	       const std::vector<std::string> abstract_allele, 
	       const std::vector<std::string> reference_allele,
	       const std::vector<std::string> report_allele,
	       const int allele_count,
	       const std::vector<unsigned char> allowable_cn_states
               );

  AlleleRecord(const std::string probeset_id, 
	       const std::vector<std::string> abstract_allele, 
	       const std::vector<std::string> reference_allele,
	       const std::vector<std::string> report_allele
               );

  std::string m_probeset_id;
  std::vector<std::string> m_abstract_allele;
  std::vector<std::string> m_reference_allele;
  std::vector<std::string> m_report_allele;
  int m_allele_count;
  std::vector<unsigned char> m_allowable_cn_states;
  
  /** 
   * @brief Function for sorting/searching a vector of allele records sorted by probeset_id
   * @param a - one of two records to compare for sorting
   * @param b - one of two records to compare for sorting
   */
/*   static bool compareAlleleRecords(AlleleRecord a, AlleleRecord b) { */
/*     if (a.m_probeset_id != b.m_probeset_id) { */
/*       return (a.m_probeset_id < b.m_probeset_id); */
/*     } */
/*     else { */
/*       return (a.m_abstract_allele < b.m_abstract_allele);     */
/*     } */
/*   } */
  static bool compareAlleleRecordsByProbesetId(AlleleRecord a, AlleleRecord b);

};


/**
 * GenoCallCoder - Class for encoding and decoding numeric genotype call
 * codes to and from allele strings
 */
class GenoCallCoder {

public:


  //typedef unsigned char code_t;
  
//   /**
//    * @brief Default constructor
//    */
//   GenoCallCoder();

  /** 
   * @brief Constructor
   * @param max_allele_count - number of possible alleles for marker with highest allele count
   * @param data_size - string describing size of call field
   * @param allele_delimiter - character to delimit alleles in string
   * @param version - numeric version of coding scheme
   */
  GenoCallCoder(const int max_allele_count=2, 
		const std::string data_size="UCHAR", 
		const std::string version="0", 
		char allele_delimiter='\0');

  /** 
   * @brief Constructor
   * @param max_allele_count - number of possible alleles for marker with highest allele count
   * @param data_size - string describing size of call field
   * @param version - numeric version of coding scheme
   * @param allele_delimiter - character to delimit alleles in string
   * @param probeset_allele_name_table - vector of (size 3 or 4) vector containing probeset ids, allele names, abstract alleles, and optional boolean report RevComp fields
   */
  //GenoCallCoder(const int max_allele_count, const std::string data_size, const std::string version, const char allele_delimiter, const std::vector<std::vector<std::string> > probeset_allele_name_table);

  /** 
   * @brief Constructor
   * @param max_allele_count - number of possible alleles for marker with highest allele count
   * @param data_size - string describing size of call field
   * @param version - numeric version of coding scheme
   * @param allele_delimiter - character to delimit alleles in string
   * @param marker_annotation_file - TsvFile instance of marker annotation file with columns for probeset ids, allele names, abstract alleles, and optional boolean report RevComp fields
   */
  GenoCallCoder(const int max_allele_count, 
		const std::string data_size, 
		const std::string version, 
		char allele_delimiter, 
		const std::string marker_annotation_file);

  /**
   * @brief Creates and populates marker translation data structure from given file path to marker annotation file
   * @param filepath - path to marker annotation file
   * @param use_report_revcomp - boolean to indicate whether report_revcomp should be utilized
   */
  //  static std::vector<std::vector<std::string> > getProbesetAlleleTranslationsFromTsvFile(std::string filepath, boolean use_report_revcomp);

  /** 
   * @brief Populate member data structures from given input
   * @param max_allele_count - number of possible alleles for marker with highest allele count
   * @param data_size - string describing size of call field
   * @param version - numeric version of coding scheme
   * @param allele_delimiter - character to delimit alleles in string
   */
  void initialize(const int max_allele_count, 
		  const std::string data_size, 
		  const std::string version, 
		  char allele_delimiter);

  /**
   * @brief Retrieve vector of probeset ids in allele name table
   */
  std::vector<std::string> getProbesetIds();

  /**
   * @brief Retrieve vector of genotype call codes allowable for given probeset_id -- based on probeset's allele count and allowable copy number states.
   * @param probeset_id - id of probeset to get codes for
   */
  std::vector<unsigned char> getValidGenotypeCallCodes(std::string probeset_id);


  /**
   * @brief Decode from call code number to abstract allele string
   * @param call_code - number to decode
   */
  // alternate names:
  //  std::string decodeCall(const unsigned char call_code);
  std::string genotypeCallNumToAbstractAllele(const unsigned char call_code);

  /**
   * @brief Decode from call code number to design allele name string for
   * given probeset id
   * 
   * @param probeset_id - id of probeset to decode
   * @param call_code - number to decode
   */
  std::string genotypeCallNumToReferenceAllele(const std::string probeset_id, const unsigned char call_code);

  /**
   * @brief Decode from call code number to reporting allele name string for
   * given probeset id
   * 
   * @param probeset_id - id of probeset to decode
   * @param call_code - number to decode
   */
  std::string genotypeCallNumToReportAllele(const std::string probeset_id, const unsigned char call_code);


  /**
   * @brief Translate design allele name to reporting allele name for given probeset id
   * 
   * @param probeset_id - id of probeset to decode
   * @param reference_allele - string of design alleles to be converted to report alleles
   */
  std::string referenceAlleleToReportAllele(const std::string probeset_id, const std::string reference_allele);

  /**
   * @brief Translate abstract allele to design allele name for given
   * probeset id
   * 
   * @param probeset_id - id of probeset to decode
   * @param abstract_allele - abstract allele to decode
   */
  std::string abstractAlleleToReferenceAllele(const std::string probeset_id, const std::string abstract_allele);

  /**
   * @brief Translate abstract allele to reporting allele name for given
   * probeset id
   * 
   * @param probeset_id - id of probeset to decode
   * @param abstract_allele - abstract allele to decode
   */
  std::string abstractAlleleToReportAllele(const std::string probeset_id, const std::string abstract_allele);

  //  std::string alleleNameToAllele(const std::string probeset_id, const std::string allele_name);

  /**
   * @brief Decode from call code number to allele name string for
   * given probeset id
   * 
   * @param probeset_id - id of probeset to decode
   * @param alleleName - number to decode
   */
  //unsigned char alleleNameToGenotypeCallNum(const std::string probeset_id, const std::string allele_name);

  // Not sure how to handle probeset names with these methods
/*   std::vector<std::string> CallVecToAlleleNameVec(const std::vector<unsigned char> &call_codes); */
/*   std::vector<std::string> AlleleVecToAlleleNameVec(const std::vector<std::string> &alleles); */
/*   std::vector<std::string> AlleleNameVecToAlleleVec(const std::vector<std::string> &allele_names); */
/*   std::vector<unsigned char> AlleleNameVecToCallVec(const std::vector<std::string> &allele_names); */


  /**
   * @brief Encode from allele string to numeric call code
   * @param allele_string - string of allele calls to encode
   */
  unsigned char abstractAlleleToGenotypeCallNum(const std::string allele_string);

  /**
   * @brief Takes vector of abstract allele characters encoded as
   * integers and returns genotype call number
   * @param allele_string - string of allele calls to encode
   */
  unsigned char abstractAlleleIntegersToGenotypeCallNum(const std::vector<int> abstract_allele_ints);

  /**
   * @brief Decode vector of call code numbers to vector of allele strings
   * @param call_codes - vector of numbers to decode
   */
  std::vector<std::string> genotypeCallNumVecToAbstractAlleleVec(const std::vector<unsigned char> &call_codes);

  /**
   * @brief Encode from vector of allele strings to vector of numeric call codes
   * @param allele_string - vector of allele call strings to encode
   */
  std::vector<unsigned char> abstractAlleleVecToGenotypeCallNumVec(const std::vector<std::string> &allele_string);

  /**
   * @brief Retrieve a vector of abstract allele calls indexed by numeric call code
   */
  std::vector<std::string> getAbstractAlleleVector() {std::vector<std::string> rtn_vec(m_abstract_codes); return rtn_vec;}

  /**
   * @brief boolean indicating if code is within proper range for given GenoCallCoder instance
   * @param call_code - numeric call code to test
   */
  bool isValidCallNum(const unsigned char call_code);

  /**
   * @brief boolean indicating if allele string is within proper set of possible alleles for given GenoCallCoder instance
   * @param allele - string of alleles to test
   */
  bool isValidAbstractAllele(const std::string allele);
  /**
 * @brief boolean indicating if call code corresponds to a homozygous call.
 * @param call_code - numeric call code to test
 */

  /**
   * @brief boolean indicating if call code corresponds to a homozygous call
   * @param call_code - numeric call code to test
   */
  bool isHom(const unsigned char call_code);

  /**
   * @brief boolean indicating if call code corresponds to a heterozygous call
   * @param call_code - numeric call code to test
   */
  bool isHet(const unsigned char call_code);

  
private:
  
  /// annotation csv file intra-column delimiter
  static const std::string m_annotation_csv_delimiter;
  /// extra codes
  static const std::string m_extra_codes[];
  /// list of columns to grab from marker annotation file
  static const std::string m_marker_annotation_file_cnames[];
  /// version zero codes
  static const std::string m_version_zero_codes[];
  /// character to represent a nocall when making partial calls (v1+)
  static const char m_abstract_nocall_char = 'n';
  /// string in annotation csv file indicating that there are no alleles to decode
  static const std::string m_annot_csv_skip_text;
  /// alphabet map
  static const char m_num2alpha[];

  /// number of alleles for marker with highest allele count
  int m_max_allele_count;
  /// string arg indicating size of call code field
  std::string m_data_size;
  /// number of bits used for encoding
  int m_data_size_bits;
  /// version dependent min call code
  unsigned int m_min_code;
  /// version dependent min translatable call code
  unsigned int m_min_translatable_code;
  /// data_size dependent max call code
  unsigned int m_max_code;
  /// version dependent min call code for extra codes
  unsigned int m_min_extra_code;
  /// version dependent max call code for extra codes
  unsigned int m_max_extra_code;
  /// version
  std::string m_version;
  /// character to delimit alleles in allele string
  char m_allele_delimiter;
  /// hard-coded codes, for now
  std::vector<std::string> m_abstract_codes;
  /// hard-coded code map, for now
  std::map<std::string, unsigned char> m_alleles;
  /// table relating abstract alleles to allele names for each marker
  std::vector<AlleleRecord> m_probeset_allele_name_table;
  /// legacy code map
  //  map<string, int> m_legacy_alleles;

  //alphabet map to integers
  std::map<char, int> m_alpha2num;

  std::string genotypeCallNumToAllele(const std::string probeset_id, 
				      const unsigned char call_code, 
				      const std::string allele_type);
  std::string alleleNameConvert(const std::string probeset_id, 
				const std::string input_allele_string, 
				const std::string input_type,
				const std::string output_type);


};

#endif /* _CALLCODER_H_ */



