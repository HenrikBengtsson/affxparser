/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.25
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


public class FusionCELData {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected FusionCELData(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(FusionCELData obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public void delete() {
    if(swigCPtr != 0 && swigCMemOwn) {
      swigCMemOwn = false;
      affx_fusionJNI.delete_FusionCELData(swigCPtr);
    }
    swigCPtr = 0;
  }

  public FusionCELData() {
    this(affx_fusionJNI.new_FusionCELData(), true);
  }

  public void SetFileName(String value) {
    affx_fusionJNI.FusionCELData_SetFileName(swigCPtr, value);
  }

  public String GetFileName() {
    return affx_fusionJNI.FusionCELData_GetFileName(swigCPtr);
  }

  public void SetError(SWIGTYPE_p_wchar_t value) {
    affx_fusionJNI.FusionCELData_SetError(swigCPtr, SWIGTYPE_p_wchar_t.getCPtr(value));
  }

  public SWIGTYPE_p_std__wstring GetError() {
    return new SWIGTYPE_p_std__wstring(affx_fusionJNI.FusionCELData_GetError(swigCPtr), true);
  }

  public SWIGTYPE_p_std__wstring GetHeaderKey(SWIGTYPE_p_wchar_t key) {
    return new SWIGTYPE_p_std__wstring(affx_fusionJNI.FusionCELData_GetHeaderKey(swigCPtr, SWIGTYPE_p_wchar_t.getCPtr(key)), true);
  }

  public int GetVersion() {
    return affx_fusionJNI.FusionCELData_GetVersion(swigCPtr);
  }

  public int GetCols() {
    return affx_fusionJNI.FusionCELData_GetCols(swigCPtr);
  }

  public int GetRows() {
    return affx_fusionJNI.FusionCELData_GetRows(swigCPtr);
  }

  public int GetNumCells() {
    return affx_fusionJNI.FusionCELData_GetNumCells(swigCPtr);
  }

  public SWIGTYPE_p_std__wstring GetHeader() {
    return new SWIGTYPE_p_std__wstring(affx_fusionJNI.FusionCELData_GetHeader(swigCPtr), true);
  }

  public SWIGTYPE_p_std__wstring GetAlg() {
    return new SWIGTYPE_p_std__wstring(affx_fusionJNI.FusionCELData_GetAlg(swigCPtr), true);
  }

  public SWIGTYPE_p_std__wstring GetParams() {
    return new SWIGTYPE_p_std__wstring(affx_fusionJNI.FusionCELData_GetParams(swigCPtr), true);
  }

  public SWIGTYPE_p_std__wstring GetAlgorithmParameter(SWIGTYPE_p_wchar_t tag) {
    return new SWIGTYPE_p_std__wstring(affx_fusionJNI.FusionCELData_GetAlgorithmParameter(swigCPtr, SWIGTYPE_p_wchar_t.getCPtr(tag)), true);
  }

  public SWIGTYPE_p_std__wstring GetAlgorithmParameterTag(int index) {
    return new SWIGTYPE_p_std__wstring(affx_fusionJNI.FusionCELData_GetAlgorithmParameterTag(swigCPtr, index), true);
  }

  public int GetNumberAlgorithmParameters() {
    return affx_fusionJNI.FusionCELData_GetNumberAlgorithmParameters(swigCPtr);
  }

  public SWIGTYPE_p_std__wstring GetAlgorithmParameters() {
    return new SWIGTYPE_p_std__wstring(affx_fusionJNI.FusionCELData_GetAlgorithmParameters(swigCPtr), true);
  }

  public SWIGTYPE_p_std__listTaffymetrix_fusion_io__FusionTagValuePairType_t GetParameters() {
    return new SWIGTYPE_p_std__listTaffymetrix_fusion_io__FusionTagValuePairType_t(affx_fusionJNI.FusionCELData_GetParameters(swigCPtr), false);
  }

  public SWIGTYPE_p_std__wstring GetChipType() {
    return new SWIGTYPE_p_std__wstring(affx_fusionJNI.FusionCELData_GetChipType(swigCPtr), true);
  }

  public int GetCellMargin() {
    return affx_fusionJNI.FusionCELData_GetCellMargin(swigCPtr);
  }

  public long GetNumOutliers() {
    return affx_fusionJNI.FusionCELData_GetNumOutliers(swigCPtr);
  }

  public long GetNumMasked() {
    return affx_fusionJNI.FusionCELData_GetNumMasked(swigCPtr);
  }

  public int IndexToX(int index) {
    return affx_fusionJNI.FusionCELData_IndexToX(swigCPtr, index);
  }

  public int IndexToY(int index) {
    return affx_fusionJNI.FusionCELData_IndexToY(swigCPtr, index);
  }

  public int XYToIndex(int x, int y) {
    return affx_fusionJNI.FusionCELData_XYToIndex__SWIG_0(swigCPtr, x, y);
  }

  public static int XYToIndex(int x, int y, int r, int c) {
    return affx_fusionJNI.FusionCELData_XYToIndex__SWIG_1(x, y, r, c);
  }

  public void GetEntry(int index, SWIGTYPE_p_FusionCELFileEntryType entry) {
    affx_fusionJNI.FusionCELData_GetEntry__SWIG_0(swigCPtr, index, SWIGTYPE_p_FusionCELFileEntryType.getCPtr(entry));
  }

  public void GetEntry(int x, int y, SWIGTYPE_p_FusionCELFileEntryType entry) {
    affx_fusionJNI.FusionCELData_GetEntry__SWIG_1(swigCPtr, x, y, SWIGTYPE_p_FusionCELFileEntryType.getCPtr(entry));
  }

  public float GetIntensity(int index) {
    return affx_fusionJNI.FusionCELData_GetIntensity__SWIG_0(swigCPtr, index);
  }

  public float GetIntensity(int x, int y) {
    return affx_fusionJNI.FusionCELData_GetIntensity__SWIG_1(swigCPtr, x, y);
  }

  public float GetStdv(int index) {
    return affx_fusionJNI.FusionCELData_GetStdv__SWIG_0(swigCPtr, index);
  }

  public float GetStdv(int x, int y) {
    return affx_fusionJNI.FusionCELData_GetStdv__SWIG_1(swigCPtr, x, y);
  }

  public short GetPixels(int index) {
    return affx_fusionJNI.FusionCELData_GetPixels__SWIG_0(swigCPtr, index);
  }

  public short GetPixels(int x, int y) {
    return affx_fusionJNI.FusionCELData_GetPixels__SWIG_1(swigCPtr, x, y);
  }

  public boolean IsMasked(int x, int y) {
    return affx_fusionJNI.FusionCELData_IsMasked__SWIG_0(swigCPtr, x, y);
  }

  public boolean IsMasked(int index) {
    return affx_fusionJNI.FusionCELData_IsMasked__SWIG_1(swigCPtr, index);
  }

  public boolean IsOutlier(int x, int y) {
    return affx_fusionJNI.FusionCELData_IsOutlier__SWIG_0(swigCPtr, x, y);
  }

  public boolean IsOutlier(int index) {
    return affx_fusionJNI.FusionCELData_IsOutlier__SWIG_1(swigCPtr, index);
  }

  public void Close() {
    affx_fusionJNI.FusionCELData_Close(swigCPtr);
  }

  public long GetFileSize() {
    return affx_fusionJNI.FusionCELData_GetFileSize(swigCPtr);
  }

  public boolean Exists() {
    return affx_fusionJNI.FusionCELData_Exists(swigCPtr);
  }

  public boolean ReadHeader() {
    return affx_fusionJNI.FusionCELData_ReadHeader(swigCPtr);
  }

  public boolean Read(boolean bIncludeMaskAndOutliers) {
    return affx_fusionJNI.FusionCELData_Read__SWIG_0(swigCPtr, bIncludeMaskAndOutliers);
  }

  public boolean Read() {
    return affx_fusionJNI.FusionCELData_Read__SWIG_1(swigCPtr);
  }

  public boolean ReadEx(String filename, int state) {
    return affx_fusionJNI.FusionCELData_ReadEx__SWIG_0(swigCPtr, filename, state);
  }

  public boolean ReadEx(String filename) {
    return affx_fusionJNI.FusionCELData_ReadEx__SWIG_1(swigCPtr, filename);
  }

  public boolean ReadEx() {
    return affx_fusionJNI.FusionCELData_ReadEx__SWIG_2(swigCPtr);
  }

  public int GetReadState() {
    return affx_fusionJNI.FusionCELData_GetReadState(swigCPtr);
  }

  public void Clear() {
    affx_fusionJNI.FusionCELData_Clear(swigCPtr);
  }

  public void SetAlgorithmName(SWIGTYPE_p_wchar_t value) {
    affx_fusionJNI.FusionCELData_SetAlgorithmName(swigCPtr, SWIGTYPE_p_wchar_t.getCPtr(value));
  }

  public void AddAlgorithmParameter(SWIGTYPE_p_wchar_t tag, SWIGTYPE_p_wchar_t value) {
    affx_fusionJNI.FusionCELData_AddAlgorithmParameter(swigCPtr, SWIGTYPE_p_wchar_t.getCPtr(tag), SWIGTYPE_p_wchar_t.getCPtr(value));
  }

  public void SetDimensions(int rows, int cols) {
    affx_fusionJNI.FusionCELData_SetDimensions(swigCPtr, rows, cols);
  }

  public void SetChipType(SWIGTYPE_p_wchar_t value) {
    affx_fusionJNI.FusionCELData_SetChipType(swigCPtr, SWIGTYPE_p_wchar_t.getCPtr(value));
  }

  public void SetMargin(int margin) {
    affx_fusionJNI.FusionCELData_SetMargin(swigCPtr, margin);
  }

  public final static int CEL_ALL = affx_fusionJNI.get_FusionCELData_CEL_ALL();
  public final static int CEL_DATA = affx_fusionJNI.get_FusionCELData_CEL_DATA();
  public final static int CEL_OUTLIER = affx_fusionJNI.get_FusionCELData_CEL_OUTLIER();
  public final static int CEL_MASK = affx_fusionJNI.get_FusionCELData_CEL_MASK();

}
