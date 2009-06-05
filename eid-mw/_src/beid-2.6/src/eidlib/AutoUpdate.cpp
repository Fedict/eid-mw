// AutoUpdate.cpp: implementation of the CAutoUpdate class.
//
//////////////////////////////////////////////////////////////////////

#include "BEIDApp.h"
#include "AutoUpdate.h"
#include "../beidcommon/config.h"
#include "Verify.h"
#include "eidliblang.h"
#include "../beidcommon/datafile.h"
#include "../beidcommon/beidconst.h"

#ifdef _WIN32
    #include <qt_windows.h>
#endif
#include <qmessagebox.h>
#include <qdatetime.h>
#include <qfile.h>
#include <qprocess.h>
#include <qregexp.h>

using namespace eidcommon;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoUpdate::CAutoUpdate()
{
    m_bAlreadyAsked = false;
}

CAutoUpdate::~CAutoUpdate()
{

}

void CAutoUpdate::run()
{
    CConfig oConfig;
    oConfig.Load();
    if(oConfig.AllowAutoUpdate())
    {
        bool bDoCheck = false;
        QDateTime dtnow = QDateTime::currentDateTime();
        QString strDateTime = dtnow.toString(Qt::ISODate);
        std::string strLastCheck = oConfig.GetAutoUpdateLastCheck();
        if(strLastCheck.length() == 0)
        {
            // First time
            bDoCheck = true;
        }
        else
        {
            int iDays = oConfig.GetAutoUpdateScheduleDays();  
            if(iDays > 0)
            {
                QDateTime dtCheck = QDateTime::fromString(strLastCheck.c_str(), Qt::ISODate);
                QDate oDate = dtCheck.date();
                dtCheck.setDate(oDate.addDays(iDays));
                bDoCheck = dtnow > dtCheck;
            }
        }

#ifdef _DEBUG
         //bDoCheck = true;
#endif
        if(bDoCheck)
        {
            CByteArray oArray;
            std::string strUri = oConfig.GetAutoUpdateVersionURL();
            if (CVerify::RetrieveData(strUri, oArray))
            {
                ParseVersionFile(oArray, m_oVersion);
#ifdef _WIN32
                if (stricmp (EIDLIB_CURRENT_VERSION, m_oVersion.GetVersion().ascii()) < 0) 
#else
                if (strcasecmp (EIDLIB_CURRENT_VERSION, m_oVersion.GetVersion().ascii()) < 0) 
#endif
                {
                    RetrieveNewVersion(false);
                }
                oConfig.UpdateLastCheck((char *)strDateTime.ascii()); 
            }
        }
    }
}


void CAutoUpdate::ParseVersionFile (const eidcommon::CByteArray & oData, CVersionInfo & oVersion)
{
	/*
		The format for the version file is as such:

		<---- Start Of File ---->
        [Windows] 
        version=2.11.06 
        url=http://belgium.be/eid/eidlib/download.html#windows 

        [Linux] 
        version=2.11.32 
        url=http://belgium.be/eid/eidlib/linux.html 

        [MacOs] 
        ... 
		<----  End Of File  ---->
	*/    

    QString strFileName;
#if defined(Q_WS_X11)
    strFileName = "/tmp/";
#elif defined(Q_WS_WIN)
    char szTempDir[MAX_PATH] = {0};
    ::GetTempPath(sizeof(szTempDir), szTempDir);
    strFileName = szTempDir;
#endif
    strFileName += "beid_ver000.tmp";

    QFile oFile(strFileName);
    if(oFile.open(IO_WriteOnly | IO_Raw ))
    {
        oFile.writeBlock((const char *)oData.GetData(), oData.GetSize());
        oFile.close();
    }
    else
    {
        return;
    }

    CDataFile oConfFile(strFileName.ascii());
    std::string strKey;
#ifdef _WIN32
    strKey = "Windows";
#else
    strKey = "Linux";
#endif

    std::string str = oConfFile.GetString("version", strKey);
    if(str.length() > 0)
    {
        oVersion.SetVersion(str.c_str()); 
    }
    str = oConfFile.GetString("url", strKey);
    if(str.length() > 0)
    {
        oVersion.SetDownloadUrl(str.c_str()); 
    }
    oConfFile.Clear(); 
    oFile.remove(strFileName);
}

void CAutoUpdate::RetrieveNewVersion (bool bNewCard /* false */)
{
    QMutexLocker locker( &m_cs );
    CConfig oConfig;
    oConfig.Load();

    if(oConfig.AllowAutoUpdate() || m_bAlreadyAsked)
    {
        return;
    }
    QString strMessage;
    int iLng = CBEIDApp::GetLang();
    if(bNewCard)
    {
        // Check if version info file already downloaded
        if(m_oVersion.GetDownloadUrl().length() > 0 )
        {
            // Not yet so download
            CByteArray oArray;
            std::string strUri =oConfig.GetAutoUpdateVersionURL(); 
            if (CVerify::RetrieveData(strUri, oArray))
            {
                ParseVersionFile(oArray, m_oVersion);
            }
            else
            {
                // Download failed, useless to ask for update
                return;
            }
        }
        strMessage = pLangCardVersion[iLng];
    }
    else
    {
        strMessage = pLangNewerVersion[iLng];
    }
    if(QMessageBox::Yes == CBEIDApp::AskDownload (strMessage)) 
    {
        ExecuteLink(m_oVersion.GetDownloadUrl());
    }
    m_bAlreadyAsked = true;
}

void CAutoUpdate::ExecuteLink (const QString & url) 
{
#if defined(Q_OS_WIN32)
  // Windows - let the OS do the work
  QT_WA( {
      ShellExecute(NULL, 0, (TCHAR*)url.ucs2(), 0, 0, SW_SHOWNORMAL );
    } , {
      ShellExecuteA( NULL, 0, url.local8Bit(), 0, 0, SW_SHOWNORMAL );
    } );
#else
  const char *q = getenv("BROWSER");
  QStringList browser;
  if(q) 
  {
    browser = QStringList::split(':', q);
  }
#if defined(Q_OS_MACX)
  browser.append("/usr/bin/open");
#endif
  for(QStringList::const_iterator cit=browser.begin(); cit!=browser.end(); ++cit) 
  {
    QString app = *cit;
    if(app.contains("%s")) 
    {
      app.replace("%s", url);
    } 
    else 
    {
      app += " " + url;
    }
    app.replace("%%", "%");
    QProcess *proc = new QProcess();
    QObject::connect(proc, SIGNAL(processExited()), proc, SLOT(deleteLater()));
    proc->setArguments(QStringList::split(QRegExp(" +"), app));
    proc->start();
  }
#endif
}

