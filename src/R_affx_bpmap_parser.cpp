#include "BPMAPFileData.h"
#include "FusionBPMAPData.h"
// #include <iostream>
#include "R_affx_constants.h"

using namespace std;
using namespace affymetrix_fusion_io;

extern "C" {
    #include <R.h>
    #include <Rdefines.h>  
    #include <wchar.h>
    #include <wctype.h>
    
    /**************************************************************
     *
     * R_affx_get_bpmap_file()
     *
     *************************************************************/

    SEXP R_affx_get_bpmap_file(SEXP fname, SEXP verbose) 
    {
        CBPMAPFileData bpmap;

        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - -
         * Process arguments
         * - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        char* bpmapFileName         = CHAR(STRING_ELT(fname,0));
        int i_verboseFlag         = INTEGER(verbose)[0];
        
        int protectCount = 0;

        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
         * Opens stuff
         * - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        if (i_verboseFlag >= R_AFFX_VERBOSE) {
            Rprintf("attempting to read: %s\n", bpmapFileName);
        }
        
        bpmap.SetFileName(bpmapFileName);
        if (bpmap.Exists() == false) {
            Rprintf("File does not exist: %s\n", bpmapFileName);
            return R_NilValue;
        }
       
        if (bpmap.Read() == false) {
            Rprintf("Unable to read file: %s, is it a BPMAP file?\n", 
                    bpmapFileName);
            return R_NilValue;
        }
        
        if (i_verboseFlag >= R_AFFX_VERBOSE) {
            Rprintf("sucessfully read: %s\n", bpmapFileName);
        }

        /* Defining our final return object */
        SEXP resultList = R_NilValue;
        SEXP resultListNames = R_NilValue;
        int nSequences = bpmap.GetNumberSequences();
        PROTECT(resultList = NEW_LIST(nSequences));
        protectCount++;
        PROTECT(resultListNames = NEW_CHARACTER(nSequences));
        protectCount++;

        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
         * Read the sequences
         * - - - - - - - - - - - - - - - - - - - - - - - - - - - */

        /* Setting up the seqInfo object */
        SEXP seqInfo = R_NilValue;
        SEXP seqInfoNames = R_NilValue;
        PROTECT(seqInfo = NEW_LIST(8));
        protectCount++;
        PROTECT(seqInfoNames = NEW_CHARACTER(8));
        protectCount++;
        SET_STRING_ELT(seqInfoNames, 0, mkChar("name"));
        SET_STRING_ELT(seqInfoNames, 1, mkChar("groupname"));
        SET_STRING_ELT(seqInfoNames, 2, mkChar("fullname"));
        SET_STRING_ELT(seqInfoNames, 3, mkChar("version"));
        SET_STRING_ELT(seqInfoNames, 4, mkChar("mapping"));
        SET_STRING_ELT(seqInfoNames, 5, mkChar("number"));
        SET_STRING_ELT(seqInfoNames, 6, mkChar("numberOfHits"));
        SET_STRING_ELT(seqInfoNames, 7, mkChar("parameters"));
        setAttrib(seqInfo, R_NamesSymbol, seqInfoNames);
        affxbpmap::CGDACSequenceItem seq;

        for(int i = 0; i < nSequences; i++)
            {
                bpmap.GetSequenceItem(i, seq);
                // we need name, fullname, groupname
                // version, probe mapping, number?
                // number of hits and finally a parameter vector
                const char* seqName = seq.GetName().c_str();
                SET_STRING_ELT(resultListNames, i, 
                               mkChar(seq.GetName().c_str()));
                SET_VECTOR_ELT(seqInfo, 0, 
                               ScalarString(mkChar(seq.GetName().c_str())));
                SET_VECTOR_ELT(seqInfo, 1, 
                               ScalarString(mkChar(seq.GroupName().c_str())));
                SET_VECTOR_ELT(seqInfo, 2, 
                               ScalarString(mkChar(seq.FullName().c_str())));
                SET_VECTOR_ELT(seqInfo, 3, 
                               ScalarString(mkChar(seq.GetSeqVersion().c_str())));
                if(seq.GetProbeMapping() == 0)
                    SET_VECTOR_ELT(seqInfo, 4, 
                                       ScalarString(mkChar("pmmm")));
                else
                    SET_VECTOR_ELT(seqInfo, 4, 
                                       ScalarString(mkChar("pm")));
                SET_VECTOR_ELT(seqInfo, 5,
                               ScalarInteger(seq.GetNumber()));
                SET_VECTOR_ELT(seqInfo, 6,
                               ScalarInteger(seq.GetNumberHits()));
                SET_VECTOR_ELT(resultList, i, seqInfo);

                /* Now we read the parameters, which is basically
                 * a long character vector of different parameter
                 * values */
                SEXP seqParameters = R_NilValue;
                SEXP seqParameterNames = R_NilValue;
                int nParameters = seq.GetNumberParameters();
                PROTECT(seqParameters = NEW_CHARACTER(nParameters));
                PROTECT(seqParameterNames = NEW_CHARACTER(nParameters));
                for(int j = 0; j < nParameters; j++)
                    {
                        SET_STRING_ELT(seqParameterNames, j, 
                                       mkChar(seq.GetParameter(j).Tag.c_str()));
                        SET_STRING_ELT(seqParameters, j, 
                                       mkChar(seq.GetParameter(j).Value.c_str()));
                    }
                setAttrib(seqParameters, R_NamesSymbol, seqParameterNames);
                SET_VECTOR_ELT(seqInfo, 7, seqParameters);
                UNPROTECT(2);
                // Read hit information
                // pmx, pmy, mmx, mmy, probeseq,
                // matchscore, strand
                // genomic position (and center position), 
                // length of probe
            }
                    
        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
         * Return stuff
         * - - - - - - - - - - - - - - - - - - - - - - - - - - - */

        bpmap.Close();
        setAttrib(resultList, R_NamesSymbol, resultListNames);
        UNPROTECT(protectCount);
        return resultList;
    } /* R_affx_get_bpmap_file() */

} /** end extern "C" **/

