/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.25
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


public class CExpressionProbeSetResults extends CProbeSetResults {
  private long swigCPtr;

  protected CExpressionProbeSetResults(long cPtr, boolean cMemoryOwn) {
    super(affx_fusionJNI.SWIGCExpressionProbeSetResultsUpcast(cPtr), cMemoryOwn);
    swigCPtr = cPtr;
  }

  protected static long getCPtr(CExpressionProbeSetResults obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public void delete() {
    if(swigCPtr != 0 && swigCMemOwn) {
      swigCMemOwn = false;
      affx_fusionJNI.delete_CExpressionProbeSetResults(swigCPtr);
    }
    swigCPtr = 0;
    super.delete();
  }

  public void setDetectionPValue(float value) {
    affx_fusionJNI.set_CExpressionProbeSetResults_DetectionPValue(swigCPtr, value);
  }

  public float getDetectionPValue() {
    return affx_fusionJNI.get_CExpressionProbeSetResults_DetectionPValue(swigCPtr);
  }

  public void setSignal(float value) {
    affx_fusionJNI.set_CExpressionProbeSetResults_Signal(swigCPtr, value);
  }

  public float getSignal() {
    return affx_fusionJNI.get_CExpressionProbeSetResults_Signal(swigCPtr);
  }

  public void setNumPairs(int value) {
    affx_fusionJNI.set_CExpressionProbeSetResults_NumPairs(swigCPtr, value);
  }

  public int getNumPairs() {
    return affx_fusionJNI.get_CExpressionProbeSetResults_NumPairs(swigCPtr);
  }

  public void setNumUsedPairs(int value) {
    affx_fusionJNI.set_CExpressionProbeSetResults_NumUsedPairs(swigCPtr, value);
  }

  public int getNumUsedPairs() {
    return affx_fusionJNI.get_CExpressionProbeSetResults_NumUsedPairs(swigCPtr);
  }

  public void setDetection(short value) {
    affx_fusionJNI.set_CExpressionProbeSetResults_Detection(swigCPtr, value);
  }

  public short getDetection() {
    return affx_fusionJNI.get_CExpressionProbeSetResults_Detection(swigCPtr);
  }

  public void setM_HasCompResults(boolean value) {
    affx_fusionJNI.set_CExpressionProbeSetResults_m_HasCompResults(swigCPtr, value);
  }

  public boolean getM_HasCompResults() {
    return affx_fusionJNI.get_CExpressionProbeSetResults_m_HasCompResults(swigCPtr);
  }

  public void setChangePValue(float value) {
    affx_fusionJNI.set_CExpressionProbeSetResults_ChangePValue(swigCPtr, value);
  }

  public float getChangePValue() {
    return affx_fusionJNI.get_CExpressionProbeSetResults_ChangePValue(swigCPtr);
  }

  public void setSignalLogRatio(float value) {
    affx_fusionJNI.set_CExpressionProbeSetResults_SignalLogRatio(swigCPtr, value);
  }

  public float getSignalLogRatio() {
    return affx_fusionJNI.get_CExpressionProbeSetResults_SignalLogRatio(swigCPtr);
  }

  public void setSignalLogRatioLow(float value) {
    affx_fusionJNI.set_CExpressionProbeSetResults_SignalLogRatioLow(swigCPtr, value);
  }

  public float getSignalLogRatioLow() {
    return affx_fusionJNI.get_CExpressionProbeSetResults_SignalLogRatioLow(swigCPtr);
  }

  public void setSignalLogRatioHigh(float value) {
    affx_fusionJNI.set_CExpressionProbeSetResults_SignalLogRatioHigh(swigCPtr, value);
  }

  public float getSignalLogRatioHigh() {
    return affx_fusionJNI.get_CExpressionProbeSetResults_SignalLogRatioHigh(swigCPtr);
  }

  public void setNumCommonPairs(int value) {
    affx_fusionJNI.set_CExpressionProbeSetResults_NumCommonPairs(swigCPtr, value);
  }

  public int getNumCommonPairs() {
    return affx_fusionJNI.get_CExpressionProbeSetResults_NumCommonPairs(swigCPtr);
  }

  public void setChange(short value) {
    affx_fusionJNI.set_CExpressionProbeSetResults_Change(swigCPtr, value);
  }

  public short getChange() {
    return affx_fusionJNI.get_CExpressionProbeSetResults_Change(swigCPtr);
  }

  public String GetDetectionString() {
    return affx_fusionJNI.CExpressionProbeSetResults_GetDetectionString(swigCPtr);
  }

  public String GetChangeString() {
    return affx_fusionJNI.CExpressionProbeSetResults_GetChangeString(swigCPtr);
  }

  public CExpressionProbeSetResults CExpressionProbeSetResults_assign(CExpressionProbeSetResults src) {
    return new CExpressionProbeSetResults(affx_fusionJNI.CExpressionProbeSetResults_CExpressionProbeSetResults_assign(swigCPtr, CExpressionProbeSetResults.getCPtr(src)), true);
  }

  public CExpressionProbeSetResults() {
    this(affx_fusionJNI.new_CExpressionProbeSetResults(), true);
  }

}
