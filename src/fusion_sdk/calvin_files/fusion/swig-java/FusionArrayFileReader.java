/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.25
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


public class FusionArrayFileReader {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected FusionArrayFileReader(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(FusionArrayFileReader obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public void delete() {
    if(swigCPtr != 0 && swigCMemOwn) {
      swigCMemOwn = false;
      affx_fusionJNI.delete_FusionArrayFileReader(swigCPtr);
    }
    swigCPtr = 0;
  }

  public FusionArrayFileReader() {
    this(affx_fusionJNI.new_FusionArrayFileReader(), true);
  }

  public boolean Read(String fileName, ArrayData arrayData) {
    return affx_fusionJNI.FusionArrayFileReader_Read(swigCPtr, fileName, ArrayData.getCPtr(arrayData));
  }

  public String DataTypeIdentifier() {
    return affx_fusionJNI.FusionArrayFileReader_DataTypeIdentifier(swigCPtr);
  }

}
