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
   * R_affx_cdf_isMm()
   *
   ************************************************************************/
  SEXP R_affx_cdf_isMm(SEXP fname, SEXP units, SEXP verbose) 
  {
    FusionCDFData cdf;
    FusionCDFFileHeader header;
    string str;
    int str_length; 
    char* cstr; 
    
    SEXP names = R_NilValue, 
         probe_sets = R_NilValue,
         r_groups = R_NilValue, 
                     r_group_names = R_NilValue,
                     isMm = R_NilValue;
    
    bool readAll = true; 
    int nsets = 0, nunits = 0;
    int iset = 0;
    char* cdfFileName = CHAR(STRING_ELT(fname, 0));
    int i_verboseFlag = INTEGER(verbose)[0];


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
     * Read header and get unit indices to be read
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
        if (iset < 1 || iset > nsets) {
          error("Argument 'units' contains an element out of range.");
        }
      }
    }

    /* Allocate R character vector and R list for the names and units */
    PROTECT(probe_sets = NEW_LIST(nunits)); 
    PROTECT(names = NEW_CHARACTER(nunits));

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Access information for the requested units one by one
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    for (int ii = 0; ii < nunits; ii++) {
      FusionCDFProbeSetInformation probeset;

      if (readAll) {
        iset = ii;
      } else {
        /* Unit indices are zero-based in Fusion SDK. */
        iset = INTEGER(units)[ii] - 1;
      }

      /* Retrieve the current unit */
      cdf.GetProbeSetInformation(iset, probeset);
      
      /* Record its name */
      str = cdf.GetProbeSetName(iset);
      str_length = str.size();
      cstr = Calloc(str_length+1, char);
      strncpy(cstr, str.c_str(), str_length);
      cstr[str_length] = '\0';
      SET_STRING_ELT(names, ii, mkChar(cstr));
      Free(cstr);
     
      /* Get the number of groups in the unit */
      int ngroups = probeset.GetNumGroups();
      
      /* Allocate an R list and a vector of names for the groups */
      PROTECT(r_groups = NEW_LIST(ngroups));
      PROTECT(r_group_names = NEW_CHARACTER(ngroups));

      /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
       * For each group, query the number of cells (probes)
       * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
      for (int igroup = 0; igroup < ngroups; igroup++) {
        FusionCDFProbeGroupInformation group;

        /* Get the current group */
        probeset.GetGroupInformation(igroup, group);

        /* Get the name of the group */
        str = group.GetName();
        str_length = str.size();
        cstr = Calloc(str_length+1, char);
        strncpy(cstr, str.c_str(), str_length);
        cstr[str_length] = '\0';
        SET_STRING_ELT(r_group_names, igroup, mkChar(cstr));
        Free(cstr);

        /* Get the number of cells (probes) in the group */
        int ncells = group.GetNumCells();
        PROTECT(isMm = NEW_LOGICAL(ncells));

        /* For each cell, record if it is an mis-match probe or not */
        for (int icell = 0; icell < ncells; icell++) {
                FusionCDFProbeInformation probe;
                group.GetCell(icell, probe);
          LOGICAL(isMm)[icell] = (probe.GetPBase() == probe.GetTBase());
        }

        SET_VECTOR_ELT(r_groups, igroup, isMm);

        UNPROTECT(1);
      }
      
      /** set the group names. **/
      setAttrib(r_groups, R_NamesSymbol, r_group_names);

      /** now set the probe_set in the main probe_set list. **/
      SET_VECTOR_ELT(probe_sets, ii, r_groups);

      /** pop the group list and group names of the stack. **/
      UNPROTECT(2);
    }
    
    /** set the names down here at the end. **/
    setAttrib(probe_sets, R_NamesSymbol, names);

    /** unprotect the names and the main probe set list.**/
    UNPROTECT(2);
   
    return probe_sets;
  } /* R_affx_cdf_isMm() */



  /************************************************************************
   *
   * R_affx_cdf_isPmOrMm()
   *
   ************************************************************************/
  SEXP R_affx_cdf_isPmOrMm(SEXP fname, SEXP units, SEXP verbose) 
  {
    FusionCDFData cdf;
    FusionCDFFileHeader header;
    string str;
    int str_length; 
    char* cstr; 
    
    SEXP names = R_NilValue, 
         probe_sets = R_NilValue,
         r_groups = R_NilValue, 
                     r_group_names = R_NilValue,
                     isPmOrMm = R_NilValue;
    
    bool readAll = true; 
    int nsets = 0, nunits = 0;
    int iset = 0;
    char* cdfFileName = CHAR(STRING_ELT(fname, 0));
    int i_verboseFlag = INTEGER(verbose)[0];


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
     * Read header and get unit indices to be read
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
        if (iset < 1 || iset > nsets) {
          error("Argument 'units' contains an element out of range.");
        }
      }
    }

    /* Allocate R character vector and R list for the names and units */
    PROTECT(probe_sets = NEW_LIST(nunits)); 
    PROTECT(names = NEW_CHARACTER(nunits));

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Access information for the requested units one by one
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    for (int ii = 0; ii < nunits; ii++) {
      FusionCDFProbeSetInformation probeset;

      if (readAll) {
        iset = ii;
      } else {
        /* Unit indices are zero-based in Fusion SDK. */
        iset = INTEGER(units)[ii] - 1;
      }

      /* Retrieve the current unit */
      cdf.GetProbeSetInformation(iset, probeset);
      
      /* Record its name */
      str = cdf.GetProbeSetName(iset);
      str_length = str.size();
      cstr = Calloc(str_length+1, char);
      strncpy(cstr, str.c_str(), str_length);
      cstr[str_length] = '\0';
      SET_STRING_ELT(names, ii, mkChar(cstr));
      Free(cstr);
      
      /* Get the number of groups in the unit */
      int ngroups = probeset.GetNumGroups();
      
      /* Allocate an R list and a vector of names for the groups */
      PROTECT(r_groups = NEW_LIST(ngroups));
      PROTECT(r_group_names = NEW_CHARACTER(ngroups));

      /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
       * For each group, query the number of cells (probes)
       * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
      for (int igroup = 0; igroup < ngroups; igroup++) {
        FusionCDFProbeGroupInformation group;

        /* Get the current group */
        probeset.GetGroupInformation(igroup, group);

        /* Get the name of the group */
        str = group.GetName();
        str_length = str.size();
        cstr = Calloc(str_length+1, char);
        strncpy(cstr, str.c_str(), str_length);
        cstr[str_length] = '\0';
        SET_STRING_ELT(r_group_names, igroup, mkChar(cstr));
        Free(cstr);

        /* Get the number of cells (probes) in the group */
        int ncells = group.GetNumCells();
        PROTECT(isPmOrMm = NEW_LOGICAL(ncells));

        /* For each cell, record if it is an mis-match probe or not */
        for (int icell = 0; icell < ncells; icell++) {
          FusionCDFProbeInformation probe;
          group.GetCell(icell, probe);
          char p_base = probe.GetPBase();
          char t_base = probe.GetTBase();
          if (p_base == t_base) {
            LOGICAL(isPmOrMm)[icell] = true;
          } else {
            if ((p_base == 'a' || p_base == 'A') && 
                (t_base == 't' || t_base == 'T'))   {
              LOGICAL(isPmOrMm)[icell] = true;
            } else if ((p_base == 't' || p_base == 'T') && 
                (t_base == 'a' || t_base == 'A'))   {
              LOGICAL(isPmOrMm)[icell] = true;
            } else if ((p_base == 'c' || p_base == 'C') && 
                (t_base == 'g' || t_base == 'G'))   {
              LOGICAL(isPmOrMm)[icell] = true;
            } else if ((p_base == 'g' || p_base == 'G') && 
                (t_base == 'c' || t_base == 'C'))   {
              LOGICAL(isPmOrMm)[icell] = true;
            } else {
              LOGICAL(isPmOrMm)[icell] = false;
            }
          }
        }

        SET_VECTOR_ELT(r_groups, igroup, isPmOrMm);

        UNPROTECT(1);
      }
      
      /** set the group names. **/
      setAttrib(r_groups, R_NamesSymbol, r_group_names);

      /** now set the probe_set in the main probe_set list. **/
      SET_VECTOR_ELT(probe_sets, ii, r_groups);

      /** pop the group list and group names of the stack. **/
      UNPROTECT(2);
    }
    
    /** set the names down here at the end. **/
    setAttrib(probe_sets, R_NamesSymbol, names);

    /** unprotect the names and the main probe set list.**/
    UNPROTECT(2);
   
    return probe_sets;
  } /* R_affx_cdf_isPmOrMm() */


} /** end extern C **/


/***************************************************************************
 * HISTORY:
 * 2006-11-27
 * o Made R_affx_cdf_isMm() & R_affx_cdf_isPmOrMm() deprecated, because
 *   they've never been used. /HB
 **************************************************************************/
