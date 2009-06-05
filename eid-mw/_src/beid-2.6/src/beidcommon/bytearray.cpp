#include <stdlib.h>
#include <string.h>

#include "bytearray.h"

namespace eidcommon
{

//////////////////////////////////////////////////////////////////////
//
// Constructors
//
//////////////////////////////////////////////////////////////////////

CByteArray::CByteArray() 
: m_uiSize(0), m_uiGrowBy(1), m_uiMaxSize(0), m_pData(NULL)
{
	SetSize(0);
}

CByteArray::CByteArray(unsigned char* pbyData, unsigned long uiSize) 
: m_uiSize(0), m_uiGrowBy(1), m_uiMaxSize(0), m_pData(NULL)
{
	SetSize(0);
	Append(pbyData, uiSize);
}

CByteArray::CByteArray(const CByteArray & oByteArray)
: m_uiSize(0), m_uiGrowBy(1), m_uiMaxSize(0), m_pData(NULL)
{
	ConcatCopy((unsigned char *)oByteArray.GetData(), oByteArray.GetSize());
}

//////////////////////////////////////////////////////////////////////
//
// Destructor
//
//////////////////////////////////////////////////////////////////////

CByteArray::~CByteArray()
{
	free(m_pData);
}


//////////////////////////////////////////////////////////////////////
//
// Attributes
//
//////////////////////////////////////////////////////////////////////

int CByteArray::GetSize() const
{
	return m_uiSize;
}

int CByteArray::GetUpperBound() const
{
	return m_uiSize - 1;
}

void CByteArray::SetSize(unsigned long uiNewSize, unsigned long uiGrowBy)
{
    if(uiNewSize > 0)
    {
	    // Check for existing data	
	    if (m_uiSize)
	    {
		    AllocArray(uiNewSize);
		    m_uiGrowBy = uiGrowBy;
	    }
	    else
	    {
		    ReAllocArray(uiNewSize);
		    m_uiGrowBy = uiGrowBy;
	    }
    }
}


//////////////////////////////////////////////////////////////////////
//
// Memory Management
//
//////////////////////////////////////////////////////////////////////

void CByteArray::AllocArray(unsigned long uiSize)
{
    if(uiSize > 0)
    {
	    if (m_pData != NULL)
		    free(m_pData);
	    m_pData = (unsigned char*) malloc(uiSize * sizeof(unsigned char));
	    m_uiSize = uiSize;
    }
}

void CByteArray::ReAllocArray(unsigned long uiSize)
{
    if(uiSize > 0)
    {
	    m_pData = (unsigned char*) realloc(m_pData, uiSize * sizeof(unsigned char));
	    m_uiSize = uiSize;
    }
}

void CByteArray::ConcatCopy(unsigned char* pData, unsigned long uiSize)
{
    if(pData != NULL && uiSize > 0)
    {
	    // Calculate the new data length and realloc memory
	    unsigned long uiOldLen = m_uiSize;
	    unsigned long uiNewLen = uiOldLen + uiSize;
	    ReAllocArray(uiNewLen);

	    // Copy the new data into the buffer
	    memcpy(m_pData+uiOldLen, pData, uiSize * sizeof(unsigned char));
    }
}

//////////////////////////////////////////////////////////////////////
//
// Operator that move elements around
//
//////////////////////////////////////////////////////////////////////

void CByteArray::InsertAt(unsigned long uiIndex, unsigned char byElement, unsigned long uiCount)
{
    if(uiIndex >= 0 && uiIndex < m_uiSize)
    {
	    // Make a copy of the current data pass the index
	    unsigned long uiTailSize = m_uiSize - uiIndex;
	    unsigned char* pbyTemp = (unsigned char*) malloc(uiTailSize * sizeof(unsigned char));
	    memcpy(pbyTemp, m_pData+uiIndex, uiTailSize * sizeof(unsigned char));

	    // Copy the new data starting at the index
	    unsigned long uiNewIndex = uiIndex;
	    for (unsigned long i=0; i < uiCount; i++)
	    {
		    if (uiNewIndex < m_uiSize)
		    {
			    m_pData[uiNewIndex] = byElement;
		    }
		    else
		    {
			    ReAllocArray(m_uiSize + m_uiGrowBy);
			    m_pData[uiNewIndex] = byElement;
		    }
		    uiNewIndex++;
	    }

	    // Concatenate the old data at the end of the new data
	    memcpy(m_pData+uiNewIndex, pbyTemp, uiTailSize * sizeof(unsigned char));
	    free(pbyTemp);
    }
}

void CByteArray::InsertAt(unsigned long uiStartIndex, CByteArray& newArray)
{
	if(uiStartIndex >= 0 && uiStartIndex < m_uiSize)
    {
	    // Make a copy of the current data pass the index
	    unsigned long uiTailSize = m_uiSize - uiStartIndex;
	    unsigned char* pbyTemp = (unsigned char*) malloc(uiTailSize * sizeof(unsigned char));
	    memcpy(pbyTemp, m_pData+uiStartIndex, uiTailSize * sizeof(unsigned char));

	    // ReAlloc the array to accomodate the new data
	    unsigned long uiNewSize = m_uiSize + newArray.GetSize();
	    ReAllocArray(uiNewSize);

	    // Copy the new data starting at the index
	    memcpy(m_pData+uiStartIndex, newArray.GetData(), newArray.GetSize() * sizeof(unsigned char));

	    // Copy the old data at the end of the new data
	    memcpy(m_pData+uiStartIndex+newArray.GetSize(), pbyTemp, uiTailSize * sizeof(unsigned char));
	    free(pbyTemp);
    }
}

void CByteArray::RemoveAt(unsigned long uiIndex, unsigned long uiCount)
{
	if(uiIndex >= 0 && uiIndex < m_uiSize && (uiIndex + uiCount) <= m_uiSize)
    {
	    // Calculate the positions
	    int nEndIndex = uiIndex + uiCount;
	    int nTailSize = m_uiSize - (uiIndex + uiCount);
	    int nNewSize  = m_uiSize - uiCount;

	    // Move the tail data to the index position
	    memcpy(m_pData+uiIndex, m_pData+nEndIndex, nTailSize * sizeof(unsigned char));

	    // Reallocate the array to the smaller size
	    ReAllocArray(nNewSize);
    }
}


//////////////////////////////////////////////////////////////////////
//
// Cleanup
//
//////////////////////////////////////////////////////////////////////

void CByteArray::RemoveAll()
{
	SetSize(0);
}

void CByteArray::FreeExtra()
{
	m_uiMaxSize = m_uiSize;
	SetSize(m_uiSize);
}


//////////////////////////////////////////////////////////////////////
//
// Potentially growing the array
//
//////////////////////////////////////////////////////////////////////

int CByteArray::Append(unsigned char byElement)
{
	ConcatCopy(&byElement, 1);
	return m_uiSize;
}

int CByteArray::Append(const CByteArray& src)
{
	int nFirst = m_uiSize;
	ConcatCopy((unsigned char*)src.GetData(), src.GetSize());
	return nFirst;
}

int CByteArray::Append(unsigned char* pbyArray, unsigned long uiSize)
{
	int nFirst = m_uiSize;
	ConcatCopy(pbyArray, uiSize);
	return nFirst;
}

//////////////////////////////////////////////////////////////////////
//
// Accessing elements
//
//////////////////////////////////////////////////////////////////////

unsigned char CByteArray::GetAt(unsigned long uiIndex) const
{
	if(uiIndex >= 0 && uiIndex < m_uiSize)
    {
	    return m_pData[uiIndex];
    }
    return 0x00;
}

void CByteArray::SetAt(unsigned char byElement, unsigned long uiIndex)
{
	if(uiIndex >= 0 && uiIndex < m_uiSize)
    {
	    m_pData[uiIndex] = byElement;
    }
}


//////////////////////////////////////////////////////////////////////
//
// Direct data access
//
//////////////////////////////////////////////////////////////////////

const unsigned char* CByteArray::GetData() const
{
	return (const unsigned char*)m_pData;
}

unsigned char* CByteArray::GetData()
{
	return (unsigned char*)m_pData;
}


//////////////////////////////////////////////////////////////////////
//
// Operator overload helpers
//
//////////////////////////////////////////////////////////////////////

unsigned char CByteArray::operator[](unsigned long uiIndex) const
{
	return GetAt(uiIndex);
}

const CByteArray& CByteArray::operator+=(CByteArray& bySrc)
{
	ConcatCopy(bySrc.GetData(), bySrc.GetSize());
	return *this;
}


const CByteArray& CByteArray::operator+=(unsigned char& byElement)
{
	Append(byElement);
	return *this;
}

} // namespace eidcommon

