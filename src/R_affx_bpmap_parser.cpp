#include "BPMAPFileData.h"
#include "BPMAPFileWriter.h"
// #include "FusionBPMAPData.h"
#include "R_affx_constants.h"

/* ToDo: Implement partial reading */
/* Need to take care of mmx, mmy, when we have a pmonly */

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

    SEXP R_affx_get_bpmap_header(SEXP fname){
        affxbpmap::CBPMAPFileData bpmap;

        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - -
         * Process arguments
         * - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        char* bpmapFileName   = CHAR(STRING_ELT(fname,0));

        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
         * Opens stuff
         * - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        bpmap.SetFileName(bpmapFileName);
        if (bpmap.ReadHeader() == false) {
            Rprintf("Unable to read file: %s\n", bpmapFileName);
            return R_NilValue;
        }

        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
         * Define Return Object: returnList, returnListNames
         *
         * We also read the object.
         * - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        SEXP returnList, returnListNames;
        PROTECT(returnList = NEW_LIST(2));
        SET_VECTOR_ELT(returnList, 0, ScalarReal(bpmap.GetVersion()));
        SET_VECTOR_ELT(returnList, 1, ScalarInteger(bpmap.GetNumberSequences()));
        PROTECT(returnListNames = NEW_CHARACTER(2));
        SET_STRING_ELT(returnListNames, 0, mkChar("version"));
        SET_STRING_ELT(returnListNames, 1, mkChar("numSequences"));
        setAttrib(returnList, R_NamesSymbol, returnListNames);
        bpmap.Close();
        UNPROTECT(2);
        return returnList;
    }  /* R_affx_get_bpmap_header */


    /**************************************************************
     *
     * R_affx_bpmap_seqinfo_item()
     *
     * An internal function
     *
     *************************************************************/

    SEXP R_affx_bpmap_seqinfo_item(affxbpmap::CGDACSequenceItem &seq, int i_verboseFlag){
                if (i_verboseFlag >= R_AFFX_REALLY_VERBOSE) {
                    Rprintf("  Reading seqInfo\n");
                }
                SEXP seqInfo, seqInfoNames, 
                    seqInfoParameters, seqInfoParameterNames;
                PROTECT(seqInfo = NEW_LIST(8));
                PROTECT(seqInfoNames = NEW_CHARACTER(8));
                SET_VECTOR_ELT(seqInfo, 0, 
                               ScalarString(mkChar(seq.GetName().c_str())));
                SET_STRING_ELT(seqInfoNames, 0, mkChar("name"));
                SET_VECTOR_ELT(seqInfo, 1, 
                               ScalarString(mkChar(seq.GroupName().c_str())));
                SET_STRING_ELT(seqInfoNames, 1, mkChar("groupname"));
                SET_VECTOR_ELT(seqInfo, 2, 
                               ScalarString(mkChar(seq.FullName().c_str())));
                SET_STRING_ELT(seqInfoNames, 2, mkChar("fullname"));
                SET_VECTOR_ELT(seqInfo, 3, 
                               ScalarString(mkChar(seq.GetSeqVersion().c_str())));
                SET_STRING_ELT(seqInfoNames, 3, mkChar("version"));
                if(seq.GetProbeMapping() == 0)
                    SET_VECTOR_ELT(seqInfo, 4, 
                                   ScalarString(mkChar("pmmm")));
                else
                    SET_VECTOR_ELT(seqInfo, 4, 
                                   ScalarString(mkChar("onlypm")));
                SET_STRING_ELT(seqInfoNames, 4, mkChar("mapping"));
                SET_VECTOR_ELT(seqInfo, 5,
                               ScalarInteger(seq.GetNumber() + 1));
                SET_STRING_ELT(seqInfoNames, 5, mkChar("number"));
                SET_VECTOR_ELT(seqInfo, 6,
                               ScalarInteger(seq.GetNumberHits()));
                SET_STRING_ELT(seqInfoNames, 6, mkChar("numberOfHits"));
                /* Now we read the parameters, which is basically
                 * a long character vector of different parameter
                 * values */
                int nParameters = seq.GetNumberParameters();
                if(nParameters > 0) {
                    if (i_verboseFlag >= R_AFFX_REALLY_VERBOSE) {
                        Rprintf("  Reading %d seqInfoParameters\n", nParameters);
                    }
                    PROTECT(seqInfoParameters = NEW_CHARACTER(nParameters));
                    PROTECT(seqInfoParameterNames = NEW_CHARACTER(nParameters));
                    for(int j = 0; j < nParameters; j++)
                        {
                            SET_STRING_ELT(seqInfoParameterNames, j, 
                                           mkChar(seq.GetParameter(j).Tag.c_str()));
                            SET_STRING_ELT(seqInfoParameters, j, 
                                           mkChar(seq.GetParameter(j).Value.c_str()));
                        }
                    setAttrib(seqInfoParameters, R_NamesSymbol, seqInfoParameterNames);
                    SET_VECTOR_ELT(seqInfo, 7, seqInfoParameters);
                    UNPROTECT(2);
                } else {
                    SET_VECTOR_ELT(seqInfo, 7, R_NilValue);
                }
                SET_STRING_ELT(seqInfoNames, 7, mkChar("parameters"));
                setAttrib(seqInfo, R_NamesSymbol, seqInfoNames);
                UNPROTECT(2);
                return seqInfo;
    } /* R_affx_bpmap_seqinfo_item */


    /**************************************************************
     *
     * R_affx_get_bpmap_seqinfo()
     *
     *************************************************************/


    SEXP R_affx_get_bpmap_seqinfo(SEXP fname, SEXP seqindices, SEXP verbose){
        affxbpmap::CBPMAPFileData bpmap;

        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - -
         * Process arguments
         * - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        char* bpmapFileName   = CHAR(STRING_ELT(fname,0));
        int i_verboseFlag     = INTEGER(verbose)[0];

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

        /* checking whether or not we are reading everything or just
         * some of the sequences. We assume that R delivers the 
         * indices in a sorted way */
        int nSequenceIndices = length(seqindices);
        int nSequences = bpmap.GetNumberSequences(); 
        bool readAllSequences = false;
        if(nSequenceIndices == 0) {
            readAllSequences = true;
        } else {
            /* and some error checking of the argument */
            for(int i = 0; i < nSequenceIndices; i++){
                if(INTEGER(seqindices)[i] < 0 | INTEGER(seqindices)[i] > nSequences)
                    error("seqIndices out of range");
                        }
            nSequences = nSequenceIndices;
        }

        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
         * Define Return Object: returnList, returnListNames
         * - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        SEXP returnList, returnListNames;
        PROTECT(returnList = NEW_LIST(nSequences));
        PROTECT(returnListNames = NEW_CHARACTER(nSequences));

        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
         * Read the sequences
         * - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        affxbpmap::CGDACSequenceItem seq;
        for(int i = 0; i < nSequences; i++){
            if(readAllSequences == true) {
                bpmap.GetSequenceItem(i, seq);
            } else {
                /* indices in Fusion are zero-based */ 
                bpmap.GetSequenceItem(INTEGER(seqindices)[i] - 1, seq);
            }
            if (i_verboseFlag >= R_AFFX_VERBOSE) {
                Rprintf("Reading sequence object: %s\n", seq.GetName().c_str());
            }
            SET_VECTOR_ELT(returnList, i, R_affx_bpmap_seqinfo_item(seq, i_verboseFlag));
            SET_STRING_ELT(returnListNames, i, mkChar(seq.FullName().c_str()));
        }
        setAttrib(returnList, R_NamesSymbol, returnListNames);
        bpmap.Close();
        UNPROTECT(2);
        return returnList;
    } /* R_affx_get_bpmap_seqinfo */


    /**************************************************************
     *
     * R_affx_get_bpmap_file()
     *
     *************************************************************/


    SEXP R_affx_get_bpmap_file(SEXP fname, SEXP seqindices, SEXP verbose) 
    {
        affxbpmap::CBPMAPFileData bpmap;

        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - -
         * Process arguments
         * - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        char* bpmapFileName   = CHAR(STRING_ELT(fname,0));
//         int i_readSeqInfo     = INTEGER(readSeqInfo)[0];
//         int i_readStartPos    = INTEGER(readStartPos)[0];
//         int i_readCenterPos   = INTEGER(readCenterPos)[0];
//         int i_readProbeSeq    = INTEGER(readProbeSeq)[0];
//         int i_readStrand      = INTEGER(readStrand)[0];
//         int i_readPMXY        = INTEGER(readPMXY)[0];
//         int i_readMMXY        = INTEGER(readMMXY)[0];
//         int i_readMatchScore  = INTEGER(readMatchScore)[0];
//         int i_readProbeLength = INTEGER(readProbeLength)[0];
        int i_verboseFlag     = INTEGER(verbose)[0];

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

        /* checking whether or not we are reading everything or just
         * some of the sequences. We assume that R delivers the 
         * indices in a sorted way */

        int nSequenceIndices = length(seqindices);
        int nSequences = bpmap.GetNumberSequences(); 
        bool readAllSequences = false;
        if(nSequenceIndices == 0) {
            readAllSequences = true;
        } else {
            /* and some error checking of the argument */
            for(int i = 0; i < nSequenceIndices; i++){
                if(INTEGER(seqindices)[i] < 0 | INTEGER(seqindices)[i] > nSequences)
                    error("seqIndices out of range");
                        }
            nSequences = nSequenceIndices;
        }

        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
         * Define Return Object: returnList, returnListNames
         *  
         * In this case, each component of resultList is call
         * seqObj, which holds the relevant components
         * - - - - - - - - - - - - - - - - - - - - - - - - - - - */

        SEXP resultList, resultListNames;
        PROTECT(resultList = NEW_LIST(nSequences));
        PROTECT(resultListNames = NEW_CHARACTER(nSequences));

        SEXP seqObj, seqObjNames;
        /* slots in seqObj */
        SEXP pmx, pmy, mmx, mmy, probeseq, strand, probelength,
            matchscore, startpos, centerpos, 
            seqInfo, seqInfoNames, seqInfoParameters, 
            seqInfoParameterNames;

        affxbpmap::CGDACSequenceItem seq;
        affxbpmap::GDACSequenceHitItemType seqHit;
       
        int seqObjLength = 11;
        int protectCount = 0;

        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
         * Read the sequences.
         * - - - - - - - - - - - - - - - - - - - - - - - - - - - */

        for(int i = 0; i < nSequences; i++)
            {
                /* Initializing memory, we free it at the end of each sequence */ 
                protectCount = 0;
                PROTECT(seqObj = NEW_LIST(seqObjLength));
                protectCount++;
                PROTECT(seqObjNames = NEW_CHARACTER(seqObjLength));
                protectCount++;

                if(readAllSequences) {
                    bpmap.GetSequenceItem(i, seq);
                } else {
                    /* Fusion indices are zero base */
                    bpmap.GetSequenceItem(INTEGER(seqindices)[i] - 1, seq);
                }
                if (i_verboseFlag >= R_AFFX_VERBOSE) {
                    Rprintf("Reading sequence object: %s\n", seq.GetName().c_str());
                }
                SET_VECTOR_ELT(seqObj, 0, R_affx_bpmap_seqinfo_item(seq, i_verboseFlag));
                SET_STRING_ELT(seqObjNames, 0, mkChar("seqInfo"));
            
                int nHits = seq.GetNumberHits();
                if (i_verboseFlag >= R_AFFX_REALLY_VERBOSE) {
                    Rprintf("  Allocating memory for  %d hits\n", nHits);
                }
                // int onlyPM = seq.GetProbeMapping();
                PROTECT(pmx = NEW_INTEGER(nHits));
                protectCount++;
                PROTECT(pmy = NEW_INTEGER(nHits));
                protectCount++;
                PROTECT(mmx = NEW_INTEGER(nHits));
                protectCount++;
                PROTECT(mmy = NEW_INTEGER(nHits));
                protectCount++;
                PROTECT(matchscore = NEW_NUMERIC(nHits));
                protectCount++;
                PROTECT(strand = NEW_INTEGER(nHits));
                protectCount++;
                PROTECT(probelength = NEW_INTEGER(nHits));
                protectCount++;
                PROTECT(startpos = NEW_INTEGER(nHits));
                protectCount++;
                PROTECT(centerpos = NEW_INTEGER(nHits));
                protectCount++;
                PROTECT(probeseq = NEW_CHARACTER(nHits));
                protectCount++;
                if (i_verboseFlag >= R_AFFX_REALLY_VERBOSE) {
                    Rprintf("Reading hit number:\n");
                }

                for(int j = 0; j < nHits; j++)
                    {
                        if(j % 1000 == 999) {
                            R_CheckUserInterrupt();
                            if (i_verboseFlag >= R_AFFX_REALLY_VERBOSE) {
                                Rprintf("  %d\n", j + 1);
                            }
                        }
                        seq.GetHitItem(j, seqHit, true);
                        INTEGER(pmx)[j] = seqHit.PMX;
                        INTEGER(pmy)[j] = seqHit.PMY;
                        INTEGER(mmx)[j] = seqHit.MMX;
                        INTEGER(mmy)[j] = seqHit.MMY;
                        INTEGER(strand)[j] = (unsigned int)seqHit.TopStrand;
                        SET_STRING_ELT(probeseq, j, mkChar(seqHit.PMProbe.c_str()));
                        INTEGER(startpos)[j] = seqHit.getStartPosition();
                        INTEGER(centerpos)[j] = seqHit.getCenterPosition();
                        REAL(matchscore)[j] = (double)seqHit.MatchScore;
                        INTEGER(probelength)[j] = (int)seqHit.ProbeLength;
                    }
                /* Now it is time to finalize the seqObj */
                SET_VECTOR_ELT(seqObj, 1, pmx);;
                SET_STRING_ELT(seqObjNames, 1, mkChar("pmx"));
                SET_VECTOR_ELT(seqObj, 2, pmy);;
                SET_STRING_ELT(seqObjNames, 2, mkChar("pmy"));
                SET_VECTOR_ELT(seqObj, 3, mmx);;
                // FIXME: only return in case of a pm-mm probe mapping
                SET_STRING_ELT(seqObjNames, 3, mkChar("mmx"));
                SET_VECTOR_ELT(seqObj, 4, mmy);;
                SET_STRING_ELT(seqObjNames, 4, mkChar("mmy"));
                SET_VECTOR_ELT(seqObj, 5, probeseq);
                SET_STRING_ELT(seqObjNames, 5, mkChar("probeseq"));
                SET_VECTOR_ELT(seqObj, 6, strand);
                SET_STRING_ELT(seqObjNames, 6, mkChar("strand"));
                SET_VECTOR_ELT(seqObj, 7, startpos);
                SET_STRING_ELT(seqObjNames, 7, mkChar("startpos"));
                SET_VECTOR_ELT(seqObj, 8, centerpos);
                SET_STRING_ELT(seqObjNames, 8, mkChar("centerpos"));
                SET_VECTOR_ELT(seqObj, 9, probelength);
                SET_STRING_ELT(seqObjNames, 9, mkChar("probelength"));
                SET_VECTOR_ELT(seqObj, 10, matchscore);
                SET_STRING_ELT(seqObjNames, 10, mkChar("matchscore"));
                if (i_verboseFlag >= R_AFFX_VERBOSE) {
                    Rprintf("Finalizing sequence %s\n", seq.FullName().c_str());
                }
                setAttrib(seqObj, R_NamesSymbol, seqObjNames);
                SET_VECTOR_ELT(resultList, i, seqObj);
                SET_STRING_ELT(resultListNames, i, 
                               mkChar(seq.FullName().c_str()));
                UNPROTECT(protectCount);
            }
    
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
     * Return stuff
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        
        if (i_verboseFlag >= R_AFFX_REALLY_VERBOSE) {
            Rprintf("Finalizing BPMAP object\n");
        }
        
        bpmap.Close();
        setAttrib(resultList, R_NamesSymbol, resultListNames);
        UNPROTECT(2);
        return resultList;
    }/* R_affx_get_bpmap_file() */


    /*********************************************************
     *********************************************************

               File Writers

     *********************************************************
     ********************************************************/


    SEXP R_affx_write_bpmap_file(SEXP bpname, SEXP tpname, SEXP verbose){
        char* bpmapFileName = CHAR(STRING_ELT(bpname,0));
        char* tpmapFileName = CHAR(STRING_ELT(tpname,0));
        affxbpmapwriter::CBPMAPFileWriter bpmap;
        bpmap.SetTpmapFileName(tpmapFileName);
        int i_verboseFlag = INTEGER(verbose)[0];
        if(bpmap.TpmapExists() ==  false){
            Rprintf("tpmap file %s does not exist.\n", tpmapFileName);
            return R_NilValue;
        }
        bpmap.SetTpmapFileName(tpmapFileName);
        if(i_verboseFlag >= R_AFFX_VERBOSE){
            Rprintf("Reading tpmap file: %s\n", tpmapFileName);
        }
        if(bpmap.ReadTpmap() == false){
            Rprintf("Tpmap file %s cannot be read\n", tpmapFileName);
            return R_NilValue;
        }
        if(i_verboseFlag >= R_AFFX_VERBOSE){
            Rprintf("Writing bpmap file %s\n", bpmapFileName);
        }
        bpmap.SetFileName(bpmapFileName);
        if(bpmap.WriteBpmap() == false){
            Rprintf("Bpmap file %s could not be written\n", bpmapFileName);
            return R_NilValue;
        }
        return R_NilValue;
    } /** end R_affx_write_bpmap_file **/
        
} /** end extern "C" **/

