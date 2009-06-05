// MyApp.cpp: implementation of the CMyApp class.
//
//////////////////////////////////////////////////////////////////////

#include "MyApp.h"
#include <qapplication.h>
#include <memory>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
std::auto_ptr<QApplication> gMyApp;

CMyApp::CMyApp()
{
    if(!qApp)
    {
        int i = 1;
        static char *s = "eidlibjni";
        std::auto_ptr<QApplication> app(new QApplication(i, &s)); 
        gMyApp = app;
    }    
}

CMyApp::~CMyApp()
{
    if(gMyApp.get())
    {
        gMyApp->wakeUpGuiThread();
    }
}
