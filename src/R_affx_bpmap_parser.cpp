#include "BPMAPFileData.h"
#include "BPMAPFileWriter.h"
// #include "FusionBPMAPData.h"
#include "R_affx_constants.h"

using namespace std;

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
        const char* bpmapFileName   = CHAR(STRING_ELT(fname,0));

        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
         * Opens stuff
         * - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        bpmap.SetFileName(bpmapFileName);
        if (bpmap.ReadHeader() == false) {
            error("Unable to read file: %s\n", bpmapFileName);
            return R_NilValue;
        }

        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
         * Define Return Object: returnList, returnListNames
         *
         * We also read the object.
         * - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        SEXP returnList, returnListNames, tmp;
        int kk =0;

        PROTECT(returnList = NEW_LIST(2));
        PROTECT(returnListNames = NEW_CHARACTER(2));

        SET_STRING_ELT(returnListNames, kk, mkChar("version"));
        tmp = NEW_NUMERIC(1);
        SET_VECTOR_ELT(returnList, kk++, tmp);
        REAL(tmp)[0] = bpmap.GetVersion();

        SET_STRING_ELT(returnListNames, kk, mkChar("numSequences"));
        tmp = NEW_INTEGER(1);
        SET_VECTOR_ELT(returnList, kk++, tmp);
        INTEGER(tmp)[0] = bpmap.GetNumberSequences();

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

    SEXP R_affx_bpmap_seqinfo_item(affxbpmap::CGDACSequenceItem &seq, 
                                   int i_verboseFlag){
                if (i_verboseFlag >= R_AFFX_REALLY_VERBOSE) {
                    Rprintf("  Reading seqInfo\n");
                }
                SEXP seqInfo, seqInfoNames,  
                    seqInfoParameters, seqInfoParameterNames;
                int kk = 0;
                string str;
                int str_length; 
                char* cstr; 

                PROTECT(seqInfo = NEW_LIST(8));
                PROTECT(seqInfoNames = NEW_CHARACTER(8));

                SET_STRING_ELT(seqInfoNames, kk, mkChar("name"));
								str = seq.GetName();
								str_length = str.size();
								cstr = Calloc(str_length+1, char);
								strncpy(cstr, str.c_str(), str_length);
								cstr[str_length] = '\0';
                SET_VECTOR_ELT(seqInfo, kk++, ScalarString(mkChar(cstr)));
                Free(cstr);

                SET_STRING_ELT(seqInfoNames, kk, mkChar("groupname"));
								str = seq.GroupName();
								str_length = str.size();
								cstr = Calloc(str_length+1, char);
								strncpy(cstr, str.c_str(), str_length);
								cstr[str_length] = '\0';
                SET_VECTOR_ELT(seqInfo, kk++, 
                               ScalarString(mkChar(cstr)));
                Free(cstr);

                SET_STRING_ELT(seqInfoNames, kk, mkChar("fullname"));
								str = seq.FullName();
								str_length = str.size();
								cstr = Calloc(str_length+1, char);
								strncpy(cstr, str.c_str(), str_length);
								cstr[str_length] = '\0';
                SET_VECTOR_ELT(seqInfo, kk++, 
                               ScalarString(mkChar(cstr)));
                Free(cstr);

                SET_STRING_ELT(seqInfoNames, kk, mkChar("version"));
								str = seq.GetSeqVersion();
								str_length = str.size();
								cstr = Calloc(str_length+1, char);
								strncpy(cstr, str.c_str(), str_length);
								cstr[str_length] = '\0';
                SET_VECTOR_ELT(seqInfo, kk++, 
                               ScalarString(mkChar(cstr)));
                Free(cstr);
                
                SET_STRING_ELT(seqInfoNames, kk, mkChar("mapping"));
                if(seq.GetProbeMapping() == 0)
                    SET_VECTOR_ELT(seqInfo, kk++, ScalarString(mkChar("pmmm")));
                else
                    SET_VECTOR_ELT(seqInfo, kk++, ScalarString(mkChar("onlypm")));

                SET_STRING_ELT(seqInfoNames, kk, mkChar("number"));
                SET_VECTOR_ELT(seqInfo, kk++, 
                               ScalarInteger(seq.GetNumber() + 1));

                SET_STRING_ELT(seqInfoNames, kk, mkChar("numberOfHits"));
                SET_VECTOR_ELT(seqInfo, kk++, 
                               ScalarInteger(seq.GetNumberHits()));

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
													str = seq.GetParameter(j).Tag;
													str_length = str.size();
													cstr = Calloc(str_length+1, char);
													strncpy(cstr, str.c_str(), str_length);
													cstr[str_length] = '\0';
                          SET_STRING_ELT(seqInfoParameterNames, j, 
                                          mkChar(cstr));
	  											Free(cstr);
													str = seq.GetParameter(j).Value;
													cstr = Calloc(str_length+1, char);
													strncpy(cstr, str.c_str(), str_length);
													cstr[str_length] = '\0';
                          SET_STRING_ELT(seqInfoParameters, j, 
                                           mkChar(cstr));
		  										Free(cstr);
                        }
                    setAttrib(seqInfoParameters, R_NamesSymbol, 
                              seqInfoParameterNames);
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
        string str;
        int str_length; 
        char* cstr; 

        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - -
         * Process arguments
         * - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        const char* bpmapFileName   = CHAR(STRING_ELT(fname,0));
        int i_verboseFlag     = INTEGER(verbose)[0];

        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
         * Opens stuff
         * - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        if (i_verboseFlag >= R_AFFX_VERBOSE) {
            Rprintf("attempting to read: %s\n", bpmapFileName);
        }
        bpmap.SetFileName(bpmapFileName);
        if (bpmap.Exists() == false) {
            error("File does not exist: %s\n", bpmapFileName);
            return R_NilValue;
        }
        if (bpmap.Read() == false) {
            error("Unable to read file: %s, is it a BPMAP file?\n", 
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
                if(INTEGER(seqindices)[i] < 0 | 
                   INTEGER(seqindices)[i] > nSequences)
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
                Rprintf("Reading sequence object: %s\n", 
                        seq.GetName().c_str());
            }
            SET_VECTOR_ELT(returnList, i, 
                           R_affx_bpmap_seqinfo_item(seq, i_verboseFlag));
            str = seq.FullName();
						str_length = str.size();
						cstr = Calloc(str_length+1, char);
						strncpy(cstr, str.c_str(), str_length);
						cstr[str_length] = '\0';
            SET_STRING_ELT(returnListNames, i, mkChar(cstr));
            Free(cstr);
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


    SEXP R_affx_get_bpmap_file(SEXP fname, SEXP seqindices, 
                               SEXP readSeqInfo, SEXP readStartPos,
                               SEXP readCenterPos, SEXP readProbeSeq,
                               SEXP readStrand, SEXP readPMXY, SEXP readMMXY,
                               SEXP readMatchScore, SEXP readProbeLength,
                               SEXP verbose) 
    {
        affxbpmap::CBPMAPFileData bpmap;
        string str;
        int str_length; 
        char* cstr; 

        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - -
         * Process arguments
         * - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        const char* bpmapFileName   = CHAR(STRING_ELT(fname,0));
        int i_readSeqInfo     = INTEGER(readSeqInfo)[0];
        int i_readStartPos    = INTEGER(readStartPos)[0];
        int i_readCenterPos   = INTEGER(readCenterPos)[0];
        int i_readProbeSeq    = INTEGER(readProbeSeq)[0];
        int i_readStrand      = INTEGER(readStrand)[0];
        int i_readPMXY        = INTEGER(readPMXY)[0];
        int i_readMMXY        = INTEGER(readMMXY)[0];
        int i_readMatchScore  = INTEGER(readMatchScore)[0];
        int i_readProbeLength = INTEGER(readProbeLength)[0];
        int i_verboseFlag     = INTEGER(verbose)[0];

        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
         * Opens stuff
         * - - - - - - - - - - - - - - - - - - - - - - - - - - - */

        if (i_verboseFlag >= R_AFFX_VERBOSE) {
            Rprintf("attempting to read: %s\n", bpmapFileName);
        }
        bpmap.SetFileName(bpmapFileName);
        if (bpmap.Exists() == false) {
            error("File does not exist: %s\n", bpmapFileName);
            return R_NilValue;
        }
        if (bpmap.Read() == false) {
            error("Unable to read file: %s, is it a BPMAP file?\n", 
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
             matchscore, startpos, centerpos;

        affxbpmap::CGDACSequenceItem seq;
        affxbpmap::GDACSequenceHitItemType seqHit;
       
        int seqObjLength = i_readSeqInfo + i_readStartPos + 
            i_readCenterPos + i_readProbeSeq + i_readStrand + 
            2 * i_readPMXY + 2 * i_readMMXY + i_readMatchScore + 
            i_readProbeLength;
        int protectCount = 0;
        int kk = 0;
        
        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
         * Read the sequences.
         * - - - - - - - - - - - - - - - - - - - - - - - - - - - */

        for(int i = 0; i < nSequences; i++)
            {
                /* Initializing memory, we free it at the end of each sequence */ 
                protectCount = 0;
                kk = 0;
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
                if(i_readSeqInfo) {
                    SET_VECTOR_ELT(seqObj, kk, 
                                   R_affx_bpmap_seqinfo_item(seq, i_verboseFlag));
                    SET_STRING_ELT(seqObjNames, kk++, mkChar("seqInfo"));
                }
                
                int nHits = seq.GetNumberHits();
                if (i_verboseFlag >= R_AFFX_REALLY_VERBOSE) {
                    Rprintf("  Allocating memory for  %d hits\n", nHits);
                }
                int onlyPM = seq.GetProbeMapping();
                if (i_readPMXY) {
                    PROTECT(pmx = NEW_INTEGER(nHits));
                    protectCount++;
                    PROTECT(pmy = NEW_INTEGER(nHits));
                    protectCount++;
                }
                if (i_readMMXY and !onlyPM) {
                    PROTECT(mmx = NEW_INTEGER(nHits));
                    protectCount++;
                    PROTECT(mmy = NEW_INTEGER(nHits));
                    protectCount++;
                }
                if (i_readMatchScore) {
                    PROTECT(matchscore = NEW_NUMERIC(nHits));
                    protectCount++;
                }
                if (i_readStrand) {
                PROTECT(strand = NEW_INTEGER(nHits));
                protectCount++;
                }
                if (i_readProbeLength) {
                PROTECT(probelength = NEW_INTEGER(nHits));
                protectCount++;
                }
                if (i_readStartPos) {
                PROTECT(startpos = NEW_INTEGER(nHits));
                protectCount++;
                }
                if (i_readCenterPos) {
                PROTECT(centerpos = NEW_INTEGER(nHits));
                protectCount++;
                }
                if (i_readProbeSeq) {
                PROTECT(probeseq = NEW_CHARACTER(nHits));
                protectCount++;
                }
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
                        seq.GetHitItem(j, seqHit, i_readProbeSeq);
                        if (i_readPMXY) {
                        INTEGER(pmx)[j] = seqHit.PMX;
                        INTEGER(pmy)[j] = seqHit.PMY;
                        }
                        if (i_readMMXY & !onlyPM) {
                        INTEGER(mmx)[j] = seqHit.MMX;
                        INTEGER(mmy)[j] = seqHit.MMY;
                        }
                        if (i_readStrand) {
                        INTEGER(strand)[j] = (unsigned int)seqHit.TopStrand;
                        }
                        if (i_readProbeSeq) {
													str = seqHit.PMProbe;
													str_length = str.size();
													cstr = Calloc(str_length+1, char);
													strncpy(cstr, str.c_str(), str_length);
													cstr[str_length] = '\0';
                          SET_STRING_ELT(probeseq, j, mkChar(cstr));
													Free(cstr);
                        }
                        if (i_readStartPos) {
                            INTEGER(startpos)[j] = seqHit.getStartPosition();
                        }
                        if (i_readCenterPos) {
                        INTEGER(centerpos)[j] = seqHit.getCenterPosition();
                        }
                        if (i_readMatchScore) {
                            REAL(matchscore)[j] = (double)seqHit.MatchScore;
                        }
                        if (i_readProbeLength) {
                        INTEGER(probelength)[j] = (int)seqHit.ProbeLength;
                        }
                    }
                /* Now it is time to finalize the seqObj */
                if (i_readPMXY) {
                    SET_VECTOR_ELT(seqObj, kk, pmx);
                    SET_STRING_ELT(seqObjNames, kk++, mkChar("pmx"));
                    SET_VECTOR_ELT(seqObj, kk, pmy);
                    SET_STRING_ELT(seqObjNames, kk++, mkChar("pmy"));
                }
                if (i_readMMXY) {
                    if(!onlyPM) {
                        SET_VECTOR_ELT(seqObj, kk, mmx);
                        SET_STRING_ELT(seqObjNames, kk++, mkChar("mmx"));
                        SET_VECTOR_ELT(seqObj, kk, mmy);
                        SET_STRING_ELT(seqObjNames, kk++, mkChar("mmy"));
                    } else {
                        SET_VECTOR_ELT(seqObj, kk, R_NilValue);
                        SET_STRING_ELT(seqObjNames, kk++, mkChar("mmx"));
                        SET_VECTOR_ELT(seqObj, kk, R_NilValue);
                        SET_STRING_ELT(seqObjNames, kk++, mkChar("mmy"));
                    }
                }
                if (i_readProbeSeq) {
                    SET_VECTOR_ELT(seqObj, kk, probeseq);
                    SET_STRING_ELT(seqObjNames, kk++, mkChar("probeseq"));
                }
                if (i_readStrand) {
                    SET_VECTOR_ELT(seqObj, kk, strand);
                    SET_STRING_ELT(seqObjNames, kk++, mkChar("strand"));
                }
                if (i_readStartPos) {
                    SET_VECTOR_ELT(seqObj, kk, startpos);
                    SET_STRING_ELT(seqObjNames, kk++, mkChar("startpos"));
                }
                if (i_readCenterPos) {
                    SET_VECTOR_ELT(seqObj, kk, centerpos);
                    SET_STRING_ELT(seqObjNames, kk++, mkChar("centerpos"));
                }
                if (i_readProbeLength) {
                    SET_VECTOR_ELT(seqObj, kk, probelength);
                    SET_STRING_ELT(seqObjNames, kk++, mkChar("probelength"));
                }
                if (i_readMatchScore) {
                    SET_VECTOR_ELT(seqObj, kk, matchscore);
                    SET_STRING_ELT(seqObjNames, kk++, mkChar("matchscore"));
                }
                if (i_verboseFlag >= R_AFFX_VERBOSE) {
                    Rprintf("Finalizing sequence %s\n", seq.FullName().c_str());
                }
                setAttrib(seqObj, R_NamesSymbol, seqObjNames);
                SET_VECTOR_ELT(resultList, i, seqObj);
								str = seq.FullName();
								str_length = str.size();
								cstr = Calloc(str_length+1, char);
								strncpy(cstr, str.c_str(), str_length);
								cstr[str_length] = '\0';
                SET_STRING_ELT(resultListNames, i, mkChar(cstr));
                Free(cstr);
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
        const char* bpmapFileName = CHAR(STRING_ELT(bpname,0));
        const char* tpmapFileName = CHAR(STRING_ELT(tpname,0));
        affxbpmapwriter::CBPMAPFileWriter bpmap;
        bpmap.SetTpmapFileName(tpmapFileName);
        int i_verboseFlag = INTEGER(verbose)[0];

        if(bpmap.TpmapExists() ==  false){
            error("tpmap file %s does not exist.\n", tpmapFileName);
            return R_NilValue;
        }
        bpmap.SetTpmapFileName(tpmapFileName);
        if(i_verboseFlag >= R_AFFX_VERBOSE){
            Rprintf("Reading tpmap file: %s\n", tpmapFileName);
        }
        if(bpmap.ReadTpmap() == false){
            error("Tpmap file %s cannot be read\n", tpmapFileName);
            return R_NilValue;
        }
        if(i_verboseFlag >= R_AFFX_VERBOSE){
            Rprintf("Writing bpmap file %s\n", bpmapFileName);
        }
        bpmap.SetFileName(bpmapFileName);
        if(bpmap.WriteBpmap() == false){
            error("Bpmap file %s could not be written\n", bpmapFileName);
            return R_NilValue;
        }
        return R_NilValue;
    } /** end R_affx_write_bpmap_file **/
        
} /** end extern "C" **/

