
#include "wxpch.h"
#include "songsheet.h"
#include "trackersheet.h"
#include "trackerdrawing.h"

namespace bodobeep
{
    wxBEGIN_EVENT_TABLE(TrackerSheet, wxScrolledCanvas)
        EVT_PAINT(TrackerSheet::onPaint)
    wxEND_EVENT_TABLE()

    TrackerSheet::TrackerSheet(SongSheet* parent, Song* theSong, Song::Channel* theChannel)
        : wxScrolledCanvas(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL | wxHSCROLL | wxNO_FULL_REPAINT_ON_RESIZE)
        , song(theSong)
        , channel(theChannel)
    {
        SetBackgroundStyle(wxBG_STYLE_PAINT);

        recalcScroll();
    }

    void TrackerSheet::recalcScroll()
    {
        int maxWd = static_cast<int>(channel->score.getTotalLength() * zoomWd + 200);
        int maxHt = static_cast<int>(maxRows * zoomHt + 100);
        SetVirtualSize(maxWd,maxHt);
        SetScrollRate(20, 20);
    }

    void TrackerSheet::onPaint(wxPaintEvent& evt)
    {
//        wxAutoBufferedPaintDC dc(this);
        wxPaintDC dc(this);
        DoPrepareDC(dc);

        wxRegionIterator iter(GetUpdateRegion());
        while(iter)
        {
            int x = iter.GetX();
            int y = iter.GetY();
            CalcUnscrolledPosition(x, y, &x, &y);
            paintRect(dc, x, y, iter.GetWidth(), iter.GetHeight());
            ++iter;
        }
    }
    
    void TrackerSheet::paintRect(wxDC& dc, int pixelX, int pixelY, int pixelWd, int pixelHt)
    {
        int posX1 = static_cast<int>(pixelX / zoomWd);
        int posX2 = static_cast<int>((pixelX + pixelWd + zoomWd) / zoomWd);

        int posY1 = static_cast<int>(pixelY / zoomHt);
        int posY2 = static_cast<int>((pixelY + pixelHt + zoomHt) / zoomHt);

        // Draw bottom null space
        if(posY2 >= maxRows)
            TrackerDrawing::drawNullSpace(dc, pixelX-1, static_cast<int>(maxRows * zoomHt), pixelWd + pixelX, pixelHt + pixelY);

        // Draw right null space
        if(posX2 >= channel->score.getTotalLength())
            TrackerDrawing::drawNullSpace(dc, static_cast<int>(channel->score.getTotalLength() * zoomWd), pixelY - 1, pixelWd + pixelX, pixelHt + pixelY);

        auto tone = channel->score.getAtTime(posX1);
        while(tone != channel->score.end() && tone->first < posX2)
        {
            // TODO poll the driver to figure out where to draw the tone
            //   This is specific to Final Fantasy
            int tonepos = static_cast<int>(tone->second.userData.get<json::object>().find("pitch")->second.get<double>());
            
            int x  = static_cast<int>(tone->first * zoomWd);
            int x2 = static_cast<int>((tone->first + tone->second.length) * zoomWd);
            int y  = 0;
            int y2 = static_cast<int>(maxRows * zoomHt);
            TrackerDrawing::drawMainBg(dc, x, y, x2, y2);

            if(tonepos >= 0)
            {
                y  = static_cast<int>( (maxRows - 1 - tonepos)     * zoomHt );
                y2 = static_cast<int>( (maxRows - 1 - tonepos + 1) * zoomHt );
                TrackerDrawing::drawTone(dc, x, y, x2, y2);
            }
            ++tone;
        }
        /*
        TrackerDrawing::drawNullSpace(dc, wxRect(0,0,5000,3000) );

        TrackerDrawing::drawMainBg(dc, wxRect(0,0,500,800));
        TrackerDrawing::drawMainBg(dc, wxRect(500,0,500,800));

        TrackerDrawing::drawTone(dc, wxRect(50, 50, 100, 20) );
        */
    }
}
