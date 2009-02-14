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
 * @file   GenoCallCoder.cpp
 * @author Ray Wheeler
 * @date   Tue May 27 13:31:31 PDT 2008
 * 
 * @brief  Genotype call en/decoder
 */

//
#include <algorithm>
#include <assert.h>
#include <map>
#include <string>
#include <vector>
#include <iostream>
//
#include "GenoCallCoder.h"
#include "file/TsvFile/TsvFile.h"
#include "util/Convert.h"
#include "util/Verbose.h"
#include "util/Err.h"

using namespace std;
using namespace affx;

const string GenoCallCoder::m_annotation_csv_delimiter = " // ";

const string GenoCallCoder::m_annot_csv_skip_text = "N/A";

const string GenoCallCoder::m_extra_codes[] = {
  string("NotAvailable"),
  string("PossibleRareAllele")
};

const string GenoCallCoder::m_marker_annotation_file_cnames[] = {
  string("Probe Set ID"),
  //string("Abstract Allele Name"),
  string("Allele Code"),
  string("Alleles Design Strand"),
  string("Alleles Reported Strand"),
  string("Alleles-Alias Reported Strand")
};

const string GenoCallCoder::m_version_zero_codes[] = {
  string(""), // [0]
  string(""), // [1]
  string(""), // [2]
  string(""), // [3]
  string(""), // [4]
  string(""), // [5]
  string("AA"), // [6]
  string("BB"), // [7]
  string("AB"), // [8]
  string("AB_A"), // [9]
  string("AB_B"), // [10]
  string("NN"), // [11]
  string(""), // [12]
  string(""), // [13]
  string(""), // [14]
  string("") // [15]
};


const char GenoCallCoder::m_num2alpha[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};



/** 
 * @brief constructor
 * @param max_allele_count - number of possible alleles for marker with highest allele count
 * @param data_size - string describing size of call field
 * @param version - numeric version of coding scheme
 * @param allele_delimiter - character to delimit alleles in string
 */

GenoCallCoder::GenoCallCoder(const int max_allele_count, 
			     const string data_size, 
			     const string version, 
			     const char allele_delimiter) {
  initialize(max_allele_count, data_size, version, allele_delimiter);
}

/** 
 * @brief constructor
 * @param max_allele_count - number of possible alleles for marker with highest allele count
 * @param data_size - string describing size of call field
 * @param version - numeric version of coding scheme
 * @param allele_delimiter - character to delimit alleles in string
 * @param probeset_allele_name_table - vector of AlleleRecords (size 3 vector) containing probeset ids, allele names, and abstract alleles
 */

// GenoCallCoder::GenoCallCoder(const int max_allele_count, const string data_size, const string version, const char allele_delimiter, const vector<AlleleRecord> probeset_allele_name_table) {
//   initialize(max_allele_count, data_size, version, allele_delimiter);
  
//   m_probeset_allele_name_table = probeset_allele_name_table;
//   vector<AlleleRecord>::iterator at_start = m_probeset_allele_name_table.begin();
//   vector<AlleleRecord>::iterator at_end = m_probeset_allele_name_table.end();
//   sort(at_start, at_end, compareAlleleRecordsByProbesetId);
// }

/** 
 * @brief Constructor
 * @param max_allele_count - number of possible alleles for marker with highest allele count
 * @param data_size - string describing size of call field
 * @param version - numeric version of coding scheme
 * @param allele_delimiter - character to delimit alleles in string
 * @param probeset_allele_name_table - vector of (size 3 or 4) vector containing probeset ids, allele names, abstract alleles, and optional boolean report RevComp fields
 */
// GenoCallCoder(const int max_allele_count, const string data_size, const string version, const char allele_delimiter, const vector<vector<string> > probeset_allele_name_table) {
//   initialize(max_allele_count, data_size, version, allele_delimiter);

//   vector<vector<string> >::iterator at_it;
//   for (at_it = probeset_allele_name_table.begin(); at_it != probeset_allele_name_table.end(); at_it++) {
    
//     if (at_it->size() < 3) {
//       Verbose::die(0, "Improperly formated row in probeset allele name table");
//       continue;
//     }

//     string probeset_id = at_it[0];
//     string reference_allele = at_it[1];
//     string abstract_allele = at_it[2];

//     string report_revcomp;
//     if (at_it->size() == 4) {
//        report_revcomp = at_it[3];
//     }
//     else {
//       report_revcomp = "0";
//     }

//     if (probeset_id.empty()) {
//       Verbose::warn(0, "Empty probeset_id string in  probeset_allele_name_table");
//       continue;
//     }

//     if (reference_allele.empty()) {
//       Verbose::warn(0, "Empty reference_allele string for probeset_id '" + probeset_id + "' in  probeset_allele_name_table.");
//       continue;
//     }

//     if (abstract_allele.empty()) {
//       Verbose::warn(0, "Empty abstract_allele string for probeset_id " + probeset_id + "' in  probeset_allele_name_table.");
//     }

//     if (report_revcomp.empty()) {
//       Verbose::warn(0, "Empty report_revcomp boolean for probeset_id " + probeset_id + "' in  probeset_allele_name_table.");
//     }


//     int allele_count = 0;
//     if (allele_count == 0) {
//       // alleles were not counted above
//       idx = abstract_allele.find(allele_delimiter);
//       while (idx != string::npos) {
// 	allele_count++;
// 	idx = abstract_allele.find(allele_delimiter, idx + 1);
//       }
//     }

//     // allele_count = number of delimiters + 1
//     allele_count++;

    

  
//   for (i = 0; 

//   m_probeset_allele_name_table = probeset_allele_name_table;



//   sort(at_start, at_end, compareAlleleRecordsByProbesetIdAndAbstractAllele);  
// }

/** 
 * @brief Constructor
 * @param max_allele_count - number of possible alleles for marker with highest allele count
 * @param data_size - string describing size of call field
 * @param version - numeric version of coding scheme
 * @param allele_delimiter - character to delimit alleles in string
 * @param marker_annotation_file - TsvFile instance of marker annotation file with columns for probeset ids, allele names, abstract alleles, and optional boolean report RevComp fields
 */
GenoCallCoder::GenoCallCoder(const int max_allele_count, 
			     const string data_size, 
			     const string version, 
			     const char allele_delimiter, 
			     const string marker_annotation_file) {
  initialize(max_allele_count, data_size, version, allele_delimiter);

  /* HACK!!! Hard code valid call codes. */
  vector<vector<vector<unsigned char> > > valid_call_codes(3);
  vector<unsigned char> cn0(1, 17);
  vector<vector<unsigned char> > cn0_row(max_allele_count + 1, cn0);
  valid_call_codes[0] = cn0_row;
  vector<unsigned char> cn1(1, 18);
  vector<vector<unsigned char> > cn1_row(max_allele_count + 1, cn1);
  valid_call_codes[1] = cn1_row;
  vector<unsigned char> cn2(0);
  vector<vector<unsigned char> > cn2_row(max_allele_count + 1, cn2);
  valid_call_codes[2] = cn2_row;
  string ab(1, allele_delimiter);
  ab += "A";

  for (int i = 2; i <= max_allele_count; i++) {
    for (int j = 1; j < i; j++) {
      valid_call_codes[1][i].push_back(18+j);
    }

    switch (i) {
    case 2:
      ab += "B";
      break;
    case 3:
      ab += "C";
      break;
    case 4:
      ab += "D";
      break;
    case 5:
      ab += "E";
      break;
    case 6:
      ab += "F";
      break;
    }

    int start = m_min_translatable_code + max_allele_count + 1;
    int stop = start + (max_allele_count + 1 + ((max_allele_count + 1) * (max_allele_count + 1))) / 2;
    for (int j = start; j < stop; j++) {
      //    for (int j = 25; j <= 50; j++) {
      if (m_abstract_codes[j].find_first_not_of(ab) == m_abstract_codes[j].npos) {
	valid_call_codes[2][i].push_back(j);
      }
    }
  }

  TsvFile annot_csv;
  annot_csv.m_optAutoTrim = true; 
  if ( annot_csv.open( marker_annotation_file ) != TSV_OK ) {
    Err::errAbort (marker_annotation_file + ": failed to open marker annotation file.");
  }

  int marker_annotation_file_cnames_count = sizeof(m_marker_annotation_file_cnames) / sizeof (m_marker_annotation_file_cnames[0]);

  vector<int> annot_csv_cidxs;
  int i;
  for (i = 0; i < marker_annotation_file_cnames_count; i++) {
    int temp_idx = annot_csv.cname2cidx(0, m_marker_annotation_file_cnames[i]);
    if (temp_idx == TSV_ERR_NOTFOUND) {
      Err::errAbort ("Couldn't find expected column name '" + m_marker_annotation_file_cnames[i] + "' in marker annotation file :" + marker_annotation_file);
    }
    else {
      annot_csv_cidxs.push_back(temp_idx);
    }
  }

  //  int c = 0;

  while (annot_csv.nextLevel(0) == TSV_OK) {
    // get probeset_id
    string probeset_id;
    if (annot_csv.get(0, annot_csv_cidxs[0], probeset_id) != TSV_OK) {
      Err::errAbort("Failure to get value from '" + m_marker_annotation_file_cnames[i] + "' field in marker annotation file:" + marker_annotation_file);
    }

    // get abstract alleles, reference, report alleles, & report aliases
    vector<vector<string> > allele_lists;
    for (i = 1; i < annot_csv_cidxs.size(); i++) {
      string col_value;
      if (annot_csv.get(0, annot_csv_cidxs[i], col_value) != TSV_OK) {
        Err::errAbort("Failure to get value from '" + m_marker_annotation_file_cnames[i] + "' field in marker annotation file:" + marker_annotation_file);
      }

      // if annot.csv row doesn't have an allele to translate, e.g. a tiling
      // probe used for copy number, then skip it.  erase allele_lists as a
      // flag to outer while-loop to skip
      if (col_value == m_annot_csv_skip_text && m_marker_annotation_file_cnames[i] != "Alleles-Alias Reported Strand") {
	allele_lists.resize(0);
	break;
      }

      int allele_start = 0;
      int idx = col_value.find(m_annotation_csv_delimiter);
      vector<string> allele_vec;
      //cout << col_value + '\n';
      while (allele_start < col_value.size()) {
	//cout << allele_start << '-' << idx << '-' << col_value.size() << ':';
	//cout << col_value.substr(allele_start, idx - allele_start) + "\n";
	allele_vec.push_back(col_value.substr(allele_start, idx - allele_start));
	allele_start = idx + m_annotation_csv_delimiter.size();
	idx = col_value.find(m_annotation_csv_delimiter, idx + m_annotation_csv_delimiter.size());
	if (idx == col_value.npos) {
	  idx = col_value.size();
	}
      }
      // if there is a report alias, then pop off vector of report
      // alleles in order to push on a vector of aliases in its place.
      // Otherwise do not store useless alias vector
      if (m_marker_annotation_file_cnames[i] == "Alleles-Alias Reported Strand") {
	if (col_value != m_annot_csv_skip_text) {
	  allele_lists.pop_back();
	}
	else {
	  break;
	}
      }
      allele_lists.push_back(allele_vec);
    }
//     if (c++ < 3) {
//       cout << allele_lists[0] + ':' + allele_lists[1] + ':' + allele_lists[2] + '\n';
//     }

    // The number of alleles for a marker can be determined by the length of the delimited list of abstract alleles, since abstract alleles and delimiters each have length == 1.  length_of_list == num_alleles + num_delimiters == num_alleles + (num_alleles - 1).  num_alleles = (length_of_list + 1) / 2.

    //    int num_alleles = (allele_lists[1].size() + 1) / 2;
    // skip row if none of "Allele Code", "Alleles Design Strand", "Alleles Reported Strand" is "N/A"
    if (allele_lists.empty()) {
      continue;
    }


    int num_alleles = allele_lists[0].size();

//     vector<unsigned char> cn_states(3);
//     cn_states.push_back(0);
//     cn_states.push_back(1);
//     cn_states.push_back(2);

    vector<unsigned char> call_code_list(1, 17);
    for (int j = 1; j < valid_call_codes.size(); j++) {
      call_code_list.insert(call_code_list.end(), valid_call_codes[j][num_alleles].begin(), valid_call_codes[j][num_alleles].end());
    }

//     for (int j = 0; j < call_code_list.size(); j++) {
//       cout << static_cast<int>(call_code_list[j]) << ' ';
//     }
//     cout << '\n';
    AlleleRecord new_record(probeset_id, allele_lists[0], allele_lists[1], allele_lists[2], num_alleles, call_code_list);
    m_probeset_allele_name_table.push_back(new_record);
  }

  sort(m_probeset_allele_name_table.begin(), m_probeset_allele_name_table.end(), AlleleRecord::compareAlleleRecordsByProbesetId);
//   for (i = 0; i < m_probeset_allele_name_table.size(); i++) {
//     cout << m_probeset_allele_name_table[i].m_probeset_id + ' ';
//     cout << m_probeset_allele_name_table[i].m_abstract_allele + ' ';
//     cout << m_probeset_allele_name_table[i].m_reference_allele + ' ';
//     cout << m_probeset_allele_name_table[i].m_report_allele + '\n';
//   }
}

/**
 * @brief Creates and populates marker translation data structure from given file path to marker annotation file
 * @param filepath - path to marker annotation file
 * @param use_report_revcomp - boolean to indicate whether report_revcomp should be utilized
 */
// static vector<vector<string> > getProbesetAlleleTranslationsFromTsvFile(string filepath, const char allele_delimiter) {


//   // replace annotation csv delimiter with user-supplied delimiter
//   string ANNOTATION_CSV_DELIMITER = "//";
//   size_type acd_size = ANNOTATION_CSV_DELIMITER.size();
//   size_type idx = reference_allele.find(ANNOTATION_CSV_DELIMITER);
//   while (idx != reference_allele.npos) {
//     reference_allele.replace(idx, acd_size, allele_delimiter);
//     idx = reference_allele.find(ANNOTATION_CSV_DELIMITER, idx + 1);
//   }
    
//   size_type idx = abstract_allele.find(ANNOTATION_CSV_DELIMITER);
//   while (idx != abstract_allele.npos) {
//     abstract_allele.replace(idx, acd_size, allele_delimiter);
//     idx = abstract_allele.find(ANNOTATION_CSV_DELIMITER, idx + 1);
//     // might as well count alleles now
//   }

// }

/** 
 * @brief initialize
 * @param max_allele_count - number of possible alleles for marker with highest allele count
 * @param data_size - string describing size of call field
 * @param version - numeric version of coding scheme
 */
void GenoCallCoder::initialize(const int max_allele_count, 
			       const string data_size, 
			       const string version, 
			       char allele_delimiter) {


  // enforcing dmet parameters for now
  if (data_size == "UCHAR") {
    m_data_size_bits = 8;
  }
  else {
    Verbose::warn(0, "'" + data_size + "' is not an accepted data_size.  'UCHAR' is currently the only accepted data_size.");
  }

  int version_zero_codes_count = sizeof(m_version_zero_codes) / sizeof(m_version_zero_codes[0]);
  int extra_codes_count = sizeof(m_extra_codes) / sizeof(m_extra_codes[0]);
  int num2alpha_count = sizeof(m_num2alpha) / sizeof(m_num2alpha[0]);
  int i;

  m_max_extra_code = (1 << m_data_size_bits) - 1;
  m_abstract_codes.resize(m_max_extra_code + 1);

  for (i = 0; i < num2alpha_count; i++) {
    m_alpha2num[m_num2alpha[i]] = i;
    //m_alpha2num.insert(map<char,int>::value_type(m_num2alpha[i], i));
  }


  if (version == "0.0" || version == "0") {
    m_max_allele_count = 2;
    m_min_code = 6;
    m_min_translatable_code = 6;
    m_max_code = 11;
    m_min_extra_code = 1 << m_data_size_bits;
    m_version = "0";

    for (i = 0; i < version_zero_codes_count; i++) {
      m_abstract_codes[i] = m_version_zero_codes[i];
      //       if (! m_version_zero_codes[i].empty()) {
      // 	m_alleles[m_version_zero_codes[i]] = i;
      //       }
    }


    if (allele_delimiter != '\0') {
      allele_delimiter = '\0';
      Verbose::warn(0, "Ignoring given allele_delimiter '" + string(1,allele_delimiter) + "' -- arbitrary allele delimter is not supported for version 0.");
//       for (i = 6; i <= 8; i++) {
// 	string new_allele_string;
// 	new_allele_string = m_abstract_codes[i][0];
// 	//cout << "init: " << new_allele_string << '\n';
// 	for (int j = 1; j < m_abstract_codes[i].size(); j++) {
// 	  new_allele_string += allele_delimiter;
// 	  new_allele_string += m_abstract_codes[i][j];
// 	  //cout << "adding: " << allele_delimiter << m_abstract_codes[i][j] << '\n';
// 	}
// 	//cout << "done: " << new_allele_string << '\n';
// 	m_abstract_codes[i].assign(new_allele_string);
//        }
    }
  }
  else {
    assert(max_allele_count == 6);
    assert(version == "1.0");
    //    assert(allele_delimiter == '\0');


    if (version == string("1.0")) {
      m_min_extra_code = 254;
      //      m_max_code = calculateMaxCall(max_allele_count, data_size);
    }

    assert(m_max_extra_code - m_min_extra_code + 1 <= extra_codes_count);

    m_max_allele_count = max_allele_count;
    m_min_code = 16;
    m_min_translatable_code = 18;
    //    m_max_code = 136;  // %!% HARD CODED.  needs to be determined dynamically
    m_version = version;

    m_abstract_codes[16] = "NoCall";  // Unable to genotype
    m_abstract_codes[17] = "ZeroCopyNumber";
    //m_abstract_codes[17] = "0";

    if (max_allele_count == 3) {
      m_abstract_codes[18] = "A";
      m_abstract_codes[19] = "B";
      m_abstract_codes[20] = "C";
      m_abstract_codes[21] = "n";  // CN=1 model applied; unable to genotype
      m_abstract_codes[22] = "AA";
      m_abstract_codes[23] = "AB";
      m_abstract_codes[24] = "AC";
      m_abstract_codes[25] = "An";
      m_abstract_codes[26] = "BB";
      m_abstract_codes[27] = "BC";
      m_abstract_codes[28] = "Bn";
      m_abstract_codes[29] = "CC";
      m_abstract_codes[30] = "Cn";
      m_abstract_codes[31] = "nn";

      m_max_code = 31;  // %!% HARD CODED.  needs to be determined dynamically
    }
    else if (max_allele_count == 6) {
      m_abstract_codes[18] = "A";
      m_abstract_codes[19] = "B";
      m_abstract_codes[20] = "C";
      m_abstract_codes[21] = "D";
      m_abstract_codes[22] = "E";
      m_abstract_codes[23] = "F";
      m_abstract_codes[24] = "n";  // CN=1 model applied; unable to genotype
      m_abstract_codes[25] = "AA";
      m_abstract_codes[26] = "AB";
      m_abstract_codes[27] = "AC";
      m_abstract_codes[28] = "AD";
      m_abstract_codes[29] = "AE";
      m_abstract_codes[30] = "AF";
      m_abstract_codes[31] = "An";
      m_abstract_codes[32] = "BB";
      m_abstract_codes[33] = "BC";
      m_abstract_codes[34] = "BD";
      m_abstract_codes[35] = "BE";
      m_abstract_codes[36] = "BF";
      m_abstract_codes[37] = "Bn";
      m_abstract_codes[38] = "CC";
      m_abstract_codes[39] = "CD";
      m_abstract_codes[40] = "CE";
      m_abstract_codes[41] = "CF";
      m_abstract_codes[42] = "Cn";
      m_abstract_codes[43] = "DD";
      m_abstract_codes[44] = "DE";
      m_abstract_codes[45] = "DF";
      m_abstract_codes[46] = "Dn";
      m_abstract_codes[47] = "EE";
      m_abstract_codes[48] = "EF";
      m_abstract_codes[49] = "En";
      m_abstract_codes[50] = "FF";
      m_abstract_codes[51] = "Fn";
      m_abstract_codes[52] = "nn";
      m_abstract_codes[53] = "AAA";
      m_abstract_codes[54] = "AAB";
      m_abstract_codes[55] = "AAC";
      m_abstract_codes[56] = "AAD";
      m_abstract_codes[57] = "AAE";
      m_abstract_codes[58] = "AAF";
      m_abstract_codes[59] = "AAn";
      m_abstract_codes[60] = "ABB";
      m_abstract_codes[61] = "ABC";
      m_abstract_codes[62] = "ABD";
      m_abstract_codes[63] = "ABE";
      m_abstract_codes[64] = "ABF";
      m_abstract_codes[65] = "ABn";
      m_abstract_codes[66] = "ACC";
      m_abstract_codes[67] = "ACD";
      m_abstract_codes[68] = "ACE";
      m_abstract_codes[69] = "ACF";
      m_abstract_codes[70] = "ACn";
      m_abstract_codes[71] = "ADD";
      m_abstract_codes[72] = "ADE";
      m_abstract_codes[73] = "ADF";
      m_abstract_codes[74] = "ADn";
      m_abstract_codes[75] = "AEE";
      m_abstract_codes[76] = "AEF";
      m_abstract_codes[77] = "AEn";
      m_abstract_codes[78] = "AFF";
      m_abstract_codes[79] = "AFn";
      m_abstract_codes[80] = "Ann";
      m_abstract_codes[81] = "BBB";
      m_abstract_codes[82] = "BBC";
      m_abstract_codes[83] = "BBD";
      m_abstract_codes[84] = "BBE";
      m_abstract_codes[85] = "BBF";
      m_abstract_codes[86] = "BBn";
      m_abstract_codes[87] = "BCC";
      m_abstract_codes[88] = "BCD";
      m_abstract_codes[89] = "BCE";
      m_abstract_codes[90] = "BCF";
      m_abstract_codes[91] = "BCn";
      m_abstract_codes[92] = "BDD";
      m_abstract_codes[93] = "BDE";
      m_abstract_codes[94] = "BDF";
      m_abstract_codes[95] = "BDn";
      m_abstract_codes[96] = "BEE";
      m_abstract_codes[97] = "BEF";
      m_abstract_codes[98] = "BEn";
      m_abstract_codes[99] = "BFF";
      m_abstract_codes[100] = "BFn";
      m_abstract_codes[101] = "Bnn";
      m_abstract_codes[102] = "CCC";
      m_abstract_codes[103] = "CCD";
      m_abstract_codes[104] = "CCE";
      m_abstract_codes[105] = "CCF";
      m_abstract_codes[106] = "CCn";
      m_abstract_codes[107] = "CDD";
      m_abstract_codes[108] = "CDE";
      m_abstract_codes[109] = "CDF";
      m_abstract_codes[110] = "CDn";
      m_abstract_codes[111] = "CEE";
      m_abstract_codes[112] = "CEF";
      m_abstract_codes[113] = "CEn";
      m_abstract_codes[114] = "CFF";
      m_abstract_codes[115] = "CFn";
      m_abstract_codes[116] = "Cnn";
      m_abstract_codes[117] = "DDD";
      m_abstract_codes[118] = "DDE";
      m_abstract_codes[119] = "DDF";
      m_abstract_codes[120] = "DDn";
      m_abstract_codes[121] = "DEE";
      m_abstract_codes[122] = "DEF";
      m_abstract_codes[123] = "DEn";
      m_abstract_codes[124] = "DFF";
      m_abstract_codes[125] = "DFn";
      m_abstract_codes[126] = "Dnn";
      m_abstract_codes[127] = "EEE";
      m_abstract_codes[128] = "EEF";
      m_abstract_codes[129] = "EEn";
      m_abstract_codes[130] = "EFF";
      m_abstract_codes[131] = "EFn";
      m_abstract_codes[132] = "Enn";
      m_abstract_codes[133] = "FFF";
      m_abstract_codes[134] = "FFn";
      m_abstract_codes[135] = "Fnn";
      m_abstract_codes[136] = "nnn";

      m_max_code = 136;  // %!% HARD CODED.  needs to be determined dynamically
    }
    for (i = 0; i < extra_codes_count; i++) {
      m_abstract_codes[m_max_extra_code - i] = m_extra_codes[i];
    }

    if (allele_delimiter != '\0') {
      for (i = m_min_translatable_code; i <= m_max_code; i++) {
	string new_allele_string;
	new_allele_string = m_abstract_codes[i][0];
	for (int j = 1; j < m_abstract_codes[i].size(); j++) {
	  new_allele_string += allele_delimiter;
	  new_allele_string += m_abstract_codes[i][j];
	}
	m_abstract_codes[i].assign(new_allele_string);
      }
    }
  }
  // populate encoding map
  for (i = m_min_code; i <= m_max_code; i++) {
    m_alleles[m_abstract_codes[i]] = i;
  }
  for (i = m_min_extra_code; i <= m_max_extra_code; i++) {
    m_alleles[m_abstract_codes[i]] = i;
  }
  
  m_allele_delimiter = allele_delimiter;
}


/**
 * @brief Retrieve vector of probeset ids in allele name table
 */
vector<string> GenoCallCoder::getProbesetIds() {
  vector<string> result;
  vector<AlleleRecord>::iterator it;
  for (it = m_probeset_allele_name_table.begin(); it != m_probeset_allele_name_table.end(); it++) {
    result.push_back(it->m_probeset_id);
  }
  return result;
}


/**
 * @brief Retrieve vector of genotype call codes allowable for given probeset_id -- based on probeset's allele count and allowable copy number states.
 * @param probeset_id - id of probeset to get codes for
 */
vector<unsigned char> GenoCallCoder::getValidGenotypeCallCodes(std::string probeset_id) {
  vector<string> temp(0);
  AlleleRecord key(probeset_id, temp, temp, temp);
  vector<AlleleRecord>::iterator result = lower_bound(m_probeset_allele_name_table.begin(), m_probeset_allele_name_table.end(), key, AlleleRecord::compareAlleleRecordsByProbesetId);
  /* HACK!!!  The field allowable_cn_states has been highjacked for some other expedient purpose. */
  return result->m_allowable_cn_states;
}

/**
 * @brief Decode from call code number to allele string
 * @param call_code - number to decode
 */
//template<typename T1> string GenoCallCoder::decodeCall(const T1 call_code) {
string GenoCallCoder::genotypeCallNumToAbstractAllele(const unsigned char call_code) {
  if (call_code < m_abstract_codes.size()) {
    return m_abstract_codes[call_code];
  }
  else {
    Verbose::warn(0, "Call code '" + ToStr(call_code) + "' is out of bounds for size " + m_data_size);
    return string("");
  }
}

/**
 * @brief Decode from call code number to reference allele name string for
 * given probeset id
 * 
 * @param probeset_id - id of probeset to decode
 * @param call_code - number to decode
 */
string GenoCallCoder::genotypeCallNumToReferenceAllele(const string probeset_id, const unsigned char call_code) {
  return genotypeCallNumToAllele(probeset_id, call_code, "reference"); 
}

/**
 * @brief Decode from call code number to report allele string for
 * given probeset id
 * 
 * @param probeset_id - id of probeset to decode
 * @param call_code - number to decode
 */
string GenoCallCoder::genotypeCallNumToReportAllele(const string probeset_id, const unsigned char call_code) {
  return genotypeCallNumToAllele(probeset_id, call_code, "report"); 
}


/**
 * @brief Decode from call code number to allele string for
 * given probeset id and allele type
 * 
 * @param probeset_id - id of probeset to decode
 * @param call_code - number to decode
 * @param allele_type - type of allele string to return
 */
string GenoCallCoder::genotypeCallNumToAllele(const string probeset_id, 
					      const unsigned char call_code, 
					      const string allele_type) {
  string allele_name_string;
  string abstract_allele_string = genotypeCallNumToAbstractAllele(call_code);  
  
  if (call_code >= m_min_translatable_code && call_code <= m_max_code) {
    allele_name_string = abstractAlleleToAlleleName(probeset_id, abstract_allele_string, allele_type);
  }
  else if ((call_code >= m_min_code && call_code < m_min_translatable_code) ||
	   (call_code >= m_min_extra_code && call_code <= m_max_extra_code)
	   ) {
    allele_name_string = abstract_allele_string;
  }
  return allele_name_string;
}

/**
 * @brief Translate abstract allele to allele name for given
 * probeset id and allele type
 * 
 * @param probeset_id - id of probeset to decode
 * @param abstract_allele - abstract allele to decode
 */
string GenoCallCoder::abstractAlleleToAlleleName(const string probeset_id, 
						 const string abstract_allele_string, 
						 const string allele_type) {
  
  string allele_name_string;

  //vector<AlleleRecord>::iterator at_start = m_probeset_allele_name_table.begin();
  //vector<AlleleRecord>::iterator at_end = m_probeset_allele_name_table.end();
  vector<string> temp(0);
  AlleleRecord key(probeset_id, temp, temp, temp);

  //cout << probeset_id + '*' + abstract_allele << '\n';

  vector<AlleleRecord>::iterator result = lower_bound(m_probeset_allele_name_table.begin(), m_probeset_allele_name_table.end(), key, AlleleRecord::compareAlleleRecordsByProbesetId);
  if (result == m_probeset_allele_name_table.end()) {
    Verbose::warn(0, "Cannot find probeset '" + probeset_id + "' in supplied probeset allele table");
  }
  else {
    //cout << result->m_probeset_id + '|' + result->m_abstract_allele + '|' + result->m_reference_allele + '|' + result->m_report_allele + '\n';
    //cout << result->m_probeset_id + '|' + result->m_abstract_allele[0] + '|' + result->m_reference_allele[0] + '|' + result->m_report_allele[0] + '\n';
/* keep this code for future reimplementation of abstractAlleleToAlleleName()
//   while (result != at_end &&
// 	 result->m_probeset_id == probeset_id && 
// 	 result->m_abstract_allele != abstract_allele
// 	 ) {
//     result++;
//   }
  
//   if (result == at_end || result->m_probeset_id != probeset_id) {
//     Verbose::warn(0, "Cannot find allele string '" + abstract_allele + "' for probeset '" + probeset_id + "'\n");
//   }
//   else {
//     allele_name = result->m_allele_name;
//   }
*/

    string::const_iterator aas_it;
    for (aas_it = abstract_allele_string.begin(); aas_it != abstract_allele_string.end(); aas_it++) {
      //cout << *aas_it << '-';
      if (*aas_it == m_allele_delimiter) {
	allele_name_string += m_allele_delimiter;
      }
      else if (*aas_it == m_abstract_nocall_char) {
	// For now, using 'NoCall' text
	allele_name_string.append(m_abstract_codes[16]);
      }
      else {
	int i = 0;
	for (i = 0; i < result->m_abstract_allele.size(); i++) {
	//      while (i < result->m_abstract_allele.size() && aas_it != abstract_allele_string.end()) {	  
	//cout << *aas_it << '-';
	  if (string(1, *aas_it) == result->m_abstract_allele[i]) {
	    if (allele_type == "reference") {
	      allele_name_string.append(result->m_reference_allele[i]);
	      break;
	    }
	    else if (allele_type == "report") {
	      allele_name_string.append(result->m_report_allele[i]);
	      break;
	    }
	  }
	}
	if (i >= result->m_abstract_allele.size()) {
	  Verbose::warn(0, "Cannot find listing for abstract allele '" + string(1, *aas_it) + "' for probeset: " + probeset_id);
	}
      }
    }
    //cout << '\n';
  }
  
  return allele_name_string;
}

/**
 * @brief Translate abstract allele to design allele name for given
 * probeset id
 * 
 * @param probeset_id - id of probeset to decode
 * @param abstract_allele - abstract allele to decode
 */
string GenoCallCoder::abstractAlleleToReferenceAllele(const string probeset_id, const string abstract_allele) {
  return abstractAlleleToAlleleName(probeset_id, abstract_allele, "reference");
}


/**
 * @brief Translate abstract allele to reporting allele name for given
 * probeset id
 * 
 * @param probeset_id - id of probeset to decode
 * @param abstract_allele - abstract allele to decode
 */
string GenoCallCoder::abstractAlleleToReportAllele(const string probeset_id, const string abstract_allele) {
  return abstractAlleleToAlleleName(probeset_id, abstract_allele, "report");
}


/**
 * @brief Encode from allele string to numeric call code
 * @param allele_string - string of allele calls to encode
 */
//template<typename T1> T1 GenoCallCoder::encodeCall(string allele_string) {
unsigned char GenoCallCoder::abstractAlleleToGenotypeCallNum(const string allele_string) {
  if (m_alleles.count(allele_string)) {
    return m_alleles[allele_string];
  }
  else { 
    Verbose::warn(0, "Allele string '" + allele_string + "' is not valid"); 
    return 0;
  }
}

/**
 * @brief Takes vector of abstract allele characters encoded as
 * integers and returns genotype call number
 * @param allele_string - string of allele calls to encode
 */
unsigned char GenoCallCoder::abstractAlleleIntegersToGenotypeCallNum(const vector<int> abstract_allele_ints) {
  
  string abstract_allele_string;
  int alphamap_size = sizeof(m_num2alpha) / sizeof(m_num2alpha[0]);

  //cout << abstract_allele_ints.size()<< ':' << m_num2alpha[abstract_allele_ints[0]] << '\n';
  for (int i = 0; i < abstract_allele_ints.size(); i++) {
    if (abstract_allele_ints[i] >= 0 && abstract_allele_ints[i] < alphamap_size) {
      abstract_allele_string.append(1, m_num2alpha[abstract_allele_ints[i]]);
    }
    else {
      Verbose::warn(0, "Abstract allele integer '" + ToStr(abstract_allele_ints[i]) + "' is out of allele integer range: [0," + ToStr(alphamap_size) + ")");
      return 0;
    }
  }
  
  return abstractAlleleToGenotypeCallNum(abstract_allele_string);
}

/**
 * @brief Decode vector of call code numbers to vector of allele strings
 * @param call_codes - vector of numbers to decode
 */
//template<typename T1> vector<string> GenoCallCoder::decodeCallVec(const vector<T1> &call_codes) {
vector<string> GenoCallCoder::genotypeCallNumVecToAbstractAlleleVec(const vector<unsigned char> &call_codes) {
  int i;
  vector<string> decoded_calls(call_codes.size());
  for (i = 0; i < call_codes.size(); i++) {
    if (call_codes[i] < m_abstract_codes.size()) {
      decoded_calls[i] = m_abstract_codes[call_codes[i]];
    }
    else {
      Verbose::warn(0, "Call code '" + ToStr(call_codes[i]) + "' is out of bounds for size " + m_data_size);
      decoded_calls[i] = string("");
    }
  }
  return decoded_calls;
}

/**
 * @brief Encode from vector of allele strings to vector of numeric call codes
 * @param allele_string - vector of allele call strings to encode
 */
//template<typename T1> vector<T1> GenoCallCoder::encodeCallVec(const vector<string> &allele_string) {
vector<unsigned char> GenoCallCoder::abstractAlleleVecToGenotypeCallNumVec(const vector<string> &allele_string) {
  int i;
  vector<unsigned char> encodedAlleles(allele_string.size());
  for (i = 0; i < allele_string.size(); i++) {
    if (m_alleles.count(allele_string[i])) {
      encodedAlleles[i] = m_alleles[allele_string[i]];
    }
    else {
      Verbose::warn(0, "Allele string '" + allele_string[i] + "' is not valid");
      encodedAlleles[i] = 0;
    }
  }
  return encodedAlleles;
}

/**
 * @brief Retrieve a vector of allele call strings indexed by numeric call code
 */
// vector<string> GenoCallCoder::getDecodeVector() {
//   const vector<string> table = m_abstract_codes;
//   return table;
// }


/**
 * @brief boolean indicating if code is within proper range for given instance
 * @param code - numeric call code to test
 */
bool GenoCallCoder::isValidCallNum(const unsigned char call_code) {
  return ((call_code >= m_min_code && call_code <= m_max_code) ||
	  (call_code >= m_min_extra_code && call_code <= m_max_extra_code));
}

/**
 * @brief boolean indicating if code is within proper range for given instance
 * @param code - numeric call code to test
 */
bool GenoCallCoder::isValidAbstractAllele(const string allele) {
  return m_alleles.count(allele);
}


/**
 * @brief boolean indicating if call code corresponds to a homozygous call.
 * @param call_code - numeric call code to test
 */
bool GenoCallCoder::isHom(const unsigned char call_code) {
  if (call_code >= m_min_translatable_code && call_code <= m_max_code) {
    string abs_allele = genotypeCallNumToAbstractAllele(call_code);
    char last_allele = abs_allele[abs_allele.size()-1];
    //cout << abs_allele + ':' + first_allele + '\n';
    if (m_alpha2num.count(last_allele) == 1) {
      //cout << "here1\n";
      string search_string(1, last_allele);
      search_string.append(string(1, m_allele_delimiter));
      if (abs_allele.find_first_not_of(search_string, 0) == string::npos) {
	//cout << "here2\n";
	return true;
      }
    }
  }
  // ZeroCopyNumber is hom
  else if (call_code == 17) {
    return true;
  }

  return false;
}
/**
 * @brief boolean indicating if call code corresponds to a heterozygous call.  false if call code corresponds to partial call.
 * @param call_code - numeric call code to test
 */
bool GenoCallCoder::isHet(const unsigned char call_code) {
  if (call_code >= m_min_translatable_code && call_code <= m_max_code) {
    string abs_allele = genotypeCallNumToAbstractAllele(call_code);
    char last_allele = abs_allele[abs_allele.size()-1];
    if (m_alpha2num.count(last_allele) == 1) {
      string search_string(1, last_allele);
      search_string.append(string(1, m_allele_delimiter));
//       cout << search_string << ':' << abs_allele << '\n';
      if (abs_allele.find_first_not_of(search_string, 0) != string::npos) {
	return true;
      }
    }
  }
  return false;
}
