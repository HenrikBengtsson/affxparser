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

    if (i_verboseFlag >= R_AFFX_VERBOSE)
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

      if (i_verboseFlag >= R_AFFX_VERBOSE)
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

    SEXP names = R_NilValue, 
      probe_sets = R_NilValue,
      cell_list = R_NilValue,
      cell_list_names = R_NilValue,
      xvals = R_NilValue,
      yvals = R_NilValue,
      pbase = R_NilValue,
      tbase = R_NilValue,
      expos = R_NilValue;
    

    int nsets = 0; 
    char* cdfFileName = CHAR(STRING_ELT(fname, 0));
    int i_verboseFlag = INTEGER(verbose)[0];

    cdf.SetFileName(cdfFileName);

    if (i_verboseFlag >= R_AFFX_VERBOSE)
      Rprintf("Attempting to read CDF File: %s\n", cdf.GetFileName().c_str());

    if (cdf.Read() == false) {
      Rprintf("Failed to read the CDF file.");
      return R_NilValue;
    }

    header = cdf.GetHeader();
    nsets = header.GetNumProbeSets();

    PROTECT(names = NEW_CHARACTER(nsets));
    PROTECT(probe_sets = NEW_LIST(nsets)); 

    for (int iset = 0; iset < nsets; iset++) {
      const char* name = cdf.GetProbeSetName(iset).c_str();

      if (i_verboseFlag >= R_AFFX_VERBOSE)
	Rprintf("Processing probeset: %s\n", name);
      
      /** the probe set names. **/
      SET_STRING_ELT(names, iset, mkChar(name));

      FusionCDFProbeSetInformation set;
      cdf.GetProbeSetInformation(iset, set);
	
      int ngroups = set.GetNumGroups();
      for (int igroup = 0; igroup < ngroups; igroup++) {
	FusionCDFProbeGroupInformation group;
	set.GetGroupInformation(igroup, group);
	
	int ncells = group.GetNumCells();
	int unp = 0, n_list_elts = 5; 

	/** XXX: need to understand what this list contains. **/
	PROTECT(cell_list = NEW_LIST(n_list_elts));
	PROTECT(cell_list_names = NEW_STRING(n_list_elts));

	PROTECT(xvals = NEW_INTEGER(ncells));
	PROTECT(yvals = NEW_INTEGER(ncells));
	PROTECT(pbase = NEW_STRING(ncells));
	PROTECT(tbase = NEW_STRING(ncells));
	PROTECT(expos = NEW_INTEGER(ncells));

	for (int icell = 0; icell < ncells; icell++) {
	  FusionCDFProbeInformation probe;
	  group.GetCell(icell, probe);

	  if (i_verboseFlag >= R_AFFX_REALLY_VERBOSE)
	    Rprintf("icell: %d x: %d, y: %d, pbase: %c, tbase: %c, expos: %d\n", 
		    icell, probe.GetX(), probe.GetY(), probe.GetPBase(), probe.GetTBase(),
		    probe.GetExpos());
	  
	  INTEGER(xvals)[icell] = probe.GetX();
	  INTEGER(yvals)[icell] = probe.GetY();

	  SET_STRING_ELT(pbase, icell, mkChar(CHARACTER_POINTER(probe.GetPBase())));
	  SET_STRING_ELT(tbase, icell, mkChar(CHARACTER_POINTER(probe.GetTBase())));
	  
	  /*	  CHAR(pbase)[icell] = probe.GetPBase();
		  CHAR(tbase)[icell] = probe.GetTBase(); */
	  INTEGER(expos)[icell] = probe.GetExpos(); 
	}

	if (i_verboseFlag >= R_AFFX_VERBOSE)
	  Rprintf("finished reading probeset information for: %s\n", name);

	/** do I have to make the attribute vector everytime? **/
	SET_VECTOR_ELT(cell_list, unp, xvals);
	SET_STRING_ELT(cell_list_names, unp++, mkChar("x"));
 
	SET_VECTOR_ELT(cell_list, unp, yvals);
	SET_STRING_ELT(cell_list_names, unp++, mkChar("y"));
	
	SET_VECTOR_ELT(cell_list, unp, pbase);
	SET_STRING_ELT(cell_list_names, unp++, mkChar("pbase"));
 
	SET_VECTOR_ELT(cell_list, unp, tbase);
	SET_STRING_ELT(cell_list_names, unp++, mkChar("tbase"));

	SET_VECTOR_ELT(cell_list, unp, expos);
	SET_STRING_ELT(cell_list_names, unp++, mkChar("expos"));
	

	/** set the names of the new list, dont really know if I need to do
	    this each and every time. **/
	setAttrib(cell_list, R_NamesSymbol, cell_list_names);
	  
	/** unprotect the vectors stored in our list. **/
	UNPROTECT(unp + 2);
      }

      /** now set the cell_list for each probe set. **/
      SET_VECTOR_ELT(probe_sets, iset, cell_list);
    }
    
    /** set the names down here at the end. **/
    setAttrib(probe_sets, R_NamesSymbol, names);

    /** unprotect the names and the main probe set list.**/
    UNPROTECT(2);
    
    return probe_sets;
  }

} /** end extern C **/
