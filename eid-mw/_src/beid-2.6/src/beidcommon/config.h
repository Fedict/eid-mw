// Config.h: interface for the CConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIG_H__F1D55E02_1F15_4A9F_A402_2BB87DD17C26__INCLUDED_)
#define AFX_CONFIG_H__F1D55E02_1F15_4A9F_A402_2BB87DD17C26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32
    #pragma warning(disable:4786 4503)
#endif

#include <string>
#include <vector>
#include <map>

class QStringList;

namespace eidcommon
{

class CConfig  
{
public:
	CConfig();
	virtual ~CConfig();
    
    void Load();
    void Unload();
    std::string GetCertStorePath() const;
    std::string GetCRLStorePath() const;
    std::string GetHttpStore() const;
    QStringList GetGovCerts() const;
    QStringList GetRootCerts() const;
    bool AllowTestRoot() const;
    bool AllowCaching() const;
    bool GetProgramAccess(const std::string & strName, const std::string & strType);
    void UpdateProgramAccess(const std::string & strName, const std::string & strType);
    std::string GetProxyHost() const;
    std::string GetProxyPort() const;
    bool AllowAutoUpdate() const;
    int GetAutoUpdateScheduleDays() const;
    std::string GetAutoUpdateVersionURL() const;
    std::string GetAutoUpdateDownloadURL() const;
    std::string GetAutoUpdateLastCheck();
    void UpdateLastCheck(const std::string & strCheck);
    void LoadAutoUpdate();
    bool AllowFirewall() const;
    bool GetInternetConnection() const;
    std::string GetServerAddress() const;
    unsigned long GetServerPort() const;
    std::string GetHttpCRLStorePath() const;
    long GetLockTimeout() const;
    QStringList GetAllowedIPAddress() const;
    bool GetServiceEnabled() const;

protected:
    std::string FindValue(const std::string & strSection, const std::string & strKey) const;
    std::string FindValueLike(const std::string & strSection, const std::string & strKey) const;
    void LoadFirewall();

private:
    std::string GetGlobalDir() const;
    std::string GetHomeDirConfigFile() const;
    void LoadSection(const std::string & strFile, const std::string & strSection);

// Members
private:
    typedef std::vector<std::pair<std::string, std::string> > ConfigVec;
    typedef ConfigVec::iterator ConfigVecIT;
    typedef ConfigVec::const_iterator ConfigVecITConst;
    typedef std::map<std::string, ConfigVec> MapConfig;
    typedef MapConfig::iterator ConfigMapIT;
    typedef MapConfig::const_iterator ConfigMapITConst;
    MapConfig m_oMapConfig;
};


}  // namespace eidcommon

#endif // !defined(AFX_CONFIG_H__F1D55E02_1F15_4A9F_A402_2BB87DD17C26__INCLUDED_)
