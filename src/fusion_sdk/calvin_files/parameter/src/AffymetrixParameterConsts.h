////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License 
// (version 2.1) as published by the Free Software Foundation.
// 
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
// for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
//
////////////////////////////////////////////////////////////////


#ifndef _AffymetrixParameterConsts_HEADER_
#define _AffymetrixParameterConsts_HEADER_

/*! \file AffymetrixParameterConsts.h This file defines constant names for static attributes.
 */

namespace affymetrix_calvin_parameter
{
/*! Defines the static attribute name for the probe array type of the physical array. */
#define ARRAY_TYPE_PARAM_NAME L"affymetrix-array-type"

/*! Defines the static attribute name for the master file. */
#define MASTER_FILE_PARAM_NAME L"affymetrix-master-file"

/*! Defines the static attribute name for the library package. */
#define LIBRARY_PACKAGE_PARAM_NAME L"affymetrix-library-package"

/*! Defines the number of characters to reserve in the parameter list for the array type name */
#define ARRAY_TYPE_MAX_LEN 100

/*! Defines the number of characters to reserce in the parameter list for the array barcode. */
#define ARRAY_BARCODE_MAX_LEN 50

/*! Defines the static attribute name for the barcode of the physical array. */
#define ARRAY_BARCODE_PARAM_NAME L"affymetrix-array-barcode"

/*! Defines the static attribute name for the array lot number of the physical array. */
#define ARRAY_LOT_PARAM_NAME L"affymetrix-array-lot"

/*! Defines the static attribute name for the expiration date of the physical array. */
#define ARRAY_EXPIRE_DATE_PARAM_NAME L"affymetrix-array-expiration-date"

/*! Defines the static attribute name for the barcode of the array plate. */
#define PLATE_BARCODE_PARAM_NAME L"affymetrix-plate-barcode"

/*! Defines the static attribute name for the plate type of the array plate. */
#define PLATE_TYPE_PARAM_NAME L"affymetrix-plate-type"

/*! Defines the static attribute name for the row location of the array plate well. */
#define PLATE_WELL_ROW_PARAM_NAME L"affymetrix-plate-well-row"

/*! Defines the static attribute name for the column location of the array plate well. */
#define PLATE_WELL_COL_PARAM_NAME L"affymetrix-plate-well-col"

/*! Defines the static attribute name for the exposure time for the plate. */
#define PLATE_EXPOSURE_TIME L"affymetrix-plate-exposuretime"

/*! Defines the static attribute name for the barcode of the plate. */
#define PLATE_BARCODE L"affymetrix-plate-barcode"

/*! Defines the static attribute name for the well position of the plate. */
#define PLATE_PEG_WELL_POSITION L"affymetrix-plate-peg-wellposition"

/*! Defines the static attribute name for the index of the subimage of the array plate. */
#define PLATE_PEG_SUBIMAGE_INDEX L"affymetrix-plate-peg-subimage-index"

/*! Defines the static attribute name for the GOS datFileName. */
#define PLATE_GCOS_DATFILENAME L"affymetrix-plate-gcos-datfilename"

/*! Defines the static attribute name for the plate GUID. */
#define PLATE_GUID L"affymetrix-plate-GUID"

/*! Defines the static attribute name for the plate peg GUID. */
#define PLATE_PLATE_PEG_GUID L"affymetrix-plate-peg-GUID"


#define AFFY_SCAN_PARAMETER_PREFIX	L"affymetrix-scanparameter-"
#define AFFY_SCAN_PARAMETER_PREFIX_S std::wstring(AFFY_SCAN_PARAMETER_PREFIX)


/*! Defines the static attribute name for the scanner array name. */
#define SCANNER_ARRAY_NAME L"arrayname"

/*! Defines the static attribute name for the array part name. */
#define SCANNER_ARRAY_PART_NUMBER L"array-part-number"

/*! Defines the static attribute name for the media type. */
#define SCANNER_MEDIA_TYPE L"media-type"

/*! Defines the static attribute name for the scanner feature size. */
#define SCANNER_FEATURE_SIZE L"feature-size"

/*! Defines the static attribute name for the scanner array width. */
#define SCANNER_ARRAY_WIDTH L"array-width"

/*! Defines the static attribute name for the scanner array height. */
#define SCANNER_ARRAY_HEIGHT L"array-height"

/*! Defines the static attribute name for the scanner array focus-width. */
#define SCANNER_ARRAY_FOCUS_WIDTH L"array-focus-width"

/*! Defines the static attribute name for the scanner array focus-height. */
#define SCANNER_ARRAY_FOCUS_HEIGHT L"array-focus-height"

/*! Defines the static attribute name for the scanner array num-wave-lengths. */
#define SCANNER_ARRAY_NUM_WAVE_LENGHTS L"array-num-wave-lengths"

/*! Defines the static attribute name for the scanner array num-wave-lengths1. */
#define SCANNER_ARRAY_WAVE_LENGHTS1 L"array-wave-lengths1"

/*! Defines the static attribute name for the scanner array num-wave-lengths2. */
#define SCANNER_ARRAY_WAVE_LENGHTS2 L"array-wave-lengths2"

/*! Defines the static attribute name for the scanner array num-wave-lengths3. */
#define SCANNER_ARRAY_WAVE_LENGHTS3 L"array-wave-lengths3"

/*! Defines the static attribute name for the scanner array num-wave-lengths4. */
#define SCANNER_ARRAY_WAVE_LENGHTS4 L"array-wave-lengths4"

/*! Defines the static attribute name for the scanner array num-wave-lengths5. */
#define SCANNER_ARRAY_WAVE_LENGHTS5 L"array-wave-lengths5"

/*! Defines the static attribute name for the scanner array numx-sub-arrays. */
#define SCANNER_ARRAY_NUMX_SUB_ARRAYS L"array-numx-sub-arrays"

/*! Defines the static attribute name for the scanner array numy-sub-arrays. */
#define SCANNER_ARRAY_NUMY_SUB_ARRAYS L"array-numy-sub-arrays"

/*! Defines the static attribute name for the scanner sub array xoffset-um. */
#define SCANNER_SUBARRAY_XOFFSET_UM L"sub-array-xoffset-um"

/*! Defines the static attribute name for the scanner sub array yoffset-um. */
#define SCANNER_SUBARRAY_YOFFSET_UM L"sub-array-yoffset-um"

/*! Defines the static attribute name for the scanner sub array xspacing-um. */
#define SCANNER_SUBARRAY_XSPACING_UM L"sub-array-xspacing-um"

/*! Defines the static attribute name for the scanner sub array yspacing-um. */
#define SCANNER_SUBARRAY_YSPACING_UM L"sub-array-yspacing-um"

/*! Defines the static attribute name for the scanner sub array af target ul-xum. */
#define SCANNER_ARRAY_AF_TARGET_UL_XUM L"array-af-target-ul-xum"

/*! Defines the static attribute name for the scanner sub array af target ul-yum. */
#define SCANNER_ARRAY_AF_TARGET_UL_YUM L"array-af-target-ul-yum"

/*! Defines the static attribute name for the scanner sub array af target lr-xum. */
#define SCANNER_ARRAY_AF_TARGET_LR_XUM L"array-af-target-lr-xum"

/*! Defines the static attribute name for the scanner sub array af target lr-yum. */
#define SCANNER_ARRAY_AF_TARGET_LR_YUM L"array-af-target-lr-yum"

/*! Defines the static attribute name for the scanner array num exposure times. */
#define SCANNER_ARRAY_NUM_EXPOSURE_TIMES L"array-num-exposure-times"

/*! Defines the static attribute name for the scanner sub array exposure times ms1. */
#define SCANNER_ARRAY_EXPOSURE_TIMES_MS1 L"array-exposure-times-ms1"

/*! Defines the static attribute name for the scanner sub array exposure times ms2. */
#define SCANNER_ARRAY_EXPOSURE_TIMES_MS2 L"array-exposure-times-ms2"

/*! Defines the static attribute name for the scanner sub array exposure times ms3. */
#define SCANNER_ARRAY_EXPOSURE_TIMES_MS3 L"array-exposure-times-ms3"

/*! Defines the static attribute name for the scanner sub array exposure times ms4. */
#define SCANNER_ARRAY_EXPOSURE_TIMES_MS4 L"affymetrix-scanner-array-exposure-times-ms4"

/*! Defines the static attribute name for the algorithm name. */
#define ALGORITHM_NAME_PARAM_NAME L"affymetrix-algorithm-name"

/*! A prefix for chip summary parameter ids. */
#define CHIP_SUMMARY_PARAMETER_NAME_PREFIX L"affymetrix-chipsummary-"

/*! A prefix for chip summary parameter ids. */
#define CHIP_SUMMARY_PARAMETER_NAME_PREFIX_S std::wstring(CHIP_SUMMARY_PARAMETER_NAME_PREFIX)

/*! Defines the static attribute prefix for algorithm parameter names */
#define ALGORITHM_PARAM_NAME_PREFIX L"affymetrix-algorithm-param-"

/*! Defines the static attribute prefix (as a wstring) for algorithm parameter names */
#define ALGORITHM_PARAM_NAME_PREFIX_S std::wstring(ALGORITHM_PARAM_NAME_PREFIX)

/*! Defines the static attribute prefix for the application meta data. */
#define APPLICATION_META_INFO_PREFIX L"affymetrix-application-meta-data-info-"

/*! Defines the static attribute prefix (as a wstring) for the application meta data. */
#define APPLICATION_META_INFO_PREFIX_S std::wstring(APPLICATION_META_INFO_PREFIX)

/*! Defines the static attribute name for the DATHeader */
#define DAT_HEADER_PARAM_NAME L"affymetrix-dat-header"

/*! Defines the static attribute name for the partial DATHeader */
#define PARTIAL_DAT_HEADER_PARAM_NAME L"affymetrix-partial-dat-header"

/*! Defines the static attribute name for the max pixel intensity */
#define MAX_PIXEL_INTENSITY_PARAM_NAME L"affymetrix-max-pixel-intensity"

/*! Defines the static attribute name for the min pixel intensity */
#define MIN_PIXEL_INTENSITY_PARAM_NAME L"affymetrix-min-pixel-intensity"

/*! Defines the static attribute name for the orientation */
#define ORIENTATION_PARAM_NAME L"affymetrix-image-orientation"

/*! Defines the static attribute name for the file version.  This is not the file format version. */
#define FILE_VERSION_PARAM_NAME L"affymetrix-file-version"

/*! Defines the static attribute name for the flip-flag which indicates if an image is flipped about the y-axis. */
#define FLIP_FLAG_PARAM_NAME L"affymetrix-image-flip-flag"

/*! Defines the static attribute name for the filter wavelength. */
#define FILTER_PARAM_NAME L"affymetrix-filter-wavelength"

/*! Defines the static attribute name for the array id */
#define ARRAY_ID_PARAM_NAME L"affymetrix-array-id"

/*! Defines the static attribute name for the pixel size */
#define PIXEL_SIZE_PARAM_NAME L"affymetrix-pixel-size"

/*! Defines the static attribute name for the scanner type */
#define SCANNER_TYPE_PARAM_NAME L"affymetrix-scanner-type"

/*! Defines the static attribute name for the scanner id */
#define SCANNER_ID_PARAM_NAME L"affymetrix-scanner-id"

/*! Defines the static attribute name for the scan date */
#define SCAN_DATE_PARAM_NAME L"affymetrix-scan-date"

/*! Defines the static attribute name for the number of pixel rows */
#define ROWS_PARAM_NAME L"affymetrix-pixel-rows"

/*! Defines the static attribute name for the number of pixel cols */
#define COLS_PARAM_NAME L"affymetrix-pixel-cols"

/*! Defines the static attribute name for the number of feature rows */
#define FEATURE_ROWS_NAME L"affymetrix-feature-rows"

/*! Defines the static attribute name for the number of feature cols */
#define FEATURE_COLS_NAME L"affymetrix-feature-cols"

/*! Defines the static attribute for the algorithm version. */
#define ALG_VERSION_PARAM_NAME std::wstring(L"affymetrix-algorithm-version")

/*! Defines the static attribute name for the number of cel rows */
#define CEL_ROWS_PARAM_NAME L"affymetrix-cel-rows"

/*! Defines the static attribute name for the number of cel columns */
#define CEL_COLS_PARAM_NAME L"affymetrix-cel-cols"

/*! Defines the static attribute name for the program company.  This is the company that created the program. */
#define PROGRAM_COMPANY L"program-company"

/*! Defines the static attribute name for the program name.  This is the program that created the file. */
#define PROGRAM_NAME L"program-name"

/*! Defines the static attribute name for the program id.  This is a version string or other identifier of the particular program. */
#define PROGRAM_ID L"program-id"

/*! CDF Data Type Expression */
#define AFFY_EXPR_PS "affymetrix-expression-probesets"

/*! CDF Data Type Genotyping */
#define AFFY_GENO_PS "affymetrix-genotyping-probesets"

/*! CDF Data Type Tag */
#define AFFY_TAG_PS "affymetrix-tag-probesets"

/*! CDF Data Type Resequencing */
#define AFFY_RESEQ_PS "affymetrix-resequencing-probesets"

/*! CDF Data Type Control */
#define AFFY_CNTRL_PS "affymetrix-control-probesets"

/*! Defines US English locale. */
#define US_ENGLISH_LOCALE L"en-US"

/*! Defines the reserve length of an affymetrix GUID */
#define AFFY_GUID_LEN 55

/*! Defines the parent file identifier GUID */
#define AFFY_PARENT_FILE_IDENTIFIER L"affymetrix-parent-dat-file-identifier"

/*! Defines the exposure time string */
#define AFFY_EXPOSURE_TIME L"affymetrix-exposure-time"

/*! Defines attribute name for array parameter */
#define AFFY_ARRAY_PARAMETER_PREFIX	L"affymetrix-ArrayParameter-"
#define AFFY_ARRAY_PARAMETER_PREFIX_S std::wstring(AFFY_ARRAY_PARAMETER_PREFIX)

/*! Defines attribute name for array parameter, feature columns */
#define FEATURE_COLUMNS			L"featureColumns"

/*! Defines attribute name for array parameter, feature rows */
#define FEATURE_ROWS			L"featureRows"

/*! Defines attribute name for array parameter, feature setback */
#define FEATURE_SET_BACK		L"featureSetback"

/*! Defines attribute name for array parameter, feature height */
#define FEATURE_HEIGHT			L"featureHeight"

/*! Defines attribute name for array parameter, feature width */
#define FEATURE_WIDTH			L"featureWidth"

/*! Defines attribute name for array parameter, part number */
#define PART_NUMBER				L"partNumber"

/*! Defines attribute name for array parameter, test type */
#define TEST_TYPE				L"testType"

#define AFFY_FILE_CREATOR		L"affymetrix-file-creator"

/*! Defines attribute name for fludics parameter */
#define AFFY_FLUIDICS_PARAMETER_PREFIX	L"affymetrix-"
#define AFFY_FLUIDICS_PARAMETER_PREFIX_S std::wstring(AFFY_FLUIDICS_PARAMETER_PREFIX)

}

#endif
