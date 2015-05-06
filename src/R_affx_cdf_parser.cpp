#include "FusionCDFData.h"
#include <iostream>
#include "R_affx_constants.h"
#include "R_affx_cdf_extras.h"

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
    int nRows = 0;
    const char* cdfFileName = CHAR(STRING_ELT(fname, 0));
    int i_verboseFlag = INTEGER(verbose)[0];
    string str;
    int str_length; 
    char* cstr; 

    cdf.SetFileName(cdfFileName);

    if (i_verboseFlag >= R_AFFX_VERBOSE) {
      Rprintf("Attempting to read CDF File: %s\n", cdf.GetFileName().c_str());
    }

    if (cdf.Read() == false) {
      error("Failed to read the CDF file.");
    }

    header = cdf.GetHeader();
    int nsets = header.GetNumProbeSets();

    PROTECT(pmmm  = NEW_LIST(nsets)); 
    PROTECT(names = NEW_CHARACTER(nsets));

    nRows = header.GetRows();

    for (int iset = 0; iset < nsets; iset++) {
      str = cdf.GetProbeSetName(iset);
      str_length = str.size();
      cstr = Calloc(str_length+1, char);
      strncpy(cstr, str.c_str(), str_length);
      cstr[str_length] = '\0';
      SET_STRING_ELT(names, iset, mkChar(cstr));
      if (i_verboseFlag >= R_AFFX_VERBOSE) {
        Rprintf("Processing probeset: %s\n", cstr);
      }
      Free(cstr);

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
    SEXP R_affx_get_cdf_file_qc(SEXP fname, SEXP unitIndices, SEXP verbose,
                                SEXP returnIndices, SEXP returnXY,
                                SEXP returnLengths,
                                SEXP returnPMInfo, SEXP returnBackgroundInfo,
                                SEXP returnType, SEXP returnQCNumbers) 
  {
    FusionCDFData cdf;
    string str;
    
    SEXP
        r_qcunits_list = R_NilValue,
        r_qcunit = R_NilValue,
        r_qcunit_names = R_NilValue,
        r_indices = R_NilValue,
        r_xvals = R_NilValue,
        r_yvals = R_NilValue,
        r_lengths = R_NilValue,
        r_pminfo = R_NilValue,
        r_backgroundinfo = R_NilValue;

    int numQCUnits = 0;
    int nqccells = 0;
    int numCols = 0;
    int numQCUnitsInFile = 0; 
    const char* cdfFileName = CHAR(STRING_ELT(fname, 0));
    int i_verboseFlag = INTEGER(verbose)[0];
    int i_returnIndices = INTEGER(returnIndices)[0];
    int i_returnXY = INTEGER(returnXY)[0];
    int i_returnLengths = INTEGER(returnLengths)[0];
    int i_returnPMInfo = INTEGER(returnPMInfo)[0];
    int i_returnBackgroundInfo = INTEGER(returnBackgroundInfo)[0];
    int i_returnType = INTEGER(returnType)[0];
    int i_returnQCNumbers = INTEGER(returnQCNumbers)[0];
    bool readEveryUnit = true;
    int ii = 0;
    int qcunit_idx;

    cdf.SetFileName(cdfFileName);
    if (i_verboseFlag >= R_AFFX_VERBOSE) {
      Rprintf("Attempting to read CDF File: %s\n", cdf.GetFileName().c_str());
    }

    if (cdf.Read() == false) {
      error("Failed to read the CDF file.");
    }

    numQCUnitsInFile  = cdf.GetHeader().GetNumQCProbeSets();
    numCols = cdf.GetHeader().GetCols();
    if(length(unitIndices) != 0){
        for (int i = 0; i < length(unitIndices); i++) {
            if(INTEGER(unitIndices)[i] < 1 || 
               INTEGER(unitIndices)[i] > numQCUnitsInFile)
	      error("Argument 'units' contains an element out of range [%d,%d]: %d", 1, numQCUnitsInFile, INTEGER(unitIndices)[i]);
        }
        numQCUnits = length(unitIndices);
        readEveryUnit = false;
    } else {
        numQCUnits = numQCUnitsInFile;
        readEveryUnit = true;
    }

    /*
    ** Now we set up stuff for the different objects we need to return
    */

    FusionCDFQCProbeSetInformation qcunit;
    FusionCDFQCProbeInformation qcprobe;

    /*
    ** First the units themselves
    */

    int numCellArguments =  i_returnIndices + 2 * i_returnXY +
        i_returnLengths + i_returnPMInfo + i_returnBackgroundInfo;

    int numQCUnitArguments = numCellArguments + 
        i_returnType + i_returnQCNumbers;

    PROTECT(r_qcunit_names = NEW_CHARACTER(numQCUnitArguments));

    ii = 0;
    if(i_returnXY) {
        SET_STRING_ELT(r_qcunit_names, ii++, mkChar("x"));
        SET_STRING_ELT(r_qcunit_names, ii++, mkChar("y"));
    }
    if(i_returnIndices) {
        SET_STRING_ELT(r_qcunit_names, ii++, mkChar("indices"));
    }
    if(i_returnLengths) {
        SET_STRING_ELT(r_qcunit_names, ii++, mkChar("length"));
    }
    if(i_returnPMInfo) {
        SET_STRING_ELT(r_qcunit_names, ii++, mkChar("pm"));
    }
    if(i_returnBackgroundInfo) {
        SET_STRING_ELT(r_qcunit_names, ii++, mkChar("background"));
    }
    if(i_returnType) {
        SET_STRING_ELT(r_qcunit_names, ii++, mkChar("type"));
    }
    if(i_returnQCNumbers) {
        SET_STRING_ELT(r_qcunit_names, ii++, mkChar("ncells"));
    }

    /*
    ** Now fixing up the QC type
    */

    SEXP r_qcTypeAsString;
    PROTECT(r_qcTypeAsString = NEW_CHARACTER(19));
    SET_STRING_ELT(r_qcTypeAsString, 0, mkChar("unknown"));
    SET_STRING_ELT(r_qcTypeAsString, 1, mkChar("checkerboardNegative"));
    SET_STRING_ELT(r_qcTypeAsString, 2, mkChar("checkerboardPositive"));
    SET_STRING_ELT(r_qcTypeAsString, 3, mkChar("hybeNegative"));
    SET_STRING_ELT(r_qcTypeAsString, 4, mkChar("hybePositive"));
    SET_STRING_ELT(r_qcTypeAsString, 5, mkChar("textFeaturesNegative"));
    SET_STRING_ELT(r_qcTypeAsString, 6, mkChar("textFeaturesPositive"));
    SET_STRING_ELT(r_qcTypeAsString, 7, mkChar("centralNegative"));
    SET_STRING_ELT(r_qcTypeAsString, 8, mkChar("centralPositive"));
    SET_STRING_ELT(r_qcTypeAsString, 9, mkChar("geneExpNegative"));
    SET_STRING_ELT(r_qcTypeAsString, 10, mkChar("geneExpPositive"));
    SET_STRING_ELT(r_qcTypeAsString, 11, mkChar("cycleFidelityNegative"));
    SET_STRING_ELT(r_qcTypeAsString, 12, mkChar("cycleFidelityPositive"));
    SET_STRING_ELT(r_qcTypeAsString, 13, mkChar("centralCrossNegative"));
    SET_STRING_ELT(r_qcTypeAsString, 14, mkChar("centralCrossPositive"));
    SET_STRING_ELT(r_qcTypeAsString, 15, mkChar("crossHybeNegative"));
    SET_STRING_ELT(r_qcTypeAsString, 16, mkChar("crossHybePositive"));
    SET_STRING_ELT(r_qcTypeAsString, 17, mkChar("SpatialNormNegative"));
    SET_STRING_ELT(r_qcTypeAsString, 18, mkChar("SpatialNormPositive"));

    PROTECT(r_qcunits_list = NEW_LIST(numQCUnits));

    if (i_verboseFlag >= R_AFFX_REALLY_VERBOSE) {
        Rprintf("Finished setup, commencing unit parsing.\n");
    }

    /*
    ** Parsing the QCUnits
    */


    for (int iqcunit = 0; iqcunit < numQCUnits; iqcunit++) {
      /* Check for interrupts */
      if(iqcunit % 1000 == 999) 
          R_CheckUserInterrupt();
      /* The index, which is 0-based in Fusion unlike our R-api */
      if(readEveryUnit)
          qcunit_idx = iqcunit;
      else
          qcunit_idx = INTEGER(unitIndices)[iqcunit] - 1;
      cdf.GetQCProbeSetInformation(qcunit_idx, qcunit);

      PROTECT(r_qcunit = NEW_LIST(numQCUnitArguments));

      if (i_verboseFlag >= R_AFFX_VERBOSE) {
          Rprintf("Processing QC unit: %d\n", qcunit_idx + 1);
      }

      nqccells = qcunit.GetNumCells();

      if(numCellArguments > 0) {
          if(i_returnXY) {
              PROTECT(r_xvals = NEW_INTEGER(nqccells));
              PROTECT(r_yvals = NEW_INTEGER(nqccells));
          }
          if(i_returnIndices)
              PROTECT(r_indices = NEW_INTEGER(nqccells));
          if(i_returnLengths)
              PROTECT(r_lengths = NEW_INTEGER(nqccells));
          if(i_returnPMInfo)
              PROTECT(r_pminfo = NEW_LOGICAL(nqccells));
          if(i_returnBackgroundInfo)
              PROTECT(r_backgroundinfo = NEW_LOGICAL(nqccells));

          for (int icell = 0; icell < nqccells; icell++) {
              qcunit.GetProbeInformation(icell, qcprobe);
              if(i_returnXY) {
                  INTEGER(r_xvals)[icell] = qcprobe.GetX();
                  INTEGER(r_yvals)[icell] = qcprobe.GetY();
              }
              if(i_returnIndices) {
                  INTEGER(r_indices)[icell] = qcprobe.GetY() * numCols +
                      qcprobe.GetX() + 1;
              }
              if(i_returnLengths) {
                  INTEGER(r_lengths)[icell] = qcprobe.GetPLen();
              }
              if(i_returnPMInfo) {
                  LOGICAL(r_pminfo)[icell] = 
                      qcprobe.IsPerfectMatchProbe();
              }
              if(i_returnBackgroundInfo) {
                  LOGICAL(r_backgroundinfo)[icell] = 
                      qcprobe.IsBackgroundProbe();
              }
          }

          ii = 0;
          if(i_returnXY) {
              SET_VECTOR_ELT(r_qcunit, ii++, r_xvals);
              SET_VECTOR_ELT(r_qcunit, ii++, r_yvals);
          }
          if(i_returnIndices) {
              SET_VECTOR_ELT(r_qcunit, ii++, r_indices);
          }
          if(i_returnLengths) {
              SET_VECTOR_ELT(r_qcunit, ii++, r_lengths);
          }
          if(i_returnPMInfo) {
              SET_VECTOR_ELT(r_qcunit, ii++, r_pminfo);
          }
          if(i_returnBackgroundInfo) {
              SET_VECTOR_ELT(r_qcunit, ii++, r_backgroundinfo);
          }
          UNPROTECT(numCellArguments);
      }
      
      if(i_returnType) {
          switch (qcunit.GetQCProbeSetType()) {
          case affxcdf::UnknownQCProbeSetType:
              SET_VECTOR_ELT(r_qcunit, ii++,
                             ScalarString(STRING_ELT(r_qcTypeAsString, 0)));
              break;
          case affxcdf::CheckerboardNegativeQCProbeSetType:
              SET_VECTOR_ELT(r_qcunit, ii++,
                             ScalarString(STRING_ELT(r_qcTypeAsString, 1)));
              break;
          case affxcdf::CheckerboardPositiveQCProbeSetType:
              SET_VECTOR_ELT(r_qcunit, ii++,
                             ScalarString(STRING_ELT(r_qcTypeAsString, 2)));
              break;
          case affxcdf::HybNegativeQCProbeSetType:
              SET_VECTOR_ELT(r_qcunit, ii++,
                             ScalarString(STRING_ELT(r_qcTypeAsString, 3)));
              break;
          case affxcdf::HybPositiveQCProbeSetType:
              SET_VECTOR_ELT(r_qcunit, ii++,
                             ScalarString(STRING_ELT(r_qcTypeAsString, 4)));
              break;
          case affxcdf::TextFeaturesNegativeQCProbeSetType:
              SET_VECTOR_ELT(r_qcunit, ii++,
                             ScalarString(STRING_ELT(r_qcTypeAsString, 5)));
              break;
          case affxcdf::TextFeaturesPositiveQCProbeSetType:
              SET_VECTOR_ELT(r_qcunit, ii++,
                             ScalarString(STRING_ELT(r_qcTypeAsString, 6)));
              break;
          case affxcdf::CentralNegativeQCProbeSetType:
              SET_VECTOR_ELT(r_qcunit, ii++,
                             ScalarString(STRING_ELT(r_qcTypeAsString, 7)));
              break;
          case affxcdf::CentralPositiveQCProbeSetType:
              SET_VECTOR_ELT(r_qcunit, ii++,
                             ScalarString(STRING_ELT(r_qcTypeAsString, 8)));
              break;
          case affxcdf::GeneExpNegativeQCProbeSetType:
              SET_VECTOR_ELT(r_qcunit, ii++,
                             ScalarString(STRING_ELT(r_qcTypeAsString, 9)));
              break;
          case affxcdf::GeneExpPositiveQCProbeSetType:
              SET_VECTOR_ELT(r_qcunit, ii++,
                             ScalarString(STRING_ELT(r_qcTypeAsString, 10)));
              break;
          case affxcdf::CycleFidelityNegativeQCProbeSetType:
              SET_VECTOR_ELT(r_qcunit, ii++,
                             ScalarString(STRING_ELT(r_qcTypeAsString, 11)));
              break;
          case affxcdf::CycleFidelityPositiveQCProbeSetType:
              SET_VECTOR_ELT(r_qcunit, ii++,
                             ScalarString(STRING_ELT(r_qcTypeAsString, 12)));
              break;
          case affxcdf::CentralCrossNegativeQCProbeSetType:
              SET_VECTOR_ELT(r_qcunit, ii++,
                             ScalarString(STRING_ELT(r_qcTypeAsString, 13)));
              break;
          case affxcdf::CentralCrossPositiveQCProbeSetType:
              SET_VECTOR_ELT(r_qcunit, ii++,
                             ScalarString(STRING_ELT(r_qcTypeAsString, 14)));
              break;
          case affxcdf::CrossHybNegativeQCProbeSetType:
              SET_VECTOR_ELT(r_qcunit, ii++,
                             ScalarString(STRING_ELT(r_qcTypeAsString, 15)));
              break;
          case affxcdf::CrossHybPositiveQCProbeSetType:
              SET_VECTOR_ELT(r_qcunit, ii++,
                             ScalarString(STRING_ELT(r_qcTypeAsString, 16)));
              break;
          case affxcdf::SpatialNormalizationNegativeQCProbeSetType:
              SET_VECTOR_ELT(r_qcunit, ii++,
                             ScalarString(STRING_ELT(r_qcTypeAsString, 17)));
              break;
          case affxcdf::SpatialNormalizationPositiveQCProbeSetType:
              SET_VECTOR_ELT(r_qcunit, ii++,
                             ScalarString(STRING_ELT(r_qcTypeAsString, 18)));
              break;
          default:
              SET_VECTOR_ELT(r_qcunit, ii++, 
                             ScalarString(STRING_ELT(r_qcTypeAsString, 0)));
              break;
          }
      }

      if(i_returnQCNumbers) {
          SET_VECTOR_ELT(r_qcunit, ii++, ScalarInteger(nqccells));
      }
      
      setAttrib(r_qcunit, R_NamesSymbol, r_qcunit_names);
      SET_VECTOR_ELT(r_qcunits_list, iqcunit, r_qcunit);
      UNPROTECT(1);
    }

    /* r_qcunit_names, r_qcunits_list, r_qcTypeAsString */
    UNPROTECT(3);
 
    return r_qcunits_list;
  }



  /************************************************************************
   *
   * R_affx_get_cdf_file_header()
   *
   ************************************************************************/
  SEXP R_affx_get_cdf_file_header(SEXP fname)
  {
    FusionCDFData cdf;
    const char* cdfFileName = CHAR(STRING_ELT(fname, 0));
    string str;
    int str_length; 
    char* cstr; 
    
    cdf.SetFileName(cdfFileName);
   
    if (cdf.ReadHeader() == false) {
      error("Failed to read the CDF file header for: %s\n", cdfFileName);
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

    SET_STRING_ELT(names, ii, mkChar("ncols"));
    PROTECT(tmp = allocVector(INTSXP, 1));
    INTEGER(tmp)[0] = header.GetCols();
    SET_VECTOR_ELT(vals, ii++, tmp); 
    UNPROTECT(1);
    
    SET_STRING_ELT(names, ii, mkChar("nrows"));
    PROTECT(tmp = allocVector(INTSXP, 1));
    INTEGER(tmp)[0] = header.GetRows();
    SET_VECTOR_ELT(vals, ii++, tmp); 
    UNPROTECT(1);
    
    SET_STRING_ELT(names, ii, mkChar("nunits"));
    PROTECT(tmp = allocVector(INTSXP, 1));
    INTEGER(tmp)[0] = header.GetNumProbeSets();
    SET_VECTOR_ELT(vals, ii++, tmp); 
    UNPROTECT(1);
    
    SET_STRING_ELT(names, ii, mkChar("nqcunits"));
    PROTECT(tmp = allocVector(INTSXP, 1));
    INTEGER(tmp)[0] = header.GetNumQCProbeSets();
    SET_VECTOR_ELT(vals, ii++, tmp); 
    UNPROTECT(1);
    
    SET_STRING_ELT(names, ii, mkChar("refseq"));
    str = header.GetReference();
    str_length = str.size();
    cstr = Calloc(str_length+1, char);
    strncpy(cstr, str.c_str(), str_length);
    cstr[str_length] = '\0';
    SET_VECTOR_ELT(vals, ii++, mkString(cstr));
    Free(cstr);

    SET_STRING_ELT(names, ii, mkChar("chiptype"));
    str = cdf.GetChipType();
    str_length = str.size();
    cstr = Calloc(str_length+1, char);
    strncpy(cstr, str.c_str(), str_length);
    cstr[str_length] = '\0';
    SET_VECTOR_ELT(vals, ii++, mkString(cstr));
    Free(cstr);

    SET_STRING_ELT(names, ii, mkChar("filename"));
    str = cdf.GetFileName();
    str_length = str.size();
    cstr = Calloc(str_length+1, char);
    strncpy(cstr, str.c_str(), str_length);
    cstr[str_length] = '\0';
    SET_VECTOR_ELT(vals, ii++, mkString(cstr));
    Free(cstr);

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
   * Currently being updated....
   *
   ************************************************************************/

    SEXP R_affx_get_cdf_file(SEXP fname, SEXP unitIndices, SEXP verbose,
                             SEXP returnUnitType, 
                             SEXP returnUnitDirection,
                             SEXP returnUnitAtomNumbers,
                             SEXP returnUnitNumber,
                             SEXP returnXY, SEXP returnIndices,
                             SEXP returnBases, SEXP returnAtoms,
                             SEXP returnIndexpos, SEXP returnIsPm,
                             SEXP returnBlockDirection,
                             SEXP returnBlockAtomNumbers)
  {
    FusionCDFData cdf;
    string str;
    int str_length; 
    char* cstr; 
    char p_base, t_base;

    SEXP
        r_units_list = R_NilValue,
        r_units_list_names = R_NilValue,
        r_unit = R_NilValue,
        r_unit_names = R_NilValue,
        r_blocks_list = R_NilValue, 
        r_blocks_list_names = R_NilValue, 
        r_block = R_NilValue,
        r_block_names = R_NilValue,
        r_xvals = R_NilValue,
        r_yvals = R_NilValue,
        r_indices = R_NilValue,
        r_pbase = R_NilValue,
        r_tbase = R_NilValue,
        r_expos = R_NilValue,
        r_indexpos  = R_NilValue,
        r_ispm = R_NilValue;
    
    /*
    ** First we read in the cdf files and get the number of units.
    */

    int numUnits = 0;
    int numCols = 0;
    int numUnitsInFile = 0; 
    const char* cdfFileName = CHAR(STRING_ELT(fname, 0));
    int i_verboseFlag = INTEGER(verbose)[0];
    int i_returnUnitType = INTEGER(returnUnitType)[0];
    int i_returnUnitDirection = INTEGER(returnUnitDirection)[0];
    int i_returnUnitAtomNumbers = INTEGER(returnUnitAtomNumbers)[0];
    int i_returnUnitNumber = INTEGER(returnUnitAtomNumbers)[0];
    int i_returnXY = INTEGER(returnXY)[0];
    int i_returnIndices = INTEGER(returnIndices)[0];
    int i_returnIsPm = INTEGER(returnIsPm)[0];
    int i_returnBases = INTEGER(returnBases)[0];
    int i_returnAtoms = INTEGER(returnAtoms)[0];
    int i_returnIndexpos = INTEGER(returnIndexpos)[0];
    int i_returnBlockDirection = INTEGER(returnBlockDirection)[0];
    int i_returnBlockAtomNumbers = INTEGER(returnBlockAtomNumbers)[0];
    bool readEveryUnit = true;

    cdf.SetFileName(cdfFileName);
    if (i_verboseFlag >= R_AFFX_VERBOSE) {
      Rprintf("Attempting to read CDF File: %s\n", cdf.GetFileName().c_str());
    }

    if (cdf.Read() == false) {
      error("Failed to read the CDF file.\n");
    }

    numUnitsInFile  = cdf.GetHeader().GetNumProbeSets();
    numCols = cdf.GetHeader().GetCols();
    if(length(unitIndices) != 0){
        for (int i = 0; i < length(unitIndices); i++) {
            if(INTEGER(unitIndices)[i] < 1 || 
               INTEGER(unitIndices)[i] > numUnitsInFile)
                error("Argument 'units' contains an element out of range [%d,%d]: %d", 1, numUnitsInFile, INTEGER(unitIndices)[i]);
        }
        numUnits = length(unitIndices);
        readEveryUnit = false;
    } else {
        numUnits = numUnitsInFile;
        readEveryUnit = true;
    }

    /*
    ** Now we set up stuff for the different objects we need to return
    */

    FusionCDFProbeSetInformation unit;
    FusionCDFProbeGroupInformation block;
    FusionCDFProbeInformation probe;
    int unitNumBlocks;
    int blockNumCells;
    char *unitName;
    const char *blockName;
    string pbaseString, tbaseString;
    int ii, unit_idx, 
        unprotectBlockInfo, numBlockArguments, 
        numUnitArguments;

    PROTECT(r_units_list_names = NEW_CHARACTER(numUnits));
    PROTECT(r_units_list = NEW_LIST(numUnits));

    /*
    ** First the units themselves
    */
 
    numUnitArguments = i_returnUnitType + 
        i_returnUnitDirection + 3 * i_returnUnitAtomNumbers +
        i_returnUnitNumber + 1;
    PROTECT(r_unit_names = NEW_CHARACTER(numUnitArguments));
    ii = 0;
    SET_STRING_ELT(r_unit_names, ii++, mkChar("groups"));
    if(i_returnUnitType)
        SET_STRING_ELT(r_unit_names, ii++, mkChar("unittype"));
    if(i_returnUnitDirection)
        SET_STRING_ELT(r_unit_names, ii++, mkChar("unitdirection"));
    if(i_returnUnitAtomNumbers) {
        SET_STRING_ELT(r_unit_names, ii++, mkChar("natoms"));
        SET_STRING_ELT(r_unit_names, ii++, mkChar("ncells"));
        SET_STRING_ELT(r_unit_names, ii++, mkChar("ncellsperatom"));
    }
    if(i_returnUnitNumber)
        SET_STRING_ELT(r_unit_names, ii++, mkChar("unitnumber"));
    
    /*
    ** Because the length of the block list can change
    ** from unit to unit, we have to protect these
    ** things below.
    */

    /*
    ** Now an individual block.
    */
    numBlockArguments = 2 * i_returnXY + i_returnIndices +
        2 * i_returnBases + i_returnAtoms + 2 * i_returnBlockAtomNumbers + 
        i_returnBlockDirection + i_returnIndexpos + i_returnIsPm;
    unprotectBlockInfo = 0;
    PROTECT(r_block_names = NEW_CHARACTER(numBlockArguments));
    ii = 0;
    if(i_returnXY) {
        SET_STRING_ELT(r_block_names, ii++, mkChar("x"));
        unprotectBlockInfo++;
        SET_STRING_ELT(r_block_names, ii++, mkChar("y"));
        unprotectBlockInfo++;
    }
    if(i_returnIndices) {
        SET_STRING_ELT(r_block_names, ii++, mkChar("indices"));
        unprotectBlockInfo++;
    }
    if(i_returnBases) {
        SET_STRING_ELT(r_block_names, ii++, mkChar("pbase"));
        unprotectBlockInfo++;
        SET_STRING_ELT(r_block_names, ii++, mkChar("tbase"));
        unprotectBlockInfo++;
    }
    if(i_returnAtoms) {
        SET_STRING_ELT(r_block_names, ii++, mkChar("atom"));
        unprotectBlockInfo++;
    }
    if(i_returnIndexpos) {
        SET_STRING_ELT(r_block_names, ii++, mkChar("indexpos"));
        unprotectBlockInfo++;
    }
    if(i_returnIsPm) {
        SET_STRING_ELT(r_block_names, ii++, mkChar("ispm"));
        unprotectBlockInfo++;
    }
    if(i_returnBlockDirection) {
        SET_STRING_ELT(r_block_names, ii++, mkChar("groupdirection"));
    }
    if(i_returnBlockAtomNumbers){
        SET_STRING_ELT(r_block_names, ii++, mkChar("natoms"));
        SET_STRING_ELT(r_block_names, ii++, mkChar("ncellsperatom"));
    } 

    /*
    ** This needed in order to reduce memory for the Unit/Block type
    */

    SEXP r_typeAsString;
    PROTECT(r_typeAsString = NEW_STRING(8));
    SET_STRING_ELT(r_typeAsString, 0, mkChar("unknown"));
    SET_STRING_ELT(r_typeAsString, 1, mkChar("expression"));
    SET_STRING_ELT(r_typeAsString, 2, mkChar("genotyping"));
    SET_STRING_ELT(r_typeAsString, 3, mkChar("resequencing"));
    SET_STRING_ELT(r_typeAsString, 4, mkChar("tag"));
    SET_STRING_ELT(r_typeAsString, 5, mkChar("copynumber"));
    SET_STRING_ELT(r_typeAsString, 6, mkChar("genotypingcontrol"));
    SET_STRING_ELT(r_typeAsString, 7, mkChar("expressioncontrol"));

    SEXP r_directionAsString;
    PROTECT(r_directionAsString = NEW_STRING(4));
    SET_STRING_ELT(r_directionAsString, 0, mkChar("nodirection"));
    SET_STRING_ELT(r_directionAsString, 1, mkChar("sense"));
    SET_STRING_ELT(r_directionAsString, 2, mkChar("antisense"));
    SET_STRING_ELT(r_directionAsString, 3, mkChar("unknown"));

    /*
    ** Reading in the units.
    */

    if (i_verboseFlag >= R_AFFX_REALLY_VERBOSE) {
        Rprintf("Finished setup, commencing unit parsing.\n");
    }

    for (int iunit = 0; iunit < numUnits; iunit++) {
      /* Check for interrupts */
      if(iunit % 1000 == 999) 
          R_CheckUserInterrupt();
      /* The index, which is 0-based in Fusion unlike our R-api */
      if(readEveryUnit)
          unit_idx = iunit;
      else
          unit_idx = INTEGER(unitIndices)[iunit] - 1;
      cdf.GetProbeSetInformation(unit_idx, unit);

      PROTECT(r_unit = NEW_LIST(numUnitArguments));
      str = cdf.GetProbeSetName(unit_idx);
      str_length = str.size();
      unitName = Calloc(str_length+1, char);
      strncpy(unitName, str.c_str(), str_length);
      unitName[str_length] = '\0';
      if (i_verboseFlag >= R_AFFX_VERBOSE) {
          Rprintf("Processing unit: %s\n", unitName);
      }
      /* There is no direct GetName method for a unit - strange */
      unitNumBlocks = unit.GetNumGroups();
      ii = 1; /* we always return the blocks */


      if(i_returnUnitType) {
          switch (unit.GetProbeSetType()) {
          case affxcdf::UnknownProbeSetType:
              SET_VECTOR_ELT(r_unit, ii++, 
                             ScalarString(STRING_ELT(r_typeAsString, 0)));
              break;
          case affxcdf::ExpressionProbeSetType:
              SET_VECTOR_ELT(r_unit, ii++,
                             ScalarString(STRING_ELT(r_typeAsString, 1)));
              break;
          case affxcdf::GenotypingProbeSetType:
              SET_VECTOR_ELT(r_unit, ii++,
                             ScalarString(STRING_ELT(r_typeAsString, 2)));
              break;
          case affxcdf::ResequencingProbeSetType:
              SET_VECTOR_ELT(r_unit, ii++, 
                             ScalarString(STRING_ELT(r_typeAsString, 3)));
              break;
          case affxcdf::TagProbeSetType:
              SET_VECTOR_ELT(r_unit, ii++,
                             ScalarString(STRING_ELT(r_typeAsString, 4)));
              break;
          case affxcdf::CopyNumberProbeSetType:
              SET_VECTOR_ELT(r_unit, ii++,
                             ScalarString(STRING_ELT(r_typeAsString, 5)));
              break;
          case affxcdf::GenotypeControlProbeSetType:
              SET_VECTOR_ELT(r_unit, ii++,
                             ScalarString(STRING_ELT(r_typeAsString, 6)));
              break;
          case affxcdf::ExpressionControlProbeSetType:
              SET_VECTOR_ELT(r_unit, ii++,
                             ScalarString(STRING_ELT(r_typeAsString, 7)));
              break;
          default:
              SET_VECTOR_ELT(r_unit, ii++, 
                             ScalarString(STRING_ELT(r_typeAsString, 0)));
              break;
          }
      }

      if(i_returnUnitDirection) {
          switch (unit.GetDirection()) {
          case affxcdf::NoDirection:
              SET_VECTOR_ELT(r_unit, ii++, 
                             ScalarString(STRING_ELT(r_directionAsString, 0)));
              break;
          case affxcdf::SenseDirection:
              SET_VECTOR_ELT(r_unit, ii++, 
                             ScalarString(STRING_ELT(r_directionAsString, 1)));
              break;
          case affxcdf::AntiSenseDirection:
              SET_VECTOR_ELT(r_unit, ii++, 
                             ScalarString(STRING_ELT(r_directionAsString, 2)));
              break;
          default:
              SET_VECTOR_ELT(r_unit, ii++, 
                             ScalarString(STRING_ELT(r_directionAsString, 3)));
              break;
          }
      }

      if(i_returnUnitAtomNumbers) {
          SET_VECTOR_ELT(r_unit, ii++, 
                         ScalarInteger(unit.GetNumLists()));
          SET_VECTOR_ELT(r_unit, ii++, 
                         ScalarInteger(unit.GetNumCells()));
          SET_VECTOR_ELT(r_unit, ii++, 
                         ScalarInteger(unit.GetNumCellsPerList()));
      }

      if(i_returnUnitNumber)
          SET_VECTOR_ELT(r_unit, ii++,
                         ScalarInteger(unit.GetProbeSetNumber()));

      PROTECT(r_blocks_list = NEW_LIST(unitNumBlocks));
      PROTECT(r_blocks_list_names = NEW_CHARACTER(unitNumBlocks));

      /*
      ** Now for the blocks
      */

      for (int iblock = 0; iblock < unitNumBlocks; iblock++) {
        unit.GetGroupInformation(iblock, block);
        PROTECT(r_block = NEW_LIST(numBlockArguments));
        blockNumCells = block.GetNumCells();
        blockName = block.GetName().c_str();
        if (i_verboseFlag >= R_AFFX_REALLY_VERBOSE) {
            Rprintf("Processing block %s\n", blockName);
        }
        if(i_returnXY) {
            PROTECT(r_xvals = NEW_INTEGER(blockNumCells));
            PROTECT(r_yvals = NEW_INTEGER(blockNumCells));
        }
        if(i_returnIndices) 
            PROTECT(r_indices = NEW_INTEGER(blockNumCells));
        if(i_returnBases) {
            PROTECT(r_pbase = NEW_CHARACTER(blockNumCells));
            PROTECT(r_tbase = NEW_CHARACTER(blockNumCells));
        }
        if(i_returnAtoms)
            PROTECT(r_expos = NEW_INTEGER(blockNumCells));
        if(i_returnIndexpos)
            PROTECT(r_indexpos = NEW_INTEGER(blockNumCells));
        if(i_returnIsPm)
            PROTECT(r_ispm = NEW_LOGICAL(blockNumCells));

        for (int icell = 0; icell < blockNumCells; icell++) {
          block.GetCell(icell, probe);
          if (i_verboseFlag >= R_AFFX_REALLY_VERBOSE) {
            Rprintf("icell: %d x: %d, y: %d, pbase: %c, tbase: %c, expos: %d, indexpos: %d\n", 
                    icell, probe.GetX(), probe.GetY(), probe.GetPBase(), 
                    probe.GetTBase(), probe.GetExpos(), probe.GetListIndex());
          }

          if(i_returnXY) {
              INTEGER(r_xvals)[icell] = probe.GetX();
              INTEGER(r_yvals)[icell] = probe.GetY();
          }

          if(i_returnIndices) {
              INTEGER(r_indices)[icell] = probe.GetY() * numCols +
                  probe.GetX() + 1;
          }

          if(i_returnBases) {
              pbaseString = probe.GetPBase();
              SET_STRING_ELT(r_pbase, icell, mkChar(pbaseString.c_str()));
              tbaseString = probe.GetTBase();
              SET_STRING_ELT(r_tbase, icell, mkChar(tbaseString.c_str()));
          }

          if(i_returnAtoms)
              INTEGER(r_expos)[icell] = probe.GetExpos();

          if(i_returnIndexpos)
              INTEGER(r_indexpos)[icell] = probe.GetListIndex();

          if(i_returnIsPm) {
              p_base = probe.GetPBase();
              t_base = probe.GetTBase();
              LOGICAL(r_ispm)[icell] = R_affx_pt_base_is_pm(p_base, t_base);
          }
        }

        if (i_verboseFlag >= R_AFFX_VERBOSE) {
          Rprintf("finished reading unit information for: %s\n", unitName);
        }

        /** Write everything into the r_block object, 
         ** finalize it and unprotect **/

        ii = 0;

        if(i_returnXY) {
            SET_VECTOR_ELT(r_block, ii++, r_xvals);
            SET_VECTOR_ELT(r_block, ii++, r_yvals);
        }

        if(i_returnIndices)
            SET_VECTOR_ELT(r_block, ii++, r_indices);

        if(i_returnBases) {
            SET_VECTOR_ELT(r_block, ii++, r_pbase);
            SET_VECTOR_ELT(r_block, ii++, r_tbase);
        }

        if(i_returnAtoms)
            SET_VECTOR_ELT(r_block, ii++, r_expos);

        if(i_returnIndexpos)
            SET_VECTOR_ELT(r_block, ii++, r_indexpos);

        if(i_returnIsPm)
            SET_VECTOR_ELT(r_block, ii++, r_ispm);

        if(i_returnBlockDirection) {
          switch (block.GetDirection()) {
          case affxcdf::NoDirection:
              SET_VECTOR_ELT(r_block, ii++, 
                             ScalarString(STRING_ELT(r_directionAsString, 0)));
              break;
          case affxcdf::SenseDirection:
              SET_VECTOR_ELT(r_block, ii++, 
                             ScalarString(STRING_ELT(r_directionAsString, 1)));
              break;
          case affxcdf::AntiSenseDirection:
              SET_VECTOR_ELT(r_block, ii++, 
                             ScalarString(STRING_ELT(r_directionAsString, 2)));
              break;
          default:
              SET_VECTOR_ELT(r_block, ii++, 
                             ScalarString(STRING_ELT(r_directionAsString, 3)));
              break;

          }
        }
        if(i_returnBlockAtomNumbers) {
            SET_VECTOR_ELT(r_block, ii++,
                           ScalarInteger(block.GetNumLists()));
            SET_VECTOR_ELT(r_block, ii++, 
                           ScalarInteger(block.GetNumCellsPerList()));
        }
        setAttrib(r_block, R_NamesSymbol, r_block_names);
        UNPROTECT(unprotectBlockInfo);

        /** Put the block into the r_blocks_list and unprotect it **/
        str = block.GetName();
        str_length = str.size();
        cstr = Calloc(str_length+1, char);
        strncpy(cstr, str.c_str(), str_length);
        cstr[str_length] = '\0';
        SET_VECTOR_ELT(r_blocks_list, iblock, r_block);
        SET_STRING_ELT(r_blocks_list_names, iblock, mkChar(cstr));
        Free(cstr);
        UNPROTECT(1);
      }

      /** set the r_block_list names. **/
      setAttrib(r_blocks_list, R_NamesSymbol, r_blocks_list_names);

      /** Finalize the unit object, and unprotect
       ** the r_blocks_list and r_blocks_list_names **/
      SET_VECTOR_ELT(r_unit, 0, r_blocks_list);
      setAttrib(r_unit, R_NamesSymbol, r_unit_names);
      UNPROTECT(2);

      /** now set the unit in the main units_list, 
       ** and unprotect it. **/
      SET_VECTOR_ELT(r_units_list, iunit, r_unit);
      SET_STRING_ELT(r_units_list_names, iunit, mkChar(unitName));
      Free(unitName);
      UNPROTECT(1);
    }

    /* The type and direction vectors */
    UNPROTECT(2);
    
    /** set the names down here at the end. **/
    setAttrib(r_units_list, R_NamesSymbol, r_units_list_names);

    /* unprotect everything we started with, that is 
    ** r_units_list, r_units_list_names as well as
    ** r_unit_names, r_block_names **/

    UNPROTECT(4);
   
    return r_units_list;
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
    string str;
    int str_length; 
    char* cstr; 

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
    const char* cdfFileName   = CHAR(STRING_ELT(fname, 0));
    int i_verboseFlag   = INTEGER(verbose)[0];

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Opens file
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    cdf.SetFileName(cdfFileName);
    if (i_verboseFlag >= R_AFFX_VERBOSE) {
      Rprintf("Attempting to read CDF File: %s\n", cdf.GetFileName().c_str());
    }

    if (cdf.Read() == false) {
      error("Failed to read the CDF file.");
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
          char s[256];
          sprintf(s, "Argument 'units' contains an element out of range: %d", unitIdx);
          error(s);
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
    PROTECT(r_probe_set_names = NEW_STRING(1));
    SET_STRING_ELT(r_probe_set_names, 0, mkChar("groups"));



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
      str = cdf.GetProbeSetName(unitIdx);
      str_length = str.size();
      cstr = Calloc(str_length+1, char);
      strncpy(cstr, str.c_str(), str_length);
      cstr[str_length] = '\0';
      /** ...and add to list of unit names. **/
			SET_STRING_ELT(unitNames, uu, mkChar(cstr));
      Free(cstr);

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
        str = group.GetName();
        str_length = str.size();
        cstr = Calloc(str_length+1, char);
        strncpy(cstr, str.c_str(), str_length);
        cstr[str_length] = '\0';
        SET_STRING_ELT(r_group_names, igroup, mkChar(cstr));
        Free(cstr);

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
  SEXP R_affx_get_cdf_units(SEXP fname, SEXP units, SEXP readXY, SEXP readBases, 
                            SEXP readExpos, SEXP readType, SEXP readDirection, 
                            SEXP readIndices, SEXP verbose)
  {
    FusionCDFData cdf;
    string str;
    int str_length; 
    char* cstr; 

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
    const char* cdfFileName   = CHAR(STRING_ELT(fname, 0));
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
      error("Failed to read the CDF file.");
    }

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Get unit indices to be read
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
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
          char s[256];
          sprintf(s, "Argument 'units' contains an element out of range: %d", unitIdx);
          error(s);
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
    int nbrOfGroupElements = 2*i_readXY + 2*i_readBases + i_readExpos 
                                          + i_readIndices + i_readDirection;


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

      if (i_readDirection) {
        SET_STRING_ELT(cell_list_names, fieldIdx++, mkChar("direction"));
      }
    }


    /* Same for all unit elements */
    PROTECT(r_probe_set_names = NEW_STRING(nbrOfUnitElements));
    int rpsi = 0;
    if (i_readType) {
      /* get the type */
      SET_STRING_ELT(r_probe_set_names, rpsi++, mkChar("type"));
    }

    if (i_readDirection) {
      /* get the direction */
      SET_STRING_ELT(r_probe_set_names, rpsi++, mkChar("direction"));
    }

    if (i_readGroups) {
      SET_STRING_ELT(r_probe_set_names, rpsi++, mkChar("groups"));
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
      str = cdf.GetProbeSetName(unitIdx);
      str_length = str.size();
      cstr = Calloc(str_length+1, char);
      strncpy(cstr, str.c_str(), str_length);
      cstr[str_length] = '\0';
      /** ...and add to list of unit names. **/
			SET_STRING_ELT(unitNames, uu, mkChar(cstr));
      Free(cstr);

      
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

          if (i_readDirection) {
            PROTECT(tmp = allocVector(INTSXP, 1));
            INTEGER(tmp)[0] = group.GetDirection();
            UNPROTECT(1);
            SET_VECTOR_ELT(cell_list, fieldIdx++, tmp);
          }

          /* Unprotect in reverse order, e.g. 'expos', ..., 'xvals' */
          UNPROTECT(protectCount); 
          
          /** set the names of the new list, dont really know if I need 
              to do this each and every time. **/
          setAttrib(cell_list, R_NamesSymbol, cell_list_names);

          /** set these cells in the group list. **/
          SET_VECTOR_ELT(r_group_list, igroup, cell_list);
          str = group.GetName();
          str_length = str.size();
          cstr = Calloc(str_length+1, char);
          strncpy(cstr, str.c_str(), str_length);
          cstr[str_length] = '\0';
          SET_STRING_ELT(r_group_names, igroup, mkChar(cstr));
          Free(cstr);
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
    string str;
    int str_length; 
    char* cstr; 
    
    SEXP names = R_NilValue;

    bool readAll = true; 
    int maxNbrOfUnits = 0, nbrOfUnits = 0; 
    int unitIdx = 0;
    const char* cdfFileName = CHAR(STRING_ELT(fname, 0));
    int i_verboseFlag = INTEGER(verbose)[0];


    cdf.SetFileName(cdfFileName);
    if (i_verboseFlag >= R_AFFX_VERBOSE) {
      Rprintf("Attempting to read CDF File: %s\n", cdf.GetFileName().c_str());
    }

    if (cdf.Read() == false) {
      error("Failed to read the CDF file.");
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
          char s[256];
          sprintf(s, "Argument 'units' contains an element out of range: %d", unitIdx);
          error(s);
        }
      }
    }

    /** the probe set names. **/
    PROTECT(names = NEW_CHARACTER(nbrOfUnits));
    if (readAll) {
      for (int uu = 0; uu < nbrOfUnits; uu++) {
        str = cdf.GetProbeSetName(uu);
        str_length = str.size();
        cstr = Calloc(str_length+1, char);
        strncpy(cstr, str.c_str(), str_length);
        cstr[str_length] = '\0';
        SET_STRING_ELT(names, uu, mkChar(cstr));
        Free(cstr);
      }
    } else {
      for (int uu = 0; uu < nbrOfUnits; uu++) {
        /* Unit indices are zero-based in Fusion SDK. */
        unitIdx = INTEGER(units)[uu] - 1;
        str = cdf.GetProbeSetName(unitIdx);
        str_length = str.size();
        cstr = Calloc(str_length+1, char);
        strncpy(cstr, str.c_str(), str_length);
        cstr[str_length] = '\0';
        SET_STRING_ELT(names, uu, mkChar(cstr));
        Free(cstr);
      }
    }

    /** unprotect the return vector. **/
    UNPROTECT(1); /* 'names' */
   
    return names;
  }  /* R_affx_get_cdf_unit_names() */




} /** end extern C **/


/***************************************************************************
 * HISTORY:
 * 2014-10-28
 * o BUG FIX: Argument 'unitIndices' to R_affx_get_cdf_file_qc()  and
     R_affx_get_cdf_file() could contain elements out of range [1,J].
     If zero or a negative unit was queried a core dump could occur.  If
     a too large unit was queried, random garbage was read.  The intended
     argument check was never performed due to a typo swapping 'condition'
     and 'increase' in a for (initialization; condition; increase) loop.
 * 2008-08-09
 * o Now unit type 5 is reported as 'copynumber' and no longer as 'unknown'.
 * 2007-01-05
 * o BUG FIX: The new group 'direction' field was added first making all
 *   fields getting names of other fields.
 * 2006-12-30
 * o Added group directions to R_affx_get_cdf_units() too.  That is the
 *   most important group element missing. /HB
 * 2006-08-28
 * o If a unit index is out of range, the error now show the invalid index.
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
