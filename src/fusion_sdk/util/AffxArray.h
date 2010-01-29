////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
//
// This program is free software; you can redistribute it and/or modify 
// it under the terms of the GNU General Public License (version 2) as 
// published by the Free Software Foundation.
// 
// This program is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
// General Public License for more details.
// 
// You should have received a copy of the GNU General Public License 
// along with this program;if not, write to the 
// 
// Free Software Foundation, Inc., 
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
        
//		if (iThis > iThat) {iCompareResult = 1;}
//		else if (iThis < iThat) {iCompareResult = -1;}
		
//		return iCompareResult;
    }

    static int compareNoCase(const AffxString& iThis, const AffxString& iThat)
    {
        return iThis.CompareNoCase(iThat);
//      int iCompareResult = 0;
        
//		if (iThis > iThat) {iCompareResult = 1;}
//		else if (iThis < iThat) {iCompareResult = -1;}
		
//		return iCompareResult;
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

	void quickSort() {shuffle(); quickSort(0, (getCount() - 1), 0);}
	void quickSort(int iCompareCode) {shuffle(); quickSort(0, (getCount() - 1), iCompareCode);}
	void quickSort(int iFrom, int iTo, int iCompareCode)
    {
        if (getCount() >= 2)
        {
            int i = iFrom;
            int j = iTo;
		    TYPE* objCenter = at(((iFrom + iTo) / 2));
            do
            {
                while ((i < iTo) && (objCenter->compareTo(*at(i), iCompareCode) > 0)) i++;
                while ((j > iFrom) && (objCenter->compareTo(*at(j), iCompareCode) < 0)) j--;
                if (i < j) {swap(i, j);}
                if (i <= j)
                {
                    i++;
                    j--;
                }
            } while (i <= j);

            if (iFrom < j) {quickSort(iFrom, j, iCompareCode);}
            if (i < iTo) {quickSort(i, iTo, iCompareCode);}
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
