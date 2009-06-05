// CardChangeMonitor.h: interface for the CCardChangeMonitor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CARDCHANGEMONITOR_H__1758B6CA_DA7C_45D5_B481_D01C94CD4E47__INCLUDED_)
#define AFX_CARDCHANGEMONITOR_H__1758B6CA_DA7C_45D5_B481_D01C94CD4E47__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPCSCManager;
class CReaderState;

class CCardChangeMonitor : public QThread
{
public:
	CCardChangeMonitor();
	virtual ~CCardChangeMonitor();
	
    virtual void run ();
    void SetPCSCManager(CPCSCManager *pManager);
    void Suspend();
    void Resume();
    void StopRunning();

protected:
    bool MatchAtr(BYTE *atr, DWORD atr_len, const char *pszReaderName);
    bool SelectApplication(const char *pszReaderName);
    void SetUnawareState();
private:
    CPCSCManager *m_pPCSCManager;
    typedef std::map<std::string, CReaderState *> MapReaderState;
    typedef MapReaderState::iterator ItMapReaderState;
    MapReaderState m_readerStates; 
    bool m_bCanSuspend;
    bool m_bSuspend;
    QWaitCondition m_oWait;
};

#endif // !defined(AFX_CARDCHANGEMONITOR_H__1758B6CA_DA7C_45D5_B481_D01C94CD4E47__INCLUDED_)
