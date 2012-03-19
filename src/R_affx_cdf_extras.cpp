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
   * R_affx_cdf_nbrOfCellsPerUnitGroup()
   *
   ************************************************************************/
  SEXP R_affx_cdf_nbrOfCellsPerUnitGroup(SEXP fname, SEXP units, SEXP verbose) 
  {
    FusionCDFData cdf;
    FusionCDFFileHeader header;
    string str;
    int str_length; 
    char* cstr; 
    
    SEXP names = R_NilValue, 
         probe_sets = R_NilValue,
         r_groups = R_NilValue, 
         r_group_names = R_NilValue;
    
    bool readAll = true; 
    int nsets = 0, nunits = 0;
    int iset = 0;
    const char* cdfFileName = CHAR(STRING_ELT(fname, 0));
    int i_verboseFlag = INTEGER(verbose)[0];

    FusionCDFProbeSetInformation probeset;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Opens file
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    cdf.SetFileName(cdfFileName);
    if (i_verboseFlag >= R_AFFX_VERBOSE) {
      Rprintf("Attempting to read CDF File: %s\n", cdf.GetFileName().c_str());
    }

    if (cdf.Read() == false) {
      error("Failed to read the CDF file.");
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
    PROTECT(names = NEW_CHARACTER(nunits));
    PROTECT(probe_sets = NEW_LIST(nunits)); 

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Access information for the requested units one by one
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    for (int ii = 0; ii < nunits; ii++) {
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
      PROTECT(r_groups = NEW_INTEGER(ngroups));
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
        INTEGER(r_groups)[igroup] = ncells;
      }
      
      /** set the group names. **/
      setAttrib(r_groups, R_NamesSymbol, r_group_names);

      /** now set the probe_set in the main probe_set list. **/
      SET_VECTOR_ELT(probe_sets, ii, r_groups);

      /** pop the group list and group names of the stack. **/
      UNPROTECT(2); /* 'r_group_names' and then 'r_groups' */
    }
    
    /** set the names down here at the end. **/
    setAttrib(probe_sets, R_NamesSymbol, names);

    /** unprotect the names and the main probe set list.**/
    UNPROTECT(2);  /* 'names' and then 'probe_sets' */
   
    return probe_sets;
  }



  /************************************************************************
   *
   * R_affx_cdf_groupNames()
   *
   ************************************************************************/
  SEXP R_affx_cdf_groupNames(SEXP fname, SEXP units, SEXP truncateGroupNames, SEXP verbose) 
  {
    FusionCDFData cdf;
    FusionCDFFileHeader header;
    string str;
    int str_length; 
    char* cstr; 
    
    SEXP names = R_NilValue, 
         probe_sets = R_NilValue,
         r_group_names = R_NilValue;
    
    bool readAll = true; 
    int nsets = 0, nunits = 0;
    int iset = 0;
    const char* cdfFileName = CHAR(STRING_ELT(fname, 0));
    int i_truncateGroupNames = INTEGER(truncateGroupNames)[0];
    int i_verboseFlag = INTEGER(verbose)[0];

    /** pointer to the name of the probeset. **/
    char* name;
    char bfr[512];

    FusionCDFProbeSetInformation probeset;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Opens file
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    cdf.SetFileName(cdfFileName);
    if (i_verboseFlag >= R_AFFX_VERBOSE) {
      Rprintf("Attempting to read CDF File: %s\n", cdf.GetFileName().c_str());
    }

    if (cdf.Read() == false) {
      error("Failed to read the CDF file.");
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
      name = Calloc(str_length+1, char);
      strncpy(name, str.c_str(), str_length);
      name[str_length] = '\0';
      SET_STRING_ELT(names, ii, mkChar(name));
      
      /* Get the number of groups in the unit */
      int ngroups = probeset.GetNumGroups();
      
      /* Allocate a vector of names for the groups */
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
 
        /* If group name starts with the unit name, and i_truncateGroupNames
           is TRUE, strip it off. */
        int len = strlen(name);
        int res = strncmp(cstr, name, len);
        if (res == 0 && i_truncateGroupNames) {
          int last = strlen(cstr)-len;
          for (int kk = 0; kk < last; kk++)
            bfr[kk] = cstr[len+kk];
          bfr[last] = '\0';
          SET_STRING_ELT(r_group_names, igroup, mkChar(bfr));
        } else {
          SET_STRING_ELT(r_group_names, igroup, mkChar(cstr));
        }
        Free(cstr);
      }
      
      /** now set the probe_set in the main probe_set list. **/
      SET_VECTOR_ELT(probe_sets, ii, r_group_names);

      /** pop the group list and group names of the stack. **/
      UNPROTECT(1);  /* 'r_group_names' */
      Free(name);
    } /* for (int ii=0 ...) */
    
    /** set the names down here at the end. **/
    setAttrib(probe_sets, R_NamesSymbol, names);

    /** unprotect the names and the main probe set list.**/
    UNPROTECT(2);  /* 'names' and then 'probe_sets' */
   
    return probe_sets;
  } /* R_affx_cdf_groupNames() */



  /************************************************************************
   *
   * R_affx_pt_base_is_pm
   *
   * Return 1 if p_base/t_base pair is PM (complementary), 0 otherwise.
   *
   ************************************************************************/
  int R_affx_pt_base_is_pm(char p_base, char t_base)
  {
    int is_pm = 0; /* 0 is false, 1 is true */
    if (p_base == t_base) {
      is_pm = 0;
    } else {
      if ((p_base == 'a' || p_base == 'A') &&
          (t_base == 't' || t_base == 'T'))   {
        is_pm = 1;
      } else if ((p_base == 't' || p_base == 'T') &&
                 (t_base == 'a' || t_base == 'A'))   {
        is_pm = 1;
      } else if ((p_base == 'c' || p_base == 'C') &&
                 (t_base == 'g' || t_base == 'G'))   {
        is_pm = 1;
      } else if ((p_base == 'g' || p_base == 'G') &&
                 (t_base == 'c' || t_base == 'C'))   {
        is_pm = 1;
      } else {
        is_pm = 0;
      }
    }
    return is_pm;
  } /* R_affx_pt_base_is_pm() */


  /************************************************************************
   *
   * R_affx_cdf_isPm()
   *
   ************************************************************************/
  SEXP R_affx_cdf_isPm(SEXP fname, SEXP units, SEXP verbose) 
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
         isPm = R_NilValue;
    
    bool readAll = true; 
    int nsets = 0, nunits = 0;
    int iset = 0;
    const char* cdfFileName = CHAR(STRING_ELT(fname, 0));
    int i_verboseFlag = INTEGER(verbose)[0];

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Opens file
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    cdf.SetFileName(cdfFileName);
    if (i_verboseFlag >= R_AFFX_VERBOSE) {
      Rprintf("Attempting to read CDF File: %s\n", cdf.GetFileName().c_str());
    }

    if (cdf.Read() == false) {
      error("Failed to read the CDF file.");
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
        PROTECT(isPm = NEW_LOGICAL(ncells));

        /* For each cell, record if it is an mis-match probe or not */
        for (int icell = 0; icell < ncells; icell++) {
          FusionCDFProbeInformation probe;
          group.GetCell(icell, probe);
          char p_base = probe.GetPBase();
          char t_base = probe.GetTBase();
          LOGICAL(isPm)[icell] = R_affx_pt_base_is_pm(p_base, t_base);
        } /* for (int icell ...) */

        SET_VECTOR_ELT(r_groups, igroup, isPm);

        UNPROTECT(1);  /* 'isPm' */
      }
      
      /** set the group names. **/
      setAttrib(r_groups, R_NamesSymbol, r_group_names);

      /** now set the probe_set in the main probe_set list. **/
      SET_VECTOR_ELT(probe_sets, ii, r_groups);

      /** pop the group list and group names of the stack. **/
      UNPROTECT(2); /* 'r_group_names' and then 'r_groups' */
    }
    
    /** set the names down here at the end. **/
    setAttrib(probe_sets, R_NamesSymbol, names);

    /** unprotect the names and the main probe set list.**/
    UNPROTECT(2);  /* 'names' and then 'probe_sets' */
   
    return probe_sets;
  } /* R_affx_cdf_isPm() */

} /** end extern C **/


/***************************************************************************
 * HISTORY:
 * 2007-03-05 
 * o Added argument 'truncateGroupNames' to R_affx_cdf_group_names().
 * 2006-11-27
 * o Added Seth Falcon's help function R_affx_pt_base_is_pm().
 * o Made R_affx_cdf_isMm() & R_affx_cdf_isPmOrMm() deprecated, because
 *   they've never been used. /HB
 * 2006-03-28
 * o Unit indices are now one-based. /HB
 * 2006-01-15
 * 2006-01-15
 * o It is now possible to specify what readCdfUnits() should return. /HB
 * 2006-01-12
 * o BUG FIX: The check of the upper-limit of unit indicies was done 
 *   assuming one-based indices. /HB
 * o Added R_affx_cdf_isMm(), R_affx_cdf_isPm() & R_affx_cdf_isPmOrMm(). /HB
 * 2006-01-11
 * o Added R_affx_cdf_nbrOfCellsPerUnitGroup() /HB
 * 2006-01-10
 * o Updated the "units" code to be more similar to the corresponding code
 *   for CEL files. /HB
 * o Added a return value to non-void function R_affx_get_cdf_file_qc(). /HB
 * 2006-01-09
 * o Added R_affx_get_cdf_units() and R_affx_get_cdf_unit.names(). /HB
 * o Created.  The purpose was to make it possible to read subsets of units
 *   and not just all units at once. /HB
 **************************************************************************/
