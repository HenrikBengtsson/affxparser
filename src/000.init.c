#include <Rdefines.h>
#include <R_ext/Rdynload.h>
#include "000.api.h"

#define CALLDEF(name, n) {#name, (DL_FUNC) &name, n}

static R_CallMethodDef callMethods[]  = {
  CALLDEF(R_affx_cdf_groupNames, 4),
  CALLDEF(R_affx_cdf_isPm, 3),
  CALLDEF(R_affx_cdf_nbrOfCellsPerUnitGroup, 3),
  CALLDEF(R_affx_get_bpmap_file, 12),
  CALLDEF(R_affx_get_bpmap_header, 1),
  CALLDEF(R_affx_get_bpmap_seqinfo, 3),
  CALLDEF(R_affx_get_cdf_cell_indices, 3),
  CALLDEF(R_affx_get_cdf_file_header, 1),
  CALLDEF(R_affx_get_cdf_file_qc, 10),
  CALLDEF(R_affx_get_cdf_file, 15),
  CALLDEF(R_affx_get_cdf_unit_names, 3),
  CALLDEF(R_affx_get_cdf_units, 9),
  CALLDEF(R_affx_get_cel_file_header, 1),
  CALLDEF(R_affx_get_cel_file, 11),
  CALLDEF(R_affx_write_bpmap_file, 3),
  {NULL, NULL, 0}
};

void R_init_affxparser(DllInfo *info) {
  R_registerRoutines(info, NULL, callMethods, NULL, NULL);
  R_useDynamicSymbols(info, FALSE);
}
