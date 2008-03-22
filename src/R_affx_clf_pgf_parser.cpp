// FIXME: TsvFile.cpp:444 has _strtoui64 on WIN32; minGW wants strtoull
#include "ClfFile.h"
#include "PgfFile.h"
#include "TsvFile.h"
#include <string>

using namespace std;
using namespace affx;

#include "RAffxErrHandler.h"
#include <Rdefines.h>

int *
new_int_elt(const char* symbol, int length, SEXP rho)
{
  SEXP tmp;
  PROTECT(tmp = NEW_INTEGER(length));
  defineVar(install(symbol), tmp, rho);
  UNPROTECT(1);
  return INTEGER(tmp);
}
  
SEXP
new_char_elt(const char* symbol, int length, SEXP rho)
{
  SEXP tmp;
  PROTECT(tmp = NEW_CHARACTER(length));
  defineVar(install(symbol), tmp, rho);
  UNPROTECT(1);
  return tmp;
}

SEXP
R_affx_read_tsv_header(TsvFile& tsv)
{
  string key, value;
  tsv.headersBegin();
  // how many chip_type and other headers?
  int nOtherHeaders = 0, nChipTypeHeaders = 0;
  while (tsv.headersNext(key, value) == TSV_OK) {
    if (key=="chip_type") {
      ++nChipTypeHeaders;
    } else {
      ++nOtherHeaders;
    }
  }
  SEXP headers, headerNames, chipTypeHeaders;
  PROTECT(headers = NEW_LIST(nOtherHeaders+1));
  PROTECT(headerNames = NEW_CHARACTER(nOtherHeaders+1));
  PROTECT(chipTypeHeaders = NEW_CHARACTER(nChipTypeHeaders));

  tsv.headersBegin();
  nChipTypeHeaders = 0;
  nOtherHeaders = 1;
  while (tsv.headersNext(key, value) == TSV_OK) {
    if (key=="chip_type") {
      SET_STRING_ELT(chipTypeHeaders, nChipTypeHeaders++, 
                     mkChar(value.c_str()));
    } else {
      SET_ELEMENT(headers, nOtherHeaders, mkString(value.c_str()));
      SET_STRING_ELT(headerNames, nOtherHeaders, mkChar(key.c_str()));
      ++nOtherHeaders;
    }
  }
  SET_ELEMENT(headers, 0, chipTypeHeaders);
  SET_STRING_ELT(headerNames, 0, mkChar("chip_type"));
  SET_NAMES(headers, headerNames);
  UNPROTECT(3);
  return headers;
}

void
R_affx_get_body(ClfFile* clf, SEXP rho)
{
  int nx, ny;
  nx = clf->getXMax() + 1;
  ny = clf->getYMax() + 1;

  int *dims, *id, *x, *y;
  dims = new_int_elt("dims", 2, rho);
  id = new_int_elt("id", nx*ny, rho);
  x = new_int_elt("x", nx*ny, rho);
  y = new_int_elt("y", nx*ny, rho);

  dims[0] = nx; dims[1] = ny;
  while(clf->next_probe() == TSV_OK) {
    *id++ = clf->probe_id;
    *x++ = clf->x;
    *y++ = clf->y;
  }
}

void
R_affx_get_body(PgfFile* pgf, SEXP rho, SEXP indices)
{
    // Count
    int nProbesets = 0, nAtoms = 0, nProbes = 0, i = 0;

    while (pgf->next_probeset() == TSV_OK) {
        if (nProbesets >= LENGTH(indices)) {
            break;
        }
        
        if (indices == R_NilValue || (INTEGER(indices)[nProbesets] - 1) == i++) {
            ++nProbesets;
            while (pgf->next_atom() == TSV_OK) {
                ++nAtoms;
                while (pgf->next_probe() == TSV_OK) {
                    ++nProbes;
                }
            }
        }
    }
    pgf->rewind();
    
    // Read
    SEXP probeset_type, probeset_name, // atom_type, 
        probe_type, probe_sequence;
    int *probeset_id, *probeset_start_atom, 
        *atom_id, *atom_exon_position, *atom_start_probe,
        *probe_id, *probe_gc_count, *probe_length, 
        *probe_interrogation_position;
    
    // probeset
    probeset_id = new_int_elt("probesetId", nProbesets, rho);
    probeset_type = new_char_elt("probesetType", nProbesets, rho);
    probeset_name = new_char_elt("probesetName", nProbesets, rho);
    probeset_start_atom = new_int_elt("probesetStartAtom", nProbesets, rho);
    // atom
    atom_id = new_int_elt("atomId", nAtoms, rho);
    // FIXME: where's atom_type? in docs but not .h or .cpp
    //   atom_type = new_char_elt("atomType", nAtoms, rho);
    atom_exon_position = new_int_elt("atomExonPosition", nAtoms, rho);
    atom_start_probe = new_int_elt("atomStartProbe", nAtoms, rho);
    // probe
    probe_id = new_int_elt("probeId", nProbes, rho);
    probe_type = new_char_elt("probeType", nProbes, rho);
    probe_gc_count = new_int_elt("probeGcCount", nProbes, rho);
    probe_length = new_int_elt("probeLength", nProbes, rho);
    probe_interrogation_position = 
        new_int_elt("probeInterrogationPosition", nProbes, rho);
    probe_sequence = new_char_elt("probeSequence", nProbes, rho);
    
    nProbesets = nAtoms = nProbes = i = 0;
    while(pgf->next_probeset() == TSV_OK) {
        if (nProbesets >= LENGTH(indices)) {
            break;
        }

        if (indices == R_NilValue || (INTEGER(indices)[nProbesets] - 1) == i++) {
            probeset_id[nProbesets] = pgf->probeset_id;
            SET_STRING_ELT(probeset_type, nProbesets, 
                           mkChar(pgf->probeset_type.c_str()));
            SET_STRING_ELT(probeset_name, nProbesets, 
                           mkChar(pgf->probeset_name.c_str()));
            probeset_start_atom[nProbesets] = 1 + nAtoms;
            ++nProbesets;
            while (pgf->next_atom() == TSV_OK) {
                atom_id[nAtoms] = pgf->atom_id;
                // FIXME: where's atom_type? in docs but not header
                atom_exon_position[nAtoms] = pgf->exon_position;
                atom_start_probe[nAtoms] = 1 + nProbes;
                ++nAtoms;
                while (pgf->next_probe() == TSV_OK) {
                    probe_id[nProbes] = pgf->probe_id;
                    SET_STRING_ELT(probe_type, nProbes, 
                                   mkChar(pgf->probe_type.c_str()));
                    probe_gc_count[nProbes] = pgf->gc_count;
                    probe_length[nProbes] = pgf->probe_length;
                    probe_interrogation_position[nProbes] = 
                        pgf->interrogation_position;
                    SET_STRING_ELT(probe_sequence, nProbes,
                                   mkChar(pgf->probe_sequence.c_str()));
                    ++nProbes;
                }
            }
        }
    }
}

extern "C" {

  SEXP
  R_affx_get_clf_file(SEXP fname, SEXP readBody, SEXP rho)
  {
    if (IS_CHARACTER(fname) == FALSE || LENGTH(fname) != 1)
      Rf_error("argument '%s' should be '%s'", "fname",
               "character(1)");
    if (IS_LOGICAL(readBody) == FALSE || LENGTH(readBody) !=1)
      Rf_error("argument '%s' should be '%s'", "readBody",
               "logical(1)");
    if (TYPEOF(rho) != ENVSXP)
      Rf_error("argument '%' should be '%s'", "rho", "environment");
    
    const char *clfFileName = CHAR(STRING_ELT(fname, 0));

    ClfFile *clf = new ClfFile();
    
    // FIXME: shortcut with sequential headers -- id, x, y are sequences
    try {
      RAffxErrHandler *err = new RAffxErrHandler(true);
      Err::pushHandler(err);
      if (clf->open(string(clfFileName)) != TSV_OK) {
        delete clf;
        Rf_error("could not open clf file '%s'", clfFileName);
      }
      // header
      SEXP tmp;
      PROTECT(tmp = R_affx_read_tsv_header(clf->m_tsv));
      defineVar(install("header"), tmp, rho);
      UNPROTECT(1);
      if (LOGICAL(readBody)[0] == TRUE) {
          R_affx_get_body(clf, rho);
      }
      delete Err::popHandler();
    } catch (Except ex) {
      delete Err::popHandler();
      clf->close();
      delete clf;
      Rf_error("%s", ex.what());
    }

    clf->close();
    delete clf;
    return rho;
  }

  SEXP 
  R_affx_get_pgf_file(SEXP fname, SEXP readBody, SEXP rho, SEXP indices)
  {
    if (IS_CHARACTER(fname) == FALSE || LENGTH(fname) != 1)
      Rf_error("argument '%s' should be '%s'", "fname",
               "character(1)");
    if (IS_LOGICAL(readBody) == FALSE || LENGTH(readBody) != 1)
      Rf_error("argument '%s' should be '%s'", "readBody",
               "logical(1)");
    if (TYPEOF(rho) != ENVSXP)
      Rf_error("argument '%' should be '%s'", "rho", "environments");

    const char *pgfFileName = CHAR(STRING_ELT(fname, 0));

    PgfFile *pgf = new PgfFile();
    try {
      RAffxErrHandler *err = new RAffxErrHandler(true);
      Err::pushHandler(err);
      if (pgf->open(string(pgfFileName)) != TSV_OK) {
        delete pgf;
        Rf_error("could not open pgf file '%s'", pgfFileName);
      }
      SEXP tmp;
      PROTECT(tmp = R_affx_read_tsv_header(pgf->m_tsv));
      defineVar(install("header"), tmp, rho);
      UNPROTECT(1);
      if (LOGICAL(readBody)[0] == TRUE) {
          R_affx_get_body(pgf, rho, indices);
      }
      pgf->close();
      delete Err::popHandler();
    } catch (Except ex) {
      delete Err::popHandler();	// errors now are fatal
      pgf->close();
      delete pgf;
      Rf_error("%s", ex.what());
    }

    delete pgf;
    return rho;
  }
}
