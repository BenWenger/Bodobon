
#include "wxpch.h"
#include "trackerdrawing.h"

namespace bodobeep
{
    namespace
    {

        wxPen            nullSpacePen       ( wxColor( 90, 90, 90),     1,  wxPENSTYLE_SOLID );
        wxBrush          nullSpaceBrush     ( wxColor( 90, 90, 90),         wxBRUSHSTYLE_SOLID );
        wxPen            bgPen              ( wxColor(185,185,185),     1,  wxPENSTYLE_SOLID );
        wxBrush          bgBrush            ( wxColor(196,196,196),         wxBRUSHSTYLE_SOLID );
        
        wxPen            tonePenA           ( wxColor(206,191,210),     1,  wxPENSTYLE_SOLID );
        wxPen            tonePenB           ( wxColor(157,143,167),     1,  wxPENSTYLE_SOLID );
        wxBrush          toneBrush          ( wxColor(222,214,224),         wxBRUSHSTYLE_SOLID );
    }
    
    void TrackerDrawing::drawNullSpace(wxDC& dc, int x, int y, int x2, int y2)
    {
        dc.SetPen(nullSpacePen);
        dc.SetBrush(nullSpaceBrush);
        dc.DrawRectangle(x, y, x2-x, y2-y);
    }

    void TrackerDrawing::drawMainBg(wxDC& dc, int x, int y, int x2, int y2)
    {
        dc.SetPen(bgPen);
        dc.SetBrush(bgBrush);
        dc.DrawRectangle(x, y, x2+1-x, y2+1-y);
    }
    
    void TrackerDrawing::drawTone(wxDC& dc, int x, int y, int x2, int y2)
    {
        if(x+1 >= x2)
            return;
        dc.SetPen(tonePenB);
        dc.SetBrush(toneBrush);
        dc.DrawRectangle(x, y, x2-x, y2-y);
        
        dc.SetPen(tonePenA);
        dc.DrawLine(x, y, x2, y);
        dc.DrawLine(x, y, x, y2);
    }
}

