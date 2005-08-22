%module affx_fusion

%include "stl.i"

%{

#include "../../portability/src/AffymetrixBaseTypes.h"

#include "../../utils/src/AffymetrixGuid.h"
#include "../../utils/src/AffyStlCollectionTypes.h"
#include "../../utils/src/checksum.h"
#include "../../utils/src/Coords.h"
#include "../../utils/src/DateTime.h"
#include "../../utils/src/FileUtils.h"
#include "../../utils/src/StringUtils.h"
#include "../../utils/src/Subgrids.h"

#include "../../exception/src/ExceptionBase.h"
#include "../../exception/src/DevelopmentException.h"
#include "../../exception/src/InterpretationException.h"

#include "../../parameter/src/ParameterException.h"
#include "../../parameter/src/Parameter.h"
#include "../../parameter/src/AffymetrixParameterConsts.h"
#include "../../parameter/src/ParameterNameValueType.h"

#include "../../Array/src/ArrayAttributes.h"
#include "../../Array/src/ArrayAudit.h"
#include "../../Array/src/ArrayAuditActionTypes.h"
#include "../../Array/src/ArrayId.h"
#include "../../Array/src/ArrayData.h"

#include "../../data/src/CDFData.h"
#include "../../data/src/CDFProbeGroupInformation.h"
#include "../../data/src/CDFProbeInformation.h"
#include "../../data/src/CDFProbeSetInformation.h"
#include "../../data/src/CDFQCProbeInformation.h"
#include "../../data/src/CDFQCProbeSetInformation.h"
#include "../../data/src/CHPBackgroundZone.h"
#include "../../data/src/CHPData.h"
#include "../../data/src/CHPExpressionEntry.h"
#include "../../data/src/CHPGenotypeEntry.h"
#include "../../data/src/CHPUniversalEntry.h"
#include "../../data/src/CELData.h"
#include "../../data/src/ColumnTypes.h"
#include "../../data/src/DataException.h"
#include "../../data/src/DataGroup.h"
#include "../../data/src/DataGroupHeader.h"
#include "../../data/src/DataSet.h"
#include "../../data/src/DataSetHeader.h"
#include "../../data/src/FileHeader.h"
#include "../../data/src/GenericData.h"
#include "../../data/src/GenericDataHeader.h"
#include "../../data/src/GenericDataTypes.h"
#include "../../data/src/cdfdatatypes.h"

#include "../../parsers/src/ArrayFileReader.h"
#include "../../parsers/src/AuditFileConstants.h"
#include "../../parsers/src/AuditFileReader.h"
#include "../../parsers/src/CDFFileReader.h"
#include "../../parsers/src/CHPFileReader.h"
#include "../../parsers/src/CelFileReader.h"
#include "../../parsers/src/DataGroupHeaderReader.h"
#include "../../parsers/src/DataGroupReader.h"
#include "../../parsers/src/DataSetHeaderReader.h"
#include "../../parsers/src/DataSetReader.h"
#include "../../parsers/src/FileException.h"
#include "../../parsers/src/FileHeaderReader.h"
#include "../../parsers/src/FileInput.h"
#include "../../parsers/src/GenericDataHeaderReader.h"
#include "../../parsers/src/GenericFileReader.h"
#include "../../parsers/src/GridControlFileReader.h"
#include "../../parsers/src/TemplateFileReader.h"
#include "../../parsers/src/TextFileReader.h"

#include "../../template/src/TemplateAuditActionTypes.h"
#include "../../template/src/TemplateData.h"

#include "../../../portability/affy-base-types.h"
#include "../../../file/FileIO.h"
#include "../../../file/GridCoordinates.h"
#include "../../../file/TagValuePair.h"
#include "../../../file/CDFFileData.h"
#include "../../../file/CELFileData.h"
#include "../../../file/CHPFileData.h"
#include "../../../file/BARFileData.h"
#include "../../../file/BPMAPFileData.h"
#include "../../../file/EXPFileData.h"
#include "../../../file/GRCFileData.h"
#include "../../../file/MSKFileData.h"
#include "../../../file/PSIFileData.h"
#include "../../../file/DttArrayFileReader.h"

#include "../src/CalvinAdapter/CalvinCELDataAdapter.h"
#include "../src/CalvinAdapter/CalvinCHPDataAdapter.h"

#include "../src/FusionTagValuePairType.h"
#include "../src/FusionArrayFileReader.h"
#include "../src/FusionProbeSetResults.h"
#include "../src/FusionCDFData.h"
#include "../src/FusionCDFQCProbeSetNames.h"
#include "../src/FusionCELDataAdapterInterface.h"
#include "../src/FusionCELData.h"
#include "../src/FusionCHPDataAdapterInterface.h"
#include "../src/FusionCHPData.h"

using namespace affymetrix_fusion_io;

%}

%rename (FPoint_equals) operator==(const FPoint& lhs) const;
%rename (FPoint_not_equals) operator!=(const FPoint& lhs) const;
%rename (Point_equals) operator==(const Point& lhs) const;
%rename (Point_not_equals) operator!=(const Point& lhs) const;
%rename (op_FRegion) operator affymetrix_calvin_utilities::FRegion();
%rename (op_Region) operator affymetrix_calvin_utilities::Region();

%rename (ParameterNameValuePair_copy) operator=(_tagParameterNameValuePair param);
%rename (ParameterNameValuePair_equals_obj) operator==(_tagParameterNameValuePair param);
%rename (ParameterNameValuePair_equals_name) operator==(const std::wstring &name);
%rename (ParameterNameValueControlVocabulary_copy) operator=(_tagParameterNameValueControlVocabulary param);
%rename (ParameterNameValueControlVocabulary_equals_obj) operator==(_tagParameterNameValueControlVocabulary param);
%rename (ParameterNameValueControlVocabulary_equals_name) operator==(const std::wstring &name);

%rename (MIMEValue_copy) operator=(const MIMEValue& source);
%rename (MIMEValue_equals) operator==(const MIMEValue& lhs);
%rename (MIMEValue_not_equals) operator!=(const MIMEValue& lhs);

%rename (ParameterNameValueType_copy) operator=(const ParameterNameValueType& param);
%rename (ParameterNameValueType_equals_obj) operator==(const ParameterNameValueType& param) const;
%rename (ParameterNameValueType_not_equals_obj) operator!=(const ParameterNameValueType& param) const;
%rename (ParameterNameValueType_equals_name) operator==(const std::wstring &name) const;
%rename (ParameterNameValueType_not_equals_name) operator!=(const std::wstring &name) const;
%rename (ParameterNameValueType_less_than) operator<(const ParameterNameValueType &param) const;
%rename (ParameterNameValueType_greater_than) operator>(const ParameterNameValueType &param) const;

%rename (ColumnType_equals) operator==(const ColumnType &p) const;
%rename (ColumnType_not_equals) operator!=(const ColumnType &p) const;

%rename (XYCoord_assign) operator=(const XYCoord &p);
%rename (XYCoord_equals) operator==(const XYCoord &p) const;
%rename (XYCoord_not_equals) operator!=(const XYCoord &p) const;
%rename (XYCoord_less_than) operator<(const XYCoord& p) const;

%rename (CHPGenotypeEntry_assign) operator=(CHPGenotypeEntry zn);
%rename (CHPExpressionEntry_assign) operator=(CHPExpressionEntry zn);
%rename (CHPUniversalEntry_assign) operator=(CHPUniversalEntry zn);
%rename (CHPBackgroundZone_assign) operator=(CHPBackgroundZone zn);

%rename (_BackgroundZoneType_assign) operator=(_BackgroundZoneType zn);
%rename (CExpressionProbeSetResults_assign) operator=(CExpressionProbeSetResults &src);
%rename (CGenotypeProbeSetResults_assign) operator=(CGenotypeProbeSetResults &src);
%rename (CUniversalProbeSetResults_assign) operator=(CUniversalProbeSetResults &src);
%rename (FusionExpressionProbeSetResults_assign) operator=(FusionExpressionProbeSetResults &src);
%rename (FusionGenotypeProbeSetResults_assign) operator=(FusionGenotypeProbeSetResults &src);
%rename (FusionUniversalProbeSetResults_assign) operator=(FusionUniversalProbeSetResults &src);

%rename (FusionTagValuePairType_assign) operator=(const FusionTagValuePairType vp);
%rename (FusionTagValuePairType_equals_obj) operator==(const FusionTagValuePairType vp);
%rename (FusionTagValuePairType_equals_tag) operator==(const wchar_t *tag);

%rename (GetQCProbeSetInformation_By_Type) GetQCProbeSetInformation(GeneChipQCProbeSetType qcType, CCDFQCProbeSetInformation & info);
%rename (FusionGetQCProbeSetInformation_By_Type) GetQCProbeSetInformation(affxcdf::GeneChipQCProbeSetType qcType, FusionCDFQCProbeSetInformation & info);

%rename (_TagValuePairType_assign) operator=(_TagValuePairType vp);
%rename (_TagValuePairType_equals_obj) operator==(_TagValuePairType vp);
%rename (_TagValuePairType_equals_tag) operator==(const char *tag);

%rename (_GDACSequenceHitItemType_less_than) operator<(const _GDACSequenceHitItemType &rhs) const;
	
	

%include "../../portability/src/AffymetrixBaseTypes.h"

%include "../../utils/src/AffymetrixGuid.h"
%include "../../utils/src/AffyStlCollectionTypes.h"
%include "../../utils/src/checksum.h"
%include "../../utils/src/Coords.h"
%include "../../utils/src/DateTime.h"
%include "../../utils/src/FileUtils.h"
%include "../../utils/src/StringUtils.h"
%include "../../utils/src/Subgrids.h"

%include "../../exception/src/ExceptionBase.h"
%include "../../exception/src/DevelopmentException.h"
%include "../../exception/src/InterpretationException.h"

%include "../../parameter/src/ParameterException.h"
%include "../../parameter/src/Parameter.h"
%include "../../parameter/src/AffymetrixParameterConsts.h"
%include "../../parameter/src/ParameterNameValueType.h"

%include "../../Array/src/ArrayAttributes.h"
%include "../../Array/src/ArrayAudit.h"
%include "../../Array/src/ArrayAuditActionTypes.h"
%include "../../Array/src/ArrayId.h"
%include "../../Array/src/ArrayData.h"

%include "../../data/src/CDFData.h"
%include "../../data/src/CDFProbeGroupInformation.h"
%include "../../data/src/CDFProbeInformation.h"
%include "../../data/src/CDFProbeSetInformation.h"
%include "../../data/src/CDFQCProbeInformation.h"
%include "../../data/src/CDFQCProbeSetInformation.h"
%include "../../data/src/CHPBackgroundZone.h"
%include "../../data/src/CHPData.h"
%include "../../data/src/CHPExpressionEntry.h"
%include "../../data/src/CHPGenotypeEntry.h"
%include "../../data/src/CHPUniversalEntry.h"
%include "../../data/src/CELData.h"
%include "../../data/src/ColumnTypes.h"
%include "../../data/src/DataException.h"
%include "../../data/src/DataGroup.h"
%include "../../data/src/DataGroupHeader.h"
%include "../../data/src/DataSet.h"
%include "../../data/src/DataSetHeader.h"
%include "../../data/src/FileHeader.h"
%include "../../data/src/GenericData.h"
%include "../../data/src/GenericDataHeader.h"
%include "../../data/src/GenericDataTypes.h"
%include "../../data/src/cdfdatatypes.h"

%include "../../parsers/src/ArrayFileReader.h"
%include "../../parsers/src/AuditFileConstants.h"
%include "../../parsers/src/AuditFileReader.h"
%include "../../parsers/src/CDFFileReader.h"
%include "../../parsers/src/CHPFileReader.h"
%include "../../parsers/src/CelFileReader.h"
%include "../../parsers/src/DataGroupHeaderReader.h"
%include "../../parsers/src/DataGroupReader.h"
%include "../../parsers/src/DataSetHeaderReader.h"
%include "../../parsers/src/DataSetReader.h"
%include "../../parsers/src/FileException.h"
%include "../../parsers/src/FileHeaderReader.h"
%include "../../parsers/src/FileInput.h"
%include "../../parsers/src/GenericDataHeaderReader.h"
%include "../../parsers/src/GenericFileReader.h"
%include "../../parsers/src/GridControlFileReader.h"
%include "../../parsers/src/TemplateFileReader.h"
%include "../../parsers/src/TextFileReader.h"

%include "../../template/src/TemplateAuditActionTypes.h"
%include "../../template/src/TemplateData.h"


%include "../../../portability/affy-base-types.h"
%include "../../../file/FileIO.h"
%include "../../../file/GridCoordinates.h"
%include "../../../file/TagValuePair.h"
%include "../../../file/CDFFileData.h"
%include "../../../file/CELFileData.h"
%include "../../../file/CHPFileData.h"
%include "../../../file/BARFileData.h"
%include "../../../file/BPMAPFileData.h"
%include "../../../file/EXPFileData.h"
%include "../../../file/GRCFileData.h"
%include "../../../file/MSKFileData.h"
%include "../../../file/PSIFileData.h"
%include "../../../file/DttArrayFileReader.h"

%include "../src/FusionTagValuePairType.h"
%include "../src/FusionArrayFileReader.h"
%include "../src/FusionProbeSetResults.h"
%include "../src/FusionCDFData.h"
%include "../src/FusionCDFQCProbeSetNames.h"
%include "../src/FusionCELData.h"
%include "../src/FusionCELDataAdapterInterface.h"
%include "../src/FusionCHPDataAdapterInterface.h"
%include "../src/FusionCHPData.h"


