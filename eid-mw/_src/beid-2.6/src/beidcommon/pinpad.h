// pinpad.h: interface for the CPinPad class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PINPAD_H__80730376_7F98_4ED7_9B76_0B5333561098__INCLUDED_)
#define AFX_PINPAD_H__80730376_7F98_4ED7_9B76_0B5333561098__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winscard.h>

class QLibrary;

typedef struct
{
    char szPinPadDll[256];
    QLibrary *pDll;
} PinPadObject;


class CPinPad  
{
public:
	CPinPad();
	virtual ~CPinPad();
    long HandlePinPad(const void *lpInBuffer, unsigned long nInBufferSize, void *lpOutBuffer, unsigned long *lpBytesReturned, SCARDHANDLE hReal = 0);

protected:
    PinPadObject *m_pPinPadDlls;
};

#endif // !defined(AFX_PINPAD_H__80730376_7F98_4ED7_9B76_0B5333561098__INCLUDED_)
