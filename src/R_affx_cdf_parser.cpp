/*
 *  R_affx_cdf_parser.cpp : A utility for parsing Affymetrix CEL files in R.
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


#include "FusionCDFData.h"
#include <iostream>
#include "R_affx_constants.h"

using namespace std;
using namespace affymetrix_fusion_io;

extern "C" {
  #include <R.h>
  #include <Rdefines.h>  
  
  #include <wchar.h>
  #include <wctype.h>


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


  SEXP R_affx_get_cdf_file_qc(SEXP fname, SEXP verbose) 
  {
    
  }

  SEXP R_affx_get_cdf_file(SEXP fname, SEXP verbose) 
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

} /** end extern C **/
