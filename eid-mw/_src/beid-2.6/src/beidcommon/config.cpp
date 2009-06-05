// Config.cpp: implementation of the CConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "config.h"
#include "datafile.h"
#include "../beidcommon/funcs.h"

#include <algorithm>
#ifdef WIN32
    #include <qt_windows.h>
#endif

#include <qlibrary.h>
#include <qdir.h>
#include <qstring.h>
#include <qstringlist.h>


/********** Harcoded config path **********/
//#define LINUX_CONF_PATH      "/usr/local/etc/"
#define LINUX_CONF_PATH      "/etc/"


#define CONFFILE_BEID "beidgui.conf"

#define CONFFILE_BEID_DEFAULT "BEID_default"
#define CONFFILE_BEID_DEFAULT_CERTS "certs"
#define CONFFILE_BEID_DEFAULT_CRL "crl"
#define CONFFILE_BEID_DEFAULT_CACHING "caching"
#define CONFFILE_BEID_DEFAULT_PROXYHOST "proxyHost"
#define CONFFILE_BEID_DEFAULT_PROXYPORT "proxyPort"
#define CONFFILE_BEID_DEFAULT_FIREWALL "applicationfilter"
#define CONFFILE_BEID_DEFAULT_INETCONNECTION "internetconnection"

#define CONFFILE_CA_CERTS "CA_certs"
#define CONFFILE_CA_CERTS_HTTPSTORE "httpstore"
#define CONFFILE_CA_CERTS_GOV "government"
#define CONFFILE_CA_CERTS_ROOT "root"
#define CONFFILE_CA_CERTS_ALLOWTESTROOT "allowtestroot"

#define CONFFILE_FIREWALL "Application_Filtering"

#define CONFFILE_AUTOUPDATE "VersionCheck"
#define CONFFILE_AUTOUPDATE_ENABLED "enabled"
#define CONFFILE_AUTOUPDATE_DAYS "delay"
#define CONFFILE_AUTOUPDATE_VERSIONURL "url"
#define CONFFILE_AUTOUPDATE_DOWNLOADURL "downloadURL"
#define CONFFILE_AUTOUPDATE_LASTCHECK "lastCheck"

#define CONFFILE_BEID_SERVICE "BEID_service"
#define CONFFILE_BEID_SERVICE_ADDRESS "serverAddress"
#define CONFFILE_BEID_SERVICE_PORT "serverPort"
#define CONFFILE_BEID_SERVICE_HTTPCRLSTORE "httpcrlstore"
#define CONFFILE_BEID_SERVICE_LOCKTIMEOUT "locktimeout"
#define CONFFILE_BEID_SERVICE_LOCKTIMEOUT_PRE   90
#define CONFFILE_BEID_SERVICE_IPADDRESS "IPAddr_allowed"
#define CONFFILE_BEID_SERVICE_ENABLED "clientConnection"

namespace eidcommon
{

class CFindKey
{
private:
    std::string m_strKey;
public:
    explicit CFindKey(const std::string & strKey) : m_strKey(strKey)
    {
    }

    bool operator() (std::pair<std::string, std::string> oValue)
    {
        return 0 == CompareNoCase(oValue.first, m_strKey);
    }
};

class CFindKeyLike
{
private:
    std::string m_strKey;
public:
    explicit CFindKeyLike(const std::string & strKey) : m_strKey(strKey)
    {
    }

    bool operator() (std::pair<std::string, std::string> oValue)
    {
#ifdef _WIN32
        return (0 == _strnicmp(oValue.first.c_str(), m_strKey.c_str(), m_strKey.length()));
#else
        return (0 == strncasecmp(oValue.first.c_str(), m_strKey.c_str(), m_strKey.length()));
#endif
    }
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CConfig::CConfig()
{

}

CConfig::~CConfig()
{

}

std::string CConfig::GetHomeDirConfigFile() const
{
    QString strConfPath = QDir::homeDirPath();
    strConfPath = QDir::convertSeparators(strConfPath);
    if(strConfPath.length() > 0 && strConfPath[(int)strConfPath.length()-1] != QDir::separator())
    {
        strConfPath += QDir::separator ();
    }
#ifndef WIN32
    strConfPath += ".";
#endif
    strConfPath += CONFFILE_BEID;
    strConfPath = QDir::convertSeparators(strConfPath);

    return strConfPath;
}

std::string CConfig::GetGlobalDir() const
{
    QString strDir;

#if defined(WIN32) // Windows
    char szWinDir[MAX_PATH + 1] = {0};
    char szSysDir[MAX_PATH + 1] = {0}; 
    ::GetWindowsDirectoryA(szWinDir, MAX_PATH + 1);
    ::GetSystemDirectoryA(szSysDir, MAX_PATH + 1);
    QString strSysDir(szSysDir);
    strSysDir = QDir::convertSeparators(strSysDir);
    if(strSysDir.right(1) != QString("\\"))
    {
        strSysDir += "\\";
    }
    QLibrary oLib(strSysDir + "Kernel32");
    if(oLib.load())
    {
        typedef UINT (WINAPI *TGetSystemWindowsDirectoryA)(LPSTR,UINT);
        TGetSystemWindowsDirectoryA pFunc  = (TGetSystemWindowsDirectoryA) oLib.resolve("GetSystemWindowsDirectoryA" );
        if(pFunc != NULL)
        {
            memset(szWinDir, 0, sizeof(szWinDir));
            pFunc(szWinDir, MAX_PATH + 1);
        }
    }
    strDir = szWinDir;
    strDir += QDir::separator ();
    strDir = QDir::convertSeparators(strDir);
#else
    strDir = LINUX_CONF_PATH;
#endif

    return strDir;
}

void CConfig::LoadSection(const std::string & strFile, const std::string & strSection)
{
    CDataFile oFile;
    if(oFile.Load(strFile))
    {
       t_Section* pSection = oFile.GetSection(strSection);
       if(pSection == NULL)
           return;

        ConfigVec oTempVec;
        ConfigVec oTempVecNew;

        if(m_oMapConfig.find(strSection) != m_oMapConfig.end())
        {
            oTempVec = m_oMapConfig[strSection];
        }
        int iSize = pSection->Keys.size();
        for(int i = 0; i < iSize; ++i)
        {
            std::string strKeyTemp = pSection->Keys[i].szKey;
            Trim(strKeyTemp);
            ConfigVecIT itVec = std::find_if(oTempVec.begin(), oTempVec.end(), CFindKey(strKeyTemp));
            std::string strValTemp = pSection->Keys[i].szValue;
            Trim(strValTemp);
            if(itVec != oTempVec.end())
            {
                (*itVec).second = strValTemp;
            }
            else
            {
                oTempVecNew.push_back(std::pair<std::string, std::string>(strKeyTemp, strValTemp));
            }
        }
        if(oTempVecNew.size() > 0)
        {
            oTempVec.insert(oTempVec.end(), oTempVecNew.begin(), oTempVecNew.end());
        }
        m_oMapConfig[strSection] = oTempVec;
    }
}

void CConfig::Load()
{
    m_oMapConfig.clear();
    std::string strConfPath = GetGlobalDir() + CONFFILE_BEID;
	LoadSection(strConfPath, CONFFILE_BEID_DEFAULT);
	LoadSection(strConfPath, CONFFILE_CA_CERTS);
	LoadSection(strConfPath, CONFFILE_FIREWALL);
	LoadSection(strConfPath, CONFFILE_AUTOUPDATE);
	LoadSection(strConfPath, CONFFILE_BEID_SERVICE);
}

void CConfig::LoadFirewall()
{
    LoadSection(GetHomeDirConfigFile(), CONFFILE_FIREWALL);
}

void CConfig::Unload()
{
    m_oMapConfig.clear();
}

std::string CConfig::FindValue(const std::string & strSection, const std::string & strKey) const
{
    std::string strRet;
    ConfigMapITConst itmap = m_oMapConfig.find(strSection);
    if(itmap != m_oMapConfig.end())
    {
        ConfigVecITConst itvec = std::find_if((*itmap).second.begin(), (*itmap).second.end(), CFindKey(strKey));
        if(itvec != (*itmap).second.end())
        {
            return (*itvec).second;
        }
    }
    return strRet;
}

std::string CConfig::FindValueLike(const std::string & strSection, const std::string & strKey) const
{
    std::string strRet;
    ConfigMapITConst itmap = m_oMapConfig.find(strSection);
    if(itmap != m_oMapConfig.end())
    {
        ConfigVecITConst itvec = std::find_if((*itmap).second.begin(), (*itmap).second.end(), CFindKeyLike(strKey));
        if(itvec != (*itmap).second.end())
        {
            return (*itvec).second;
        }
    }
    return strRet;
}

std::string CConfig::GetCertStorePath() const
{
    std::string strTemp = FindValue(CONFFILE_BEID_DEFAULT, CONFFILE_BEID_DEFAULT_CERTS);
    strTemp = QDir::convertSeparators(strTemp.c_str()).ascii();
    if(strTemp.length() > 0 && strTemp[(int)strTemp.length()-1] != QDir::separator())
    {
        strTemp += QDir::separator ();
    }
    return strTemp;
}

std::string CConfig::GetCRLStorePath() const
{
    std::string strTemp = FindValue(CONFFILE_BEID_DEFAULT, CONFFILE_BEID_DEFAULT_CRL);
    strTemp = QDir::convertSeparators(strTemp.c_str()).ascii();
    if(strTemp.length() > 0 && strTemp[(int)strTemp.length()-1] != QDir::separator())
    {
        strTemp += QDir::separator ();
    }
    return strTemp;
}

std::string CConfig::GetHttpStore() const
{
    std::string strTemp = FindValue(CONFFILE_CA_CERTS, CONFFILE_CA_CERTS_HTTPSTORE);
    if(strTemp.length() > 0 && strTemp[(int)strTemp.length()-1] != '/')
    {
        strTemp += '/';
    }
    return strTemp;
}

QStringList CConfig::GetGovCerts() const
{
    QStringList strListCerts;
    std::string strCerts = FindValue(CONFFILE_CA_CERTS, CONFFILE_CA_CERTS_GOV);
    if(strCerts.length() > 0)
    {
        return QStringList::split(QString(","), strCerts.c_str());
    }
    return strListCerts;
}

QStringList CConfig::GetRootCerts() const
{
    QStringList strListCerts;
    std::string strCerts = FindValue(CONFFILE_CA_CERTS, CONFFILE_CA_CERTS_ROOT);
    if(strCerts.length() > 0)
    {
        return QStringList::split(QString(","), strCerts.c_str());
    }
    return NULL;
}

bool CConfig::AllowTestRoot() const
{
    bool bRet = false;
    std::string strMode = FindValue(CONFFILE_CA_CERTS, CONFFILE_CA_CERTS_ALLOWTESTROOT);
    if(strMode.length() > 0 && 0 == strcmp(strMode.c_str(), "1"))
    {
        bRet = true;
    }
    return bRet;
}

bool CConfig::AllowCaching() const
{
    bool bRet = false;
    std::string strCache = FindValue(CONFFILE_BEID_DEFAULT, CONFFILE_BEID_DEFAULT_CACHING);
    if(strCache.length() > 0 && 0 == strcmp(strCache.c_str(), "1"))
    {
        bRet = true;
    }
    return bRet;
}

bool CConfig::GetProgramAccess(const std::string & strName, const std::string & strType)
{
    bool bRet = false;
    LoadFirewall();
    std::string strValue = FindValueLike(CONFFILE_FIREWALL, strName);
    if(strValue.length() > 0)
    {
        QStringList strList = QStringList::split(QString(","), strValue.c_str());
        for ( QStringList::Iterator it = strList.begin(); it != strList.end(); ++it ) 
        {
            const char *pszDummy = (*it).ascii();
            if(0 == strcmp(pszDummy, "*") || 0 == strcmp(pszDummy, strType.c_str()))
            {
                bRet = true;
                break;
            }
        }
    }

/* No PCSC privacy service on Mac -> always allow access to the card */
#ifdef NO_FIREWALL
    return true;
#else
    return bRet;
#endif
}

void CConfig::UpdateProgramAccess(const std::string & strName, const std::string & strType)
{
    std::string strConfPath = GetHomeDirConfigFile();
    CDataFile oFile;
    oFile.SetFileName(strConfPath); 
    if(oFile.Load(strConfPath))
    {
        std::string str = oFile.GetString(strName, CONFFILE_FIREWALL);
        if(str.length() > 0)
        {
            str += (",");
            str += strType;
            oFile.SetValue(strName, str, "", CONFFILE_FIREWALL);
        }
        else
        {
            oFile.SetValue(strName, strType, "", CONFFILE_FIREWALL);
        }
    }
    else
    {
        oFile.SetValue(strName, strType, "", CONFFILE_FIREWALL);
    }
    oFile.Save(); 
}


std::string CConfig::GetProxyHost() const
{
    return FindValue(CONFFILE_BEID_DEFAULT, CONFFILE_BEID_DEFAULT_PROXYHOST);
}

std::string CConfig::GetProxyPort() const
{
    return FindValue(CONFFILE_BEID_DEFAULT, CONFFILE_BEID_DEFAULT_PROXYPORT);
}

bool CConfig::AllowAutoUpdate() const
{
    bool bRet = false;
    std::string strUpd = FindValue(CONFFILE_AUTOUPDATE, CONFFILE_AUTOUPDATE_ENABLED);
    if(strUpd.length() > 0 && 0 == strcmp(strUpd.c_str(), "1"))
    {
        bRet = true;
    }
    return bRet;
}

int CConfig::GetAutoUpdateScheduleDays() const
{
    int iRet = 0;
    std::string strDays = FindValue(CONFFILE_AUTOUPDATE, CONFFILE_AUTOUPDATE_DAYS);
    if(strDays.length() > 0)
    {
        if(!from_string<int>(iRet, strDays, std::dec))
        {
            iRet = 0;
        }
    }
    return iRet;
}

std::string CConfig::GetAutoUpdateVersionURL() const
{
    return FindValue(CONFFILE_AUTOUPDATE, CONFFILE_AUTOUPDATE_VERSIONURL);
}

std::string CConfig::GetAutoUpdateDownloadURL() const
{
    return FindValue(CONFFILE_AUTOUPDATE, CONFFILE_AUTOUPDATE_DOWNLOADURL);
}

std::string CConfig::GetAutoUpdateLastCheck()
{
    LoadAutoUpdate();
    return FindValue(CONFFILE_AUTOUPDATE, CONFFILE_AUTOUPDATE_LASTCHECK);
}

void CConfig::LoadAutoUpdate()
{
    LoadSection(GetHomeDirConfigFile(), CONFFILE_AUTOUPDATE);
}

void CConfig::UpdateLastCheck(const std::string & strCheck)
{
    std::string strConfPath = GetHomeDirConfigFile();
    CDataFile oFile;
    oFile.SetFileName(strConfPath); 
    oFile.Load(strConfPath);
    oFile.SetValue(CONFFILE_AUTOUPDATE_LASTCHECK, strCheck, "", CONFFILE_AUTOUPDATE);
    oFile.Save(); 
}

bool CConfig::AllowFirewall() const
{
    bool bRet = true;
    std::string strFireWall = FindValue(CONFFILE_BEID_DEFAULT, CONFFILE_BEID_DEFAULT_FIREWALL);
    if(strFireWall.length() > 0 && 0 == strcmp(strFireWall.c_str(), "0"))
    {
        bRet = false;
    }
    return bRet;
}

bool CConfig::GetInternetConnection() const
{
    bool bRet = true;
    std::string strInet = FindValue(CONFFILE_BEID_DEFAULT, CONFFILE_BEID_DEFAULT_INETCONNECTION);
    if(strInet.length() > 0 && 0 == strcmp(strInet.c_str(), "0"))
    {
        bRet = false;
    }
    return bRet;
}

std::string CConfig::GetServerAddress() const
{
    return FindValue(CONFFILE_BEID_SERVICE, CONFFILE_BEID_SERVICE_ADDRESS);
}

unsigned long CConfig::GetServerPort() const
{
    unsigned long lPort = 0;
    std::string strPort = FindValue(CONFFILE_BEID_SERVICE, CONFFILE_BEID_SERVICE_PORT);
    if(strPort.length() > 0)
    {
        if(!from_string<unsigned long>(lPort, strPort, std::dec))
        {
            lPort = 0;
        }
    }
    return lPort;
}

std::string CConfig::GetHttpCRLStorePath() const
{
    std::string strTemp = FindValue(CONFFILE_BEID_SERVICE, CONFFILE_BEID_SERVICE_HTTPCRLSTORE);
    if(strTemp.length() > 0 && strTemp[(int)strTemp.length()-1] != '/')
    {
        strTemp += '/';
    }
    return strTemp;
}

long CConfig::GetLockTimeout() const
{
    long lRet = CONFFILE_BEID_SERVICE_LOCKTIMEOUT_PRE;
    std::string strTimeOut = FindValue(CONFFILE_BEID_SERVICE, CONFFILE_BEID_SERVICE_LOCKTIMEOUT);
    if(strTimeOut.length() > 0)
    {
        if(!from_string<long>(lRet, strTimeOut, std::dec) || (lRet < CONFFILE_BEID_SERVICE_LOCKTIMEOUT_PRE))
        {
            lRet = CONFFILE_BEID_SERVICE_LOCKTIMEOUT_PRE;
        }
    }
    return lRet;
}

QStringList CConfig::GetAllowedIPAddress() const
{
    QStringList strList;
    std::string strIP = FindValue(CONFFILE_BEID_SERVICE, CONFFILE_BEID_SERVICE_IPADDRESS);
    if(strIP.length() > 0)
    {
        return QStringList::split(QString(","), strIP.c_str());
    }
    return strList;
}

bool CConfig::GetServiceEnabled() const
{
    bool bRet = true;
    std::string strEnabled = FindValue(CONFFILE_BEID_SERVICE, CONFFILE_BEID_SERVICE_ENABLED);
    if(strEnabled.length() > 0 && 0 == strcmp(strEnabled.c_str(), "0"))
    {
        bRet = false;
    }
    return bRet;
}


}  // namespace eidcommon

