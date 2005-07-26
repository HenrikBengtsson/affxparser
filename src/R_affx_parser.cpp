/*
 *  R_affx_parser.cpp : A utility for parsing Affymetrix files in R.
 *
 *  Copyright (C) 2004-2005 James Bullard
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2.1 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "CELFileData.h"
#include "CDFFileData.h"
#include "CHPFileData.h"
#include "BARFileData.h"
#include "BPMAPFileData.h"

using namespace affxcdf;
using namespace affxcel;
using namespace affxchp;
using namespace affxbar;
using namespace affxbpmap;

#include <iostream>
#include <stdio.h>

using namespace std;

extern "C" {
  
  /* uncomment for relatively profuse debugging. */
  /* #define R_AFFX_DEBUG */

#include <R.h>
#include <Rdefines.h>

  /**
   * filename: the absolutely specfied name of the file.
   * complemntary_logic: whether to use different logic for PM/MM matching. 
   */ 
  SEXP R_affx_get_pmmm_list(SEXP fname, SEXP complementary_logic) {
     SEXP names, dim, pmmm, pairs;
     int nRows = 0, nCols = 0;
     
     char* filename = CHAR(STRING_ELT(fname,0));

     CCDFProbeInformation cel;
     CCDFProbeSetInformation set;
     CCDFProbeGroupInformation group;
     CCDFFileHeader header;
     CCDFFileData cdf;

     cdf.SetFileName(filename);
     
#ifdef R_AFFX_DEBUG
     Rprintf("Attempting to read CDF File: %s\n", cdf.GetFileName().c_str());
#endif

     if (INTEGER(complementary_logic)[0] == 1) {
       cdf.SetComplementaryLogic(true);

#ifdef R_AFFX_DEBUG
       Rprintf("Using complementary logic.\n");
#endif
     
     }

     if (cdf.Exists()) {
       cdf.Read();
     }
     else {
       return R_NilValue;
     }
     
     header = cdf.GetHeader();

     PROTECT(names = NEW_CHARACTER(header.GetNumProbeSets()));
     PROTECT(pmmm  = NEW_LIST(header.GetNumProbeSets())); 

     nRows = header.GetRows();
     nCols = header.GetCols();
     
#ifdef R_AFFX_DEBUG
     Rprintf("Read CDF file. nRows = %d, nCols = %d\n", nRows, nCols);
#endif

     for (int ips = 0; ips < header.GetNumProbeSets(); ips++) {
       const char* name = cdf.GetProbeSetName(ips).c_str();
       SET_STRING_ELT(names, ips, mkChar(name));

       cdf.GetProbeSetInformation(ips, set);

       int n = set.GetNumGroups();
       
       for (int ig = 0; ig < n; ig++) {
	 set.GetGroupInformation(ig, group);
	
	 PROTECT(pairs = NEW_INTEGER(group.GetNumCells()));
 
	 for (int ic = 0; ic < group.GetNumCells(); ic++) {
	   group.GetCell(ic, cel);
	   int x = cel.GetX();
	   int y = cel.GetY();

	   /**
	    *return((x - xy.offset) + 1 + nr * (y - xy.offset))
	    *
	    *XXX: understand what the offsets do (or just parameterize with them).
	    *XXX: the groups need to be thought through better.
	    */
	   int index = (x + 1) + (nRows * y);
	   	   
	   INTEGER(pairs)[ic] = index; 
	 }

	 /** Set up the dimensions here we are transposed. **/
	 PROTECT(dim = NEW_INTEGER(2));
	 INTEGER_POINTER(dim)[1] = group.GetNumCells()/2;
	 INTEGER_POINTER(dim)[0] = 2;
	 SET_DIM(pairs, dim);

	 /** Set the pmmm matrix for this ProbeSet. **/
	 SET_VECTOR_ELT(pmmm, ips, pairs);

	 /** pop off dimension, and the pairs matrix. **/
	 UNPROTECT(2);
       }
     } 
     /** set the names of the list. **/
     setAttrib(pmmm, R_NamesSymbol, names);

     /** pop the names, and the vector. **/
     UNPROTECT(2);

     return pmmm;
  }

  /**
   * return a list of the meta data associated with this cell
   * file. This is the information stored in the header. 
   */
  SEXP extract_cel_file_meta(CCELFileHeaderData header) {
    SEXP names, vals;
    PROTECT(names = NEW_CHARACTER(11));
    PROTECT(vals  = NEW_LIST(11));
    
    int i = 0; 
    SEXP tmp;

    SET_STRING_ELT(names, i, mkChar("version"));
    tmp = allocVector(INTSXP, 1);
    INTEGER(tmp)[0] = header.GetVersion();
    SET_VECTOR_ELT(vals, i++, tmp); 

    SET_STRING_ELT(names, i, mkChar("cols"));
    tmp = allocVector(INTSXP, 1);
    INTEGER(tmp)[0] = header.GetCols();
    SET_VECTOR_ELT(vals, i++, tmp);

    SET_STRING_ELT(names, i, mkChar("rows"));
    tmp = allocVector(INTSXP, 1);
    INTEGER(tmp)[0] = header.GetRows();
    SET_VECTOR_ELT(vals, i++, tmp);
    
    SET_STRING_ELT(names, i, mkChar("total"));
    tmp = allocVector(INTSXP, 1);
    INTEGER(tmp)[0] = header.GetNumCells();
    SET_VECTOR_ELT(vals, i++, tmp);
   
    SET_STRING_ELT(names, i, mkChar("alg"));
    SET_VECTOR_ELT(vals, i++, mkString(header.GetAlg().c_str())); 
       
    SET_STRING_ELT(names, i, mkChar("Params"));
    SET_VECTOR_ELT(vals, i++, mkString(header.GetParams().c_str()));
       
    SET_STRING_ELT(names, i, mkChar("ChipType"));
    SET_VECTOR_ELT(vals, i++, mkString(header.GetChipType().c_str()));

    SET_STRING_ELT(names, i, mkChar("header"));
    SET_VECTOR_ELT(vals, i++, mkString(header.GetHeader().c_str()));

    SET_STRING_ELT(names, i, mkChar("CellMargin"));
    tmp = allocVector(INTSXP, 1);
    INTEGER(tmp)[0] = header.GetCellMargin();
    SET_VECTOR_ELT(vals, i++, tmp);

    SET_STRING_ELT(names, i, mkChar("noutliers"));
    tmp = allocVector(INTSXP, 1);
    INTEGER(tmp)[0] = header.GetNumOutliers();
    SET_VECTOR_ELT(vals, i++, tmp);

    SET_STRING_ELT(names, i, mkChar("nmasked"));
    tmp = allocVector(INTSXP, 1);
    INTEGER(tmp)[0] = header.GetNumMasked();
    SET_VECTOR_ELT(vals, i++, tmp);

    setAttrib(vals, R_NamesSymbol, names);
    UNPROTECT(2);
    
    return vals;
  }
  
  /**
   * This function returns a list of all of the pertinent information
   * from a cel file into a list.
   */
  SEXP R_affx_get_cel_file(SEXP fname) {
    char* filename = CHAR(STRING_ELT(fname,0));
    CCELFileData cel;
    CELFileEntryType entry;
    cel.SetFileName(filename); 
    
    SEXP result_list = R_NilValue;
    SEXP header_list = R_NilValue;
    SEXP dim; 
    SEXP matrix = R_NilValue;
    SEXP corner_matrix = R_NilValue;
    SEXP outliers = R_NilValue;
    SEXP masked = R_NilValue;
    SEXP names = R_NilValue;

    if (cel.Exists()) {
      cel.Read();
      CCELFileHeaderData &header = cel.GetHeader();

      /** set up the names of the result list. **/
      PROTECT(names = NEW_CHARACTER(5));
      SET_STRING_ELT(names, 0, mkChar("header"));
      SET_STRING_ELT(names, 1, mkChar("corners"));
      SET_STRING_ELT(names, 2, mkChar("intensities"));
      SET_STRING_ELT(names, 3, mkChar("outliers"));
      SET_STRING_ELT(names, 4, mkChar("masked"));

      PROTECT(result_list = NEW_LIST(5));

      /** read the header. **/
      PROTECT(header_list = extract_cel_file_meta(header));
      SET_VECTOR_ELT(result_list, 0, header_list);
      
      /** now we want to read the the corner information. **/
      GridCoordinatesType &corners = header.GridCorners();
      
      PROTECT(corner_matrix = NEW_INTEGER(8));
      INTEGER(corner_matrix)[0] = corners.upperleft.x;
      INTEGER(corner_matrix)[1] = corners.upperright.x;
      INTEGER(corner_matrix)[2] = corners.lowerleft.x;
      INTEGER(corner_matrix)[3] = corners.lowerright.x;
      INTEGER(corner_matrix)[4] = corners.upperleft.y;
      INTEGER(corner_matrix)[5] = corners.upperright.y;
      INTEGER(corner_matrix)[6] = corners.lowerleft.y;
      INTEGER(corner_matrix)[7] = corners.lowerright.y;

      PROTECT(dim = NEW_INTEGER(2));
      INTEGER_POINTER(dim)[1] = 2;
      INTEGER_POINTER(dim)[0] = 4;
      SET_DIM(corner_matrix, dim);

      SET_VECTOR_ELT(result_list, 1, corner_matrix);

      /** set up matrix for the intensity values. **/
      int numCells = header.GetNumCells();
      PROTECT(matrix = NEW_NUMERIC(numCells * 5));
      
      for (int icel = 0; icel < numCells; icel++) {
	cel.GetEntry(icel, entry);
	
	/** this works like this because of the column/row flop. **/
	REAL(matrix)[icel] = cel.IndexToX(icel);
	REAL(matrix)[icel + numCells] = cel.IndexToY(icel);
	REAL(matrix)[icel + 2*numCells] = entry.Intensity;
	REAL(matrix)[icel + 3*numCells] = entry.Stdv;
	REAL(matrix)[icel + 4*numCells] = entry.Pixels;
	
#ifdef R_AFFX_DEBUG
	Rprintf("X: %d, Y: %d, Intensity: %f, Stdv: %f, Pixels: %d\n", cel.IndexToX(icel),
		cel.IndexToY(icel), entry.Intensity, entry.Stdv, entry.Pixels);
#endif
      }

      /** Set up the dimensions here we are transposed. **/
      PROTECT(dim = NEW_INTEGER(2));
      INTEGER_POINTER(dim)[1] = 5;
      INTEGER_POINTER(dim)[0] = numCells;
      SET_DIM(matrix, dim);

      SET_VECTOR_ELT(result_list, 2, matrix);
     
      /** now we construct two vectors for the outliers and the masked cels. **/
      PROTECT(outliers = NEW_INTEGER(header.GetNumOutliers()));
      PROTECT(masked = NEW_INTEGER(header.GetNumMasked()));

      int noutlier = 0;
      int nmasked  = 0;
      
      for (int icel = 0; icel < numCells; icel++) {

	if (cel.IsOutlier(icel) == true) {
	  INTEGER(outliers)[noutlier++] = icel;
	}

	if (cel.IsMasked(icel) == true) {
	  INTEGER(masked)[nmasked++] = icel;
	}
      }
      SET_VECTOR_ELT(result_list, 3, outliers);
      SET_VECTOR_ELT(result_list, 4, masked);

      /** set up the names, better do it here for consistency. **/
      setAttrib(result_list, R_NamesSymbol, names);
      
      /** this is pretty feo. **/
      UNPROTECT(9);
    
      /** close the file handle? **/
      cel.Close();
    }
    return result_list;
  }

  
  SEXP R_affx_get_bpmap_file(SEXP fname) {
    char* filename = CHAR(STRING_ELT(fname,0));
    
    GDACSequenceHitItemType hit;
    affxbpmap::TagValuePairType param;
    CGDACSequenceItem seq;
    CBPMAPFileData bpmap;
    bpmap.SetFileName(filename);

    /** a list index. **/
    int i = 0, header_total = 2, iseq;

    /** a list of the meta-data. */
    SEXP names, vals, tmp, sequences;
    SEXP seq_header_names, seq_header_vals;
    SEXP sequence_names, sequence_vals;

    PROTECT(names = NEW_CHARACTER(header_total));
    PROTECT(vals  = NEW_LIST(header_total));

    if (bpmap.Exists()) {
      bpmap.Read();

      SET_STRING_ELT(names, i, mkChar("nseqs"));
      tmp = allocVector(INTSXP, 1);
      INTEGER(tmp)[0] = bpmap.GetNumberSequences();
      SET_VECTOR_ELT(vals, i++, tmp); 

      SET_STRING_ELT(names, i, mkChar("version"));
      tmp = allocVector(REALSXP, 1);
      REAL(tmp)[0] = bpmap.GetVersion();
      SET_VECTOR_ELT(vals, i++, tmp); 
      
      setAttrib(vals, R_NamesSymbol, names);

      /** this list will store all of our sequences along with their headers. **/
      PROTECT(sequences = NEW_LIST(bpmap.GetNumberSequences()));

      Rprintf("entering sequence loop. \n");

      for (iseq = 0; iseq < bpmap.GetNumberSequences(); iseq++) {
	bpmap.GetSequenceItem(iseq, seq);

	PROTECT(sequence_vals  = NEW_LIST(4));
	PROTECT(sequence_names = NEW_CHARACTER(4));
 
	PROTECT(seq_header_names = NEW_CHARACTER(6));
	PROTECT(seq_header_vals  = NEW_LIST(6));

	i = 0; /** this will iterate over the sequence header entries. **/

	SET_STRING_ELT(seq_header_names, i, mkChar("name"));
	SET_VECTOR_ELT(seq_header_vals, i++, mkString(seq.GetName().c_str()));

	SET_STRING_ELT(seq_header_names, i, mkChar("groupName"));
	SET_VECTOR_ELT(seq_header_vals, i++, mkString(seq.GroupName().c_str()));

	SET_STRING_ELT(seq_header_names, i, mkChar("seqVersion"));
	SET_VECTOR_ELT(seq_header_vals, i++, mkString(seq.GroupName().c_str()));
	
	SET_STRING_ELT(seq_header_names, i, mkChar("number"));
	tmp = allocVector(INTSXP, 1);
	INTEGER(tmp)[0] = seq.GetNumber();
	SET_VECTOR_ELT(vals, i++, tmp);

	SET_STRING_ELT(seq_header_names, i, mkChar("nhits"));
	tmp = allocVector(INTSXP, 1);
	INTEGER(tmp)[0] = seq.GetNumberHits();
	SET_VECTOR_ELT(vals, i++, tmp);

	SET_STRING_ELT(seq_header_names, i, mkChar("nparams"));
	tmp = allocVector(INTSXP, 1);
	INTEGER(tmp)[0] = seq.GetNumberParameters();
	SET_VECTOR_ELT(vals, i++, tmp);

	/** set up the names, better do it here for consistency. **/
	setAttrib(seq_header_vals, R_NamesSymbol, seq_header_names);

	/** lets put the header into our sequence list. **/
	SET_STRING_ELT(sequence_names, 0, mkChar("header"));
	SET_VECTOR_ELT(sequence_vals, 0, seq_header_vals);

	/** unprotect the header information. **/
	UNPROTECT(2);

	/** now we construct the parameter list. **/
	SEXP param_names, param_vals;
	PROTECT(param_names = NEW_CHARACTER(seq.GetNumberParameters()));
	PROTECT(param_vals  = NEW_LIST(seq.GetNumberParameters()));

	for (int iparam = 0; iparam < seq.GetNumberParameters(); iparam++) {
	  param = seq.GetParameter(iparam);

	  SET_STRING_ELT(param_names, iparam, mkChar(param.Tag.c_str()));
	  SET_VECTOR_ELT(param_vals, iparam, mkString(param.Value.c_str()));
	}
	
	setAttrib(param_vals, R_NamesSymbol, param_names);
	
	/** now lets put our parameters in the sequence list. **/
	SET_STRING_ELT(sequence_names, 1, mkChar("params"));
	SET_VECTOR_ELT(sequence_vals, 1, param_vals);
	
	/** unprotect the parameter information. **/
	UNPROTECT(2);

	int numHits = seq.GetNumberHits();

	Rprintf("numHits:%d\n", numHits);
	Rprintf("before allocation...\n");

	SEXP oligos, matrix;
	PROTECT(oligos = NEW_LIST(numHits));
	
	Rprintf("allocated oligo list...\n");

	PROTECT(matrix = NEW_INTEGER(numHits*8));

	Rprintf("entering looop...\n");

	for (int ihit = 0; ihit < seq.GetNumberHits(); ihit++) {
	  seq.GetHitItem(ihit, hit, true);
	  
	  INTEGER(matrix)[ihit] = hit.PMX;
	  
	  INTEGER(matrix)[ihit + 1*numHits] = hit.PMY;
	  /*
	  INTEGER(matrix)[ihit + 2*numHits] = hit.MMX;
	  INTEGER(matrix)[ihit + 3*numHits] = hit.MMY;
	  INTEGER(matrix)[ihit + 4*numHits] = (int) hit.MatchScore;
	  INTEGER(matrix)[ihit + 5*numHits] = (int) hit.Position;
	  INTEGER(matrix)[ihit + 6*numHits] = hit.ProbeLength;
	  INTEGER(matrix)[ihit + 7*numHits] = hit.TopStrand;
	  */

	  /** SET_STRING_ELT(oligos, ihit, mkString(hit.PMProbe.c_str())); */
	}
	Rprintf("finished with loop...\n");

	/** put the sequence information in a list. **/
	SET_STRING_ELT(sequence_names, 2, mkChar("hits"));
	SET_VECTOR_ELT(sequence_vals, 2, matrix);

	SET_STRING_ELT(sequence_names, 3, mkChar("oligos"));
	SET_VECTOR_ELT(sequence_vals, 3, oligos);

	setAttrib(sequence_vals, R_NamesSymbol, sequence_names);

	/** unprotect the sequence information. **/
	UNPROTECT(2);
	
	/** now we need to add the entire sequence to a list. **/
	SET_VECTOR_ELT(sequences, iseq, sequence_vals);

	/** now we unprotect the entire sequence list (not the list of sequences, but the 
	    (header, params, hits, oligos). **/
	UNPROTECT(2);
      }
    }

    /** the overall bpmap header and the sequences. **/
    UNPROTECT(3);
    
    /** return the sequences along with the file headers. **/
    return sequences;
  }

  
  SEXP R_affx_get_bar_file(SEXP fname) {
    char* filename = CHAR(STRING_ELT(fname,0));

    affxbar::TagValuePairType param;
    CBARFileData bar;
    bar.SetFileName(filename);
	
    if (bar.Exists()) {
      bar.Read();
      
    }
  }

  /** 
      This method is a debugging method for 
      allocation. 
  **/
  SEXP R_affx_allocator(SEXP size) {
    int nsize = INTEGER(size)[0];
    
    Rprintf("size:%d\n", nsize);
    
    Rprintf("before allocation...\n");
    SEXP oligos, matrix;
    PROTECT(oligos = NEW_LIST(nsize));
    Rprintf("allocated oligo list...\n");

    PROTECT(matrix = NEW_INTEGER((nsize)*8));
    
    Rprintf("entering looop...\n");
    
    UNPROTECT(2);
    
    return matrix;
  }
  
} /** end extern "C" **/

/**
void test_bar(string file)
{
	affxbar::TagValuePairType param;
	CBARFileData bar;
	bar.SetFileName(file.c_str());
	if (bar.Exists())
	{
		bar.Read();

		cout << "Version = " << bar.GetVersion() << endl;
		cout << "#Seqs = " << bar.GetNumberSequences() << endl;
		cout << "#Cols = " << bar.GetNumberColumns() << endl;
		cout << "#Params = " << bar.GetNumberParameters() << endl;

		for (int iparam=0; iparam<bar.GetNumberParameters(); iparam++)
		{
			param = bar.GetParameter(iparam);
			cout << param.Tag << " = " << param.Value << endl;
		}

		for (int iseq=0; iseq<bar.GetNumberSequences(); iseq++)
		{
			CGDACSequenceResultItem res;
			bar.GetResults(iseq, res);

			cout << "Name = " << res.GetName() << endl;
			cout << "Group = " << res.GetGroupName() << endl;
			cout << "Version = " << res.GetVersion() << endl;
			cout << "#Pts = " << res.GetNumberDataPoints() << endl;
			cout << "#Cols = " << res.GetNumberColumns() << endl;

			for (int iseqparam=0; iseqparam<res.GetNumberParameters(); iseqparam++)
			{
				affxbar::TagValuePairType &param = res.GetParameter(iseqparam);
				cout << param.Tag << "=" << param.Value << endl;
			}

			BarSequenceResultData data;
			for (int ipt=0; ipt<res.GetNumberDataPoints(); ipt++)
			{
				for (int icol=0; icol<res.GetNumberColumns(); icol++)
				{
					res.GetData(ipt, icol, data);

					switch (res.GetColumnType(icol))
					{
					case BAR_DATA_INTEGER:
						cout << data.iValue << endl;
						break;

					case BAR_DATA_FLOAT:
						cout << data.fValue << endl;
						break;
					}
				}
			}
		}
	}
}
**/
 
