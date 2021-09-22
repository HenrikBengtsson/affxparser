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


#ifndef _UTIL_FRUGALVECTOR_H_
#define _UTIL_FRUGALVECTOR_H_

#include <cstdio>
//

template<typename T>
class FrugalVector {

public:
  typedef T           value_type;
  typedef T*          pointer;
  typedef const T*    const_pointer;
  typedef T&          reference;
  typedef const T&    const_reference;
  typedef size_t      size_type;
  typedef T*          iterator;
  typedef const T*    const_iterator;
  typedef FrugalVector<T>			vector_type;

  T *m_Start;
  T *m_End;

  FrugalVector(size_type n, const value_type &val) {
    m_Start = new T[n];
    m_End = m_Start + n;
    std::fill(m_Start, m_End, val);
  }

  FrugalVector(size_type n) {
    m_Start = new T[n];
    m_End = m_Start + n;
  }
  
  FrugalVector() {
    m_Start = m_End = NULL;
  }
  
  ~FrugalVector() {
    delete [] m_Start;
  }

  inline T* begin() { return m_Start; }
  inline T* end() { return m_End; }

  inline const T* begin() const { return m_Start; }
  inline const T* end() const { return m_End; }

  inline reference
  operator[](size_type n) { return *(begin() + n); }

  inline const_reference
  operator[](size_type n) const { return *(begin() + n); }

  void resize(size_type n, const T &t) {
    if(n > size()) {
      T *t_start = new T[n];
      std::copy(m_Start, m_End, t_start);
      std::fill(t_start + size(), t_start + n, t);
      delete [] m_Start;
      m_Start = t_start;
      m_End = m_Start + n;
    }
    else if(n < size() && n != 0) {
      T * t_start = new T[n];
      std::copy(m_Start, m_Start + n, t_start);
      delete [] m_Start;
      m_Start = t_start;
      m_End = m_Start + n;
    }
    else if(n == 0) {
      clear();
    }
  }
  
  inline void resize(size_type n) { resize(n, value_type()); }

  inline void clear() {
    FreezArray(m_Start);
    m_End = NULL;
  }

  inline void push_back(const value_type& t) {
    resize(size() + 1);
    *(m_End - 1) = t;
  }

  inline size_type capacity() const { return size_type(m_End - m_Start); }

  inline bool empty() const { return m_Start == m_End; }
  
  inline size_type size() const { return size_type(m_End - m_Start); }

};

#endif /* _UTIL_FRUGALVECTOR_H_ */
