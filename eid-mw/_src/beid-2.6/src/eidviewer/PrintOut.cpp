// PrintOut.cpp: implementation of the CPrintOut class.
//
//////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#ifdef _WIN32
    #pragma warning(disable : 4786 4514)
#endif

#include "eidviewerApp.h"
#include "PrintOut.h"
#include <wx/image.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPrintOut::CPrintOut(wxString strTitle,  CPrintOutData *pData)  : wxPrintout(strTitle)
{
    m_pData = pData;
}

CPrintOut::~CPrintOut()
{

}

bool CPrintOut::OnPrintPage(int page)
{
    wxDC *dc = GetDC();
    if (dc)
    {
        if (page == 1)
            DrawPage(dc);
        else
            return false;

        dc->SetDeviceOrigin(0, 0);
        dc->SetUserScale(1.0, 1.0);
        return true;
    }
    else
        return false;
}

bool CPrintOut::OnBeginDocument(int startPage, int endPage)
{
    if (!wxPrintout::OnBeginDocument(startPage, endPage))
        return false;
    
    return true;
}

void CPrintOut::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
{
    *minPage = 1;
    *maxPage = 1;
    *selPageFrom = 1;
    *selPageTo = 1;
}

bool CPrintOut::HasPage(int pageNum)
{
    return (pageNum == 1);
}

void CPrintOut::DrawPage(wxDC *dc)
{
    if(m_pData == NULL)
        return;

    // Get the logical pixels per inch of screen and printer
    int ppiScreenX, ppiScreenY;
    GetPPIScreen(&ppiScreenX, &ppiScreenY);
    int ppiPrinterX, ppiPrinterY;
    GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);
    
    // This scales the DC so that the printout roughly represents the
    // the screen scaling. The text point size _should_ be the right size
    // but in fact is too small for some reason. This is a detail that will
    // need to be addressed at some point but can be fudged for the
    // moment.
    float scale = (float)((float)ppiPrinterX/(float)ppiScreenX);
    
    // Now we have to check in case our real page size is reduced
    // (e.g. because we're drawing to a print preview memory DC)
    int pageWidth, pageHeight;
    int w, h;
    dc->GetSize(&w, &h);
    GetPageSizePixels(&pageWidth, &pageHeight);
    
    // If printer pageWidth == current DC width, then this doesn't
    // change. But w might be the preview bitmap width, so scale down.
    float overallScale = scale * (float)(w/(float)pageWidth);
    dc->SetUserScale(overallScale, overallScale);
    
    // Calculate conversion factor for converting millimetres into
    // logical units.
    // There are approx. 25.1 mm to the inch. There are ppi
    // device units to the inch. Therefore 1 mm corresponds to
    // ppi/25.1 device units. We also divide by the
    // screen-to-printer scaling factor, because we need to
    // unscale to pass logical units to DrawLine.

    float mmToLogical = (float)(ppiPrinterX/(scale*25.1));

    bool bClearFont = false;
    wxFont *pFont = wxGetApp().GetDataFont();
    if(pFont == NULL)
    {
        pFont = new wxFont(10, wxDEFAULT, wxNORMAL, wxNORMAL);
        bClearFont = true;
    }
    dc->SetFont(*pFont);
    dc->SetPen(*wxBLACK_PEN);
    dc->SetBackgroundMode(wxTRANSPARENT);

    int pageWidthMM, pageHeightMM;
    
    GetPageSizeMM(&pageWidthMM, &pageHeightMM);
    
    int leftMargin = 10;
    int topMargin = 10;
    int rightMargin = 10;
    
    float leftMarginLogical = (float)(mmToLogical*leftMargin);
    float topMarginLogical = (float)(mmToLogical*topMargin);
    float rightMarginLogical = (float)(mmToLogical*(pageWidthMM - rightMargin));
    
    // Draw Logo
    dc->DrawBitmap(m_pData->m_Logo, (long)leftMarginLogical, (long)topMarginLogical, true); 

    // Draw Picture
    if(m_pData->m_Picture.Ok())
    {
        float xPos = (float)(((((pageWidthMM - leftMargin - rightMargin)/2.0)+leftMargin)*mmToLogical) - (m_pData->m_Picture.GetWidth()/2.0));
        dc->DrawBitmap(m_pData->m_Picture, (long)xPos, (long)topMarginLogical, true); 
    }

    long currentX = 0;
    long currentY = 0;
    long xExtent = 0;
    long yExtent = 0;
    long maxHeight = 0;

    maxHeight = (long)m_pData->m_Logo.GetHeight();
    if (maxHeight < (long)m_pData->m_Picture.GetHeight())
    {
        maxHeight = (long)m_pData->m_Picture.GetHeight();
    }
    
    currentY = maxHeight + (long)topMarginLogical + 10;

    // Draw Horizontal Line
    dc->DrawLine( (long)leftMarginLogical, currentY, (long)rightMarginLogical, currentY);
    currentY += 2;

    // Draw Header Text
    dc->GetTextExtent(m_pData->m_strLangCountry, &xExtent, &yExtent);
    float xPos = (float)(((((pageWidthMM - leftMargin - rightMargin)/2.0)+leftMargin)*mmToLogical) - (xExtent/2.0));
    dc->DrawText(m_pData->m_strLangCountry, (long)xPos, currentY);
    currentY += yExtent;

    dc->GetTextExtent(m_pData->m_strLangCard, &xExtent, &yExtent);
    xPos = (float)(((((pageWidthMM - leftMargin - rightMargin)/2.0)+leftMargin)*mmToLogical) - (xExtent/2.0));
    dc->DrawText(m_pData->m_strLangCard, (long)xPos, currentY);
    currentY += yExtent;

    // Draw Horizontal Line
    dc->DrawLine( (long)leftMarginLogical, currentY, (long)rightMarginLogical, currentY);
    currentY += 2;

    // Data Fields
    currentX = (long)((((pageWidthMM - leftMargin - rightMargin)/2.0)+leftMargin)*mmToLogical);

    dc->DrawText(m_pData->m_strCardNrLabel, (long)leftMarginLogical, currentY);
    dc->DrawText(m_pData->m_strCardNr, currentX, currentY);
    currentY += yExtent;
    dc->DrawText(m_pData->m_strChipNrLabel, (long)leftMarginLogical, currentY);
    dc->DrawText(m_pData->m_strChipNr, currentX, currentY);
    currentY += yExtent;
    dc->DrawText(m_pData->m_strValidFromLabel, (long)leftMarginLogical, currentY);
    dc->DrawText(m_pData->m_strValidFrom, currentX, currentY);
    currentY += yExtent;
    dc->DrawText(m_pData->m_strValidUntilLabel, (long)leftMarginLogical, currentY);
    dc->DrawText(m_pData->m_strValidUntil, currentX, currentY);
    currentY += yExtent;
    dc->DrawText(m_pData->m_strNatNrLabel, (long)leftMarginLogical, currentY);
    dc->DrawText(m_pData->m_strNatNr, currentX, currentY);
    currentY += yExtent;
    dc->DrawText(m_pData->m_strTitleLabel, (long)leftMarginLogical, currentY);
    dc->DrawText(m_pData->m_strTitle, currentX, currentY);
    currentY += yExtent;
    dc->DrawText(m_pData->m_strNameLabel, (long)leftMarginLogical, currentY);
    dc->DrawText(m_pData->m_strName, currentX, currentY);
    currentY += yExtent;
    dc->DrawText(m_pData->m_strGivenNameLabel, (long)leftMarginLogical, currentY);
    dc->DrawText(m_pData->m_strGivenName, currentX, currentY);
    currentY += yExtent;
    dc->DrawText(m_pData->m_strNationalLabel, (long)leftMarginLogical, currentY);
    dc->DrawText(m_pData->m_strNational, currentX, currentY);
    currentY += yExtent;
    dc->DrawText(m_pData->m_strSexLabel, (long)leftMarginLogical, currentY);
    dc->DrawText(m_pData->m_strSex, currentX, currentY);
    currentY += yExtent;
    dc->DrawText(m_pData->m_strBirthDateLabel, (long)leftMarginLogical, currentY);
    dc->DrawText(m_pData->m_strBirthDate, currentX, currentY);
    currentY += yExtent;
    dc->DrawText(m_pData->m_strBirthPlaceLabel, (long)leftMarginLogical, currentY);
    dc->DrawText(m_pData->m_strBirthPlace, currentX, currentY);
    currentY += yExtent;
    dc->DrawText(m_pData->m_strAddressLabel, (long)leftMarginLogical, currentY);
    dc->DrawText(m_pData->m_strStreet, currentX, currentY);
    currentY += yExtent;
    wxString strTemp(m_pData->m_strZip);
    strTemp += wxT(" ");
    strTemp += m_pData->m_strMunicip;
    dc->DrawText(strTemp, currentX, currentY);
    currentY += yExtent;
    
    // Draw Horizontal Line
    dc->DrawLine( (long)leftMarginLogical, currentY, (long)rightMarginLogical, currentY);
    currentY += 2;

    // Draw Issuing Municipality
    dc->DrawText(m_pData->m_strIssMunicLabel, (long)leftMarginLogical, currentY);
    dc->DrawText(m_pData->m_strIssMunic, currentX, currentY);
    currentY += yExtent;

    // Draw Horizontal Line
    dc->DrawLine( (long)leftMarginLogical, currentY, (long)rightMarginLogical, currentY);
    currentY += 2;

    if(bClearFont)
    {
        delete pFont;
        pFont = NULL;
    }
}

