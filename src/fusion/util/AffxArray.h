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

#ifndef _AffxArray_h_
#define _AffxArray_h_
/**
 * @file AffxArray.h
 *
 * @brief This header contains the AffxArray class definition.
 */
#include "util/AffxByteArray.h"
#include "util/AffxString.h"
//
#include <map>
#include <vector>
#include <algorithm>
//

/**
 * @brief  A template class derived from std::vector.
 *
 */
template <class TYPE> class AffxArray : public std::vector<TYPE*>
{
public:

    static int compareAsInt(const AffxString& strThis, const AffxString& strThat)
    {
        AffxByteArray baThis(strThis);
        AffxByteArray baThat(strThat);
        int iThis = 0;
        int iThat = 0;
        try{iThis = baThis.parseInt();} catch(...) {iThis = 0;}
        try{iThat = baThat.parseInt();} catch(...) {iThat = 0;}
        return compare(iThis, iThat);
    }

    static int compareAsIntDescending(const AffxString& strThis, const AffxString& strThat)
    {
        AffxByteArray baThis(strThis);
        AffxByteArray baThat(strThat);
        int iThis = 0;
        int iThat = 0;
        try{iThis = baThis.parseInt();} catch(...) {iThis = 0;}
        try{iThat = baThat.parseInt();} catch(...) {iThat = 0;}
        return compareDescending(iThis, iThat);
    }

    static int compareCase(const AffxString& iThis, const AffxString& iThat)
    {
        return iThis.compareTo(iThat, 0);
//      int iCompareResult = 0;

//        if (iThis > iThat) {iCompareResult = 1;}
//        else if (iThis < iThat) {iCompareResult = -1;}

//        return iCompareResult;
    }

    static int compareNoCase(const AffxString& iThis, const AffxString& iThat)
    {
        return iThis.CompareNoCase(iThat);
//      int iCompareResult = 0;

//        if (iThis > iThat) {iCompareResult = 1;}
//        else if (iThis < iThat) {iCompareResult = -1;}

//        return iCompareResult;
    }

    // false then true.
    static int compare(bool bThis, bool bThat)
    {
        int iCompareResult = 0;

        if ((bThis) && (!bThat)) {iCompareResult = 1;}
        else if ((!bThis) && (bThat)) {iCompareResult = -1;}

        return iCompareResult;
    }

    static int compare(int iThis, int iThat)
    {
        int iCompareResult = 0;

        if (iThis > iThat) {iCompareResult = 1;}
        else if (iThis < iThat) {iCompareResult = -1;}

        return iCompareResult;
    }

    static int compare(unsigned int iThis, unsigned int iThat)
    {
        int iCompareResult = 0;

        if (iThis > iThat) {iCompareResult = 1;}
        else if (iThis < iThat) {iCompareResult = -1;}

        return iCompareResult;
    }

    static int compare(double dThis, double dThat)
    {
        int iCompareResult = 0;

        if (dThis > dThat) {iCompareResult = 1;}
        else if (dThis < dThat) {iCompareResult = -1;}

        return iCompareResult;
    }

    static int compareCaseDescending(const AffxString& iThis, const AffxString& iThat)
    {
        int iCompareResult = 0;

        if (iThis > iThat) {iCompareResult = -1;}
        else if (iThis < iThat) {iCompareResult = 1;}

        return iCompareResult;
    }

    // true then false.
    static int compareDescending(bool bThis, bool bThat)
    {
        int iCompareResult = 0;

        if ((!bThis) && (bThat)) {iCompareResult = 1;}
        else if ((bThis) && (!bThat)) {iCompareResult = -1;}

        return iCompareResult;
    }

    static int compareDescending(int iThis, int iThat)
    {
        int iCompareResult = 0;

        if (iThis < iThat) {iCompareResult = 1;}
        else if (iThis > iThat) {iCompareResult = -1;}

        return iCompareResult;
    }

    static int compareDescending(double dThis, double dThat)
    {
        int iCompareResult = 0;

        if (dThis < dThat) {iCompareResult = 1;}
        else if (dThis > dThat) {iCompareResult = -1;}

        return iCompareResult;
    }
public:
    AffxArray(int iSize, int iCapacity) {reserve(iCapacity); resize(iSize);}
    AffxArray() {reserve(1000);}
    virtual ~AffxArray() {}

    void reserve(int iSize) {return std::vector<TYPE*>::reserve(iSize);}
    typename std::vector<TYPE*>::size_type size() const {return std::vector<TYPE*>::size();}
    void resize(int iNewSize) {return std::vector<TYPE*>::resize(iNewSize);}
    TYPE*& at(int iIndex) {return std::vector<TYPE*>::at(iIndex);}

    void nullAll()
    {
        for (int iIndex = 0; (iIndex < getCount()); iIndex++)
        {
            at(iIndex) = NULL;
        }
        std::vector<TYPE*>::clear();
    }

    void deleteAll()
    {
        for (int iIndex = 0; (iIndex < getCount()); iIndex++)
        {
            if (at(iIndex) != NULL) {delete at(iIndex);}
            at(iIndex) = NULL;
        }
        std::vector<TYPE*>::clear();
    }

    int getCount() {return (int)size();}
    void setAt(int iIndex, TYPE* obj) {if ((iIndex >= 0) && (iIndex < getCount())) {at(iIndex) = obj;}}
    TYPE* getAt(int iIndex)
    {
        TYPE* obj = NULL;

        if ((iIndex >= 0) && (iIndex < getCount()))
        {
            obj = at(iIndex);
        }

        return obj;
    }

    void add(TYPE* obj)
    {
        push_back(obj);
    }

    int binarySearch(TYPE& obj, int iCompareCode)
    {
        int iSearchIndex = -1;
        int iLow = 0;
        int iHigh = getCount() - 1;
        int iMid = 0;
        int iCompareResult = 0;

        while (iLow <= iHigh)
        {
            iMid = (iLow + iHigh) / 2;
            iCompareResult = obj.compareTo(*at(iMid), iCompareCode);
            if (iCompareResult < 0)
            {
                iHigh = iMid - 1;
            }
            else if (iCompareResult > 0)
            {
                iLow = iMid + 1;
            }
            else
            {
                iSearchIndex = iMid;
                break;
            }
        }

        return iSearchIndex;
    }

    void quickSort() {/*shuffle();*/ quickSort(0, (getCount() - 1), 0);}
    void quickSort(int iCompareCode) {/*shuffle();*/ quickSort(0, (getCount() - 1), iCompareCode);}
    void quickSort(int iFrom, int iTo, int iCompareCode)
    {
        int jTo = iTo + 1;

        // If a new value of iCompareCode for elements of type TYPE is required, add the corresponding new
        // case to the switch statement below and add the specialized template functor implementing the new
        // comparison to class TYPE. See CNProbeSet.h for an example.
        //
        // The "::template" qualifiers below are needed to tell the compiler not to interpret the left angle
        // bracket as "less than". The "typename" then similarly indicates that ComparePred<i>() is not
        // a template function but a template class.
        //
        switch (iCompareCode) {
        case 0:
            std::sort(this->begin() + iFrom, this->begin() + jTo, typename TYPE::template ComparePred<0>());
            break;
        case 1:
            std::sort(this->begin() + iFrom, this->begin() + jTo, typename TYPE::template ComparePred<1>());
            break;
        case 2:
            std::sort(this->begin() + iFrom, this->begin() + jTo, typename TYPE::template ComparePred<2>());
            break;
        case 3:
            std::sort(this->begin() + iFrom, this->begin() + jTo, typename TYPE::template ComparePred<3>());
            break;
        default:
            Err::errAbort("Unknown compare code found in AffxArray");
        }
    }

    void swap(int iIndex1, int iIndex2)
    {
        TYPE* temp = at(iIndex1);
        at(iIndex1) = at(iIndex2);
        at(iIndex2) = temp;
    }

    void shuffle()
    {
        int iNewIndex = 0;
        TYPE* pobj = NULL;
        for (int iIndex = (getCount() - 1); (iIndex > 0); iIndex--)
        {
            iNewIndex = (rand() % getCount());
            if ((iNewIndex >= 0) && (iNewIndex < getCount()))
            {
                pobj = at(iNewIndex);
                at(iNewIndex) = at(iIndex);
                at(iIndex) = pobj;
            }
        }
    }

    void qsort() {qsort(0, (getCount() - 1), 0);}
    void qsort(int iCompareCode) {qsort(0, (getCount() - 1), iCompareCode);}
    void qsort(int iFrom, int iTo, int iCompareCode)
    {
        if (getCount() >= 2)
        {
            int i = iFrom;
            int j = iTo;
            TYPE* objCenter = getAt((iFrom + iTo) / 2);
            do
            {
                while ((i < iTo) && (objCenter->compareTo(*getAt(i), iCompareCode) > 0)) i++;
                while ((j > iFrom) && (objCenter->compareTo(*getAt(j), iCompareCode) < 0)) j--;
                if (i < j) {TYPE* temp = getAt(i); setAt(i, getAt(j)); setAt(j, temp);}
                if (i <= j)
                {
                    i++;
                    j--;
                }
            } while (i <= j);

            if (iFrom < j) {qsort(iFrom, j, iCompareCode);}
            if (i < iTo) {qsort(i, iTo, iCompareCode);}
        }
    }

    void removeAt(int iIndex)
    {
        erase(std::vector<TYPE*>::begin() + iIndex, std::vector<TYPE*>::begin() + iIndex + 1);
    }

    void deleteAt(int iIndex)
    {
        delete at(iIndex);
        erase(std::vector<TYPE*>::begin() + iIndex, std::vector<TYPE*>::begin() + iIndex + 1);
    }
};

#endif // _AffxArray_h_
