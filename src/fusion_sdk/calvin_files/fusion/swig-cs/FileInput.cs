/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.25
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


using System;
using System.Runtime.InteropServices;

public class FileInput : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal FileInput(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(FileInput obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~FileInput() {
    Dispose();
  }

  public virtual void Dispose() {
    if(swigCPtr.Handle != IntPtr.Zero && swigCMemOwn) {
      swigCMemOwn = false;
      affx_fusionPINVOKE.delete_FileInput(swigCPtr);
    }
    swigCPtr = new HandleRef(null, IntPtr.Zero);
    GC.SuppressFinalize(this);
  }

  public static char ReadInt8(SWIGTYPE_p_std__ifstream instr) {
    char ret = affx_fusionPINVOKE.FileInput_ReadInt8__SWIG_0(SWIGTYPE_p_std__ifstream.getCPtr(instr));
    if (affx_fusionPINVOKE.SWIGPendingException.Pending) throw affx_fusionPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static short ReadInt16(SWIGTYPE_p_std__ifstream instr) {
    short ret = affx_fusionPINVOKE.FileInput_ReadInt16__SWIG_0(SWIGTYPE_p_std__ifstream.getCPtr(instr));
    if (affx_fusionPINVOKE.SWIGPendingException.Pending) throw affx_fusionPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static int ReadInt32(SWIGTYPE_p_std__ifstream instr) {
    int ret = affx_fusionPINVOKE.FileInput_ReadInt32__SWIG_0(SWIGTYPE_p_std__ifstream.getCPtr(instr));
    if (affx_fusionPINVOKE.SWIGPendingException.Pending) throw affx_fusionPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static byte ReadUInt8(SWIGTYPE_p_std__ifstream instr) {
    byte ret = affx_fusionPINVOKE.FileInput_ReadUInt8__SWIG_0(SWIGTYPE_p_std__ifstream.getCPtr(instr));
    if (affx_fusionPINVOKE.SWIGPendingException.Pending) throw affx_fusionPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static ushort ReadUInt16(SWIGTYPE_p_std__ifstream instr) {
    ushort ret = affx_fusionPINVOKE.FileInput_ReadUInt16__SWIG_0(SWIGTYPE_p_std__ifstream.getCPtr(instr));
    if (affx_fusionPINVOKE.SWIGPendingException.Pending) throw affx_fusionPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static uint ReadUInt32(SWIGTYPE_p_std__ifstream instr) {
    uint ret = affx_fusionPINVOKE.FileInput_ReadUInt32__SWIG_0(SWIGTYPE_p_std__ifstream.getCPtr(instr));
    if (affx_fusionPINVOKE.SWIGPendingException.Pending) throw affx_fusionPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static float ReadFloat(SWIGTYPE_p_std__ifstream instr) {
    float ret = affx_fusionPINVOKE.FileInput_ReadFloat__SWIG_0(SWIGTYPE_p_std__ifstream.getCPtr(instr));
    if (affx_fusionPINVOKE.SWIGPendingException.Pending) throw affx_fusionPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static char ReadInt8(SWIGTYPE_p_p_char instr) {
    char ret = affx_fusionPINVOKE.FileInput_ReadInt8__SWIG_1(SWIGTYPE_p_p_char.getCPtr(instr));
    if (affx_fusionPINVOKE.SWIGPendingException.Pending) throw affx_fusionPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static short ReadInt16(SWIGTYPE_p_p_char instr) {
    short ret = affx_fusionPINVOKE.FileInput_ReadInt16__SWIG_1(SWIGTYPE_p_p_char.getCPtr(instr));
    if (affx_fusionPINVOKE.SWIGPendingException.Pending) throw affx_fusionPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static int ReadInt32(SWIGTYPE_p_p_char instr) {
    int ret = affx_fusionPINVOKE.FileInput_ReadInt32__SWIG_1(SWIGTYPE_p_p_char.getCPtr(instr));
    if (affx_fusionPINVOKE.SWIGPendingException.Pending) throw affx_fusionPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static byte ReadUInt8(SWIGTYPE_p_p_char instr) {
    byte ret = affx_fusionPINVOKE.FileInput_ReadUInt8__SWIG_1(SWIGTYPE_p_p_char.getCPtr(instr));
    if (affx_fusionPINVOKE.SWIGPendingException.Pending) throw affx_fusionPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static ushort ReadUInt16(SWIGTYPE_p_p_char instr) {
    ushort ret = affx_fusionPINVOKE.FileInput_ReadUInt16__SWIG_1(SWIGTYPE_p_p_char.getCPtr(instr));
    if (affx_fusionPINVOKE.SWIGPendingException.Pending) throw affx_fusionPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static uint ReadUInt32(SWIGTYPE_p_p_char instr) {
    uint ret = affx_fusionPINVOKE.FileInput_ReadUInt32__SWIG_1(SWIGTYPE_p_p_char.getCPtr(instr));
    if (affx_fusionPINVOKE.SWIGPendingException.Pending) throw affx_fusionPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static float ReadFloat(SWIGTYPE_p_p_char instr) {
    float ret = affx_fusionPINVOKE.FileInput_ReadFloat__SWIG_1(SWIGTYPE_p_p_char.getCPtr(instr));
    if (affx_fusionPINVOKE.SWIGPendingException.Pending) throw affx_fusionPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static SWIGTYPE_p_std__wstring ReadString16(SWIGTYPE_p_std__ifstream instr, int len) {
    SWIGTYPE_p_std__wstring ret = new SWIGTYPE_p_std__wstring(affx_fusionPINVOKE.FileInput_ReadString16__SWIG_0(SWIGTYPE_p_std__ifstream.getCPtr(instr), len), true);
    if (affx_fusionPINVOKE.SWIGPendingException.Pending) throw affx_fusionPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static SWIGTYPE_p_std__wstring ReadString16(SWIGTYPE_p_std__ifstream instr) {
    SWIGTYPE_p_std__wstring ret = new SWIGTYPE_p_std__wstring(affx_fusionPINVOKE.FileInput_ReadString16__SWIG_1(SWIGTYPE_p_std__ifstream.getCPtr(instr)), true);
    if (affx_fusionPINVOKE.SWIGPendingException.Pending) throw affx_fusionPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static string ReadString8(SWIGTYPE_p_std__ifstream instr, int len) {
    string ret = affx_fusionPINVOKE.FileInput_ReadString8__SWIG_0(SWIGTYPE_p_std__ifstream.getCPtr(instr), len);
    if (affx_fusionPINVOKE.SWIGPendingException.Pending) throw affx_fusionPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static string ReadString8(SWIGTYPE_p_std__ifstream instr) {
    string ret = affx_fusionPINVOKE.FileInput_ReadString8__SWIG_1(SWIGTYPE_p_std__ifstream.getCPtr(instr));
    if (affx_fusionPINVOKE.SWIGPendingException.Pending) throw affx_fusionPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static SWIGTYPE_p_std__wstring ReadString16(SWIGTYPE_p_p_char instr, int len) {
    SWIGTYPE_p_std__wstring ret = new SWIGTYPE_p_std__wstring(affx_fusionPINVOKE.FileInput_ReadString16__SWIG_2(SWIGTYPE_p_p_char.getCPtr(instr), len), true);
    if (affx_fusionPINVOKE.SWIGPendingException.Pending) throw affx_fusionPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static SWIGTYPE_p_std__wstring ReadString16(SWIGTYPE_p_p_char instr) {
    SWIGTYPE_p_std__wstring ret = new SWIGTYPE_p_std__wstring(affx_fusionPINVOKE.FileInput_ReadString16__SWIG_3(SWIGTYPE_p_p_char.getCPtr(instr)), true);
    if (affx_fusionPINVOKE.SWIGPendingException.Pending) throw affx_fusionPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static string ReadString8(SWIGTYPE_p_p_char instr, int len) {
    string ret = affx_fusionPINVOKE.FileInput_ReadString8__SWIG_2(SWIGTYPE_p_p_char.getCPtr(instr), len);
    if (affx_fusionPINVOKE.SWIGPendingException.Pending) throw affx_fusionPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static string ReadString8(SWIGTYPE_p_p_char instr) {
    string ret = affx_fusionPINVOKE.FileInput_ReadString8__SWIG_3(SWIGTYPE_p_p_char.getCPtr(instr));
    if (affx_fusionPINVOKE.SWIGPendingException.Pending) throw affx_fusionPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static int ReadBlob(SWIGTYPE_p_std__ifstream instr, SWIGTYPE_p_p_void value) {
    int ret = affx_fusionPINVOKE.FileInput_ReadBlob(SWIGTYPE_p_std__ifstream.getCPtr(instr), SWIGTYPE_p_p_void.getCPtr(value));
    if (affx_fusionPINVOKE.SWIGPendingException.Pending) throw affx_fusionPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public FileInput() : this(affx_fusionPINVOKE.new_FileInput(), true) {
  }

}
