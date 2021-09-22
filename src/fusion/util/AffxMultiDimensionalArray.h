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

#ifndef _AffxMultiDimensionalArray_h_
#define _AffxMultiDimensionalArray_h_
/**
 * @file AffxMultiDimensionalArray.h
 *
 * @brief This header contains the AffxMultiDimensionalArray class definition.
 */

#include "file/TsvFile/TsvFile.h"
#include "util/AffxArray.h"
#include "util/Err.h"
#include "util/Util.h"
//
#include "portability/affy-base-types.h"

#include <limits>
#include <memory.h>
#include <vector>
//

using namespace std;

/**
 * @brief  A class for managing one, two, or three dimensional arrays of primitive data types.
 *
 */
template<class TYPE, typename SIZE = int> class AffxMultiDimensionalArray
{
private:
    TYPE* m_ar;
    SIZE m_iXDimension;
    SIZE m_iYDimension;
    SIZE m_iZDimension;

    // to make nth_element() calls look nicer
    TYPE* begin() { return m_ar; }
    TYPE* end() { return m_ar + m_iXDimension; }

public:
    static double infinity;
    TYPE* getPointer() {mustBeVector(); return m_ar;}
    SIZE getXDimension() {return m_iXDimension;}
    SIZE getYDimension() {return m_iYDimension;}
    SIZE getZDimension() {return m_iZDimension;}
    void convertMatrixToVector() {m_iXDimension = m_iXDimension * m_iYDimension; m_iYDimension = 1;}

    SIZE length() {return m_iXDimension;}
    bool isVector() {return ((m_iXDimension > 0) && (m_iYDimension == 1) && (m_iZDimension == 1));}
    bool isMatrix() {return ((m_iXDimension > 0) && (m_iYDimension > 1) && (m_iZDimension == 1));}

    void increment(SIZE x) {m_ar[x]++;}

    AffxMultiDimensionalArray()
    {
        m_iXDimension = 1;
        m_iYDimension = 1;
        m_iZDimension = 1;
        m_ar = new TYPE[getCount()];
        if (m_ar == NULL) {Err::errAbort("Run is out of memory.");}
        initialize();
    }

    AffxMultiDimensionalArray(SIZE iXDimension)
    {
        m_iXDimension = iXDimension;
        m_iYDimension = 1;
        m_iZDimension = 1;
        m_ar = new TYPE[getCount()];
        if (m_ar == NULL) {Err::errAbort("Run is out of memory.");}
        initialize();
    }

    AffxMultiDimensionalArray(SIZE iXDimension, SIZE iYDimension)
    {
        m_iXDimension = iXDimension;
        m_iYDimension = iYDimension;
        m_iZDimension = 1;
        m_ar = new TYPE[getCount()];
        if (m_ar == NULL) {Err::errAbort("Run is out of memory.");}
        initialize();
    }

    AffxMultiDimensionalArray(SIZE iXDimension, SIZE iYDimension, SIZE iZDimension)
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

    void initialize(SIZE iXDimension)
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
    void initialize(SIZE iXDimension, SIZE iYDimension)
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

    void initialize(SIZE iXDimension, SIZE iYDimension, SIZE iZDimension)
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
        for (SIZE iIndex = 0; (iIndex < length()); iIndex++)
        {
            if (get(iIndex) != infinity)
            {
                b = false;
                break;
            }
        }
        return b;
    }

    SIZE getCount()
    {
        return (m_iXDimension * m_iYDimension * m_iZDimension);
    }

    SIZE compareTo(AffxMultiDimensionalArray& that)
    {
        SIZE iResult = 0;
        SIZE iLength = min(getCount(), that.getCount());

        bool bEqual = true;
        for (SIZE iIndex = 0; (iIndex < iLength); iIndex++)
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
            for (SIZE i = 0; (i < getCount()); i++)
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
    void set_unchecked(SIZE x, TYPE t)
    {
        m_ar[x] = t;
    }
    void set(SIZE x, TYPE t)
    {
        SIZE iIndex = x;
        if ((iIndex < 0) || (iIndex >= getCount())) {Err::errAbort("AffxMultiDimensionalArray::set(x, TYPE) index out of bounds. " + ::getInt(x));}
        m_ar[iIndex] = t;
    }
    void* getPointer(SIZE x)
    {
        SIZE iIndex = x;
        if ((iIndex < 0) || (iIndex >= getCount())) {Err::errAbort("AffxMultiDimensionalArray::getPointer(x) index out of bounds. " + ::getInt(x));}
        return &m_ar[x];
    }
    void set(SIZE x, SIZE y, TYPE t)
    {
        SIZE iIndex = ((x * m_iYDimension) + y);
        if ((iIndex < 0) || (iIndex >= getCount())) {Err::errAbort("AffxMultiDimensionalArray::set(x, y, TYPE) index out of bounds. " + ::getInt(x) + ", " + ::getInt(y));}
        m_ar[iIndex] = t;
    }
    void* getPointer(SIZE x, SIZE y)
    {
        SIZE iIndex = ((x * m_iYDimension) + y);
        if ((iIndex < 0) || (iIndex >= getCount())) {Err::errAbort("AffxMultiDimensionalArray::getPointer(x, y) index out of bounds. " + ::getInt(x) + ", " + ::getInt(y));}
        return &m_ar[iIndex];
    }
    void set(SIZE x, SIZE y, SIZE z, TYPE t)
    {
        SIZE iIndex = ((x * m_iYDimension * m_iZDimension) + (y * m_iZDimension) + z);
        if ((iIndex < 0) || (iIndex >= getCount())) {Err::errAbort("AffxMultiDimensionalArray::set(x, y, z, TYPE) index out of bounds. " + ::getInt(x) + ", " + ::getInt(y) + ", " + ::getInt(z));}
        m_ar[iIndex] = t;
    }
    TYPE get(SIZE x)
    {
        SIZE iIndex = x;
        if ((iIndex < 0) || (iIndex >= getCount())) {Err::errAbort("AffxMultiDimensionalArray::get(x) index out of bounds. " + ::getInt(x));}
        return m_ar[iIndex];
    }
    TYPE get_unchecked(SIZE x)
    {
        return m_ar[x];
    }
    TYPE get(SIZE x, SIZE y)
    {
        SIZE iIndex = ((x * m_iYDimension) + y);
        if ((iIndex < 0) || (iIndex >= getCount())) {Err::errAbort("AffxMultiDimensionalArray::get(x, y) index out of bounds. " + ::getInt(x) + ", " + ::getInt(y));}
        return m_ar[iIndex];
    }
    TYPE get(SIZE x, SIZE y, SIZE z)
    {
        SIZE iIndex = ((x * m_iYDimension * m_iZDimension) + (y * m_iZDimension) + z);
        if ((iIndex < 0) || (iIndex >= getCount())) {Err::errAbort("AffxMultiDimensionalArray::get(x, y, z) index out of bounds. " + ::getInt(x) + ", " + ::getInt(y) + ", " + ::getInt(z));}
        return m_ar[iIndex];
    }

    void mustBeVector()
    {
        if (!isVector())
        {
//            AffxLog::throwable(new Exception("AffxMultidimensionalArray function must operate on a vector."));
        }
    }

    void insertAt(SIZE x, TYPE t)
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

    void removeAt(SIZE x)
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
        for (SIZE iIndex = 1; (iIndex < ar.length()); iIndex++)
        {
            for (SIZE iIndex2 = 0; (iIndex2 < iIndex); iIndex2++)
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
//        if (length() < 2) {AffxLog::throwable(new Exception("AffxMultidimensionalArray function diff(int) must operate on a vector with more than one element in it."));}
        AffxMultiDimensionalArray<TYPE> ar(this->m_iXDimension - 1);
        for (SIZE iIndex = 1; (iIndex < length()); iIndex++)
        {
            ar.set((iIndex - 1), get(iIndex) - get(iIndex - 1));
        }
        return ar;
    }

    AffxMultiDimensionalArray<TYPE> rev()
    {
        mustBeVector();
        AffxMultiDimensionalArray<TYPE> ar(this->length());
        for (SIZE iIndex = (length() - 1); (iIndex >= 0); iIndex--)
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
            for (SIZE iIndex = 0; (iIndex < ar.length()); iIndex++)
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

    SIZE nonZeroLength()
    {
        SIZE iCount = 0;
        for (SIZE iIndex = 0; (iIndex < length()); iIndex++)
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
            for (SIZE iIndex = 0; (iIndex < length()); iIndex++)
            {
                dSum += get(iIndex);
            }
            dMean = dSum / (double)length();
        }
        return dMean;
    }

    double mean(SIZE iLength)
    {
        double dMean = 0.0;
        mustBeVector();
        if (iLength > 0)
        {
            double dSum = 0.0;
            for (SIZE iIndex = 0; (iIndex < iLength); iIndex++)
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
//        if ((bXDimension) && (bYDimension)) {AffxLog::throwable(new Exception("AffxMultidimensionalArray function must operate on a vector."));}
        if (bXDimension)
        {
//            if (vect.length() != m_iXDimension) {AffxLog::throwable(new Exception("AffxMultidimensionalArray function must operate on a vector of the appropriate size."));}
            for (SIZE iXIndex = 0; (iXIndex < m_iXDimension); iXIndex++)
            {
                for (SIZE iYIndex = 0; (iYIndex < m_iYDimension); iYIndex++)
                {
                    vect.set(iXIndex, (TYPE)(vect.get(iXIndex) + log(get(iXIndex, iYIndex))));
                }
            }
            for (SIZE iXIndex = 0; (iXIndex < m_iXDimension); iXIndex++)
            {
                vect.set(iXIndex, (vect.get(iXIndex) / m_iYDimension));
            }
        }
        else if (bYDimension)
        {
//            if (vect.length() != m_iYDimension) {AffxLog::throwable(new Exception("AffxMultidimensionalArray function must operate on a vector of the appropriate size."));}
            for (SIZE iXIndex = 0; (iXIndex < m_iXDimension); iXIndex++)
            {
                for (SIZE iYIndex = 0; (iYIndex < m_iYDimension); iYIndex++)
                {
                    vect.set(iYIndex, (TYPE)(vect.get(iYIndex) + log(get(iXIndex, iYIndex))));
                }
            }
            for (SIZE iYIndex = 0; (iYIndex < m_iYDimension); iYIndex++)
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
//        if ((bXDimension) && (bYDimension)) {AffxLog::throwable(new Exception("AffxMultidimensionalArray function must operate on a vector."));}
        if (bXDimension)
        {
//            if (vectMeans.length() != m_iXDimension) {AffxLog::throwable(new Exception("AffxMultidimensionalArray function must operate on a vector of the appropriate size."));}
//            if (vectVariances.length() != m_iXDimension) {AffxLog::throwable(new Exception("AffxMultidimensionalArray function must operate on a vector of the appropriate size."));}
            // Calculate Means.
            for (SIZE iXIndex = 0; (iXIndex < m_iXDimension); iXIndex++)
            {
                for (SIZE iYIndex = 0; (iYIndex < m_iYDimension); iYIndex++)
                {
                    vectMeans.set(iXIndex, (TYPE)(vectMeans.get(iXIndex) + log(get(iXIndex, iYIndex))));
                }
            }
            for (SIZE iXIndex = 0; (iXIndex < m_iXDimension); iXIndex++)
            {
                vectMeans.set(iXIndex, (vectMeans.get(iXIndex) / m_iYDimension));
            }
            // Calculate variances.
            for (SIZE iXIndex = 0; (iXIndex < m_iXDimension); iXIndex++)
            {
                for (SIZE iYIndex = 0; (iYIndex < m_iYDimension); iYIndex++)
                {
                    vectVariances.set(iXIndex, (TYPE)(vectVariances.get(iXIndex) + ((log(get(iXIndex, iYIndex)) - vectMeans.get(iXIndex)) * (log(get(iXIndex, iYIndex)) - vectMeans.get(iXIndex)))));
                }
            }
            for (SIZE iXIndex = 0; (iXIndex < m_iXDimension); iXIndex++)
            {
                vectVariances.set(iXIndex, (vectVariances.get(iXIndex) / (m_iYDimension - 1)));
            }
        }
        else if (bYDimension)
        {
//            if (vectMeans.length() != m_iYDimension) {AffxLog::throwable(new Exception("AffxMultidimensionalArray function must operate on a vector of the appropriate size."));}
//            if (vectVariances.length() != m_iYDimension) {AffxLog::throwable(new Exception("AffxMultidimensionalArray function must operate on a vector of the appropriate size."));}
            // Calculate Means.
            for (SIZE iXIndex = 0; (iXIndex < m_iXDimension); iXIndex++)
            {
                for (SIZE iYIndex = 0; (iYIndex < m_iYDimension); iYIndex++)
                {
                    vectMeans.set(iYIndex, (TYPE)(vectMeans.get(iYIndex) + log(get(iXIndex, iYIndex))));
                }
            }
            for (SIZE iYIndex = 0; (iYIndex < m_iYDimension); iYIndex++)
            {
                vectMeans.set(iYIndex, (vectMeans.get(iYIndex) / m_iXDimension));
            }
            // Calculate variances.
            for (SIZE iXIndex = 0; (iXIndex < m_iXDimension); iXIndex++)
            {
                for (SIZE iYIndex = 0; (iYIndex < m_iYDimension); iYIndex++)
                {
                    vectVariances.set(iYIndex, (TYPE)(vectVariances.get(iYIndex) + ((log(get(iXIndex, iYIndex)) - vectMeans.get(iYIndex)) * (log(get(iXIndex, iYIndex)) - vectMeans.get(iYIndex)))));
                }
            }
            for (SIZE iYIndex = 0; (iYIndex < m_iYDimension); iYIndex++)
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
            SIZE iCount = 0;
            for (SIZE iIndex = 0; (iIndex < length()); iIndex++)
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
            for (SIZE iIndex = 0; (iIndex < length()); iIndex++)
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
            for (SIZE iIndex = 0; (iIndex < length()); iIndex++)
            {
                dSumOfSquares += ((get(iIndex) - dMean) * (get(iIndex) - dMean));
            }
            dVariance = dSumOfSquares / (length() - 1);
        }
        return dVariance;
    }

    double var(SIZE iLength)
    {
        double dVariance = 0.0;
        double dMean = mean(iLength);
        mustBeVector();
        if (iLength > 1)
        {
            double dSumOfSquares = 0.0;
            for (SIZE iIndex = 0; iIndex < iLength; iIndex++)
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
        SIZE iLength = getXDimension();
        double dSum = 0;
        for (SIZE iElementIndex = 0; (iElementIndex < iLength); iElementIndex++)
        {
            dSum += get(iElementIndex, 0);
        }
        double m1 = (dSum / (double)iLength);
        dSum = 0;
        for (SIZE iElementIndex = 0; (iElementIndex < iLength); iElementIndex++)
        {
            dSum += get(iElementIndex, 1);
        }
        double m2 = (dSum / (double)iLength);
        double dNumerator = 0;
        double d1 = 0;;
        double d2 = 0;
        for (SIZE iElementIndex = 0; (iElementIndex < iLength); iElementIndex++)
        {
            dNumerator += (get(iElementIndex, 0) - m1) * (get(iElementIndex, 1) - m2);
            d1 += (get(iElementIndex, 0) - m1) * (get(iElementIndex, 0) - m1) ;
            d2 += (get(iElementIndex, 1) - m2) * (get(iElementIndex, 1) - m2) ;
        }
//        double dDenominator = sqrt(d1 * d2);
        return dNumerator / sqrt(d1) / sqrt(d2);
    }

    void test()
    {
        mustBeVector();
        for (SIZE iIndex = 0; (iIndex < length()); iIndex++)
        {
            TYPE t = get(iIndex);
            SIZE x = 0;
        }
    }

    void quickSort() {mustBeVector(); quickSort(0, (length() - 1), 0);}
    void quickSort(SIZE iFrom, SIZE iTo, SIZE iCompareCode)
    {
        if ((m_ar != NULL) && (length() >= 2))
        {
            TYPE objCenter = 0;
            SIZE i = iFrom;
            SIZE j = iTo;
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

    void swap(SIZE iIndex1, SIZE iIndex2)
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

    double percentile(double dPercentile, SIZE iLength, bool bSort = true)
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
                for(SIZE i = 0; i < this->length(); i++)
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


    TYPE  median()
    {
        if (length() == 0) {return numeric_limits<double>::quiet_NaN();}
        TYPE d = 0.0;
        mustBeVector();

        std::vector<TYPE> vNonNanVector;
        typename std::vector<TYPE>::iterator iter; 

        for(int iIndex=0; iIndex <length(); iIndex++)
        {
            if(get_unchecked(iIndex) == get_unchecked(iIndex))
            {
                vNonNanVector.push_back(get(iIndex)); 
            }  
        }

        int length = vNonNanVector.size();
        if(length == 0)
        {
            return numeric_limits<TYPE>::quiet_NaN();
        }
        if (length % 2 == 0) {
            iter = vNonNanVector.begin() + ((int)floor((double)length / 2.0) - 1);
            std::nth_element(vNonNanVector.begin(), iter, vNonNanVector.end());
            std::nth_element(iter, iter + 1, vNonNanVector.end());
            d = *iter + (*(iter + 1) - *iter)/2.0; 
        }
        else {
            iter = vNonNanVector.begin() + (int)floor((double)length / 2.0);
            std::nth_element(vNonNanVector.begin(), iter, vNonNanVector.end());
            d = *iter;                   
        }
        return d;
    }

    TYPE  median(SIZE iLength)
    {
        if (iLength == 0) {return numeric_limits<double>::quiet_NaN();}
        TYPE d = 0.0;
        mustBeVector();

        std::vector<TYPE> vNonNanVector;
        typename std::vector<TYPE>::iterator iter;

        for(int iIndex=0; iIndex <iLength; iIndex++)
        {
            if(get(iIndex) == get(iIndex))
            {
                vNonNanVector.push_back(get(iIndex));
            }
        }

        int iLocalLength = vNonNanVector.size();

        if(iLocalLength == 0)
        {
            return numeric_limits<TYPE>::quiet_NaN();
        }
        if ((iLocalLength % 2) == 0) {
            iter = vNonNanVector.begin() + ((int)floor((double)iLocalLength / 2.0) - 1);
            std::nth_element(vNonNanVector.begin(), iter, vNonNanVector.begin() + iLocalLength);
            std::nth_element(iter, iter + 1, vNonNanVector.begin() + iLocalLength);
            d = *iter + (*(iter + 1) - *iter)/2.0; 
        }
        else {
            iter = vNonNanVector.begin() + (int)floor((double)iLocalLength / 2.0);
            std::nth_element(vNonNanVector.begin(), iter, vNonNanVector.begin() + iLocalLength);
            d = *iter;                  
        }
        return d;
    }

    static SIZE readVectorSize(const AffxString& strFileName, bool bHeader)  {

      SIZE iCount = affx::TsvFile::getLineCountInFile(strFileName);
      if ( iCount > 0 && bHeader ) {
        iCount--;
      }
    }

    bool readVector(const AffxString& strFileName, bool bHeader)
    {
        try {
          affx::TsvFile tsv;
          SIZE iCount = 0;
          std::string col;
          tsv.m_optAutoTrim = true;
          tsv.m_optHasColumnHeader = bHeader;
          tsv.m_optAutoColumns = !bHeader;
          if (tsv.open(strFileName) == affx::TSV_OK) {
            while (tsv.nextLevel(0) == affx::TSV_OK) {
              tsv.get(0,0,col);
              set(iCount, AffxByteArray(col).parseDouble());
              iCount++;
            }
            tsv.clear();
          } else {return false;}
          return true;
        } catch(...) {return false;}
    }

    static SIZE readMatrixYSize(const AffxString& strFileName, bool bColHeader)
    {
      SIZE iCount = affx::TsvFile::getLineCountInFile(strFileName);
      if ( (iCount > 0) && bColHeader ) {
        iCount--;
      }
      return iCount;
    }

    static SIZE readMatrixXSize(const AffxString& strFileName, bool bRowHeader)
    {
      affx::TsvFile tsv;
      SIZE iCount = 0;
      if (tsv.openTable(strFileName) == affx::TSV_OK)    {
        tsv.nextLevel(0);
        iCount = tsv.getColumnCount(0);
        if (bRowHeader) {iCount--;}
        tsv.clear();
      } else {return 0;}
      return iCount;
    }

    bool readMatrixXHeader(const AffxString& strFileName, bool bRowHeader, AffxArray<AffxString>& ar)
    {
        ar.deleteAll();
        affx::TsvFile tsv;
        std::string cname;
        SIZE iCount =  bRowHeader ? 1 : 0;
        tsv.m_optAutoTrim = true;
        tsv.m_optAbortOnError = false;
        if (tsv.open(strFileName) == affx::TSV_OK) {
          for ( ; iCount < tsv.getColumnCount(0); iCount++ ) {
            if ( tsv.cidx2cname(0, iCount, cname) == affx::TSV_OK) {
              ar.add(new AffxString(cname));
            }
          }
          tsv.clear();
        } else {return false;}
        return true;
    }

    bool readMatrixYHeader(const AffxString& strFileName, bool bColHeader, AffxArray<AffxString>& ar)
    {
        ar.deleteAll();
        affx::TsvFile tsv;
        SIZE iCount = 0;
        tsv.m_optAutoTrim = true;
        tsv.m_optAbortOnError = false;
        tsv.m_optHasColumnHeader = bColHeader;
        tsv.m_optAutoColumns = !bColHeader;
        
        if (tsv.open(strFileName) == affx::TSV_OK)  {
          std::string col;
          while (tsv.nextLevel(0) == affx::TSV_OK)  {
            tsv.get(0,0,col);
            ar.add(new AffxString(col));
          }
          tsv.clear();
        } else {return false;}
        return true;
    }

    bool readMatrix(const AffxString& strFileName, bool bColHeader, bool bRowHeader, SIZE iColumnCount) {
      try {
        affx::TsvFile tsv;
        tsv.m_optAutoTrim = true;
        tsv.m_optAbortOnError = true;
        tsv.m_optHasColumnHeader = bColHeader;
        tsv.m_optAutoColumns = !bColHeader;
        SIZE iCount = 0;
        if (bRowHeader) {iColumnCount++;}
        
        if (tsv.open(strFileName) ==  affx::TSV_OK) {
          while (tsv.nextLevel(0) == affx::TSV_OK) {
            SIZE iColumnNumber = bRowHeader ? 1 : 0;
            SIZE iColIndex = 0;
            std::string col;
            for (; (iColumnNumber < iColumnCount); iColumnNumber++, iColIndex++) {
              tsv.get(0,iColumnNumber,  col);
              set(iColIndex, iCount, AffxByteArray(col).parseDouble());
            }
            iCount++;
          }
          tsv.clear();
        } else {return false;}
        return true;
        } catch(...) {return false;}
    }

    SIZE binarySearch(TYPE& obj)
    {
        SIZE iSearchIndex = -1;
        SIZE iLow = 0;
        SIZE iHigh = m_iXDimension - 1;
        SIZE iMid = 0;
        SIZE iCompareResult = 0;

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

    SIZE binarySearch(TYPE obj, unsigned int& uiLowIndex, unsigned int& uiHighIndex)
    {
        SIZE iSearchIndex = -1;
        if (obj < get(0)) {uiLowIndex = uiHighIndex = 0; return -1;}
        if (obj > get(m_iXDimension - 1)) {uiLowIndex = uiHighIndex = (m_iXDimension + 1); return -1;}

        SIZE iMid = 0;
        SIZE iLow = 0;
        SIZE iHigh = m_iXDimension - 1;
        SIZE iCompareResult = 0;

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

        for(SIZE i = 0; (i < getXDimension()); ++i)
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

    double normDifference()
    {
      SIZE iLength = getXDimension();
      double dSum = 0;
      for (SIZE iElementIndex = 0; (iElementIndex < iLength); iElementIndex++) {
          double dDiff = get(iElementIndex, 0) - get(iElementIndex, 1);
          dSum += dDiff * dDiff;
      }
      return sqrt(dSum);
    }
};

template <class TYPE,typename SIZE> double AffxMultiDimensionalArray<TYPE,SIZE>::infinity;

#endif

