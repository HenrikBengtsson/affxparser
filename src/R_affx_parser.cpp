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


#include "FusionCELData.h"
#include "FusionCDFData.h"
#include <iostream>

using namespace std;
using namespace affymetrix_fusion_io;

extern "C" {


  #include <R.h>
  #include <Rdefines.h>  

  #include <wchar.h>
  #include <wctype.h>


  /* uncomment for relatively profuse debugging. */
  int R_AFFX_VERBOSE = 1;
  int R_AFFX_REALLY_VERBOSE = 2;

 
  /**
   * return a list of the meta data associated with this cell
   * file. This is the information stored in the header. 
   */
  SEXP R_affx_extract_cel_file_meta(FusionCELData cel) {
    SEXP names, vals;

    PROTECT(names = NEW_CHARACTER(11));
    PROTECT(vals  = NEW_LIST(11));
    
    int i = 0; 
    SEXP tmp;

    SET_STRING_ELT(names, i, mkChar("version"));
    tmp = allocVector(INTSXP, 1);
    INTEGER(tmp)[0] = cel.GetVersion();
    SET_VECTOR_ELT(vals, i++, tmp); 

    SET_STRING_ELT(names, i, mkChar("cols"));
    tmp = allocVector(INTSXP, 1);
    INTEGER(tmp)[0] = cel.GetCols();
    SET_VECTOR_ELT(vals, i++, tmp);

    SET_STRING_ELT(names, i, mkChar("rows"));
    tmp = allocVector(INTSXP, 1);
    INTEGER(tmp)[0] = cel.GetRows();
    SET_VECTOR_ELT(vals, i++, tmp);
    
    SET_STRING_ELT(names, i, mkChar("total"));
    tmp = allocVector(INTSXP, 1);
    INTEGER(tmp)[0] = cel.GetNumCells();
    SET_VECTOR_ELT(vals, i++, tmp);
   

#ifdef SUPPORT_MBCS
    int str_length; 
    char* cstr; 
    
    str_length = cel.GetAlg().size();
    cstr = Calloc(str_length, char);
    wcstombs(cstr, cel.GetAlg().c_str(), str_length);
    SET_STRING_ELT(names, i, mkChar("Algorithm"));
    SET_VECTOR_ELT(vals, i++, mkString(cstr)); 
    Free(cstr);

    str_length = cel.GetParams().size();
    cstr = Calloc(str_length, char);
    wcstombs(cstr, cel.GetParams().c_str(), str_length);
    SET_STRING_ELT(names, i, mkChar("Parameters"));
    SET_VECTOR_ELT(vals, i++, mkString(cstr));
    Free(cstr);
   
    str_length = cel.GetChipType().size();
    cstr = Calloc(str_length, char);
    wcstombs(cstr, cel.GetChipType().c_str(), str_length);
    SET_STRING_ELT(names, i, mkChar("ChipType"));
    SET_VECTOR_ELT(vals, i++, mkString(cstr));
    Free(cstr);
	
    str_length = cel.GetHeader().size();
    cstr = Calloc(str_length, char);
    wcstombs(cstr, cel.GetHeader().c_str(), str_length);
    SET_STRING_ELT(names, i, mkChar("header"));
    SET_VECTOR_ELT(vals, i++, mkString(cstr));
    Free(cstr);
#endif    

    SET_STRING_ELT(names, i, mkChar("CellMargin"));
    tmp = allocVector(INTSXP, 1);
    INTEGER(tmp)[0] = cel.GetCellMargin();
    SET_VECTOR_ELT(vals, i++, tmp);
    
    SET_STRING_ELT(names, i, mkChar("noutliers"));
    tmp = allocVector(INTSXP, 1);
    INTEGER(tmp)[0] = cel.GetNumOutliers();
    SET_VECTOR_ELT(vals, i++, tmp);

    SET_STRING_ELT(names, i, mkChar("nmasked"));
    tmp = allocVector(INTSXP, 1);
    INTEGER(tmp)[0] = cel.GetNumMasked();
    SET_VECTOR_ELT(vals, i++, tmp);

    setAttrib(vals, R_NamesSymbol, names);
    UNPROTECT(2);
    
    return vals;
  }

  /** This quickly reads only the cel file header. **/
  SEXP R_affx_get_cel_file_header(SEXP fname) 
  {
    FusionCELData cel;
    char* celFileName = CHAR(STRING_ELT(fname,0));

    cel.SetFileName(celFileName);
    if (cel.ReadHeader() == false) {
      Rprintf("Unable to read file: %s\n", celFileName);
      return R_NilValue;
    }
    else {
      return  R_affx_extract_cel_file_meta(cel);
    }
  }

  /** read cel file either partially or completely. **/
  SEXP R_affx_get_cel_file(SEXP fname, SEXP readHeader, SEXP readIntensities, SEXP readX,
			   SEXP readY, SEXP readPixels, SEXP readStdvs, SEXP readOutliers,
			   SEXP readMasked, SEXP indicesToRead, SEXP verbose) 
  {
    FusionCELData cel;
    int noutlier = 0, nmasked = 0, unp = 0, j = 0;

    SEXP  
      names = R_NilValue,
      header = R_NilValue,
      intensities = R_NilValue,
      xvals = R_NilValue,
      yvals = R_NilValue,
      pixels = R_NilValue,
      stdvs = R_NilValue,
      outliers = R_NilValue,
      masked = R_NilValue;


    /** extract our arguments.**/
    char* celFileName         = CHAR(STRING_ELT(fname,0));
    int i_readHeader          = INTEGER(readHeader)[0];
    int i_readIntensities     = INTEGER(readIntensities)[0];
    int i_readX               = INTEGER(readX)[0];
    int i_readY               = INTEGER(readY)[0];
    int i_readPixels          = INTEGER(readPixels)[0];
    int i_readStdvs           = INTEGER(readStdvs)[0];
    int i_readOutliers        = INTEGER(readOutliers)[0];
    int i_readMasked          = INTEGER(readMasked)[0];
    int i_verboseFlag         = INTEGER(verbose)[0];
    
    /** here we will store the above entries in that order. **/
    SEXP result_list = NEW_LIST(8);

    if (i_verboseFlag == R_AFFX_VERBOSE)
      Rprintf("attempting to read: %s\n", celFileName);
    
    /**
       XXX: there are three Read methods - I do not know which
       one is the most appropriate here, but this default method
       seems to read everything. Ex(celFileName, FusionCELData::CEL_ALL)
    **/
    cel.SetFileName(celFileName);
    if (cel.Read(true) == false) {
      Rprintf("Unable to read file: %s\n", celFileName);
      return R_NilValue;
    }

    if (i_verboseFlag == R_AFFX_VERBOSE)
      Rprintf("sucessfully read: %s\n", celFileName);


    int numCells; 
    bool readAll = true;

    numCells = cel.GetNumCells();

    if (length(indicesToRead) == 0) {
      /** get them all, the typical case. **/
      readAll = true;
    }
    else {
      readAll = false;

      int numIndices = length(indicesToRead);
      if (numIndices < numCells)
	numCells = numIndices;
    }

    if (i_verboseFlag == R_AFFX_VERBOSE)
      Rprintf("read %d cells.\n", numCells);

   
    /** conditionally allocate space for each vector we want to return. **/
    if (i_readHeader != 0) {
      header = R_affx_extract_cel_file_meta(cel);
    }
    if (i_readIntensities != 0) {
      PROTECT(intensities = NEW_NUMERIC(numCells));
      unp++;
    }
    if (i_readX != 0) {
      PROTECT(xvals = NEW_INTEGER(numCells));
      unp++;
    }
    if (i_readY != 0) {
      PROTECT(yvals = NEW_INTEGER(numCells));
      unp++;
    }
    if (i_readPixels != 0) {
      PROTECT(pixels = NEW_INTEGER(numCells));
      unp++;
    }
    if (i_readStdvs != 0) {
      PROTECT(stdvs = NEW_NUMERIC(numCells));
      unp++;
    }
    if (i_readOutliers != 0) {
      if (i_verboseFlag == R_AFFX_VERBOSE)
	Rprintf("Number of outliers %d\n", cel.GetNumOutliers());

      PROTECT(outliers = NEW_INTEGER(cel.GetNumOutliers()));
      unp++;
    }
    if (i_readMasked != 0) {
      if (i_verboseFlag == R_AFFX_VERBOSE)
	Rprintf("Number of masked %d\n", cel.GetNumMasked());

      PROTECT(masked = NEW_INTEGER(cel.GetNumMasked()));
      unp++;
    }
        
    for (int icel = 0, index = 0; icel < numCells; icel++) {
      index = icel;

      if (readAll == false) {
	index = INTEGER(indicesToRead)[icel];
      }

      if (i_verboseFlag == R_AFFX_REALLY_VERBOSE)            
	Rprintf("index: %d, x: %d, y: %d, intensity: %f, stdv: %f, pixels: %d\n", 
		index, cel.IndexToX(index), cel.IndexToY(index),
		cel.GetIntensity(index), cel.GetStdv(index), cel.GetPixels(index));

      if (i_readIntensities != 0) {
	REAL(intensities)[icel] = cel.GetIntensity(index);
      }
      if (i_readX != 0) {
	INTEGER(xvals)[icel] = cel.IndexToX(index);
      }
      if (i_readY != 0) {
	INTEGER(yvals)[icel] = cel.IndexToY(index);
      }
      if (i_readPixels != 0) {
	INTEGER(pixels)[icel] = cel.GetPixels(index);
      }
      if (i_readStdvs != 0) {
	REAL(stdvs)[icel] = cel.GetStdv(index);
      }

      if (i_readOutliers != 0) {
	if (cel.IsOutlier(index)) {
	  INTEGER(outliers)[noutlier++] = index;
	}
      }
      if (i_readMasked != 0) {
	if (cel.IsMasked(index)) {
	  INTEGER(masked)[nmasked++] = index;
	}
      }
    }

    /** resize here if we only read part of the cel then we only want the outliers
	which correspond to that part. **/
    if (i_readOutliers != 0) {
      if (noutlier == 0) {
	outliers = R_NilValue;
      }
      else if (noutlier < cel.GetNumOutliers()) {
	PROTECT(outliers = lengthgets(outliers, noutlier));
	unp++;
      }
    }
    
    if (i_readMasked != 0) {
      if (nmasked == 0) {
	masked = R_NilValue;
      }
      else if (nmasked < cel.GetNumMasked()) {
	PROTECT(masked = lengthgets(masked, nmasked));
	unp++;
      }
    }

    /** set up the names of the result list. **/
    PROTECT(names = NEW_CHARACTER(8));

    SET_STRING_ELT(names, j, mkChar("header"));
    SET_VECTOR_ELT(result_list, j++, header);

    SET_STRING_ELT(names, j, mkChar("intensities"));
    SET_VECTOR_ELT(result_list, j++, intensities);

    SET_STRING_ELT(names, j, mkChar("x"));    
    SET_VECTOR_ELT(result_list, j++, xvals);

    SET_STRING_ELT(names, j, mkChar("y"));
    SET_VECTOR_ELT(result_list, j++, yvals);

    SET_STRING_ELT(names, j, mkChar("pixels"));        
    SET_VECTOR_ELT(result_list, j++, pixels);

    SET_STRING_ELT(names, j, mkChar("stdvs"));        
    SET_VECTOR_ELT(result_list, j++, stdvs);

    SET_STRING_ELT(names, j, mkChar("outliers"));
    SET_VECTOR_ELT(result_list, j++, outliers);

    SET_STRING_ELT(names, j, mkChar("masked"));
    SET_VECTOR_ELT(result_list, j++, masked);

    /** set the names of the list entries. **/
    setAttrib(result_list, R_NamesSymbol, names);

    if (i_verboseFlag == R_AFFX_VERBOSE)
      Rprintf("finished reading reading CEL file.\n");


    /** add one for the result list. **/
    UNPROTECT(++unp);
 
    return result_list;
  }


  SEXP R_affx_get_pmmm_list(SEXP fname, SEXP complementary_logic, SEXP verbose) 
  {
    FusionCDFData cdf;
    FusionCDFFileHeader header;
    SEXP names, dim, pmmm, pairs;
    int nRows = 0, nCols = 0;
    char* cdfFileName = CHAR(STRING_ELT(fname, 0));
    int i_verboseFlag = INTEGER(verbose)[0];

    cdf.SetFileName(cdfFileName);

    if (i_verboseFlag == R_AFFX_VERBOSE)
      Rprintf("Attempting to read CDF File: %s\n", cdf.GetFileName().c_str());


    if (cdf.Read() == false) {
      Rprintf("Failed to read the CDF file.");
      return R_NilValue;
    }

    header = cdf.GetHeader();
    int nsets = header.GetNumProbeSets();

    PROTECT(names = NEW_CHARACTER(nsets));
    PROTECT(pmmm  = NEW_LIST(nsets)); 

    nRows = header.GetRows();
    nCols = header.GetCols();

    for (int iset = 0; iset < nsets; iset++) {
      const char* name = cdf.GetProbeSetName(iset).c_str();

      if (i_verboseFlag == R_AFFX_VERBOSE)
	Rprintf("Processing probeset: %s\n", name);


      SET_STRING_ELT(names, iset, mkChar(name));

      FusionCDFProbeSetInformation set;
      cdf.GetProbeSetInformation(iset, set);
	
      int ngroups = set.GetNumGroups();
      for (int igroup = 0; igroup < ngroups; igroup++) {
	
	FusionCDFProbeGroupInformation group;
	set.GetGroupInformation(igroup, group);
	int ncells = group.GetNumCells();
	
	PROTECT(pairs = NEW_INTEGER(ncells));
	      
	for (int icell = 0; icell < ncells; icell++) {
	  FusionCDFProbeInformation probe;
	  group.GetCell(icell, probe);

	  int x = probe.GetX();
	  int y = probe.GetY();

	  int index = (x + 1) + (nRows * y);
	  INTEGER(pairs)[icell] = index;
	}

	/** Set up the dimensions here we are transposed. **/
	PROTECT(dim = NEW_INTEGER(2));
	INTEGER_POINTER(dim)[1] = ncells/2;
	INTEGER_POINTER(dim)[0] = 2;
	SET_DIM(pairs, dim);

	/** Set the pmmm matrix for this ProbeSet. **/
	SET_VECTOR_ELT(pmmm, iset, pairs);

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

} /** end extern "C" **/
