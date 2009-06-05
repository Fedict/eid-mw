// ReaderState.h: interface for the CReaderState class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_READERSTATE_H__5868239A_9758_4287_A70A_E642CC42573D__INCLUDED_)
#define AFX_READERSTATE_H__5868239A_9758_4287_A70A_E642CC42573D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CReaderState 
{
public:
	CReaderState();
	virtual ~CReaderState();
    unsigned long GetState() { return m_ulState;}
    void SetState(unsigned long ulState) { m_ulState = ulState; }

private:
    unsigned long m_ulState;
};

#endif // !defined(AFX_READERSTATE_H__5868239A_9758_4287_A70A_E642CC42573D__INCLUDED_)
