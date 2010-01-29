////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007 Affymetrix, Inc.
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


#ifndef _ProbeSetMultiDataData_HEADER_
#define _ProbeSetMultiDataData_HEADER_

/*! \file ProbeSetMultiDataData.h This file provides types to hold MultiData results.
 */

//
#include "calvin_files/parameter/src/ParameterNameValueType.h"
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//
#include <cstring>
#include <sstream>
#include <string>
#include <vector>
//

namespace affymetrix_calvin_data
{

/*! The AA call */
#define SNP_AA_CALL 6

/*! The BB call */
#define SNP_BB_CALL 7

/*! The AB call */
#define SNP_AB_CALL 8

/*! The no call allele call */
#define SNP_NO_CALL 11

/*! Stores data for a genotype result of a probe set. */
typedef struct _ProbeSetMultiDataGenotypeData
{
	/*! The name of the probe set. */
	std::string name;

	/*! The call. */
  u_int8_t call;

	/*! The confidence of the call. */
	float confidence;

	/*! Other metrics associated with the call. */
	std::vector<affymetrix_calvin_parameter::ParameterNameValueType> metrics;

} ProbeSetMultiDataGenotypeData;

/*! A value to represent the X chromosome. */
#define X_CHR 24

/*! A value to represent the Y chromosome. */
#define Y_CHR 25

/*! A value to represent the MT chromosome. */
#define MT_CHR 26

/*! A value to represent the no chromosome. */
#define NO_CHR 255

/*! Stores data for a copy number result of a probe set. */
typedef struct _ProbeSetMultiDataCopyNumberData
{
	/*! The name of the probe set. */
	std::string name;

  /*! The chromosome value. */
  u_int8_t chr;

	/*! The physical position. */
	u_int32_t position;

	/*! Other metrics associated with the result. */
	std::vector<affymetrix_calvin_parameter::ParameterNameValueType> metrics;

} ProbeSetMultiDataCopyNumberData;

/*! A no call for the cyto result. */
#define CYTO_NO_CALL 0

/*! An absent call for the cyto result. */
#define CYTO_ABSENT_CALL 1

/*! A present call for the cyto result. */
#define CYTO_PRESENT_CALL 2

/*! A gain call for the cyto result. */
#define CYTO_LOSS_CALL 2

/*! A gain call for the cyto result. */
#define CYTO_GAIN_CALL 3

/*! Stores data for a cyto results. */
typedef struct _ProbeSetMultiDataCytoRegionData
{
	/*! The name of the region. */
	std::string name;

	/*! The chromosome value. */
	u_int8_t chr;

	/*! The physical start position. */
	u_int32_t startPosition;

	/*! The physical stop position. */
	u_int32_t stopPosition;

	/*! The call for the region. */
	u_int8_t call;

	/*! The confidence score. */
	float confidenceScore;

	/*! Other metrics associated with the result. */
	std::vector<affymetrix_calvin_parameter::ParameterNameValueType> metrics;

} ProbeSetMultiDataCytoRegionData;

/*! Stores data for an expression result of a probe set. */
typedef struct _ProbeSetMultiDataExpressionData
{
	/*! The name of the probe set. */
	std::string name;

    /*! The quantification of the probe set. */
	float quantification;

	/*! Other metrics associated with the call. */
	std::vector<affymetrix_calvin_parameter::ParameterNameValueType> metrics;

} ProbeSetMultiDataExpressionData;


/*! Stores data for a copy number variation results. */
typedef struct _ProbeSetMultiDataCopyNumberVariationRegionData
{
	/*! The name of the region. */
	std::string name;

        /*! The signal for the region */
        float signal;

	/*! The call for the region. */
	u_int8_t call;

	/*! The confidence score. */
	float confidenceScore;

	/*! Other metrics associated with the result. */
	std::vector<affymetrix_calvin_parameter::ParameterNameValueType> metrics;

} ProbeSetMultiDataCopyNumberVariationRegionData;

/*! Stores data for a DMET copy number result of a probe set. */
typedef struct _DmetCopyNumberData
{
	/*! The name of the probe set. */
	std::string name;

  /*! The call for the region. */
  int16_t call;

	/*! The confidence score. */
	float confidence;

	/*! The copy number force*/
	int16_t force;

	/*! copy number estimate */
	float estimate;

	/*! lower bounds */
	float lower;

	/*! upper bounds */
	float upper;

	/*! Other metrics associated with the result. */
	std::vector<affymetrix_calvin_parameter::ParameterNameValueType> metrics;

} DmetCopyNumberData;

/*! Stores data for a DMET genotype probe set. */
typedef struct _DmetMultiAllelicData
{
	/*! The name of the probe set. */
	std::string name;

  /*! The call for the region. */
  u_int8_t call;

	/*! The confidence score. */
	float confidence;

	/*! The copy number force*/
	u_int8_t force;

	/*! copy number estimate */
	u_int8_t alleleCount;

	float signalA;
	float signalB;
	float signalC;
	float signalD;
	float signalE;
	float signalF;

	u_int8_t contextA;
	u_int8_t contextB;
	u_int8_t contextC;
	u_int8_t contextD;
	u_int8_t contextE;
	u_int8_t contextF;

	/*! Other metrics associated with the result. */
	std::vector<affymetrix_calvin_parameter::ParameterNameValueType> metrics;

} DmetMultiAllelicData;

/*! Stores data for a DMET genotype probe set. */
typedef struct _DmetBiAllelicData
{
	/*! The name of the probe set. */
	std::string name;

  /*! The call for the region. */
  u_int8_t call;

	/*! The confidence score. */
	float confidence;

	/*! The copy number force*/
	u_int8_t force;

	/*! lower bounds */
	float signalA;
	float signalB;

	u_int8_t contextA;
	u_int8_t contextB;

	/*! Other metrics associated with the result. */
	std::vector<affymetrix_calvin_parameter::ParameterNameValueType> metrics;

} DmetBiAllelicData;

/*! Convert a string representation of a chromosome to a numeric representation.
 * @param chr The chromosome value.
 * @return A numeric representation of the chromosome value.
 */
u_int8_t ChromosomeFromString(const std::string& chr);

/*! Convert a numeric representation of a chromosome to a string representation.
 * @param chr The chromosome value.
 * @return A string representation of the chromosome value.
 */
std::string ChromosomeToString(u_int8_t chr);

/*! Convert a string representation of a cyto call to a numeric representation.
 * @param call The call value.
 * @return A numeric representation of the call value.
 */
u_int8_t CytoCallFromString(const std::string &call);

/*! Convert a numeric representation of a call to a string representation.
 * @param call The call value.
 * @return A string representation of the call value.
 */
std::string CytoCallToString(u_int8_t call);

/*! Convert a string representation of a genotype call to a numeric representation.
 * @param call The call value.
 * @return A numeric representation of the call value.
 */
u_int8_t GenotypeCallFromString(const std::string &call);

/*! Convert a numeric representation of a genotype call to a string representation.
 * @param call The call value.
 * @return A string representation of the call value.
 */
std::string GenotypeCallToString(u_int8_t call);


}

#endif
