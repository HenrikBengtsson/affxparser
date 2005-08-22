/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.25
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


using System;
using System.Runtime.InteropServices;

public class XYCoord : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal XYCoord(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(XYCoord obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~XYCoord() {
    Dispose();
  }

  public virtual void Dispose() {
    if(swigCPtr.Handle != IntPtr.Zero && swigCMemOwn) {
      swigCMemOwn = false;
      affx_fusionPINVOKE.delete_XYCoord(swigCPtr);
    }
    swigCPtr = new HandleRef(null, IntPtr.Zero);
    GC.SuppressFinalize(this);
  }

  public XYCoord() : this(affx_fusionPINVOKE.new_XYCoord__SWIG_0(), true) {
  }

  public XYCoord(short x, short y) : this(affx_fusionPINVOKE.new_XYCoord__SWIG_1(x, y), true) {
  }

  public short xCoord {
    set {
      affx_fusionPINVOKE.set_XYCoord_xCoord(swigCPtr, value);
    } 
    get {
      short ret = affx_fusionPINVOKE.get_XYCoord_xCoord(swigCPtr);
      return ret;
    } 
  }

  public short yCoord {
    set {
      affx_fusionPINVOKE.set_XYCoord_yCoord(swigCPtr, value);
    } 
    get {
      short ret = affx_fusionPINVOKE.get_XYCoord_yCoord(swigCPtr);
      return ret;
    } 
  }

  public XYCoord XYCoord_assign(XYCoord p) {
    XYCoord ret = new XYCoord(affx_fusionPINVOKE.XYCoord_XYCoord_assign(swigCPtr, XYCoord.getCPtr(p)), true);
    if (affx_fusionPINVOKE.SWIGPendingException.Pending) throw affx_fusionPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public bool XYCoord_equals(XYCoord p) {
    bool ret = affx_fusionPINVOKE.XYCoord_XYCoord_equals(swigCPtr, XYCoord.getCPtr(p));
    if (affx_fusionPINVOKE.SWIGPendingException.Pending) throw affx_fusionPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public bool XYCoord_not_equals(XYCoord p) {
    bool ret = affx_fusionPINVOKE.XYCoord_XYCoord_not_equals(swigCPtr, XYCoord.getCPtr(p));
    if (affx_fusionPINVOKE.SWIGPendingException.Pending) throw affx_fusionPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public bool XYCoord_less_than(XYCoord p) {
    bool ret = affx_fusionPINVOKE.XYCoord_XYCoord_less_than(swigCPtr, XYCoord.getCPtr(p));
    if (affx_fusionPINVOKE.SWIGPendingException.Pending) throw affx_fusionPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

}
