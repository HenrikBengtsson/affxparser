#include "FusionCELData.h"
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
   * R_affx_extract_cel_file_meta()
   *
   * return a list of the meta data associated with this cell
   * file. This is the information stored in the header. 
   *
   ************************************************************************/
  SEXP R_affx_extract_cel_file_meta(FusionCELData &cel) {
    SEXP names, vals;
    SEXP tmp;
    int kk = 0;
    string str;
    int str_length; 
    char* cstr; 

    PROTECT(names = NEW_CHARACTER(14));
    PROTECT(vals  = NEW_LIST(14));

    SET_STRING_ELT(names, kk, mkChar("filename"));
    str = cel.GetFileName();
    str_length = str.size();
    cstr = Calloc(str_length+1, char);
    strncpy(cstr, str.c_str(), str_length);
    cstr[str_length] = '\0';
    SET_VECTOR_ELT(vals, kk++, mkString(cstr));
    Free(cstr);

    SET_STRING_ELT(names, kk, mkChar("version"));
    PROTECT(tmp = allocVector(INTSXP, 1));
    INTEGER(tmp)[0] = cel.GetVersion();
    SET_VECTOR_ELT(vals, kk++, tmp);
    UNPROTECT(1);

    SET_STRING_ELT(names, kk, mkChar("cols"));
    PROTECT(tmp = allocVector(INTSXP, 1));
    INTEGER(tmp)[0] = cel.GetCols();
    SET_VECTOR_ELT(vals, kk++, tmp);
    UNPROTECT(1);

    SET_STRING_ELT(names, kk, mkChar("rows"));
    PROTECT(tmp = allocVector(INTSXP, 1));
    INTEGER(tmp)[0] = cel.GetRows();
    SET_VECTOR_ELT(vals, kk++, tmp);
    UNPROTECT(1);
    
    SET_STRING_ELT(names, kk, mkChar("total"));
    PROTECT(tmp = allocVector(INTSXP, 1));
    INTEGER(tmp)[0] = cel.GetNumCells();
    SET_VECTOR_ELT(vals, kk++, tmp);
    UNPROTECT(1);
   

#ifdef SUPPORT_MBCS
    str_length = cel.GetAlg().size();
    cstr = Calloc(str_length+1, char);
    wcstombs(cstr, cel.GetAlg().c_str(), str_length);
    cstr[str_length] = '\0';
    SET_STRING_ELT(names, kk, mkChar("algorithm"));
    SET_VECTOR_ELT(vals, kk++, mkString(cstr)); 
    Free(cstr);

    str_length = cel.GetParams().size();
    cstr = Calloc(str_length+1, char);
    wcstombs(cstr, cel.GetParams().c_str(), str_length);
    cstr[str_length] = '\0';
    SET_STRING_ELT(names, kk, mkChar("parameters"));
    SET_VECTOR_ELT(vals, kk++, mkString(cstr));
    Free(cstr);
   
    str_length = cel.GetChipType().size();
    cstr = Calloc(str_length+1, char);
    wcstombs(cstr, cel.GetChipType().c_str(), str_length);
    cstr[str_length] = '\0';
    SET_STRING_ELT(names, kk, mkChar("chiptype"));
    SET_VECTOR_ELT(vals, kk++, mkString(cstr));
    Free(cstr);
        
    str_length = cel.GetHeader().size();
    cstr = Calloc(str_length+1, char);
    wcstombs(cstr, cel.GetHeader().c_str(), str_length);
    cstr[str_length] = '\0';
    SET_STRING_ELT(names, kk, mkChar("header"));
    SET_VECTOR_ELT(vals, kk++, mkString(cstr));
    Free(cstr);

    str_length = cel.GetDatHeader().size();
    cstr = Calloc(str_length+1, char);
    wcstombs(cstr, cel.GetDatHeader().c_str(), str_length);
    cstr[str_length] = '\0';
    SET_STRING_ELT(names, kk, mkChar("datheader"));
    SET_VECTOR_ELT(vals, kk++, mkString(cstr));
    Free(cstr);

    str_length = cel.GetLibraryPackageName().size();
    cstr = Calloc(str_length+1, char);
    wcstombs(cstr, cel.GetLibraryPackageName().c_str(), str_length);
    cstr[str_length] = '\0';
    SET_STRING_ELT(names, kk, mkChar("librarypackage"));
    SET_VECTOR_ELT(vals, kk++, mkString(cstr));
    Free(cstr);

#else

    SET_STRING_ELT(names, kk, mkChar("algorithm"));
    SET_VECTOR_ELT(vals, kk++, R_NilValue);

    SET_STRING_ELT(names, kk, mkChar("parameters"));
    SET_VECTOR_ELT(vals, kk++, R_NilValue);

    SET_STRING_ELT(names, kk, mkChar("chiptype"));
    SET_VECTOR_ELT(vals, kk++, R_NilValue);

    SET_STRING_ELT(names, kk, mkChar("header"));
    SET_VECTOR_ELT(vals, kk++, R_NilValue);

    SET_STRING_ELT(names, kk, mkChar("datheader"));
    SET_VECTOR_ELT(vals, kk++, R_NilValue);

    SET_STRING_ELT(names, kk, mkChar("librarypackage"));
    SET_VECTOR_ELT(vals, kk++, R_NilValue);

#endif    

    SET_STRING_ELT(names, kk, mkChar("cellmargin"));
    PROTECT(tmp = allocVector(INTSXP, 1));
    INTEGER(tmp)[0] = cel.GetCellMargin();
    SET_VECTOR_ELT(vals, kk++, tmp);
    UNPROTECT(1);
    
    SET_STRING_ELT(names, kk, mkChar("noutliers"));
    PROTECT(tmp = allocVector(INTSXP, 1));
    INTEGER(tmp)[0] = cel.GetNumOutliers();
    SET_VECTOR_ELT(vals, kk++, tmp);
    UNPROTECT(1);

    SET_STRING_ELT(names, kk, mkChar("nmasked"));
    PROTECT(tmp = allocVector(INTSXP, 1));
    INTEGER(tmp)[0] = cel.GetNumMasked();
    SET_VECTOR_ELT(vals, kk++, tmp);
    UNPROTECT(1);


    setAttrib(vals, R_NamesSymbol, names);

    UNPROTECT(2);
    
    return vals;
  }



  /************************************************************************
   *
   * R_affx_get_cel_file_header()
   *
   * This quickly reads only the cel file header.
   *
   ************************************************************************/
  SEXP R_affx_get_cel_file_header(SEXP fname) 
  {
    FusionCELData cel;
    SEXP header = R_NilValue;

    const char* celFileName = CHAR(STRING_ELT(fname,0));
    cel.SetFileName(celFileName);
    // if (cel.ReadHeader() == false) {
    if (cel.Exists() == false) {
      error("Cannot read CEL file header. File not found: %s\n", celFileName);
    }
    cel.Read();

    try {
      PROTECT(header = R_affx_extract_cel_file_meta(cel));
      UNPROTECT(1);
    } catch(affymetrix_calvin_exceptions::CalvinException& ex) {
      error("[affxparser Fusion SDK exception] Failed to parse header of CEL file: %s\n", celFileName);
    }
    
    return header;
  }



  /************************************************************************
   *
   * R_affx_get_cel_file()
   *
   * read cel file either partially or completely.
   *
   ************************************************************************/
  SEXP R_affx_get_cel_file(SEXP fname, SEXP readHeader, SEXP readIntensities, SEXP readX,
                           SEXP readY, SEXP readPixels, SEXP readStdvs, SEXP readOutliers,
                           SEXP readMasked, SEXP indices, SEXP verbose) 
  {
    FusionCELData cel;

    SEXP  
      header = R_NilValue,
      xvals = R_NilValue,
      yvals = R_NilValue,
      intensities = R_NilValue,
      stdvs = R_NilValue,
      pixels = R_NilValue,
      outliers = R_NilValue,
      masked = R_NilValue;

    unsigned int nbrOfOutliers = 0, nbrOfMasked = 0;
    unsigned int outliersCount = 0, maskedCount = 0;

    int protectCount = 0;
   

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Process arguments
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    const char* celFileName   = CHAR(STRING_ELT(fname,0));
    int i_readHeader          = INTEGER(readHeader)[0];
    int i_readX               = INTEGER(readX)[0];
    int i_readY               = INTEGER(readY)[0];
    int i_readIntensities     = INTEGER(readIntensities)[0];
    int i_readStdvs           = INTEGER(readStdvs)[0];
    int i_readPixels          = INTEGER(readPixels)[0];
    int i_readOutliers        = INTEGER(readOutliers)[0];
    int i_readMasked          = INTEGER(readMasked)[0];
    int i_verboseFlag         = INTEGER(verbose)[0];


    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Opens file
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    if (i_verboseFlag >= R_AFFX_VERBOSE) {
      Rprintf("attempting to read: %s\n", celFileName);
    }
    /**
       XXX: there are three Read methods - I do not know which
       one is the most appropriate here, but this default method
       seems to read everything. Ex(celFileName, FusionCELData::CEL_ALL)
    **/
    cel.SetFileName(celFileName);
    if (cel.Exists() == false) {
      error("Cannot read CEL file. File not found: %s\n", celFileName);
    }
    if (cel.Read(true) == false) {
      error("Cannot read CEL file: %s\n", celFileName);
    }

    if (i_verboseFlag >= R_AFFX_VERBOSE) {
      Rprintf("sucessfully read: %s\n", celFileName);
    }


    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Get cell indices to be read
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    bool readAll = true;
    int maxNbrOfCells;
    int nbrOfCells = length(indices);

    try {
      maxNbrOfCells = cel.GetNumCells();
    } catch(affymetrix_calvin_exceptions::CalvinException& ex) {
      UNPROTECT(protectCount);
      error("[affxparser Fusion SDK exception] Failed to parse CEL file: %s\n", celFileName);
    }
    
    if (nbrOfCells == 0) {
      nbrOfCells = maxNbrOfCells;
    } else {
      readAll = false;
      /* Validate argument 'indices': */
      for (int ii = 0; ii < nbrOfCells; ii++) {
        int index = INTEGER(indices)[ii];
        /* Cell indices are zero-based in Fusion SDK. */
        if (index < 1 || index > maxNbrOfCells) {
          error("Argument 'indices' contains an element out of range.");
        }
      }
      nbrOfCells = length(indices);
    }

    if (i_verboseFlag >= R_AFFX_VERBOSE) {
      Rprintf("Reading %d cells.\n", nbrOfCells);
    }

   
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Read header (optional)
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    if (i_readHeader != 0) {
      try {
        PROTECT(header = R_affx_extract_cel_file_meta(cel));
        protectCount++;
      } catch(affymetrix_calvin_exceptions::CalvinException& ex) {
        error("[affxparser Fusion SDK exception] Failed to parse header of CEL file: %s\n", celFileName);
      }
    }

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * Allocate memory for each vector to be returned.
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    /* Read X and Y (optional) */
    if (i_readX != 0) {
      PROTECT(xvals = NEW_INTEGER(nbrOfCells));
      protectCount++;
    }
    if (i_readY != 0) {
      PROTECT(yvals = NEW_INTEGER(nbrOfCells));
      protectCount++;
    }

    /* Read intensities (optional) */
    if (i_readIntensities != 0) {
      PROTECT(intensities = NEW_NUMERIC(nbrOfCells));
      protectCount++;
    }

    /* Read standard deviations (optional) */
    if (i_readStdvs != 0) {
      PROTECT(stdvs = NEW_NUMERIC(nbrOfCells));
      protectCount++;
    }

    /* Read number of pixels (optional) */
    if (i_readPixels != 0) {
      PROTECT(pixels = NEW_INTEGER(nbrOfCells));
      protectCount++;
    }


    /* Read outlier features (optional) */
    if (i_readOutliers != 0) {
      try {
        nbrOfOutliers = cel.GetNumOutliers();
      } catch(affymetrix_calvin_exceptions::CalvinException& ex) {
        UNPROTECT(protectCount);
        error("[affxparser Fusion SDK exception] Failed to parse CEL file: %s\n", celFileName);
      }
      if (i_verboseFlag >= R_AFFX_VERBOSE)
        Rprintf("Number of outliers to be read: %d\n", nbrOfOutliers);

      PROTECT(outliers = NEW_INTEGER(nbrOfOutliers));
      protectCount++;
    }

    /* Read masked features (optional) */
    if (i_readMasked != 0) {
      try {
        nbrOfMasked = cel.GetNumMasked();
      } catch(affymetrix_calvin_exceptions::CalvinException& ex) {
        UNPROTECT(protectCount);
        error("[affxparser Fusion SDK exception] Failed to parse CEL file: %s\n", celFileName);
      }
      if (i_verboseFlag >= R_AFFX_VERBOSE)
        Rprintf("Number of masked to be read: %d\n", nbrOfMasked);

      PROTECT(masked = NEW_INTEGER(nbrOfMasked));
      protectCount++;
    }

    /** here we will store the above entries in that order. **/
    SEXP result_list;
    SEXP names;
    /* Number of elements in return list */
    int nbrOfElements = (i_readHeader + i_readX + i_readY + i_readIntensities
     + i_readStdvs + i_readPixels + i_readOutliers + i_readMasked);

    PROTECT(result_list = NEW_LIST(nbrOfElements));
    protectCount++;
    PROTECT(names = NEW_CHARACTER(nbrOfElements));
    protectCount++;
   
        
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * For each cell
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    for (int icel = 0, index = 0; icel < nbrOfCells; icel++) {
      if (i_verboseFlag >= R_AFFX_VERBOSE) {
        if (icel % 1000 == 0 || icel == nbrOfCells-1) {
          Rprintf("%d/%d, ", icel+1, nbrOfCells);
        }
      }

      if (readAll) {
        index = icel;
      } else {
        /* Cell indices are zero-based in Fusion SDK */
        index = INTEGER(indices)[icel] - 1;
      }

      try {
        if (i_verboseFlag >= R_AFFX_REALLY_VERBOSE) {
          Rprintf("index: %d, x: %d, y: %d, intensity: %f, stdv: %f, pixels: %d\n", index, cel.IndexToX(index), cel.IndexToY(index), cel.GetIntensity(index), cel.GetStdv(index), cel.GetPixels(index));
        }

        /* Read X and Y (optional) */
        if (i_readX != 0) {
          INTEGER(xvals)[icel] = cel.IndexToX(index);
        }
        if (i_readY != 0) {
          INTEGER(yvals)[icel] = cel.IndexToY(index);
        }
  
        if (i_readIntensities != 0) {
          REAL(intensities)[icel] = cel.GetIntensity(index);
        }
  
        /* Read standard deviations (optional) */
        if (i_readStdvs != 0) {
          REAL(stdvs)[icel] = cel.GetStdv(index);
        }
  
        /* Read number of pixels (optional) */
        if (i_readPixels != 0) {
          INTEGER(pixels)[icel] = cel.GetPixels(index);
        }
      } catch(affymetrix_calvin_exceptions::CalvinException& ex) {
        UNPROTECT(protectCount);
        error("[affxparser Fusion SDK exception] Failed to parse CEL file: %s\n", celFileName);
      }


      /* Read outlier features (optional) */
      if (i_readOutliers != 0) {
	bool isOutlier;
	try {
          isOutlier = cel.IsOutlier(index);
        } catch(affymetrix_calvin_exceptions::CalvinException& ex) {
          UNPROTECT(protectCount);
          error("[affxparser Fusion SDK exception] Failed to parse CEL file: %s\n", celFileName);
        }
        if (isOutlier) {
          if (outliersCount >= nbrOfOutliers)
            error("Internal error: Too many cells flagged as outliers.");
          /* Cell indices are one-based in R */
          INTEGER(outliers)[outliersCount++] = index + 1;
        }
      }

      /* Read masked features (optional) */
      if (i_readMasked != 0) {
	bool isMasked;
	try {
          isMasked = cel.IsMasked(index);
        } catch(affymetrix_calvin_exceptions::CalvinException& ex) {
          UNPROTECT(protectCount);
          error("[affxparser Fusion SDK exception] Failed to parse CEL file: %s\n", celFileName);
        }
        if (isMasked) {
          if (maskedCount >= nbrOfMasked)
            error("Internal error: Too many cells flagged as masked.");
          /* Cell indices are one-based in R */
          INTEGER(masked)[maskedCount++] = index + 1;
        }
      }
    } /* for (int icel ...) */
    

    /** resize here if we only read part of the cel then we only want the outliers
        which correspond to that part. **/
    if (i_readOutliers != 0) {
      if (outliersCount == 0) {
        outliers = R_NilValue;
      } 
      else if (outliersCount < nbrOfOutliers) {
        PROTECT(SET_LENGTH(outliers, outliersCount));
        protectCount++;
      }
    }
    
    if (i_readMasked != 0) {
      if (maskedCount == 0) {
        masked = R_NilValue;
      } 
      else if (maskedCount < nbrOfMasked) {
        PROTECT(SET_LENGTH(masked, maskedCount));
        protectCount++;
      }
    }

    /** set up the names of the result list. **/
    int jj = 0;
    if (i_readHeader != 0) {
      SET_STRING_ELT(names, jj, mkChar("header"));
      SET_VECTOR_ELT(result_list, jj++, header);
    }

    if (i_readX != 0) {
      SET_STRING_ELT(names, jj, mkChar("x"));    
      SET_VECTOR_ELT(result_list, jj++, xvals);
    }

    if (i_readY != 0) {
      SET_STRING_ELT(names, jj, mkChar("y"));
      SET_VECTOR_ELT(result_list, jj++, yvals);
    }

    if (i_readIntensities != 0) {
      SET_STRING_ELT(names, jj, mkChar("intensities"));
      SET_VECTOR_ELT(result_list, jj++, intensities);
    }

    if (i_readStdvs != 0) {
      SET_STRING_ELT(names, jj, mkChar("stdvs"));        
      SET_VECTOR_ELT(result_list, jj++, stdvs);
    }

    if (i_readPixels != 0) {
      SET_STRING_ELT(names, jj, mkChar("pixels"));        
      SET_VECTOR_ELT(result_list, jj++, pixels);
    }

    if (i_readOutliers != 0) {
      SET_STRING_ELT(names, jj, mkChar("outliers"));
      SET_VECTOR_ELT(result_list, jj++, outliers);
    }

    if (i_readMasked != 0) {
      SET_STRING_ELT(names, jj, mkChar("masked"));
      SET_VECTOR_ELT(result_list, jj++, masked);
    }

    /** set the names of the list entries. **/
    setAttrib(result_list, R_NamesSymbol, names);

    if (i_verboseFlag >= R_AFFX_VERBOSE) {
      Rprintf("Finished reading CEL file.\n");
    }

    /* Note: If possible, that is if we do not fill the PROTECT stack
       (limited to 10,000), it is much safer to UNPROTECT() everything
       at the end.  Remember, it is a *stack* where UNPROTECT() is for
       the latest PROTECT():ed object; it is *not* a FIFO queue  */
    UNPROTECT(protectCount);
 
    return result_list;
  } /* R_affx_get_cel_file() */





} /** end extern "C" **/

/***************************************************************************
 * HISTORY:
 * 2015-05-05
 * o ROBUSTNESS: Now using try-catch to pass exceptions to R.
 * 2006-09-15
 * o BUG FIX: Forgot to allocate space for NULL terminator in 'd' in call
 *   wcstombs(d,s,n), alternatively making sure d[last] == '\0'.  I added
 *   both explicitly just in case.
 * 2006-03-28
 * o Cell indices are now one-based. /HB
 * o Added validation to argument 'indices'. /HB
 * o Renamed argument 'indicesToRead' to 'indices' (as in R). /HB
 * 2006-03-26
 * o Note: PROTECT()/UNPROTECT() is a LIFO stack and not a FIFO queue!
 * o Replaced 'x=lengthgets(x,n)' with macro 'SET_LENGTH(x, n)'.
 * o Added check to make sure assignment to vectors of outliers and masked
 *   is not out of bound.
 * o Added PROTECT() to all allocVector() allocated variables in
 *   R_affx_extract_cel_file_meta().
 * o Removed integer variable 'numIndices'.  It was only used once.
 * o Added PROTECT() around 'outliers=R_NilValue' and 'masked=R_NilValue'
 *   just in case.
 * 2006-02-19
 * o In R_affx_get_cel_file(): 'noutliers' and 'nmasked' are declared 
 *   'unsigned int' (not just 'int').
 * o Re-incorporated code in the affxparser devel code.
 * 2006-01-16
 * o BUG FIX: Changed the order of elements returned to reflect the order
 *   in the CEL file.
 * o BUG FIX: R_affx_get_cel_file() had a few memory bugs.  Especially, the
 *   returned list was never protected.
 * o Renamed a few variable to be more readable and searchable, e.g. jj.
 * 2006-01-13 
 * o BUG FIX: The FusionCELData argument was passed as copy by value instead
 *   of copy by reference to R_affx_extract_cel_file_meta(), which made the
 *   object to be destructed twice giving strange memory problem. /HB
 **************************************************************************/ 
