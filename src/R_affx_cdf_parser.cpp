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

  /** This function conforms to the specification for getting an environment
      for CEL files. **/
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
  /** 
   * XXX: This might look very similar to the code for reading of the normal
   * probe sets ; so i am going to wait until i am confident that i have that
   * implemented nicely.
   */
  }

  SEXP R_affx_extract_cdf_file_meta(FusionCDFFileHeader header) {

    SEXP names = R_NilValue, 
      vals = R_NilValue,
      tmp = R_NilValue;
    
    PROTECT(names = NEW_CHARACTER(5));
    PROTECT(vals  = NEW_LIST(5));
      
    int i = 0;

    /* 
     * Luis should add a version number
     */

    /*
     * Need to add "filename" element
     */
    
    SET_STRING_ELT(names, i, mkChar("cols"));
    tmp = allocVector(INTSXP, 1);
    INTEGER(tmp)[0] = header.GetCols();
    SET_VECTOR_ELT(vals, i++, tmp); 
    
    SET_STRING_ELT(names, i, mkChar("rows"));
    tmp = allocVector(INTSXP, 1);
    INTEGER(tmp)[0] = header.GetRows();
    SET_VECTOR_ELT(vals, i++, tmp); 
    
    SET_STRING_ELT(names, i, mkChar("probesets"));
    tmp = allocVector(INTSXP, 1);
    INTEGER(tmp)[0] = header.GetNumProbeSets();
    SET_VECTOR_ELT(vals, i++, tmp); 
    
    SET_STRING_ELT(names, i, mkChar("qcprobesets"));
    tmp = allocVector(INTSXP, 1);
    INTEGER(tmp)[0] = header.GetNumQCProbeSets();
    SET_VECTOR_ELT(vals, i++, tmp); 
    
    /*
     * Possible problem with "reference" and multibyte char set...
     */

    SET_STRING_ELT(names, i, mkChar("reference"));
    SET_VECTOR_ELT(vals, i++, mkString(header.GetReference().c_str()));

    /*
     * Need to add chiptype elemen with check for SUPPORT_MCBS
     */

    /** set the names down here at the end. **/
    setAttrib(vals, R_NamesSymbol, names);
    UNPROTECT(2);
    
    return vals; 
  }

  SEXP R_affx_get_cdf_file_header(SEXP fname)
  {
    FusionCDFData cdf;
    FusionCDFFileHeader header;

    char* cdfFileName = CHAR(STRING_ELT(fname, 0));
    
    cdf.SetFileName(cdfFileName);
    if (cdf.ReadHeader() == false) {
      Rprintf("Failed to read the CDF file header for: %s\n", cdfFileName);
      return R_NilValue;
    }
    else { 
	return R_affx_extract_cdf_file_meta(cdf.GetHeader());
    }
  }

  SEXP R_affx_get_cdf_file(SEXP fname, SEXP verbose) 
  {
    FusionCDFData cdf;
    FusionCDFFileHeader header;
    
    /*
     * What about returning the header as well?
     */

    SEXP names = R_NilValue, 
      probe_sets = R_NilValue,
      cell_list = R_NilValue,
      cell_list_names = R_NilValue,
      xvals = R_NilValue,
      yvals = R_NilValue,
      pbase = R_NilValue,
      tbase = R_NilValue,
      expos = R_NilValue,
      ps_group = R_NilValue,
      r_probe_set = R_NilValue, /** one might already want to standardize on this naming scheme... **/
      r_probe_set_names = R_NilValue,
      tmp = R_NilValue; 
    

    int nsets = 0; 
    char* cdfFileName = CHAR(STRING_ELT(fname, 0));
    int i_verboseFlag = INTEGER(verbose)[0];

    /** 
	XXX: I am not sure this is the most elegant way to handle these in R. 
	I initialize it hear for kicks.
    **/
    char p_base[2] = "X";
    char t_base[2] = "X";
    
    /** pointer to the name of the probeset. **/
    const char* name;
    int probeSetType, direction;
    
    FusionCDFProbeSetInformation probeset;

    cdf.SetFileName(cdfFileName);
    if (i_verboseFlag >= R_AFFX_VERBOSE)
      Rprintf("Attempting to read CDF File: %s\n", cdf.GetFileName().c_str());

    if (cdf.Read() == false) {
      Rprintf("Failed to read the CDF file.");
      return R_NilValue;
    }

    header = cdf.GetHeader();
    nsets  = header.GetNumProbeSets();

    PROTECT(names = NEW_CHARACTER(nsets));
    PROTECT(probe_sets = NEW_LIST(nsets)); 

    for (int iset = 0; iset < nsets; iset++) {
      cdf.GetProbeSetInformation(iset, probeset);
      
      /** i am not sure why there is not a method on ProbeSetInformation for the name. **/
      name = cdf.GetProbeSetName(iset).c_str();
      
      probeSetType = probeset.GetProbeSetType();
      direction = probeset.GetDirection();

      if (i_verboseFlag >= R_AFFX_VERBOSE)
	Rprintf("Processing probeset: %s with type: %d, direction: %d\n ", name, 
		probeSetType, direction);
      
      /** the probe set names. **/
      SET_STRING_ELT(names, iset, mkChar(name));
	
      int ngroups = probeset.GetNumGroups();
      for (int igroup = 0; igroup < ngroups; igroup++) {
	FusionCDFProbeGroupInformation group;
	probeset.GetGroupInformation(igroup, group);
	
	int ncells = group.GetNumCells();
	int 
	  unp = 0, 
	  n_list_elts = 6; 

	/** XXX: need to understand what this list contains. **/
	PROTECT(cell_list = NEW_LIST(n_list_elts));
	PROTECT(cell_list_names = NEW_STRING(n_list_elts));

	PROTECT(xvals = NEW_INTEGER(ncells));
	PROTECT(yvals = NEW_INTEGER(ncells));
	PROTECT(pbase = NEW_STRING(ncells));
	PROTECT(tbase = NEW_STRING(ncells));
	PROTECT(expos = NEW_INTEGER(ncells));
	PROTECT(ps_group = NEW_INTEGER(ncells));

	for (int icell = 0; icell < ncells; icell++) {
	  FusionCDFProbeInformation probe;
	  group.GetCell(icell, probe);

	  if (i_verboseFlag >= R_AFFX_REALLY_VERBOSE)
	    Rprintf("icell: %d x: %d, y: %d, pbase: %c, tbase: %c, expos: %d\n", 
		    icell, probe.GetX(), probe.GetY(), probe.GetPBase(), probe.GetTBase(),
		    probe.GetExpos());
	  
	  INTEGER(xvals)[icell] = probe.GetX();
	  INTEGER(yvals)[icell] = probe.GetY();
	  
	  p_base[0] = probe.GetPBase();
	  t_base[0] = probe.GetTBase();
	  
	  SET_STRING_ELT(pbase, icell, mkChar(p_base));
	  SET_STRING_ELT(tbase, icell, mkChar(t_base));
	  
	  INTEGER(expos)[icell] = probe.GetExpos(); 
	  INTEGER(ps_group)[icell] = igroup; 
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

	SET_VECTOR_ELT(cell_list, unp, ps_group);
	SET_STRING_ELT(cell_list_names, unp++, mkChar("group"));
	
	/** set the names of the new list, dont really know if I need to do
	    this each and every time. **/
	setAttrib(cell_list, R_NamesSymbol, cell_list_names);
	  
	/** unprotect the vectors stored in our list. **/
	UNPROTECT(unp + 2);
      }
      
      int rpsi = 0;
      PROTECT(r_probe_set = NEW_LIST(3));
      PROTECT(r_probe_set_names = NEW_LIST(3));

      SET_VECTOR_ELT(r_probe_set, rpsi, cell_list);
      SET_VECTOR_ELT(r_probe_set_names, rpsi++, mkChar("celllist"));
            
      tmp = allocVector(INTSXP, 1);
      INTEGER(tmp)[0] = probeSetType;
      SET_VECTOR_ELT(r_probe_set, rpsi, tmp);
      SET_VECTOR_ELT(r_probe_set_names, rpsi++, mkChar("type"));
      
      tmp = allocVector(INTSXP, 1);
      INTEGER(tmp)[0] = direction;
      SET_VECTOR_ELT(r_probe_set, rpsi, tmp);
      SET_VECTOR_ELT(r_probe_set_names, rpsi++, mkChar("direction"));

      /** set up the names. **/
      setAttrib(r_probe_set, R_NamesSymbol, r_probe_set_names);

      /** now set the probe_set in the main probe_set list. **/
      SET_VECTOR_ELT(probe_sets, iset, r_probe_set);

      /** pop the names and the probe_set of the stack. **/
      UNPROTECT(2);
    }
    
    /** set the names down here at the end. **/
    setAttrib(probe_sets, R_NamesSymbol, names);

    /** unprotect the names and the main probe set list.**/
    UNPROTECT(2);
   
    return probe_sets;
  }

} /** end extern C **/
