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

  /* uncomment for relatively profuse debugging. */
  /* #define R_AFFX_DEBUG */

  #include <R.h>
  #include <Rdefines.h>  
 
  
  SEXP R_affx_get_cel_file(SEXP fname, SEXP readHeader, SEXP readOutlierAndMaskedData) 
  {

    char* celFileName = CHAR(STRING_ELT(fname,0));
    int readOutlierAndMaskedFlag = INTEGER(readOutlierAndMaskedData)[0];
    int readHeaderFlag = INTEGER(readHeader)[0];
   
    /** here we will store intensity matrix, header, outliers, and masked information. **/
    SEXP result_list = NEW_LIST(4);
    
#ifdef R_AFFX_DEBUG
    Rprintf("in R_affx_get_cel_file.\n");
#endif

    FusionCELData cel;
    cel.SetFileName(celFileName);

#ifdef R_AFFX_DEBUG
    Rprintf("attempting to read: %s\n", celFileName);
#endif
    
    /**
       XXX: there are three read methods - I do not know which
       one is the most appropriate here, but this default method
       seems to read everything. 
    **/
    if (cel.Read() == false)
    {
      Rprintf("Unable to read file: %s\n", celFileName);
      return R_NilValue;
    }

#ifdef R_AFFX_DEBUG
    Rprintf("sucessfully read: %s\n", celFileName);
#endif

    int numCells = cel.GetNumCells();

#ifdef R_AFFX_DEBUG
    Rprintf("read %d cells.\n", numCells);
#endif

    SEXP matrix = R_NilValue,dim = R_NilValue,outliers = R_NilValue,
      masked = R_NilValue, header = R_NilValue, names = R_NilValue;

    int noutlier = 0, nmasked = 0;
    PROTECT(matrix = NEW_NUMERIC(numCells * 5));
    
    if (readOutlierAndMaskedFlag != 0) {
#ifdef R_AFFX_DEBUG
      Rprintf("Reading outlier ( %d ) and masked ( %d ) cells.\n", 
	      cel.GetNumOutliers(), cel.GetNumMasked());
#endif

      PROTECT(outliers = NEW_INTEGER(cel.GetNumOutliers()));
      PROTECT(masked   = NEW_INTEGER(cel.GetNumMasked()));
    }

    if (readHeaderFlag != 0) {
      const wchar_t* header_wc = cel.GetHeader().c_str();
      // int sizeof_wc      = strlen(header_wc);

      (cel.GetHeader()).length();

      // header = mkString(cel.GetHeader().c_str());
    }

    FusionCELFileEntryType entry;

    for (int icel = 0; icel < numCells; icel++) {
      /**
       * this segfaults 
       * cel.GetEntry(icel, entry);
       * Rprintf("%f, %f, %d\n", entry.Intensity, entry.Stdv, entry.Pixels);
       **/ 

#ifdef R_AFFX_DEBUG            
      Rprintf("%d -- x:%d, y:%d, intensity:%f, stdv:%f, pixels:%d\n", 
	      icel, cel.IndexToX(icel),cel.IndexToY(icel),cel.GetIntensity(icel),
	      cel.GetStdv(icel),cel.GetPixels(icel));
#endif
      
      /** this works like this because of the column/row flop. **/
      REAL(matrix)[icel]              = cel.IndexToX(icel);
      REAL(matrix)[icel + numCells]   = cel.IndexToY(icel);
      REAL(matrix)[icel + 2*numCells] = cel.GetIntensity(icel);

      /** XXX: problem parsing the calvin cel files. 
            REAL(matrix)[icel + 3*numCells] = cel.GetStdv(icel);
            REAL(matrix)[icel + 4*numCells] = cel.GetPixels(icel);
      
      
      if (readOutlierAndMaskedFlag != 0) {
	if (cel.IsOutlier(icel) == true) {
	  INTEGER(outliers)[noutlier++] = icel;
	}
	
	if (cel.IsMasked(icel) == true) {
	  INTEGER(masked)[nmasked++] = icel;
	}
      }
      **/

    }

#ifdef R_AFFX_DEBUG
    Rprintf("read cell file intensities.");
#endif
  
    /** Set up the dimensions here we are transposed. **/
    PROTECT(dim = NEW_INTEGER(2));
    INTEGER_POINTER(dim)[1] = 5;
    INTEGER_POINTER(dim)[0] = numCells;
    SET_DIM(matrix, dim);

    SET_VECTOR_ELT(result_list, 0, header);
    SET_VECTOR_ELT(result_list, 1, matrix);
    SET_VECTOR_ELT(result_list, 2, outliers);
    SET_VECTOR_ELT(result_list, 3, masked);

    /** set up the names of the result list. **/
    PROTECT(names = NEW_CHARACTER(4));
    SET_STRING_ELT(names, 0, mkChar("header"));
    SET_STRING_ELT(names, 1, mkChar("intensities"));
    SET_STRING_ELT(names, 2, mkChar("outliers"));
    SET_STRING_ELT(names, 3, mkChar("masked"));

    /** set the names of the list entries. **/
    setAttrib(result_list, R_NamesSymbol, names);

    if (readOutlierAndMaskedFlag != 0) {
      UNPROTECT(5);
    }
    else {
      UNPROTECT(3);
    }
    return result_list;
  }


  SEXP R_affx_get_pmmm_list(SEXP fname, SEXP complementary_logic) 
  {
    FusionCDFData cdf;
    FusionCDFFileHeader header;
    SEXP names, dim, pmmm, pairs;
    int nRows = 0, nCols = 0;
    char* cdfFileName = CHAR(STRING_ELT(fname, 0));

    cdf.SetFileName(cdfFileName);

#ifdef R_AFFX_DEBUG
    Rprintf("Attempting to read CDF File: %s\n", cdf.GetFileName().c_str());
#endif

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

#ifdef R_AFFX_DEBUG
    Rprintf("Processing probeset: %s\n", name);
#endif

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

	  /**
	   *return((x - xy.offset) + 1 + nr * (y - xy.offset))
	   *
	   *XXX: understand what the offsets do (or just parameterize with them).
	   *XXX: the groups need to be thought through better.
	   */
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
