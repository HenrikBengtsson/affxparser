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


  SEXP R_affx_get_cdf_file_qc(SEXP fname, SEXP units, SEXP verbose) 
  {
    /** 
     * XXX: This might look very similar to the code for reading of the normal
     * probe sets ; so i am going to wait until i am confident that i have that
     * implemented nicely.
     **/
    SEXP ans = R_NilValue;
    return ans;
  }

  SEXP R_affx_get_cdf_file_header(SEXP fname)
  {
    FusionCDFData cdf;
    char* cdfFileName = CHAR(STRING_ELT(fname, 0));
    
    cdf.SetFileName(cdfFileName);
   
    if (cdf.ReadHeader() == false) {
      Rprintf("Failed to read the CDF file header for: %s\n", cdfFileName);
      return R_NilValue;
    }
    else { 
      FusionCDFFileHeader header;

      SEXP names = R_NilValue, 
      vals = R_NilValue,
      tmp = R_NilValue;
    
      int i = 0, LIST_ELTS = 7;
      
      header = cdf.GetHeader();

      PROTECT(names = NEW_CHARACTER(LIST_ELTS));
      PROTECT(vals  = NEW_LIST(LIST_ELTS));

      /* 
       * Luis should add a version number
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
      
      SET_STRING_ELT(names, i, mkChar("reference"));
      SET_VECTOR_ELT(vals, i++, mkString(header.GetReference().c_str()));

      SET_STRING_ELT(names, i, mkChar("chiptype"));
      SET_VECTOR_ELT(vals, i++, mkString(cdf.GetChipType().c_str()));

      SET_STRING_ELT(names, i, mkChar("filename"));
      SET_VECTOR_ELT(vals, i++, mkString(cdf.GetFileName().c_str()));

      /** set the names down here at the end. **/
      setAttrib(vals, R_NamesSymbol, names);
      UNPROTECT(2);
    
      return vals; 
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
      r_group_list = R_NilValue, 
      r_group_names = R_NilValue, 
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
      
      PROTECT(r_group_list = NEW_LIST(ngroups));
      PROTECT(r_group_names = NEW_CHARACTER(ngroups));

      for (int igroup = 0; igroup < ngroups; igroup++) {
	FusionCDFProbeGroupInformation group;
	probeset.GetGroupInformation(igroup, group);
	
	int ncells = group.GetNumCells();
	int 
	  unp = 0, 
	  n_list_elts = 5; 
	
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
	  
	  p_base[0] = probe.GetPBase();
	  t_base[0] = probe.GetTBase();
	  
	  SET_STRING_ELT(pbase, icell, mkChar(p_base));
	  SET_STRING_ELT(tbase, icell, mkChar(t_base));
	  
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
	
	/** set these cells in the group list. **/
	SET_VECTOR_ELT(r_group_list, igroup, cell_list);
	SET_STRING_ELT(r_group_names, igroup, mkChar(group.GetName().c_str()));
	
	/** unprotect the vectors stored in our list. **/
	UNPROTECT(unp + 2);
      }
      
      /** set the group names. **/
      setAttrib(r_group_list, R_NamesSymbol, r_group_names);

      int rpsi = 0;
      PROTECT(r_probe_set = NEW_LIST(3));
      PROTECT(r_probe_set_names = NEW_LIST(3));

      SET_VECTOR_ELT(r_probe_set, rpsi, r_group_list);
      SET_VECTOR_ELT(r_probe_set_names, rpsi++, mkChar("groups"));
            
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
      UNPROTECT(4);
    }
    
    /** set the names down here at the end. **/
    setAttrib(probe_sets, R_NamesSymbol, names);

    /** unprotect the names and the main probe set list.**/
    UNPROTECT(2);
   
    return probe_sets;
  }


  SEXP R_affx_get_cdf_units(SEXP fname, SEXP units, SEXP readXY, SEXP readCells, SEXP readBases, SEXP readExpos, SEXP readType, SEXP readDirection, SEXP verbose) 
  {
    FusionCDFData cdf;
    FusionCDFFileHeader header;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Note: This function does what R_affx_get_cdf_file() does and more.
     *       The plan is to remove the latter.
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Process arguments
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    char* cdfFileName   = CHAR(STRING_ELT(fname, 0));
    int i_readXY        = INTEGER(readXY)[0];
    int i_readCells     = INTEGER(readCells)[0];
    int i_readBases     = INTEGER(readBases)[0];
    int i_readExpos     = INTEGER(readExpos)[0];
    int i_readType      = INTEGER(readType)[0];
    int i_readDirection = INTEGER(readDirection)[0];
    int i_verboseFlag   = INTEGER(verbose)[0];

    int i_readGroups    = (i_readXY || i_readCells || i_readBases || i_readExpos);

    /*
     * What about returning the header as well?
     */
    SEXP names = R_NilValue, 
      probe_sets = R_NilValue,
      cell_list = R_NilValue,
      cell_list_names = R_NilValue,
      xvals = R_NilValue,
      yvals = R_NilValue,
      cells = R_NilValue,
      pbase = R_NilValue,
      tbase = R_NilValue,
      expos = R_NilValue,
      r_group_list = R_NilValue, 
      r_group_names = R_NilValue, 
      r_probe_set = R_NilValue, /** one might already want to standardize on this naming scheme... **/
      r_probe_set_names = R_NilValue,
      tmp = R_NilValue; 
    

    bool readAll = true; 
    int nsets = 0, nunits = 0;
    int iset = 0;
    int x = 0, y = 0, ncol = 0;
    
    /** 
     XXX: I am not sure this is the most elegant way to handle these in R. 
     I initialize it hear for kicks.
    **/
    char p_base[2] = "X";
    char t_base[2] = "X";
    
    /** pointer to the name of the probeset. **/
    const char* name;
    int probeSetType = -1, direction = -1;
    
    FusionCDFProbeSetInformation probeset;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Opens file
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    cdf.SetFileName(cdfFileName);
    if (i_verboseFlag >= R_AFFX_VERBOSE)
      Rprintf("Attempting to read CDF File: %s\n", cdf.GetFileName().c_str());

    if (cdf.Read() == false) {
      Rprintf("Failed to read the CDF file.");
      return R_NilValue;
    }

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Get unit indices to be read
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    header = cdf.GetHeader();
    nsets = header.GetNumProbeSets();
    nunits = length(units);
    if (nunits == 0) {
      nunits  = nsets;
    } else {
      readAll = false;
      /* Validate argument 'units': */
       for (int ii = 0; ii < nunits; ii++) {
        iset = INTEGER(units)[ii];
        if (iset < 0 || iset >= nsets) {
          error("Argument 'units' contains an element out of range.");
        }
      }
    }

    if (i_readCells) {
      ncol = header.GetCols();
    }

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Allocate named list of units to be returned
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    PROTECT(names = NEW_CHARACTER(nunits));
    PROTECT(probe_sets = NEW_LIST(nunits)); 

    int nbrOfUnitElements = i_readGroups + i_readType + i_readDirection;
    int nbrOfGroupElements = 2*i_readXY + i_readCells + 2*i_readBases + i_readExpos;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * For each unit
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    for (int ii = 0; ii < nunits; ii++) {
      if (readAll) {
        iset = ii;
      } else {
        iset = INTEGER(units)[ii];
      }

      cdf.GetProbeSetInformation(iset, probeset);
      
      /* get the name */
      name = cdf.GetProbeSetName(iset).c_str();

      /** ...and add to list of unit names. **/
      SET_STRING_ELT(names, ii, mkChar(name));
      
      PROTECT(r_probe_set = NEW_LIST(nbrOfUnitElements));
      PROTECT(r_probe_set_names = NEW_LIST(nbrOfUnitElements));
      int rpsi = 0;
     
      if (i_readType) {
        /* get the type */
        probeSetType = probeset.GetProbeSetType();
        tmp = allocVector(INTSXP, 1);
        INTEGER(tmp)[0] = probeSetType;
        SET_VECTOR_ELT(r_probe_set, rpsi, tmp);
        SET_VECTOR_ELT(r_probe_set_names, rpsi++, mkChar("type"));
      }

      if (i_readDirection) {
        /* get the direction */
        direction = probeset.GetDirection();

        tmp = allocVector(INTSXP, 1);
        INTEGER(tmp)[0] = direction;
        SET_VECTOR_ELT(r_probe_set, rpsi, tmp);
        SET_VECTOR_ELT(r_probe_set_names, rpsi++, mkChar("direction"));
      }

      if (i_verboseFlag >= R_AFFX_VERBOSE) {
        Rprintf("Processing probeset: %s with type: %d, direction: %d\n ", 
                                             name, probeSetType, direction);
      }

      if (i_readGroups) {
        int ngroups = probeset.GetNumGroups();
     
        PROTECT(r_group_list = NEW_LIST(ngroups));
        PROTECT(r_group_names = NEW_CHARACTER(ngroups));

        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
         * For each group in the current unit
         * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        for (int igroup = 0; igroup < ngroups; igroup++) {
          FusionCDFProbeGroupInformation group;
          probeset.GetGroupInformation(igroup, group);
        
          int ncells = group.GetNumCells();
          int unp = 0;
        
          PROTECT(cell_list = NEW_LIST(nbrOfGroupElements));
          PROTECT(cell_list_names = NEW_STRING(nbrOfGroupElements));
  
          if (i_readXY) {
            PROTECT(xvals = NEW_INTEGER(ncells));
            PROTECT(yvals = NEW_INTEGER(ncells));
          }

          if (i_readCells) {
            PROTECT(cells = NEW_INTEGER(ncells));
          }

          if (i_readBases) {
            PROTECT(pbase = NEW_STRING(ncells));
            PROTECT(tbase = NEW_STRING(ncells));
          }

          if (i_readExpos) {
            PROTECT(expos = NEW_INTEGER(ncells));
          }

          /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
           * For each cell in the current group
           * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
          for (int icell = 0; icell < ncells; icell++) {
            FusionCDFProbeInformation probe;
            group.GetCell(icell, probe);
          
            if (i_verboseFlag >= R_AFFX_REALLY_VERBOSE) {
              Rprintf("icell: %d x: %d, y: %d, pbase: %c, tbase: %c, expos: %d\n",
                           icell, probe.GetX(), probe.GetY(), probe.GetPBase(), 
                                           probe.GetTBase(), probe.GetExpos());
            }
             
            if (i_readXY || i_readCells) {
              x = probe.GetX();
              y = probe.GetY();

              if (i_readXY) {
                INTEGER(xvals)[icell] = x;
                INTEGER(yvals)[icell] = y;
              }
              
              if (i_readCells) {
                INTEGER(cells)[icell] = y*ncol + x;
              }
            }

            if (i_readBases) {
              p_base[0] = probe.GetPBase();
              t_base[0] = probe.GetTBase();
              SET_STRING_ELT(pbase, icell, mkChar(p_base));
              SET_STRING_ELT(tbase, icell, mkChar(t_base));
            }
            
            if (i_readExpos) {
              INTEGER(expos)[icell] = probe.GetExpos();
            }
          } /* for (int icell ...) */
          
          if (i_verboseFlag >= R_AFFX_VERBOSE) {
            Rprintf("finished reading probeset information for: %s\n", name);
          }
          
          /** do I have to make the attribute vector everytime? **/
          if (i_readXY) {
            SET_VECTOR_ELT(cell_list, unp, xvals);
            SET_STRING_ELT(cell_list_names, unp++, mkChar("x"));
            SET_VECTOR_ELT(cell_list, unp, yvals);
            SET_STRING_ELT(cell_list_names, unp++, mkChar("y"));
          }

          if (i_readCells) {
            SET_VECTOR_ELT(cell_list, unp, cells);
            SET_STRING_ELT(cell_list_names, unp++, mkChar("cells"));
          }
           
          if (i_readBases) {
            SET_VECTOR_ELT(cell_list, unp, pbase);
            SET_STRING_ELT(cell_list_names, unp++, mkChar("pbase"));
            SET_VECTOR_ELT(cell_list, unp, tbase);
            SET_STRING_ELT(cell_list_names, unp++, mkChar("tbase"));
          }
          
          if (i_readExpos) {
            SET_VECTOR_ELT(cell_list, unp, expos);
            SET_STRING_ELT(cell_list_names, unp++, mkChar("expos"));
          }
          
          /** set the names of the new list, dont really know if I need to do
              this each and every time. **/
          setAttrib(cell_list, R_NamesSymbol, cell_list_names);
          
          /** set these cells in the group list. **/
          SET_VECTOR_ELT(r_group_list, igroup, cell_list);
          SET_STRING_ELT(r_group_names, igroup, mkChar(group.GetName().c_str()));
          
          /** unprotect the vectors stored in our list. **/
          if (i_readExpos) {
            UNPROTECT(1);
          }
          if (i_readBases) {
            UNPROTECT(2);
          }
          if (i_readCells) {
            UNPROTECT(1);
          }
          if (i_readXY) {
            UNPROTECT(2);
          }

          UNPROTECT(2);
        } /* for (int igroup ...) */

        /** set the group names. **/
        setAttrib(r_group_list, R_NamesSymbol, r_group_names);

        /** add groups to current unit. **/
        SET_VECTOR_ELT(r_probe_set, rpsi, r_group_list);
        SET_VECTOR_ELT(r_probe_set_names, rpsi++, mkChar("groups"));

        UNPROTECT(2);
      } if (i_readGroups)
      
      /** add current unit to list of all units. **/
      setAttrib(r_probe_set, R_NamesSymbol, r_probe_set_names);
      SET_VECTOR_ELT(probe_sets, ii, r_probe_set);

      /** pop the names and the probe_set of the stack. **/
      UNPROTECT(2);
    } /* for (int ii...) */
    
    /** set all unit names. **/
    setAttrib(probe_sets, R_NamesSymbol, names);

    /** unprotect the names and the main probe set list.**/
    UNPROTECT(2);
   
    return probe_sets;
  } /* R_affx_get_cdf_units() */

  SEXP R_affx_get_cdf_unit_names(SEXP fname, SEXP units, SEXP verbose) 
  {
    FusionCDFData cdf;
    FusionCDFFileHeader header;
    
    SEXP names = R_NilValue;

    bool readAll = true; 
    int nsets = 0, nunits = 0; 
    int iset = 0;
    char* cdfFileName = CHAR(STRING_ELT(fname, 0));
    int i_verboseFlag = INTEGER(verbose)[0];

    /** pointer to the name of the probeset. **/
    const char* name;
    
    cdf.SetFileName(cdfFileName);
    if (i_verboseFlag >= R_AFFX_VERBOSE)
      Rprintf("Attempting to read CDF File: %s\n", cdf.GetFileName().c_str());

    if (cdf.Read() == false) {
      Rprintf("Failed to read the CDF file.");
      return R_NilValue;
    }

    header = cdf.GetHeader();
    nsets = header.GetNumProbeSets();

    nunits = length(units);
    if (nunits == 0) {
      nunits = nsets;
    } else {
      readAll = false;
      /* Validate argument 'units': */
                   for (int ii = 0; ii < nunits; ii++) {
        iset = INTEGER(units)[ii];
        if (iset < 0 || iset >= nsets) {
          error("Argument 'units' contains an element out of range.");
        }
      }
    }

    /** the probe set names. **/
    PROTECT(names = NEW_CHARACTER(nunits));
    if (readAll) {
      for (int ii = 0; ii < nunits; ii++) {
        name = cdf.GetProbeSetName(ii).c_str();
        SET_STRING_ELT(names, ii, mkChar(name));
      }
    } else {
      for (int ii = 0; ii < nunits; ii++) {
        iset = INTEGER(units)[ii];
        name = cdf.GetProbeSetName(iset).c_str();
        SET_STRING_ELT(names, ii, mkChar(name));
      }
    }

    /** unprotect the names and the main probe set list.**/
    UNPROTECT(1);
   
    return names;
  }




} /** end extern C **/


/***************************************************************************
 * HISTORY:
 * 2006-02-21
 * o Added argument 'readCells' to R_affx_get_cdf_units(). /HB
 * 2006-01-15
 * o It is now possible to specify what readCdfUnits() should return. /HB
 * 2006-01-12
 * o BUG FIX: The check of the upper-limit of unit indicies was done 
 *   assuming one-based indices. /HB
 * 2006-01-10
 * o Updated the "units" code to be more similar to the corresponding code
 *   for CEL files. /HB
 * o Added a return value to non-void function R_affx_get_cdf_file_qc(). /HB
 * 2006-01-09
 * o Added R_affx_get_cdf_units() and R_affx_get_cdf_unit.names(). /HB
 * o Created.  The purpose was to make it possible to read subsets of units
 *   and not just all units at once. /HB
 **************************************************************************/
