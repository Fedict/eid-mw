// ScConfig.h: interface for the CScConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCCONFIG_H__CEEC6F54_ABB8_45EB_AB77_E3DB1B143F3F__INCLUDED_)
#define AFX_SCCONFIG_H__CEEC6F54_ABB8_45EB_AB77_E3DB1B143F3F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Types of configuration file
#define BELGIAN_EID_CARD 0x00000020

// Error codes
#define ERR_BAD_CONFIG_ID           0x80000001
#define ERR_READING_CONFIGFILE    0x80000002
#define ERR_MEMORY_ALLOC           0x80000003

// Config file content flags
#define CARD_ID_FLAG       0x00000001
#define SLOT_INFO_FLAG   0x00000002


class CScConfig  
{
public:
	CScConfig();
	virtual ~CScConfig();

public:
    static CScConfig *GetConfigObject (const wxString & strFileName);
    static CScConfig *CreateConfigObject (unsigned long dwID);
    virtual bool Read(const wxString & strFileName) = 0;
    virtual bool Write(const wxString & strFileName) = 0;
};

#endif // !defined(AFX_SCCONFIG_H__CEEC6F54_ABB8_45EB_AB77_E3DB1B143F3F__INCLUDED_)
