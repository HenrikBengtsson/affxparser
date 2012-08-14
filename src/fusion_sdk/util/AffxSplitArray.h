////////////////////////////////////////////////////////////////
//
// Copyright (C) 2009 Affymetrix, Inc.
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

#ifndef _AffxSplitArray_h_
#define _AffxSplitArray_h_
/**
 * @file AffxSplitArray.h
 *
 * @brief This header contains the AffxSplitArray class definition.
 */
#include "util/Convert.h"
#include "util/Verbose.h"
//
#include <vector>
//

template <class TYPE, int NUM_ARRAYS = 10> class AffxSplitArray
{
private:
	/*! Vector of arrays of data objects. */
	std::vector<TYPE*> vData;
	/*! The size of each array of data in the data vector. */
	std::vector<int> vDataSizes;
	/*! The total number of data objects stored. */
	int vDataSize;
	/*! Allocate memory for the data.
	 * @param n The number of data objects.
	 * @param narrays The number of vector elements to create for the data storage.
	 */
	void allocate(int n, int narrays)
	{
		// NB: this code assumes n >= narrays
		vDataSize = n;
		vDataSizes.resize(narrays);
		vData.resize(narrays);

		for (int i=0; i<narrays; i++)
		{
			vDataSizes[i] = 0;
			vData[i] = NULL;
		}

		int entriesPerSet = n / narrays;
		int nentries = 0;

		//Verbose::out(1, "allocating arrays - " + ToStr(narrays));
		for (int i=0; i<narrays-1; i++)
		{
			//Verbose::out(1, "allocating array");
			vDataSizes[i] = entriesPerSet;
			vData[i] = new TYPE[entriesPerSet];
			nentries += entriesPerSet;
		}
		//Verbose::out(1, "allocating array");
		vDataSizes[narrays-1] = n - nentries;
		vData[narrays-1] = new TYPE[n - nentries];
		//Verbose::out(1, "allocating array complete");
	}

public:
	/*! Construct the class. */
	AffxSplitArray() { clear(); }
	/*! Clean up. */
	~AffxSplitArray() { clear(); }
	/*! Returns the number of data elements stored. */
	int size() const { return vDataSize; }
	/*! Allocate storage for the data.
	 * @param n The number of data elements to store.
	 */
	void allocate(int n)
	{
//#ifndef _MSC_VER
//		allocate(n, 1);
//#else
//		allocate(n, 4);
//#endif
		try
		{
			// Try to allocate in a single vector array. If this
			// succeeds then return. Failures will thown an exception.
			allocate(n, 1);
			return;
		}
		catch (...)
		{
			clear();
		}
		// The system failed to create a single array for the data.
		// Try to split the allocation into NUM_ARRAYS smaller allocations
		allocate(n, NUM_ARRAYS);
	}
	/*! Clear the data. */
	void clear()
	{
		//Verbose::out(1, "clearing array");
		vDataSize = 0;
		vDataSizes.clear();
		int n = vData.size();
		for (int i=0; i<n; i++)
			delete[] vData[i];
		vData.clear();
	}
	/*! Get the element
	 * @param i The index
	 */
	TYPE *getAt(int i)
	{
		int entriesPerSet = vDataSizes[0];
		int idx = i / entriesPerSet;
		if (idx >= vDataSizes.size())
			idx = vDataSizes.size() - 1;

		i -= (entriesPerSet * idx);
		return &vData[idx][i];
	}

	TYPE& operator[](int i)
	{
		return *getAt(i);
	}

	const TYPE& operator[](int i) const
	{
		return *getAt(i);
	}
};


#endif // _AffxArray_h_
