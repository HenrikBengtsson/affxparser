#include <Rinternals.h>
#include <stdlib.h> // for NULL
#include <R_ext/Rdynload.h>

/* FIXME: 
   Check these declarations against the C/Fortran source code.
*/

/* .Call calls */
extern SEXP R_affx_cdf_groupNames(SEXP, SEXP, SEXP, SEXP);
extern SEXP R_affx_cdf_isPm(SEXP, SEXP, SEXP);
extern SEXP R_affx_cdf_nbrOfCellsPerUnitGroup(SEXP, SEXP, SEXP);
extern SEXP R_affx_get_bpmap_file(SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP);
extern SEXP R_affx_get_bpmap_header(SEXP);
extern SEXP R_affx_get_bpmap_seqinfo(SEXP, SEXP, SEXP);
extern SEXP R_affx_get_cdf_cell_indices(SEXP, SEXP, SEXP);
extern SEXP R_affx_get_cdf_file(SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP);
extern SEXP R_affx_get_cdf_file_header(SEXP);
extern SEXP R_affx_get_cdf_file_qc(SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP);
extern SEXP R_affx_get_cdf_unit_names(SEXP, SEXP, SEXP);
extern SEXP R_affx_get_cdf_units(SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP);
extern SEXP R_affx_get_cel_file(SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP);
extern SEXP R_affx_get_cel_file_header(SEXP);
extern SEXP R_affx_get_chp_file(SEXP, SEXP);
extern SEXP R_affx_get_clf_file(SEXP, SEXP, SEXP);
extern SEXP R_affx_get_pgf_file(SEXP, SEXP, SEXP, SEXP);
extern SEXP R_affx_write_bpmap_file(SEXP, SEXP, SEXP);

static const R_CallMethodDef CallEntries[] = {
    {"R_affx_cdf_groupNames",             (DL_FUNC) &R_affx_cdf_groupNames,              4},
    {"R_affx_cdf_isPm",                   (DL_FUNC) &R_affx_cdf_isPm,                    3},
    {"R_affx_cdf_nbrOfCellsPerUnitGroup", (DL_FUNC) &R_affx_cdf_nbrOfCellsPerUnitGroup,  3},
    {"R_affx_get_bpmap_file",             (DL_FUNC) &R_affx_get_bpmap_file,             12},
    {"R_affx_get_bpmap_header",           (DL_FUNC) &R_affx_get_bpmap_header,            1},
    {"R_affx_get_bpmap_seqinfo",          (DL_FUNC) &R_affx_get_bpmap_seqinfo,           3},
    {"R_affx_get_cdf_cell_indices",       (DL_FUNC) &R_affx_get_cdf_cell_indices,        3},
    {"R_affx_get_cdf_file",               (DL_FUNC) &R_affx_get_cdf_file,               15},
    {"R_affx_get_cdf_file_header",        (DL_FUNC) &R_affx_get_cdf_file_header,         1},
    {"R_affx_get_cdf_file_qc",            (DL_FUNC) &R_affx_get_cdf_file_qc,            10},
    {"R_affx_get_cdf_unit_names",         (DL_FUNC) &R_affx_get_cdf_unit_names,          3},
    {"R_affx_get_cdf_units",              (DL_FUNC) &R_affx_get_cdf_units,               9},
    {"R_affx_get_cel_file",               (DL_FUNC) &R_affx_get_cel_file,               11},
    {"R_affx_get_cel_file_header",        (DL_FUNC) &R_affx_get_cel_file_header,         1},
    {"R_affx_get_chp_file",               (DL_FUNC) &R_affx_get_chp_file,                2},
    {"R_affx_get_clf_file",               (DL_FUNC) &R_affx_get_clf_file,                3},
    {"R_affx_get_pgf_file",               (DL_FUNC) &R_affx_get_pgf_file,                4},
    {"R_affx_write_bpmap_file",           (DL_FUNC) &R_affx_write_bpmap_file,            3},
    {NULL, NULL, 0}
};

void R_init_affxparser(DllInfo *dll)
{
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
