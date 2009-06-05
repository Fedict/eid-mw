#ifndef _CBYTEARRAY_H_
#define _CBYTEARRAY_H_

namespace eidcommon
{

class CByteArray  
{
public:
// Construction
	CByteArray();
	CByteArray(unsigned char* pbData, unsigned long uiSize);
    CByteArray(const CByteArray & oByteArray);

// Destruction
	virtual ~CByteArray();

// Attributes
	int GetSize() const;
	int GetUpperBound() const;
	void SetSize(unsigned long uiNewSize, unsigned long uiGrowBy = 1);

// Operations	
	// Cleanup
	void RemoveAll();
	void FreeExtra();

	// Accessing elements
	unsigned char GetAt(unsigned long uiIndex) const;
	void SetAt(unsigned char byElement, unsigned long uiIndex);

	// Direct access to the elemtn data
	const unsigned char *GetData() const;
	unsigned char *GetData();

	// Potentially growing the array
	int Append(unsigned char newElement);
	int Append(const CByteArray& src);
	int Append(unsigned char* pbyArray, unsigned long uiSize);

	// Overloaded operator helpers
	unsigned char operator[](unsigned long uiIndex) const;
	const CByteArray& operator+=(CByteArray& byArray);
	const CByteArray& operator+=(unsigned char& byElement);

	// Operations that move elements around
	void InsertAt(unsigned long uiIndex, unsigned char byElement, unsigned long uiCount = 1);
	void InsertAt(unsigned long uiStartIndex, CByteArray& newArray);
	void RemoveAt(unsigned long uiIndex, unsigned long uiCount = 1);
	
protected:
	// Memory Management
	void AllocArray(unsigned long uiSize);
	void ReAllocArray(unsigned long uiSize);
	void ConcatCopy(unsigned char* pData, unsigned long uiSize);

	// Member Variables
	unsigned long	m_uiSize;
	unsigned long	m_uiGrowBy;
	unsigned long	m_uiMaxSize;
	unsigned char *m_pData;
};

} // namespace eidcommon

#endif
