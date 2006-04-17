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

  /************************************************************************
   *
   * R_affx_get_pmmm_list()
   *
   * This function conforms to the specification for getting an 
   * environment for CEL files.
   *
   ************************************************************************/
  SEXP R_affx_get_pmmm_list(SEXP fname, SEXP complementary_logic, SEXP verbose) 
  {
    FusionCDFData cdf;
    FusionCDFFileHeader header;
    SEXP names, dim, pmmm, pairs;
    int nRows = 0, nCols = 0;
    char* cdfFileName = CHAR(STRING_ELT(fname, 0));
    int i_verboseFlag = INTEGER(verbose)[0];

    cdf.SetFileName(cdfFileName);

    if (i_verboseFlag >= R_AFFX_VERBOSE) {
      Rprintf("Attempting to read CDF File: %s\n", cdf.GetFileName().c_str());
    }

    if (cdf.Read() == false) {
      Rprintf("Failed to read the CDF file.");
      return R_NilValue;
    }

    header = cdf.GetHeader();
    int nsets = header.GetNumProbeSets();

    PROTECT(pmmm  = NEW_LIST(nsets)); 
    PROTECT(names = NEW_CHARACTER(nsets));

    nRows = header.GetRows();
    nCols = header.GetCols();

    for (int iset = 0; iset < nsets; iset++) {
      const char* name = cdf.GetProbeSetName(iset).c_str();

      if (i_verboseFlag >= R_AFFX_VERBOSE) {
        Rprintf("Processing probeset: %s\n", name);
      }

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
        UNPROTECT(2);  /* 'dim' and then 'pairs' */
      }
    }
    /** set the names of the list. **/
    setAttrib(pmmm, R_NamesSymbol, names);

    /** pop the names, and the vector. **/
    UNPROTECT(2);  /* 'names' and then 'pmmm' */

    return pmmm;
  }



  /************************************************************************
   *
   * R_affx_get_cdf_file_qc()
   *
   ************************************************************************/
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



  /************************************************************************
   *
   * R_affx_get_cdf_file_header()
   *
   ************************************************************************/
  SEXP R_affx_get_cdf_file_header(SEXP fname)
  {
    FusionCDFData cdf;
    char* cdfFileName = CHAR(STRING_ELT(fname, 0));
    
    cdf.SetFileName(cdfFileName);
   
    if (cdf.ReadHeader() == false) {
      Rprintf("Failed to read the CDF file header for: %s\n", cdfFileName);
      return R_NilValue;
    }

    SEXP
      vals = R_NilValue,
      names = R_NilValue, 
      tmp = R_NilValue;
  
    int ii = 0, LIST_ELTS = 7;
    
    FusionCDFFileHeader header = cdf.GetHeader();

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Allocate return list
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    PROTECT(vals  = NEW_LIST(LIST_ELTS));
    PROTECT(names = NEW_CHARACTER(LIST_ELTS));

    /* 
     * Luis should add a version number
     */

    SET_STRING_ELT(names, ii, mkChar("cols"));
    PROTECT(tmp = allocVector(INTSXP, 1));
    INTEGER(tmp)[0] = header.GetCols();
    SET_VECTOR_ELT(vals, ii++, tmp); 
    UNPROTECT(1);
    
    SET_STRING_ELT(names, ii, mkChar("rows"));
    PROTECT(tmp = allocVector(INTSXP, 1));
    INTEGER(tmp)[0] = header.GetRows();
    SET_VECTOR_ELT(vals, ii++, tmp); 
    UNPROTECT(1);
    
    SET_STRING_ELT(names, ii, mkChar("probesets"));
    PROTECT(tmp = allocVector(INTSXP, 1));
    INTEGER(tmp)[0] = header.GetNumProbeSets();
    SET_VECTOR_ELT(vals, ii++, tmp); 
    UNPROTECT(1);
    
    SET_STRING_ELT(names, ii, mkChar("qcprobesets"));
    PROTECT(tmp = allocVector(INTSXP, 1));
    INTEGER(tmp)[0] = header.GetNumQCProbeSets();
    SET_VECTOR_ELT(vals, ii++, tmp); 
    UNPROTECT(1);
    
    SET_STRING_ELT(names, ii, mkChar("reference"));
    SET_VECTOR_ELT(vals, ii++, mkString(header.GetReference().c_str()));

    SET_STRING_ELT(names, ii, mkChar("chiptype"));
    SET_VECTOR_ELT(vals, ii++, mkString(cdf.GetChipType().c_str()));

    SET_STRING_ELT(names, ii, mkChar("filename"));
    SET_VECTOR_ELT(vals, ii++, mkString(cdf.GetFileName().c_str()));

    /** set the names down here at the end. **/
    setAttrib(vals, R_NamesSymbol, names);

    /** Unprotect the returned list. **/
    UNPROTECT(2);  /* 'names' and then 'vals' */
  
    return vals; 
  }  /* R_affx_get_cdf_file_header() */



  /************************************************************************
   *
   * R_affx_get_cdf_file()
   *
   * Note: 
   * Use R_affx_get_cdf_units() instead.
   *
   ************************************************************************/
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
    if (i_verboseFlag >= R_AFFX_VERBOSE) {
      Rprintf("Attempting to read CDF File: %s\n", cdf.GetFileName().c_str());
    }

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
      
      /** i am not sure why there is not a method on ProbeSetInformation 
          for the name. **/
      name = cdf.GetProbeSetName(iset).c_str();
      
      probeSetType = probeset.GetProbeSetType();
      direction = probeset.GetDirection();

      if (i_verboseFlag >= R_AFFX_VERBOSE) {
        Rprintf("Processing probeset: %s with type: %d, direction: %d\n ", name, probeSetType, direction);
      }
      
      /** the probe set names. **/
      SET_STRING_ELT(names, iset, mkChar(name));

      int ngroups = probeset.GetNumGroups();
      
      PROTECT(r_group_list = NEW_LIST(ngroups));
      PROTECT(r_group_names = NEW_CHARACTER(ngroups));

      for (int igroup = 0; igroup < ngroups; igroup++) {
        FusionCDFProbeGroupInformation group;
        probeset.GetGroupInformation(igroup, group);
        
        int ncells = group.GetNumCells();
        int unp = 0, n_list_elts = 5; 
        
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

          if (i_verboseFlag >= R_AFFX_REALLY_VERBOSE) {
            Rprintf("icell: %d x: %d, y: %d, pbase: %c, tbase: %c, expos: %d\n", icell, probe.GetX(), probe.GetY(), probe.GetPBase(), probe.GetTBase(), probe.GetExpos());
          }
          
          INTEGER(xvals)[icell] = probe.GetX();
          INTEGER(yvals)[icell] = probe.GetY();
          
          p_base[0] = probe.GetPBase();
          t_base[0] = probe.GetTBase();
          
          SET_STRING_ELT(pbase, icell, mkChar(p_base));
          SET_STRING_ELT(tbase, icell, mkChar(t_base));
          
          INTEGER(expos)[icell] = probe.GetExpos(); 
        }

        if (i_verboseFlag >= R_AFFX_VERBOSE) {
          Rprintf("finished reading probeset information for: %s\n", name);
        }

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
            
      PROTECT(tmp = allocVector(INTSXP, 1));
      INTEGER(tmp)[0] = probeSetType;
      SET_VECTOR_ELT(r_probe_set, rpsi, tmp);
      SET_VECTOR_ELT(r_probe_set_names, rpsi++, mkChar("type"));
      UNPROTECT(1);
      
      PROTECT(tmp = allocVector(INTSXP, 1));
      INTEGER(tmp)[0] = direction;
      SET_VECTOR_ELT(r_probe_set, rpsi, tmp);
      SET_VECTOR_ELT(r_probe_set_names, rpsi++, mkChar("direction"));
      UNPROTECT(1);

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
  } /* R_affx_get_cdf_file() */




  /************************************************************************
   *
   * R_affx_get_cdf_cell_indices()
   *
   * Description:
   * This function returns a names R list structure where each element
   * corresponds to one unit.  The CDF header is not returned, because 
   * then it would be hard to differentiate that element from a unit; the
   * number of list elements should equal the number of units read.
   *
   ************************************************************************/
  SEXP R_affx_get_cdf_cell_indices(SEXP fname, SEXP units, SEXP verbose) 
  {
    FusionCDFData cdf;

    SEXP
      /* Returned list of units */
      resUnits = R_NilValue,
      unitNames = R_NilValue,
      /* */
      /** one might already want to standardize on this naming scheme... **/
      r_probe_set = R_NilValue, 
      r_probe_set_names = R_NilValue,
      /* */
      r_group_list = R_NilValue, 
      r_group_names = R_NilValue, 
      /* Group fields */
      indices = R_NilValue,
      cell_list = R_NilValue,
      cell_list_names = R_NilValue;

    bool readAll = true; 
    int maxNbrOfUnits = 0, nbrOfUnits = 0, unitIdx = 0;
    int ncol = 0;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Process arguments
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    char* cdfFileName   = CHAR(STRING_ELT(fname, 0));
    int i_verboseFlag   = INTEGER(verbose)[0];

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Opens file
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    cdf.SetFileName(cdfFileName);
    if (i_verboseFlag >= R_AFFX_VERBOSE) {
      Rprintf("Attempting to read CDF File: %s\n", cdf.GetFileName().c_str());
    }

    if (cdf.Read() == false) {
      Rprintf("Failed to read the CDF file.");
      return R_NilValue;
    }

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Get unit indices to be read
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    FusionCDFFileHeader header = cdf.GetHeader();
    maxNbrOfUnits = header.GetNumProbeSets();
    nbrOfUnits = length(units);
    if (nbrOfUnits == 0) {
      nbrOfUnits  = maxNbrOfUnits;
    } else {
      readAll = false;
      /* Validate argument 'units': */
       for (int uu = 0; uu < nbrOfUnits; uu++) {
        unitIdx = INTEGER(units)[uu];
        /* Unit indices are zero-based in Fusion SDK. */
        if (unitIdx < 1 || unitIdx > maxNbrOfUnits) {
          error("Argument 'units' contains an element out of range.");
        }
      }
    }

    ncol = header.GetCols();

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Allocate 'resUnits' list
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    PROTECT(resUnits = NEW_LIST(nbrOfUnits)); 
    PROTECT(unitNames = NEW_CHARACTER(nbrOfUnits));


    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Create field names for all groups in all units
     *
     * Since all groups in all units have fields with identical names,
     * we can allocated the field names ones and let all groups share
     * the same name vector.  This will save memory (appox 232 bytes
     * per group or 13-14Mb for a 100K SNP chip) and speed up things
     * about 10-20%.
		 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    PROTECT(cell_list_names = NEW_STRING(1));
    SET_STRING_ELT(cell_list_names, 0, mkChar("indices"));

    /* Same for all unit elements */
    PROTECT(r_probe_set_names = NEW_LIST(1));
    SET_VECTOR_ELT(r_probe_set_names, 0, mkChar("groups"));



    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * For each unit
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    FusionCDFProbeSetInformation probeset;

    for (int uu = 0; uu < nbrOfUnits; uu++) {
      /* Make it possible to interrupt */
      if(uu % 1000 == 999) R_CheckUserInterrupt();

      if (readAll) {
        unitIdx = uu;
      } else {
        /* Unit indices are zero-based in Fusion SDK. */
        unitIdx = INTEGER(units)[uu] - 1;
      }

      cdf.GetProbeSetInformation(unitIdx, probeset);

      /* get the name */
      /* 'name' is a pointer to a const char: */
      const char* name = cdf.GetProbeSetName(unitIdx).c_str();

      /** ...and add to list of unit names. **/
			SET_STRING_ELT(unitNames, uu, mkChar(name));

      PROTECT(r_probe_set = NEW_LIST(1));

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
      
        PROTECT(cell_list = NEW_LIST(1));
        PROTECT(indices = NEW_INTEGER(ncells));

        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
         * For each cell in the current group...
         * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        for (int icell = 0; icell < ncells; icell++) {
          FusionCDFProbeInformation probe;
          group.GetCell(icell, probe);
        
          int x = probe.GetX();
          int y = probe.GetY();

          /* Cell indices are one-based in R. */
          INTEGER(indices)[icell] = y*ncol + x + 1;
        } /* for (int icell ...) */


        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
         * Assign field values
				 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        /** do I have to make the attribute vector everytime? **/
        SET_VECTOR_ELT(cell_list, 0, indices);

        /** set the names of the new list, dont really know if I need 
            to do this each and every time. **/
        setAttrib(cell_list, R_NamesSymbol, cell_list_names);

        /** set these cells in the group list. **/
        SET_VECTOR_ELT(r_group_list, igroup, cell_list);
        SET_STRING_ELT(r_group_names, igroup, mkChar(group.GetName().c_str()));

        /* Unprotect in reverse order */
 		    UNPROTECT(2);  /* 'indices' and then 'cell_list' */
      } /* for (int igroup ...) */

      /** set the group names. **/
      setAttrib(r_group_list, R_NamesSymbol, r_group_names);

      /** add groups to current unit. **/
      SET_VECTOR_ELT(r_probe_set, 0, r_group_list);

      /** add current unit to list of all units. **/
      setAttrib(r_probe_set, R_NamesSymbol, r_probe_set_names);
      SET_VECTOR_ELT(resUnits, uu, r_probe_set);

      /* 'r_group_names' and then 'r_group_list' and 'r_probe_set' */
      UNPROTECT(3); 
    } /* for (int uu...) */

    UNPROTECT(2);  /* 'r_probe_set_names' and then  'cell_list_names' */
    
    /** set all unit names. **/
    setAttrib(resUnits, R_NamesSymbol, unitNames);

    /** unprotect return list. **/
    UNPROTECT(2); /* 'unitNames' and then 'resUnits' */

    return resUnits;
  } /* R_affx_get_cdf_cell_indices() */




  /************************************************************************
   *
   * R_affx_get_cdf_units()
   *
   * Description:
   * This function returns a names R list structure where each element
   * corresponds to one unit.  The CDF header is not returned, because 
   * then it would be hard to differentiate that element from a unit; the
   * number of list elements should equal the number of units read.
   *
   * Note: 
   * This function does what R_affx_get_cdf_file() does and more.  The 
   * plan is to remove the latter.
   *
   ************************************************************************/
  SEXP R_affx_get_cdf_units(SEXP fname, SEXP units, SEXP readXY, SEXP readBases, SEXP readExpos, SEXP readType, SEXP readDirection, SEXP readIndices, SEXP verbose) 
  {
    FusionCDFData cdf;

    SEXP
      /* Returned list of units */
      resUnits = R_NilValue,
      unitNames = R_NilValue,
      /* */
      /** one might already want to standardize on this naming scheme... **/
      r_probe_set = R_NilValue, 
      r_probe_set_names = R_NilValue,
      /* */
      r_group_list = R_NilValue, 
      r_group_names = R_NilValue, 
      /* Group fields */
      xvals = R_NilValue,
      yvals = R_NilValue,
      indices = R_NilValue,
      pbase = R_NilValue,
      tbase = R_NilValue,
      expos = R_NilValue,
      cell_list = R_NilValue,
      cell_list_names = R_NilValue,
      tmp = R_NilValue; 

    bool readAll = true; 
    int maxNbrOfUnits = 0, nbrOfUnits = 0, unitIdx = 0;
    int ncol = 0;

    /** 
     XXX: I am not sure this is the most elegant way to handle these in R. 
     I initialize it hear for kicks.
    **/
    char p_base[2] = "X";
    char t_base[2] = "X";

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Process arguments
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    char* cdfFileName   = CHAR(STRING_ELT(fname, 0));
    int i_readXY        = INTEGER(readXY)[0];
    int i_readBases     = INTEGER(readBases)[0];
    int i_readExpos     = INTEGER(readExpos)[0];
    int i_readType      = INTEGER(readType)[0];
    int i_readDirection = INTEGER(readDirection)[0];
    int i_readIndices   = INTEGER(readIndices)[0];
    int i_verboseFlag   = INTEGER(verbose)[0];

    int i_readGroups = i_readXY || i_readBases || i_readExpos || i_readIndices;


    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * A special case?
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    if (i_readIndices && !i_readXY && !i_readBases && !i_readExpos && 
                                   				!i_readType && !i_readDirection) {
      return R_affx_get_cdf_cell_indices(fname, units, verbose);
    }


    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Opens file
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    cdf.SetFileName(cdfFileName);
    if (i_verboseFlag >= R_AFFX_VERBOSE) {
      Rprintf("Attempting to read CDF File: %s\n", cdf.GetFileName().c_str());
    }

    if (cdf.Read() == false) {
      Rprintf("Failed to read the CDF file.");
      return R_NilValue;
    }

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Get unit indices to be read
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    FusionCDFFileHeader header = cdf.GetHeader();
    maxNbrOfUnits = header.GetNumProbeSets();
    nbrOfUnits = length(units);
    if (nbrOfUnits == 0) {
      nbrOfUnits  = maxNbrOfUnits;
    } else {
      readAll = false;
      /* Validate argument 'units': */
       for (int uu = 0; uu < nbrOfUnits; uu++) {
        unitIdx = INTEGER(units)[uu];
        /* Unit indices are zero-based in Fusion SDK. */
        if (unitIdx < 1 || unitIdx > maxNbrOfUnits) {
          error("Argument 'units' contains an element out of range.");
        }
      }
    }

    if (i_readIndices) {
      ncol = header.GetCols();
    }

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Allocate 'resUnits' list
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    PROTECT(resUnits = NEW_LIST(nbrOfUnits)); 
    PROTECT(unitNames = NEW_CHARACTER(nbrOfUnits));

    int nbrOfUnitElements = i_readGroups + i_readType + i_readDirection;
    int nbrOfGroupElements = 2*i_readXY + 2*i_readBases + i_readExpos + i_readIndices;


    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Create field names for all groups in all units
     *
     * Since all groups in all units have fields with identical names,
     * we can allocated the field names ones and let all groups share
     * the same name vector.  This will save memory (appox 232 bytes
     * per group or 13-14Mb for a 100K SNP chip) and speed up things
     * about 10-20%.
		 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    int fieldIdx = 0;
    if (i_readGroups) {
      PROTECT(cell_list_names = NEW_STRING(nbrOfGroupElements));
      if (i_readXY) {
        SET_STRING_ELT(cell_list_names, fieldIdx++, mkChar("x"));
        SET_STRING_ELT(cell_list_names, fieldIdx++, mkChar("y"));
      }
  
      if (i_readIndices) {
        SET_STRING_ELT(cell_list_names, fieldIdx++, mkChar("indices"));
      }
       
      if (i_readBases) {
        SET_STRING_ELT(cell_list_names, fieldIdx++, mkChar("pbase"));
        SET_STRING_ELT(cell_list_names, fieldIdx++, mkChar("tbase"));
      }
      
      if (i_readExpos) {
        SET_STRING_ELT(cell_list_names, fieldIdx++, mkChar("expos"));
      }
    }


    /* Same for all unit elements */
    PROTECT(r_probe_set_names = NEW_LIST(nbrOfUnitElements));
    int rpsi = 0;
    if (i_readType) {
      /* get the type */
      SET_VECTOR_ELT(r_probe_set_names, rpsi++, mkChar("type"));
    }

    if (i_readDirection) {
      /* get the direction */
      SET_VECTOR_ELT(r_probe_set_names, rpsi++, mkChar("direction"));
    }

    if (i_readGroups) {
      SET_VECTOR_ELT(r_probe_set_names, rpsi++, mkChar("groups"));
    }



    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * For each unit
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    FusionCDFProbeSetInformation probeset;


    for (int uu = 0; uu < nbrOfUnits; uu++) {
      /* Make it possible to interrupt */
      if(uu % 1000 == 999) R_CheckUserInterrupt();

      if (i_verboseFlag >= R_AFFX_REALLY_VERBOSE) {
        Rprintf("Unit %d/%d...", uu+1, nbrOfUnits);
      } else if (i_verboseFlag >= R_AFFX_VERBOSE) {
        if (uu % 1000 == 0 || uu == nbrOfUnits-1 || 
                                    i_verboseFlag >= R_AFFX_REALLY_VERBOSE) {
          Rprintf("%d/%d, ", uu+1, nbrOfUnits);
        }
      }

      if (readAll) {
        unitIdx = uu;
      } else {
        /* Unit indices are zero-based in Fusion SDK. */
        unitIdx = INTEGER(units)[uu] - 1;
      }

      if (i_verboseFlag >= R_AFFX_REALLY_VERBOSE) {
        Rprintf("Unit: %d", unitIdx);
      }

      cdf.GetProbeSetInformation(unitIdx, probeset);

      if (i_verboseFlag >= R_AFFX_REALLY_VERBOSE) {
        Rprintf(", ");
      }
      
      /* get the name */
      /* 'name' is a pointer to a const char: */
      const char* name = cdf.GetProbeSetName(unitIdx).c_str();

      if (i_verboseFlag >= R_AFFX_REALLY_VERBOSE) {
        /* TROUBLESHOOTING: Something goes wrong with 'name'?!? */
        Rprintf("name: [0x%x:", name);
        for (int kk=0; kk < 20; kk++) {
          printf(" %02d", kk);   /* TROUBLESHOOTING: kk=0 is outputted */ 
          char ch = (char)name[kk];   /* When R crashes it happens here! */
          printf("=");           /* TROUBLESHOOTING: ...but not this! */
          int value = (int)name[kk];
          printf("=");
          printf("(%03d)", value);
          printf("%c", ch);
        }
				/* 
        for (int kk=0; kk < 20; kk++) {
          Rprintf(" %02d", kk);
          int value = (int)name[kk];
          Rprintf("=");
          Rprintf("(%03d)", value);
          Rprintf("%c", (char)name[kk]);
        }
				*/
        Rprintf("] ");
        /* TROUBLESHOOTING: If the above is removed, it crashed below: */
        Rprintf("%s\n", name);
      }

      /** ...and add to list of unit names. **/
			SET_STRING_ELT(unitNames, uu, mkChar(name));

      
      PROTECT(r_probe_set = NEW_LIST(nbrOfUnitElements));
      rpsi = 0;

      if (i_readType) {
        /* get the type */
        PROTECT(tmp = allocVector(INTSXP, 1));
        INTEGER(tmp)[0] = probeset.GetProbeSetType();
        SET_VECTOR_ELT(r_probe_set, rpsi++, tmp);
        UNPROTECT(1);
      }

      if (i_readDirection) {
        /* get the direction */
        PROTECT(tmp = allocVector(INTSXP, 1));
        INTEGER(tmp)[0] = probeset.GetDirection();
        SET_VECTOR_ELT(r_probe_set, rpsi++, tmp);
        UNPROTECT(1);
      }


      if (i_readGroups) {
        int ngroups = probeset.GetNumGroups();
     
        PROTECT(r_group_list = NEW_LIST(ngroups));
        PROTECT(r_group_names = NEW_CHARACTER(ngroups));
          

        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
         * For each group in the current unit
         * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        for (int igroup = 0; igroup < ngroups; igroup++) {
					/*
          if (i_verboseFlag >= R_AFFX_REALLY_VERBOSE) {
            Rprintf("Group %d/%d...\n", igroup+1, ngroups);
          }
          */
          FusionCDFProbeGroupInformation group;
          probeset.GetGroupInformation(igroup, group);
        
          int ncells = group.GetNumCells();
        
          PROTECT(cell_list = NEW_LIST(nbrOfGroupElements));

          int protectCount = 0;

          if (i_readXY) {
            PROTECT(xvals = NEW_INTEGER(ncells));
            protectCount++;
            PROTECT(yvals = NEW_INTEGER(ncells));
            protectCount++;
          }

          if (i_readIndices) {
            PROTECT(indices = NEW_INTEGER(ncells));
            protectCount++;
          }

          if (i_readBases) {
            PROTECT(pbase = NEW_STRING(ncells));
            protectCount++;
            PROTECT(tbase = NEW_STRING(ncells));
            protectCount++;
          }

          if (i_readExpos) {
            PROTECT(expos = NEW_INTEGER(ncells));
            protectCount++;
          }

          /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
           * For each cell in the current group...
           * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
          for (int icell = 0; icell < ncells; icell++) {
            FusionCDFProbeInformation probe;
            group.GetCell(icell, probe);
          
            if (i_readXY || i_readIndices) {
              int x = probe.GetX();
              int y = probe.GetY();

              if (i_readXY) {
                INTEGER(xvals)[icell] = x;
                INTEGER(yvals)[icell] = y;
              }
              
              if (i_readIndices) {
                /* Cell indices are one-based in R. */
                INTEGER(indices)[icell] = y*ncol + x + 1;
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

            if (i_verboseFlag > R_AFFX_REALLY_VERBOSE) {
              Rprintf("cell: %2d, (x,y)=(%4d,%4d), (pbase,tbase)=(%c,%c), expos: %2d\n", icell, probe.GetX(), probe.GetY(), probe.GetPBase(), probe.GetTBase(), probe.GetExpos());
            }
          } /* for (int icell ...) */


          /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
           * Assign field values
					 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
          fieldIdx = 0;
          /** do I have to make the attribute vector everytime? **/
          if (i_readXY) {
            SET_VECTOR_ELT(cell_list, fieldIdx++, xvals);
            SET_VECTOR_ELT(cell_list, fieldIdx++, yvals);
          }

          if (i_readIndices) {
            SET_VECTOR_ELT(cell_list, fieldIdx++, indices);
          }
           
          if (i_readBases) {
            SET_VECTOR_ELT(cell_list, fieldIdx++, pbase);
            SET_VECTOR_ELT(cell_list, fieldIdx++, tbase);
          }
          
          if (i_readExpos) {
            SET_VECTOR_ELT(cell_list, fieldIdx++, expos);
          }

          /* Unprotect in reverse order, e.g. 'expos', ..., 'xvals' */
          UNPROTECT(protectCount); 
          
          /** set the names of the new list, dont really know if I need 
              to do this each and every time. **/
          setAttrib(cell_list, R_NamesSymbol, cell_list_names);

          /** set these cells in the group list. **/
          SET_VECTOR_ELT(r_group_list, igroup, cell_list);
          SET_STRING_ELT(r_group_names, igroup, mkChar(group.GetName().c_str()));
          UNPROTECT(1); /* 'cell_list' */

					/*
          if (i_verboseFlag >= R_AFFX_REALLY_VERBOSE) {
            Rprintf("Group %d/%d...done\n", igroup+1, ngroups);
          }
          */
        } /* for (int igroup ...) */

        /** set the group names. **/
        setAttrib(r_group_list, R_NamesSymbol, r_group_names);

        /** add groups to current unit. **/
        SET_VECTOR_ELT(r_probe_set, rpsi, r_group_list);

        UNPROTECT(2); /* 'r_group_names' and then 'r_group_list' */
      } /* if (i_readGroups) */
 

      /** add current unit to list of all units. **/
      setAttrib(r_probe_set, R_NamesSymbol, r_probe_set_names);
      SET_VECTOR_ELT(resUnits, uu, r_probe_set);

      UNPROTECT(1);  /* 'r_probe_set' */

      if (i_verboseFlag >= R_AFFX_REALLY_VERBOSE) {
        Rprintf("Unit %d/%d...done\n", uu+1, nbrOfUnits);
      }
    } /* for (int uu...) */

    UNPROTECT(1);  /* 'r_probe_set_names' */

    if (i_readGroups) {
      UNPROTECT(1); /* 'cell_list_names' */
    }
    
    /** set all unit names. **/
    setAttrib(resUnits, R_NamesSymbol, unitNames);

    if (i_verboseFlag >= R_AFFX_REALLY_VERBOSE) {
      Rprintf("R_affx_get_cdf_units()...done\n");
    }

    /** unprotect return list. **/
    UNPROTECT(2); /* 'unitNames' and then 'resUnits' */
   
    return resUnits;
  } /* R_affx_get_cdf_units() */



  /************************************************************************
   *
   * R_affx_get_cdf_unit_names()
   *
   ************************************************************************/
  SEXP R_affx_get_cdf_unit_names(SEXP fname, SEXP units, SEXP verbose) 
  {
    FusionCDFData cdf;
    
    SEXP names = R_NilValue;

    bool readAll = true; 
    int maxNbrOfUnits = 0, nbrOfUnits = 0; 
    int unitIdx = 0;
    char* cdfFileName = CHAR(STRING_ELT(fname, 0));
    int i_verboseFlag = INTEGER(verbose)[0];

    /** pointer to the name of the probeset. **/
    const char* name;
    
    cdf.SetFileName(cdfFileName);
    if (i_verboseFlag >= R_AFFX_VERBOSE) {
      Rprintf("Attempting to read CDF File: %s\n", cdf.GetFileName().c_str());
    }

    if (cdf.Read() == false) {
      Rprintf("Failed to read the CDF file.");
      return R_NilValue;
    }

    FusionCDFFileHeader header = cdf.GetHeader();
    maxNbrOfUnits = header.GetNumProbeSets();

    nbrOfUnits = length(units);
    if (nbrOfUnits == 0) {
      nbrOfUnits = maxNbrOfUnits;
    } else {
      readAll = false;
      /* Validate argument 'units': */
      for (int uu = 0; uu < nbrOfUnits; uu++) {
        unitIdx = INTEGER(units)[uu];
        /* Unit indices are zero-based in Fusion SDK. */
        if (unitIdx < 1 || unitIdx > maxNbrOfUnits) {
          error("Argument 'units' contains an element out of range.");
        }
      }
    }

    /** the probe set names. **/
    PROTECT(names = NEW_CHARACTER(nbrOfUnits));
    if (readAll) {
      for (int uu = 0; uu < nbrOfUnits; uu++) {
        name = cdf.GetProbeSetName(uu).c_str();
        SET_STRING_ELT(names, uu, mkChar(name));
      }
    } else {
      for (int uu = 0; uu < nbrOfUnits; uu++) {
        /* Unit indices are zero-based in Fusion SDK. */
        unitIdx = INTEGER(units)[uu] - 1;
        name = cdf.GetProbeSetName(unitIdx).c_str();
        SET_STRING_ELT(names, uu, mkChar(name));
      }
    }

    /** unprotect the return vector. **/
    UNPROTECT(1); /* 'names' */
   
    return names;
  }  /* R_affx_get_cdf_unit_names() */




} /** end extern C **/


/***************************************************************************
 * HISTORY:
 * 2006-04-02
 * o Added R_affx_get_cdf_cell_indices() for faster reading of cell indices.
 * 2006-04-01
 * o For R_affx_get_cdf_units() the field names in all groups and units are
 *   pointing to the same vector of names in memory.  This save about 
 *   13-15Mb and speeds up the reading.  I think/hope this is valid to do!
 * 2006-03-28
 * o Unit indices and cell indices are now one-based. /HB
 * o Renamed argument 'readCells' to 'readIndices' and returned field
 *   'cells' to 'indices'.
 * 2006-03-26
 * o Note: PROTECT()/UNPROTECT() is a LIFO stack and not a FIFO queue!
 * o Added more verbose output.  Trying to find where R crashes.  There
 *   seems to be some kind of memory leak, but I cannot pinpoint it. /HB
 * o Added PROTECT() to all allocVector() allocated variables in
 *   R_affx_get_cdf_units().
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
