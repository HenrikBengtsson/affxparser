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

#ifndef _AffxSTL_h_
#define _AffxSTL_h_
/**
 * @file AffxSTL.h
 *
 * @brief This header contains definitions of STL-related tools
 */
//
//
#undef max
#include <limits>
#include <functional>


using std::size_t;
using std::ptrdiff_t;
using std::numeric_limits;
using std::unary_function;

namespace affxstl {

/**
 * @brief  An allocator which releases memory ASAP (calls ::operator delete upon exiting scope). 
 */
template <class T> class quickReleaseAlloc;

template <>
class quickReleaseAlloc<void> {
public:
    typedef void*       pointer;
    typedef const void* const_pointer;
    typedef void        value_type;

    template <class U> struct rebind {
        typedef quickReleaseAlloc<U> other;
    };
};

template <class T>
class quickReleaseAlloc {
public:
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;
    typedef T*          pointer;
    typedef const T*    const_pointer;
    typedef T&          reference;
    typedef const T&    const_reference;
    typedef T           value_type;

    template <class U>
    struct rebind {
        typedef quickReleaseAlloc<U> other;
    };

    pointer address(reference value) const
    {
        return &value;
    }

    const_pointer address(const_reference value) const
    {
        return &value;
    }

    quickReleaseAlloc() throw() {}
    quickReleaseAlloc(const quickReleaseAlloc&) throw() {}
    template <class U> quickReleaseAlloc(const quickReleaseAlloc<U>&) throw() {}

    ~quickReleaseAlloc() throw() {}

    size_type max_size() const throw()
    {
        return numeric_limits<size_t>::max()/sizeof(T);
    }

    pointer allocate(size_type num, quickReleaseAlloc<void>::const_pointer = 0)
    {
        return (pointer)(::operator new(num*sizeof(T)));
    }

    void construct(pointer p, const T& value)
    {
        new ((void *)p) T(value);
    }

    void destroy(pointer p)
    {
        p->~T();
    }

    void deallocate(pointer p, size_type num)
    {
        ::operator delete((void *)p);
    }
};

template <class T, class U>
bool operator==(const quickReleaseAlloc<T>&, const quickReleaseAlloc<U>&) throw()
{
   return true;
}

template <class T, class U>
bool operator!=(const quickReleaseAlloc<T>&, const quickReleaseAlloc<U>&) throw()
{
   return false;
}

/**
 * @brief  An implementation of the nonexistent copy_if (after Meyers' STL book, Item 36). 
 */
template <class InputItr, class OutputItr, class Predicate>
OutputItr copy_if(InputItr fromBegin, InputItr fromEnd, OutputItr toBegin, Predicate pred)
{
    while (fromBegin != fromEnd) {
        if (pred(*fromBegin)) {
            *toBegin = *fromBegin;
            ++toBegin;
        }
        ++fromBegin;
    }
    return toBegin;
}

/**
 * @brief  Adopted from the old SGI template library. Makes possible things like:
 *
 *  // copy all keys of map<string, float> M to vector<string> V:
 *  transform(M.begin(), M.end(), back_inserter(V), select1st<map<string, float>::value_type>());
 *
 *  // copy all values to vector<float> W:
 *  transform(M.begin(), M.end(), back_inserter(W), select2nd<map<string, float>::value_type>());
 */
template <class PT>
struct select1st : public unary_function<PT, typename PT::first_type> {
    const typename PT::first_type& operator()(const PT& v) const
    {
        return v.first;
    }
};

template <class PT>
struct select2nd : public unary_function<PT, typename PT::second_type> {
    const typename PT::second_type& operator()(const PT& v) const
    {
        return v.second;
    }
};

}  // namespace affxstl


#endif // _AffxSTL_h_
