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

  SEXP R_affx_cdf_nbrOfCellsPerUnitGroup(SEXP fname, SEXP units, SEXP verbose) 
  {
    FusionCDFData cdf;
    FusionCDFFileHeader header;
    
    SEXP names = R_NilValue, 
         probe_sets = R_NilValue,
         r_groups = R_NilValue, 
         r_group_names = R_NilValue;
    
    bool readAll = true; 
    int nsets = 0, nunits = 0;
    int iset = 0;
    char* cdfFileName = CHAR(STRING_ELT(fname, 0));
    int i_verboseFlag = INTEGER(verbose)[0];

    /** pointer to the name of the probeset. **/
    const char* name;
    
    FusionCDFProbeSetInformation probeset;

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
        if (iset < 0 || iset >= nsets) {
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
        iset = INTEGER(units)[ii];
      }

      /* Retrieve the current unit */
      cdf.GetProbeSetInformation(iset, probeset);
      
      /* Record its name */
      name = cdf.GetProbeSetName(iset).c_str();
      SET_STRING_ELT(names, ii, mkChar(name));
      
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
        const char *groupName = group.GetName().c_str();
        SET_STRING_ELT(r_group_names, igroup, mkChar(groupName));

        /* Get the number of cells (probes) in the group */
        int ncells = group.GetNumCells();
        INTEGER(r_groups)[igroup] = ncells;
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
  }



  SEXP R_affx_cdf_groupNames(SEXP fname, SEXP units, SEXP verbose) 
  {
    FusionCDFData cdf;
    FusionCDFFileHeader header;
    
    SEXP names = R_NilValue, 
         probe_sets = R_NilValue,
         r_group_names = R_NilValue;
    
    bool readAll = true; 
    int nsets = 0, nunits = 0;
    int iset = 0;
    char* cdfFileName = CHAR(STRING_ELT(fname, 0));
    int i_verboseFlag = INTEGER(verbose)[0];

    /** pointer to the name of the probeset. **/
    const char* name;
    char str[512];

    FusionCDFProbeSetInformation probeset;

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
        if (iset < 0 || iset >= nsets) {
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
        iset = INTEGER(units)[ii];
      }

      /* Retrieve the current unit */
      cdf.GetProbeSetInformation(iset, probeset);
      
      /* Record its name */
      name = cdf.GetProbeSetName(iset).c_str();
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
        const char *groupName = group.GetName().c_str();
 
        /* If group name starts with the unit name, strip it off. */
        int len = strlen(name);
        int res = strncmp(groupName, name, len);
        if (res == 0) {
          int last = strlen(groupName)-len;
          for (int kk = 0; kk < last; kk++)
            str[kk] = groupName[len+kk];
          str[last] = '\0';
          SET_STRING_ELT(r_group_names, igroup, mkChar(str));
        } else {
          SET_STRING_ELT(r_group_names, igroup, mkChar(groupName));
        }
      }
      
      /** now set the probe_set in the main probe_set list. **/
      SET_VECTOR_ELT(probe_sets, ii, r_group_names);

      /** pop the group list and group names of the stack. **/
      UNPROTECT(1);
    }
    
    /** set the names down here at the end. **/
    setAttrib(probe_sets, R_NamesSymbol, names);

    /** unprotect the names and the main probe set list.**/
    UNPROTECT(2);
   
    return probe_sets;
  } /* R_affx_cdf_groupNames() */



  SEXP R_affx_cdf_isPm(SEXP fname, SEXP units, SEXP verbose) 
  {
    FusionCDFData cdf;
    FusionCDFFileHeader header;
    
    SEXP names = R_NilValue, 
         probe_sets = R_NilValue,
         r_groups = R_NilValue, 
                     r_group_names = R_NilValue,
                     isPm = R_NilValue;
    
    bool readAll = true; 
    int nsets = 0, nunits = 0;
    int iset = 0;
    char* cdfFileName = CHAR(STRING_ELT(fname, 0));
    int i_verboseFlag = INTEGER(verbose)[0];

    /** pointer to the name of the probeset. **/
    const char* name;

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
        if (iset < 0 || iset >= nsets) {
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
      FusionCDFProbeSetInformation probeset;

      if (readAll) {
        iset = ii;
      } else {
        iset = INTEGER(units)[ii];
      }

      /* Retrieve the current unit */
      cdf.GetProbeSetInformation(iset, probeset);
      
      /* Record its name */
      name = cdf.GetProbeSetName(iset).c_str();
      SET_STRING_ELT(names, ii, mkChar(name));
      
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
        const char *groupName = group.GetName().c_str();
        SET_STRING_ELT(r_group_names, igroup, mkChar(groupName));

        /* Get the number of cells (probes) in the group */
        int ncells = group.GetNumCells();
        PROTECT(isPm = NEW_LOGICAL(ncells));

        /* For each cell, record if it is an mis-match probe or not */
        for (int icell = 0; icell < ncells; icell++) {
                FusionCDFProbeInformation probe;
                group.GetCell(icell, probe);
          char p_base = probe.GetPBase();
          char t_base = probe.GetTBase();
          if (p_base == t_base) {
            LOGICAL(isPm)[icell] = false;
          } else {
            if ((p_base == 'a' || p_base == 'A') && 
                (t_base == 't' || t_base == 'T'))   {
              LOGICAL(isPm)[icell] = true;
            } else if ((p_base == 't' || p_base == 'T') && 
                (t_base == 'a' || t_base == 'A'))   {
              LOGICAL(isPm)[icell] = true;
            } else if ((p_base == 'c' || p_base == 'C') && 
                (t_base == 'g' || t_base == 'G'))   {
              LOGICAL(isPm)[icell] = true;
            } else if ((p_base == 'g' || p_base == 'G') && 
                (t_base == 'c' || t_base == 'C'))   {
              LOGICAL(isPm)[icell] = true;
            } else {
              LOGICAL(isPm)[icell] = false;
            }
          }
        }

        SET_VECTOR_ELT(r_groups, igroup, isPm);

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
  } /* R_affx_cdf_isPm() */




  SEXP R_affx_cdf_isMm(SEXP fname, SEXP units, SEXP verbose) 
  {
    FusionCDFData cdf;
    FusionCDFFileHeader header;
    
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

    /** pointer to the name of the probeset. **/
    const char* name;

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
        if (iset < 0 || iset >= nsets) {
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
      FusionCDFProbeSetInformation probeset;

      if (readAll) {
        iset = ii;
      } else {
        iset = INTEGER(units)[ii];
      }

      /* Retrieve the current unit */
      cdf.GetProbeSetInformation(iset, probeset);
      
      /* Record its name */
      name = cdf.GetProbeSetName(iset).c_str();
      SET_STRING_ELT(names, ii, mkChar(name));
      
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
        const char *groupName = group.GetName().c_str();
        SET_STRING_ELT(r_group_names, igroup, mkChar(groupName));

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



  SEXP R_affx_cdf_isPmOrMm(SEXP fname, SEXP units, SEXP verbose) 
  {
    FusionCDFData cdf;
    FusionCDFFileHeader header;
    
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

    /** pointer to the name of the probeset. **/
    const char* name;

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
        if (iset < 0 || iset >= nsets) {
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
      FusionCDFProbeSetInformation probeset;

      if (readAll) {
        iset = ii;
      } else {
        iset = INTEGER(units)[ii];
      }

      /* Retrieve the current unit */
      cdf.GetProbeSetInformation(iset, probeset);
      
      /* Record its name */
      name = cdf.GetProbeSetName(iset).c_str();
      SET_STRING_ELT(names, ii, mkChar(name));
      
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
        const char *groupName = group.GetName().c_str();
        SET_STRING_ELT(r_group_names, igroup, mkChar(groupName));

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