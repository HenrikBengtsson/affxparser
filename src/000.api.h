/* C-level API that is called from R */
SEXP R_affx_cdf_groupNames(SEXP fname, SEXP units, SEXP truncateGroupNames, SEXP verbose);
SEXP R_affx_cdf_isPm(SEXP fname, SEXP units, SEXP verbose);
SEXP R_affx_cdf_nbrOfCellsPerUnitGroup(SEXP fname, SEXP units, SEXP verbose);
SEXP R_affx_get_bpmap_file(SEXP fname, SEXP seqindices,  SEXP readSeqInfo, SEXP readStartPos, SEXP readCenterPos, SEXP readProbeSeq, SEXP readStrand, SEXP readPMXY, SEXP readMMXY, SEXP readMatchScore, SEXP readProbeLength, SEXP verbose);
SEXP R_affx_get_bpmap_header(SEXP fname);
SEXP R_affx_get_bpmap_seqinfo(SEXP fname, SEXP seqindices, SEXP verbose);
SEXP R_affx_get_cdf_cell_indices(SEXP fname, SEXP units, SEXP verbose);
SEXP R_affx_get_cdf_file_header(SEXP fname);
SEXP R_affx_get_cdf_file_qc(SEXP fname, SEXP unitIndices, SEXP verbose, SEXP returnIndices, SEXP returnXY, SEXP returnLengths, SEXP returnPMInfo, SEXP returnBackgroundInfo, SEXP returnType, SEXP returnQCNumbers);
SEXP R_affx_get_cdf_file(SEXP fname, SEXP unitIndices, SEXP verbose, SEXP returnUnitType, SEXP returnUnitDirection, SEXP returnUnitAtomNumbers, SEXP returnUnitNumber, SEXP returnXY, SEXP returnIndices, SEXP returnBases, SEXP returnAtoms, SEXP returnIndexpos, SEXP returnIsPm, SEXP returnBlockDirection, SEXP returnBlockAtomNumbers);
SEXP R_affx_get_cdf_unit_names(SEXP fname, SEXP units, SEXP verbose);
SEXP R_affx_get_cdf_units(SEXP fname, SEXP units, SEXP readXY, SEXP readBases, SEXP readExpos, SEXP readType, SEXP readDirection, SEXP readIndices, SEXP verbose);
SEXP R_affx_get_cel_file_header(SEXP fname);
SEXP R_affx_get_cel_file(SEXP fname, SEXP readHeader, SEXP readIntensities, SEXP readX, SEXP readY, SEXP readPixels, SEXP readStdvs, SEXP readOutliers, SEXP readMasked, SEXP indices, SEXP verbose);
SEXP R_affx_write_bpmap_file(SEXP bpname, SEXP tpname, SEXP verbose);
