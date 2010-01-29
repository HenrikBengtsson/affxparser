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

#ifndef _AffxMultiDimensionalArray_h_
#define _AffxMultiDimensionalArray_h_
/**
 * @file AffxMultiDimensionalArray.h
 *
 * @brief This header contains the AffxMultiDimensionalArray class definition. 
 */
#include "util/AffxArray.h"
#include "util/AffxFile.h"
#include "util/Err.h"
#include "util/Util.h"
#include "stats/stats.h"
//
#include "portability/affy-base-types.h"

#include <limits>
#include <memory.h>
//

using namespace std;

/**
 * @brief  A class for managing one, two, or three dimensional arrays of primitive data types.
 * 
 */
template<class TYPE> class AffxMultiDimensionalArray
{
private:
	TYPE* m_ar;
	int m_iXDimension;
	int m_iYDimension;
	int m_iZDimension;

public:
	static double infinity;
	TYPE* getPointer() {mustBeVector(); return m_ar;}
	int getXDimension() {return m_iXDimension;}
	int getYDimension() {return m_iYDimension;}
	int getZDimension() {return m_iZDimension;}
	void convertMatrixToVector() {m_iXDimension = m_iXDimension * m_iYDimension; m_iYDimension = 1;}

	int length() {return m_iXDimension;}
	bool isVector() {return ((m_iXDimension > 0) && (m_iYDimension == 1) && (m_iZDimension == 1));}
	bool isMatrix() {return ((m_iXDimension > 0) && (m_iYDimension > 1) && (m_iZDimension == 1));}

	void increment(int x) {m_ar[x]++;}

	AffxMultiDimensionalArray()
	{
		m_iXDimension = 1;
		m_iYDimension = 1;
		m_iZDimension = 1;
		m_ar = new TYPE[getCount()];
		if (m_ar == NULL) {Err::errAbort("Run is out of memory.");}
		initialize();
	}

	AffxMultiDimensionalArray(int iXDimension)
	{
		m_iXDimension = iXDimension;
		m_iYDimension = 1;
		m_iZDimension = 1;
		m_ar = new TYPE[getCount()];
		if (m_ar == NULL) {Err::errAbort("Run is out of memory.");}
		initialize();
	}

	AffxMultiDimensionalArray(int iXDimension, int iYDimension)
	{
		m_iXDimension = iXDimension;
		m_iYDimension = iYDimension;
		m_iZDimension = 1;
		m_ar = new TYPE[getCount()];
		if (m_ar == NULL) {Err::errAbort("Run is out of memory.");}
		initialize();
	}

	AffxMultiDimensionalArray(int iXDimension, int iYDimension, int iZDimension)
	{
		m_iXDimension = iXDimension;
		m_iYDimension = iYDimension;
		m_iZDimension = iZDimension;
		m_ar = new TYPE[getCount()];
		if (m_ar == NULL) {Err::errAbort("Run is out of memory.");}
		initialize();
	}

	AffxMultiDimensionalArray(const AffxMultiDimensionalArray<TYPE>& that)
	{
		m_iXDimension = that.m_iXDimension;
		m_iYDimension = that.m_iYDimension;
		m_iZDimension = that.m_iZDimension;
		m_ar = new TYPE[getCount()];
		if (m_ar == NULL) {Err::errAbort("Run is out of memory.");}
		memcpy(m_ar, that.m_ar, (getCount() * sizeof(TYPE)));
	}
	
	void initialize(int iXDimension)
	{
		if (m_ar != 0)
		{
			delete[] m_ar;
			m_ar = 0;
		}
		m_iXDimension = iXDimension;
		m_iYDimension = 1;
		m_iZDimension = 1;
		m_ar = new TYPE[getCount()];
		if (m_ar == NULL) {Err::errAbort("Run is out of memory.");}
		initialize();
	}
	void initialize(int iXDimension, int iYDimension)
	{
		if (m_ar != 0)
		{
			delete[] m_ar;
			m_ar = 0;
		}
		m_iXDimension = iXDimension;
		m_iYDimension = iYDimension;
		m_iZDimension = 1;
		m_ar = new TYPE[getCount()];
		if (m_ar == NULL) {Err::errAbort("Run is out of memory.");}
		initialize();
	}

	void initialize(int iXDimension, int iYDimension, int iZDimension)
	{
		if (m_ar != 0)
		{
			delete[] m_ar;
			m_ar = 0;
		}
		m_iXDimension = iXDimension;
		m_iYDimension = iYDimension;
		m_iZDimension = iZDimension;
		m_ar = new TYPE[getCount()];
		if (m_ar == NULL) {Err::errAbort("Run is out of memory.");}
		initialize();
	}

	const AffxMultiDimensionalArray<TYPE>& operator=(AffxMultiDimensionalArray<TYPE>& that)
	{
		if (this == &that) return that;
		if (m_ar != 0)
		{
			delete[] m_ar;
			m_ar = 0;
		}
		m_iXDimension = that.m_iXDimension;
		m_iYDimension = that.m_iYDimension;
		m_iZDimension = that.m_iZDimension;
		m_ar = new TYPE[getCount()];
		if (m_ar == NULL) {Err::errAbort("Run is out of memory.");}
		memcpy(m_ar, that.m_ar, (getCount() * sizeof(TYPE)));	
		return *this;
	}

	virtual ~AffxMultiDimensionalArray()
	{
		clear();
	}

	void clear()
	{
		if (m_ar != 0)
		{
			delete[] m_ar;
			m_ar = 0;			
			m_iXDimension = 0;
			m_iYDimension = 0;
			m_iZDimension = 0;
		}
	}

	static void defineInfinity()
	{
		double d1 = 1.0;
		double d2 = 0.0;	
		infinity = (d1 / d2);
	}

	bool isAllInfinity()
	{
		bool b = true;
		for (int iIndex = 0; (iIndex < length()); iIndex++)
		{
			if (get(iIndex) != infinity)
			{
				b = false;
				break;
			}
		}
		return b;
	}

	int getCount()
	{
		return (m_iXDimension * m_iYDimension * m_iZDimension);
	}

    int compareTo(AffxMultiDimensionalArray& that)
	{	
		int iResult = 0;
		int iLength = min(getCount(), that.getCount());

		bool bEqual = true;
		for (int iIndex = 0; (iIndex < iLength); iIndex++)
		{
			if (m_ar[iIndex] != that.m_ar[iIndex])
			{
				bEqual = false;
				iResult = (m_ar[iIndex] - that.m_ar[iIndex]);
				break;
			}
		}
		if (bEqual) {iResult = (getCount() - that.getCount());}

		return iResult;
	}

	bool equals(AffxMultiDimensionalArray& that)
	{
		bool bEquals = false;
		if (getCount() == that.getCount())
		{
			for (int i = 0; (i < getCount()); i++)
			{
				bEquals = true;
				if (m_ar[i] != that.m_ar[i])
				{
					bEquals = false;
					break;
				}
			}
		}
		return bEquals;
	}

	void initialize()
	{		
		memset(m_ar, 0, getCount() * sizeof(TYPE));
	}
	void set(int x, TYPE t)
	{
		int iIndex = x;
		if ((iIndex < 0) || (iIndex >= getCount())) {Err::errAbort("AffxMultiDimensionalArray::set(x, TYPE) index out of bounds. " + ::getInt(x));}
		m_ar[iIndex] = t;
	}
	void* getPointer(int x)
	{
		int iIndex = x;
		if ((iIndex < 0) || (iIndex >= getCount())) {Err::errAbort("AffxMultiDimensionalArray::getPointer(x) index out of bounds. " + ::getInt(x));}
		return &m_ar[x];
	}
	void set(int x, int y, TYPE t)
	{
		int iIndex = ((x * m_iYDimension) + y);
		if ((iIndex < 0) || (iIndex >= getCount())) {Err::errAbort("AffxMultiDimensionalArray::set(x, y, TYPE) index out of bounds. " + ::getInt(x) + ", " + ::getInt(y));}
		m_ar[iIndex] = t;
	}
	void* getPointer(int x, int y)
	{
		int iIndex = ((x * m_iYDimension) + y);
		if ((iIndex < 0) || (iIndex >= getCount())) {Err::errAbort("AffxMultiDimensionalArray::getPointer(x, y) index out of bounds. " + ::getInt(x) + ", " + ::getInt(y));}
		return &m_ar[iIndex];
	}
	void set(int x, int y, int z, TYPE t)
	{		
		int iIndex = ((x * m_iYDimension * m_iZDimension) + (y * m_iZDimension) + z);
		if ((iIndex < 0) || (iIndex >= getCount())) {Err::errAbort("AffxMultiDimensionalArray::set(x, y, z, TYPE) index out of bounds. " + ::getInt(x) + ", " + ::getInt(y) + ", " + ::getInt(z));}
		m_ar[iIndex] = t;
	}
	TYPE get(int x)
	{
		int iIndex = x;
		if ((iIndex < 0) || (iIndex >= getCount())) {Err::errAbort("AffxMultiDimensionalArray::get(x) index out of bounds. " + ::getInt(x));}
		return m_ar[iIndex];
	}
	TYPE get(int x, int y)
	{
		int iIndex = ((x * m_iYDimension) + y);
		if ((iIndex < 0) || (iIndex >= getCount())) {Err::errAbort("AffxMultiDimensionalArray::get(x, y) index out of bounds. " + ::getInt(x) + ", " + ::getInt(y));}
		return m_ar[iIndex];
	}
	TYPE get(int x, int y, int z)
	{
		int iIndex = ((x * m_iYDimension * m_iZDimension) + (y * m_iZDimension) + z);
		if ((iIndex < 0) || (iIndex >= getCount())) {Err::errAbort("AffxMultiDimensionalArray::get(x, y, z) index out of bounds. " + ::getInt(x) + ", " + ::getInt(y) + ", " + ::getInt(z));}
		return m_ar[iIndex];
	}

	void mustBeVector() 
	{
		if (!isVector()) 
		{
//			AffxLog::throwable(new Exception("AffxMultidimensionalArray function must operate on a vector."));
		}
	}

	void insertAt(int x, TYPE t)
	{
		mustBeVector();
		AffxMultiDimensionalArray<TYPE> arTemp(*this);
		if (m_ar != 0)
		{
			delete[] m_ar;
			m_ar = 0;
		}
		m_iXDimension++;
		m_ar = new TYPE[getCount()];
		if (m_ar == NULL) {Err::errAbort("Run is out of memory.");}
		initialize();
		if (x > 0) {memcpy(m_ar, arTemp.getPointer(), (x * sizeof(TYPE)));}	
		memcpy((m_ar + x), &t, sizeof(TYPE));
		if (x < arTemp.length()) {memcpy((m_ar + x + 1), (arTemp.getPointer() + x), ((arTemp.length() - x) * sizeof(TYPE)));} 
	}

	void removeAt(int x)
	{
		if (length() < 1) return;
		mustBeVector();
		if (x < (length() - 1))
		{
			memmove((m_ar + x), (m_ar + x + 1), ((m_iXDimension - x - 1) * sizeof(TYPE)));
		}
		m_iXDimension--;
	}

	AffxMultiDimensionalArray<TYPE> unique()
	{
		mustBeVector();
		AffxMultiDimensionalArray<TYPE> ar(*this);
		for (int iIndex = 1; (iIndex < ar.length()); iIndex++)
		{
			for (int iIndex2 = 0; (iIndex2 < iIndex); iIndex2++)
			{
				if (ar.get(iIndex) == ar.get(iIndex2)) 
				{
					ar.removeAt(iIndex); 
					iIndex--; 
					break;
				}
			}
		}
		return ar;
	}

	AffxMultiDimensionalArray<TYPE> diff()
	{
		mustBeVector();
//		if (length() < 2) {AffxLog::throwable(new Exception("AffxMultidimensionalArray function diff(int) must operate on a vector with more than one element in it."));}
		AffxMultiDimensionalArray<TYPE> ar(this->m_iXDimension - 1);
		for (int iIndex = 1; (iIndex < length()); iIndex++)
		{
			ar.set((iIndex - 1), get(iIndex) - get(iIndex - 1));
		}
		return ar;
	}

	AffxMultiDimensionalArray<TYPE> rev()
	{
		mustBeVector();
		AffxMultiDimensionalArray<TYPE> ar(this->length());
		for (int iIndex = (length() - 1); (iIndex >= 0); iIndex--)
		{
			ar.set(((length() - 1) - iIndex), get(iIndex));
		}
		return ar;
	}

	void calculatePointEstimates(double& dPointEstimate, double& dPointEstimateLower, double& dPointEstimateUpper)
	{
		if (nonZeroLength() == 0) {dPointEstimate = 0.0; dPointEstimateLower = 0.0; dPointEstimateUpper = 0.0; return;}
		if (mean() == 0.0) {dPointEstimate = 0.0; dPointEstimateLower = 0.0; dPointEstimateUpper = 0.0; return;}
		else if (round(mean(), 4) == 1.0) {dPointEstimate = 1.0; dPointEstimateLower = 1.0; dPointEstimateUpper = 1.0; return;}
		else
		{
			AffxMultiDimensionalArray<TYPE> ar(length());
			for (int iIndex = 0; (iIndex < ar.length()); iIndex++)
			{
				double d = get(iIndex);
				if (d == 0.0) {d = d + 0.0001;}
				if (d == 1.0) {d = d - 0.0001;}
				ar.set(iIndex, log(d/(1-d)));
			}
			double dMean = ar.mean();
			double dSd = ar.sd();
			double dMeanUpper = dMean+2*dSd/sqrt((double)ar.length());
			double dMeanLower = dMean-2*dSd/sqrt((double)ar.length());

			dPointEstimateUpper = exp(dMeanUpper)/(1+exp(dMeanUpper));
			dPointEstimate = exp(dMean)/(1+exp(dMean));
			dPointEstimateLower = exp(dMeanLower)/(1+exp(dMeanLower));
		}
	}

	int nonZeroLength()
	{
		int iCount = 0;
		for (int iIndex = 0; (iIndex < length()); iIndex++)
		{
			if (get(iIndex) != 0.0) {iCount++;}
		}
		return iCount;
	}
	
	double mean()
	{	
		double dMean = 0.0;
		mustBeVector();    
		if (length() > 0)
	    {
	        double dSum = 0.0;
	        for (int iIndex = 0; (iIndex < length()); iIndex++)
	        {
	            dSum += get(iIndex);
	        }
			dMean = dSum / (double)length();
	    }
		return dMean;
	}

	double mean(int iLength)
	{	
		double dMean = 0.0;
		mustBeVector();    
		if (iLength > 0)
	    {
	        double dSum = 0.0;
	        for (int iIndex = 0; (iIndex < iLength); iIndex++)
	        {
	            dSum += get(iIndex);
	        }
			dMean = dSum / (double)iLength;
	    }
		return dMean;
	}

	void logMean(bool bXDimension, bool bYDimension, AffxMultiDimensionalArray<TYPE>& vect)
	{
		vect.initialize();
		if ((!bXDimension) && (!bYDimension)) {return;}
//		if ((bXDimension) && (bYDimension)) {AffxLog::throwable(new Exception("AffxMultidimensionalArray function must operate on a vector."));}
		if (bXDimension)
		{
//			if (vect.length() != m_iXDimension) {AffxLog::throwable(new Exception("AffxMultidimensionalArray function must operate on a vector of the appropriate size."));}
			for (int iXIndex = 0; (iXIndex < m_iXDimension); iXIndex++)
			{
				for (int iYIndex = 0; (iYIndex < m_iYDimension); iYIndex++)
				{
					vect.set(iXIndex, (TYPE)(vect.get(iXIndex) + log(get(iXIndex, iYIndex))));
				}
			}		
			for (int iXIndex = 0; (iXIndex < m_iXDimension); iXIndex++)
			{
				vect.set(iXIndex, (vect.get(iXIndex) / m_iYDimension));
			}
		}		
		else if (bYDimension)
		{
//			if (vect.length() != m_iYDimension) {AffxLog::throwable(new Exception("AffxMultidimensionalArray function must operate on a vector of the appropriate size."));}
			for (int iXIndex = 0; (iXIndex < m_iXDimension); iXIndex++)
			{
				for (int iYIndex = 0; (iYIndex < m_iYDimension); iYIndex++)
				{
					vect.set(iYIndex, (TYPE)(vect.get(iYIndex) + log(get(iXIndex, iYIndex))));
				}
			}		
			for (int iYIndex = 0; (iYIndex < m_iYDimension); iYIndex++)
			{
				vect.set(iYIndex, (vect.get(iYIndex) / m_iXDimension));
			}
		}
	}

	void logVar(bool bXDimension, bool bYDimension, AffxMultiDimensionalArray<TYPE>& vectMeans, AffxMultiDimensionalArray<TYPE>& vectVariances)
	{
		vectMeans.initialize();
		vectVariances.initialize();
		if ((!bXDimension) && (!bYDimension)) {return;}
//		if ((bXDimension) && (bYDimension)) {AffxLog::throwable(new Exception("AffxMultidimensionalArray function must operate on a vector."));}
		if (bXDimension)
		{
//			if (vectMeans.length() != m_iXDimension) {AffxLog::throwable(new Exception("AffxMultidimensionalArray function must operate on a vector of the appropriate size."));}
//			if (vectVariances.length() != m_iXDimension) {AffxLog::throwable(new Exception("AffxMultidimensionalArray function must operate on a vector of the appropriate size."));}
			// Calculate Means.
			for (int iXIndex = 0; (iXIndex < m_iXDimension); iXIndex++)
			{
				for (int iYIndex = 0; (iYIndex < m_iYDimension); iYIndex++)
				{
					vectMeans.set(iXIndex, (TYPE)(vectMeans.get(iXIndex) + log(get(iXIndex, iYIndex))));
				}
			}		
			for (int iXIndex = 0; (iXIndex < m_iXDimension); iXIndex++)
			{
				vectMeans.set(iXIndex, (vectMeans.get(iXIndex) / m_iYDimension));
			}
			// Calculate variances.
			for (int iXIndex = 0; (iXIndex < m_iXDimension); iXIndex++)
			{
				for (int iYIndex = 0; (iYIndex < m_iYDimension); iYIndex++)
				{
					vectVariances.set(iXIndex, (TYPE)(vectVariances.get(iXIndex) + ((log(get(iXIndex, iYIndex)) - vectMeans.get(iXIndex)) * (log(get(iXIndex, iYIndex)) - vectMeans.get(iXIndex)))));
				}
			}		
			for (int iXIndex = 0; (iXIndex < m_iXDimension); iXIndex++)
			{
				vectVariances.set(iXIndex, (vectVariances.get(iXIndex) / (m_iYDimension - 1)));
			}
		}		
		else if (bYDimension)
		{
//			if (vectMeans.length() != m_iYDimension) {AffxLog::throwable(new Exception("AffxMultidimensionalArray function must operate on a vector of the appropriate size."));}
//			if (vectVariances.length() != m_iYDimension) {AffxLog::throwable(new Exception("AffxMultidimensionalArray function must operate on a vector of the appropriate size."));}
			// Calculate Means.
			for (int iXIndex = 0; (iXIndex < m_iXDimension); iXIndex++)
			{
				for (int iYIndex = 0; (iYIndex < m_iYDimension); iYIndex++)
				{
					vectMeans.set(iYIndex, (TYPE)(vectMeans.get(iYIndex) + log(get(iXIndex, iYIndex))));
				}
			}		
			for (int iYIndex = 0; (iYIndex < m_iYDimension); iYIndex++)
			{
				vectMeans.set(iYIndex, (vectMeans.get(iYIndex) / m_iXDimension));
			}
			// Calculate variances.
			for (int iXIndex = 0; (iXIndex < m_iXDimension); iXIndex++)
			{
				for (int iYIndex = 0; (iYIndex < m_iYDimension); iYIndex++)
				{
					vectVariances.set(iYIndex, (TYPE)(vectVariances.get(iYIndex) + ((log(get(iXIndex, iYIndex)) - vectMeans.get(iYIndex)) * (log(get(iXIndex, iYIndex)) - vectMeans.get(iYIndex)))));
				}
			}		
			for (int iYIndex = 0; (iYIndex < m_iYDimension); iYIndex++)
			{
				vectVariances.set(iYIndex, (vectVariances.get(iYIndex) / (m_iXDimension - 1)));
			}
		}
	}

	double nonZeroMean()
	{	
		double dMean = 0.0;
		mustBeVector();    
		if (length() > 0)
	    {
	        double dSum = 0.0;
			int iCount = 0;
	        for (int iIndex = 0; (iIndex < length()); iIndex++)
	        {
				if (get(iIndex) != 0)
				{
					dSum += get(iIndex);
					iCount++;
				}
	        }
			if (iCount == 0) {dMean = 0.0;} else {dMean = dSum / (double)iCount;}
	    }
		return dMean;
	}

	double sd()
	{	
		double dStandardDeviation = 0.0;
		double dMean = mean();
		mustBeVector();    
		if (length() > 1)
	    {
	        double dSumOfSquares = 0.0;
	        for (int iIndex = 0; (iIndex < length()); iIndex++)
	        {
	            dSumOfSquares += ((get(iIndex) - dMean) * (get(iIndex) - dMean));
	        }
			dStandardDeviation = sqrt(dSumOfSquares / (length() - 1));
	    }
		return dStandardDeviation;
	}

	double var()
	{	
		double dVariance = 0.0;
		double dMean = mean();
		mustBeVector();    
		if (length() > 1)
	    {
	        double dSumOfSquares = 0.0;
	        for (int iIndex = 0; (iIndex < length()); iIndex++)
	        {
	            dSumOfSquares += ((get(iIndex) - dMean) * (get(iIndex) - dMean));
	        }
			dVariance = dSumOfSquares / (length() - 1);
	    }
		return dVariance;
	}

    double var(int iLength)
	{	
		double dVariance = 0.0;
		double dMean = mean(iLength);
		mustBeVector();    
		if (iLength > 1)
	    {
	        double dSumOfSquares = 0.0;
	        for (int iIndex = 0; iIndex < iLength; iIndex++)
	        {
	            dSumOfSquares += ((get(iIndex) - dMean) * (get(iIndex) - dMean));
	        }
			dVariance = dSumOfSquares / (iLength - 1);
	    }
		return dVariance;
	}


	double corr()
	{
		if (getYDimension() != 2 ) {return std::numeric_limits<double>::quiet_NaN();}
		int iLength = getXDimension();
		double dSum = 0;
		for (int iElementIndex = 0; (iElementIndex < iLength); iElementIndex++)
		{
			dSum += get(iElementIndex, 0);
		}
		double m1 = (dSum / (double)iLength);
		dSum = 0;
		for (int iElementIndex = 0; (iElementIndex < iLength); iElementIndex++)
		{
			dSum += get(iElementIndex, 1);
		}
		double m2 = (dSum / (double)iLength);
		double dNumerator = 0;
		double d1 = 0;;
		double d2 = 0;
		for (int iElementIndex = 0; (iElementIndex < iLength); iElementIndex++)
		{
			dNumerator += (get(iElementIndex, 0) - m1) * (get(iElementIndex, 1) - m2);
			d1 += (get(iElementIndex, 0) - m1) * (get(iElementIndex, 0) - m1) ;
			d2 += (get(iElementIndex, 1) - m2) * (get(iElementIndex, 1) - m2) ;
		}
//		double dDenominator = sqrt(d1 * d2);
		return dNumerator / sqrt(d1) / sqrt(d2);
	}

	void test()
	{
		mustBeVector();
		for (int iIndex = 0; (iIndex < length()); iIndex++)
		{
			TYPE t = get(iIndex);
			int x = 0;
		}
	}
	
	void quickSort() {mustBeVector(); quickSort(0, (length() - 1), 0);}
	void quickSort(int iFrom, int iTo, int iCompareCode)
    {
        if ((m_ar != NULL) && (length() >= 2))
        {
		    TYPE objCenter = 0;
            int i = iFrom;
            int j = iTo;
		    objCenter = get((iFrom + iTo) / 2);
            do
            {
                while ((i < iTo) && (objCenter > get(i))) i++;
                while ((j > iFrom) && (objCenter < get(j))) j--;
                if (i < j) {swap(i, j);}
                if (i <= j)
                {
                    i++;
                    j--;
                }
            } while (i <= j);

            if (iFrom < j) {quickSort(iFrom, j, iCompareCode);}
            if (i < iTo) {quickSort(i, iTo, iCompareCode);}
            objCenter = 0;
        }
    }

    void swap(int iIndex1, int iIndex2)
    {
		TYPE temp = get(iIndex1);
		set(iIndex1, get(iIndex2));
		set(iIndex2, temp);
    }
	
	double percentile(double dPercentile, bool bSort = true)
	{
		if (length() == 0) {return numeric_limits<double>::quiet_NaN();}
		double d = 0.0;
		mustBeVector();
		if (bSort) {quickSort();}
		double dIndex = ((length() - 1) * dPercentile);
		double dMultiplier = dIndex - floor(dIndex);
		d = get((int)floor(dIndex)) + ((get((int)ceil(dIndex)) - get((int)floor(dIndex))) * dMultiplier);
		return d;
	}

	double percentile(double dPercentile, int iLength, bool bSort = true)
	{
		if (iLength == 0) {return numeric_limits<double>::quiet_NaN();}
		double d = 0.0;
		mustBeVector();
		if (bSort) {quickSort(0, (iLength - 1), 0);}
		double dIndex = ((iLength - 1) * dPercentile);
		double dMultiplier = dIndex - floor(dIndex);
		d = get((int)floor(dIndex)) + ((get((int)ceil(dIndex)) - get((int)floor(dIndex))) * dMultiplier);
		return d;
	}
  
	double finiteMedian()
	{
		mustBeVector();
                vector<double> finiteNumbers;
                for(int i = 0; i < this->length(); i++) 
                {
                  double d = (double) get(i);
                  if(Util::isFinite(d)) 
                  {
                    finiteNumbers.push_back(d);
                  }
                }
                if (finiteNumbers.empty()) {return numeric_limits<double>::quiet_NaN();}
                double med = median_in_place(finiteNumbers.begin(),finiteNumbers.end());
                return med;
        }
  

	double median()
	{
		if (length() == 0) {return numeric_limits<double>::quiet_NaN();}
		double d = 0.0;
		mustBeVector();
//		AffxMultiDimensionalArray<TYPE> ar(this->length());
//		for (int iIndex = (length() - 1); (iIndex >= 0); iIndex--)
//		{
//			ar.set(((length() - 1) - iIndex), get(iIndex));
//		}
		quickSort();
		if ((length() % 2) == 0)
		{
			d = get((int)((double)length() / 2.0) - 1) + ((double)(get((int)((double)length() / 2.0)) - get((int)((double)length() / 2.0) - 1)) / 2.0);
		}
		else
		{
			d = get((int)floor((double)length() / 2.0));
		}
		return d;
	}

	double median(int iLength)
	{
		if (iLength == 0) {return numeric_limits<double>::quiet_NaN();}
		double d = 0.0;
		mustBeVector();
		quickSort(0, (iLength - 1), 0);
		if ((iLength % 2) == 0)
		{
			d = get((int)((double)iLength / 2.0) - 1) + ((double)(get((int)((double)iLength / 2.0)) - get((int)((double)iLength / 2.0) - 1)) / 2.0);
		}
		else
		{
			d = get((int)floor((double)iLength / 2.0));
		}
		return d;
	}

	static int readVectorSize(const AffxString& strFileName, bool bHeader)
	{
		AffxFile file;
		AffxByteArray baLine;
		AffxByteArray baColumn;
		int iCount = 0;
		if (file.open(strFileName, AffxFile::LOAD))
		{
			if (bHeader) {file.readLine(baLine);}
			while (file.readLine(baLine)) {iCount++;}
			file.close();
		} else {return 0;}
		return iCount;
	}

	bool readVector(const AffxString& strFileName, bool bHeader)
	{
		try
		{
		AffxFile file;
		AffxByteArray baLine;
		AffxByteArray baColumn;
		int iCount = 0;
		if (file.open(strFileName, AffxFile::LOAD))
		{
			if (bHeader) {file.readLine(baLine);}
			while (file.readLine(baLine)) {set(iCount, baLine.getColumn(1, baColumn).trim().parseDouble()); iCount++;}
			file.close();
		} else {return false;}
		return true;
		} catch(...) {return false;}
	}

	static int readMatrixYSize(const AffxString& strFileName, bool bColHeader)
	{
		AffxFile file;
		AffxByteArray baLine;
		AffxByteArray baColumn;
		int iCount = 0;
		if (file.open(strFileName, AffxFile::LOAD))
		{
			if (bColHeader) {file.readLine(baLine);}
			while (file.readLine(baLine)) {iCount++;}
			file.close();
		} else {return 0;}
		return iCount;
	}

	static int readMatrixXSize(const AffxString& strFileName, bool bRowHeader)
	{
		AffxFile file;
		AffxByteArray baLine;
		AffxByteArray baColumn;
		int iCount = 0;
		if (file.open(strFileName, AffxFile::LOAD))
		{
			file.readLine(baLine);
			iCount = baLine.getColumnCount();
			if (bRowHeader) {iCount--;}
			file.close();
		} else {return 0;}
		return iCount;
	}

	bool readMatrixXHeader(const AffxString& strFileName, bool bRowHeader, AffxArray<AffxString>& ar)
	{
		ar.deleteAll();
		AffxFile file;
		AffxByteArray baLine;
		AffxByteArray baColumn;
		int iCount = 0;
		if (file.open(strFileName, AffxFile::LOAD))
		{
			if (file.readLine(baLine)) 
			{
				int iColumnCount = baLine.getColumnCount();
				int iColumnNumber = 1;
				if (bRowHeader) {iColumnNumber++;}
				for (; (iColumnNumber <= iColumnCount); iColumnNumber++)
				{
					ar.add(new AffxString(baLine.getColumn(iColumnNumber, baColumn).trim().toString()));
				}
				iCount++;
			}
			file.close();
		} else {return false;}
		return true;
	}

	bool readMatrixYHeader(const AffxString& strFileName, bool bColHeader, AffxArray<AffxString>& ar)
	{
		ar.deleteAll();
		AffxFile file;
		AffxByteArray baLine;
		AffxByteArray baColumn;
		int iCount = 0;
		if (file.open(strFileName, AffxFile::LOAD))
		{
			if (bColHeader) {file.readLine(baLine);}
			while (file.readLine(baLine)) 
			{
				int iColumnNumber = 1;
				ar.add(new AffxString(baLine.getColumn(iColumnNumber, baColumn).trim().toString()));
				iCount++;
			}
			file.close();
		} else {return false;}
		return true;
	}

	bool readMatrix(const AffxString& strFileName, bool bColHeader, bool bRowHeader, int iColumnCount)
	{
		try
		{
		AffxFile file;
		AffxByteArray baLine;
		AffxByteArray baColumn;
		int iCount = 0;
		if (bRowHeader) {iColumnCount++;}
		if (file.open(strFileName, AffxFile::LOAD))
		{
			if (bColHeader) {file.readLine(baLine);}
			while (file.readLine(baLine)) 
			{
				int iColumnNumber = 1;
				if (bRowHeader) {iColumnNumber++;}
				int iColIndex = 0;
				for (; (iColumnNumber <= iColumnCount); iColumnNumber++)
				{
					set(iColIndex, iCount, baLine.getColumn(iColumnNumber, baColumn).trim().parseDouble());
					iColIndex++;
				}
				iCount++;
			}
			file.close();
		} else {return false;}
		return true;
		} catch(...) {return false;}
	}

	int binarySearch(TYPE& obj)
	{
	    int iSearchIndex = -1;
		int iLow = 0;
		int iHigh = m_iXDimension - 1;
		int iMid = 0;
		int iCompareResult = 0;

		while (iLow <= iHigh)
		{
			iMid = (iLow + iHigh) / 2;
			iCompareResult = AffxArray<TYPE>::compare(obj, get(iMid));
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

	int binarySearch(TYPE obj, unsigned int& uiLowIndex, unsigned int& uiHighIndex)
	{
	    int iSearchIndex = -1;
		if (obj < get(0)) {uiLowIndex = uiHighIndex = 0; return -1;}
		if (obj > get(m_iXDimension - 1)) {uiLowIndex = uiHighIndex = (m_iXDimension + 1); return -1;}

		int iMid = 0;
		int iLow = 0;
		int iHigh = m_iXDimension - 1;
		int iCompareResult = 0;

		while (iLow <= iHigh)
		{
			iMid = (iLow + iHigh) / 2;
			iCompareResult = AffxArray<TYPE>::compare(obj, get(iMid));
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
		// Set boundries where get(uiLowIndex) <= n <=  get(uiHighIndex)
		uiLowIndex = uiHighIndex = iMid;
		if (get(uiLowIndex) == obj)
		{
			while ((uiLowIndex > 0) && (get(uiLowIndex - 1) == obj)) {uiLowIndex--;}
			while ((uiHighIndex < (m_iXDimension - 1)) && (obj == get(uiHighIndex + 1))) {uiHighIndex++;} 
		}
		else
		{
			while ((uiLowIndex > 0) && (get(uiLowIndex) > obj)) {uiLowIndex--;}
			while ((uiHighIndex < (m_iXDimension - 1)) && (obj > get(uiHighIndex))) {uiHighIndex++;} 
		}

		return iSearchIndex;
	}

	double regress()
	{
		double dSlope = 0;
		double dIntercept = 0;
		double dXCoefficient = 0;
        double dSumX = 0.0;
        double dSumXsq = 0.0;
        double dSumY = 0.0;
        double dSumYsq = 0.0;
        double dSumXY = 0.0;
        double dMaxX = -99999;
        double dMaxY = -99999;
        double x = 0.0;
        double y = 0.0;
        double xSq = 0.0;
        double ySq = 0.0;

        for(int i = 0; (i < getXDimension()); ++i)
        {
            x = get(i, 0);
            y = get(i, 1);
            xSq = x * x;
            ySq = y * y;
            dSumX += x;
            dSumXsq += xSq;
            dSumY += y;
            dSumYsq += ySq;
            dSumXY += (x * y);
			dMaxX = Max(x, dMaxX);
			dMaxY = Max(y, dMaxY);
        }
        double dSlopeNumerator = 0.0;
        double dSlopeDenomenator = 0.0;

        dSlopeNumerator = (getXDimension() * dSumXY) - (dSumX * dSumY);
        dSlopeDenomenator = (getXDimension() * dSumXsq) - (dSumX * dSumX);

        if ((dSlopeDenomenator <= 0) || (getXDimension() <= 2))
        {
			return numeric_limits<double>::quiet_NaN();
        }

		dSlope = dSlopeNumerator / dSlopeDenomenator;
        dIntercept = (dSumY - (dSlope * dSumX)) / getXDimension();
		double dSxSq = (dSumXsq - (dSumXsq / getXDimension())) / (getXDimension() - 1);
	    double dSySq = (dSumYsq - (dSumYsq / getXDimension())) / (getXDimension() - 1);
	    dXCoefficient = dSlope * (sqrt(dSxSq) / sqrt(dSySq));
        return dSlope;
	}
};

template <class TYPE> double AffxMultiDimensionalArray<TYPE>::infinity;

#endif

