// Pin.h: interface for the CPin class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PIN_H__5B3C6332_3D0B_4DE9_869D_9394BC629BD9__INCLUDED_)
#define AFX_PIN_H__5B3C6332_3D0B_4DE9_869D_9394BC629BD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wx/string.h>
#include "../beidcommon/bytearray.h"

class CPin  
{
public:
	CPin();
	virtual ~CPin();
    CPin(const CPin & oPin);
    CPin& operator =( const CPin & oPin );
    void Copy(const CPin & oPin);

    void SetReference(int iRef) { m_iRef = iRef; }
    int GetReference() const { return m_iRef; }
    void SetTriesLeft(int iLeft) { m_iTriesLeft = iLeft; }
    int GetTriesLeft() const { return m_iTriesLeft; }
    void SetLabel(wxString strLabel) { m_strLabel = strLabel; }
    wxString GetLabel() const { return m_strLabel; }
    void SetFlags(int iFlags) { m_iFlags = iFlags;}
    int GetFlags() const { return m_iFlags;}
    void SetType(int iType) { m_iType = iType; }
    int GetType() const { return m_iType;}
    void SetAuthID(unsigned char *pucID, unsigned long ulLen) { m_AuthID.Append(pucID, ulLen); }
    //CMyByteArray GetAuthID() const { return m_AuthID;}
    void SetApplication(wxString strApp) { m_strApp = strApp; }
    wxString GetApplication() const { return m_strApp;}
    void SetPinPad(bool bPinPad) { m_bPinPad = bPinPad; }
    bool GetPinPad() const { return m_bPinPad; }
    void SetUsageCode(long lUsageCode) { m_lUsageCode = lUsageCode; }
    long GetUsageCode() const { return m_lUsageCode; }
private:
    int m_iRef;
    int m_iTriesLeft;
    wxString m_strLabel;
    int m_iFlags;
    int m_iType;
    eidcommon::CByteArray m_AuthID;
    wxString m_strApp;
    bool m_bPinPad;
    long m_lUsageCode;
};

#endif // !defined(AFX_PIN_H__5B3C6332_3D0B_4DE9_869D_9394BC629BD9__INCLUDED_)
