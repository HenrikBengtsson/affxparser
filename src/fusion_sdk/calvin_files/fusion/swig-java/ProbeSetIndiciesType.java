/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.25
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


public class ProbeSetIndiciesType {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected ProbeSetIndiciesType(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(ProbeSetIndiciesType obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public void delete() {
    if(swigCPtr != 0 && swigCMemOwn) {
      swigCMemOwn = false;
      affx_fusionJNI.delete_ProbeSetIndiciesType(swigCPtr);
    }
    swigCPtr = 0;
  }

  public void setProbeSetName(SWIGTYPE_p_std__string value) {
    affx_fusionJNI.set_ProbeSetIndiciesType_probeSetName(swigCPtr, SWIGTYPE_p_std__string.getCPtr(value));
  }

  public SWIGTYPE_p_std__string getProbeSetName() {
    long cPtr = affx_fusionJNI.get_ProbeSetIndiciesType_probeSetName(swigCPtr);
    return (cPtr == 0) ? null : new SWIGTYPE_p_std__string(cPtr, false);
  }

  public void setIndicies(SWIGTYPE_p_std__listTint_t value) {
    affx_fusionJNI.set_ProbeSetIndiciesType_indicies(swigCPtr, SWIGTYPE_p_std__listTint_t.getCPtr(value));
  }

  public SWIGTYPE_p_std__listTint_t getIndicies() {
    return new SWIGTYPE_p_std__listTint_t(affx_fusionJNI.get_ProbeSetIndiciesType_indicies(swigCPtr), true);
  }

  public ProbeSetIndiciesType() {
    this(affx_fusionJNI.new_ProbeSetIndiciesType(), true);
  }

}
