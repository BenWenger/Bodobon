
#include "wxpch.h"
#include "songsheet.h"
#include "trackersheet.h"
#include "trackerdrawing.h"
#include "driver.h"

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

        pitchRange = song->host->driver->getPitchRange(theChannel->name, song);
        toneFieldDetails = song->host->driver->getToneEditDetails(theChannel->name, song);

        recalcScroll();
    }

    void TrackerSheet::recalcScroll()
    {
        int maxWd = static_cast<int>(channel->score.getTotalLength() * zoomWd + 200);
        int maxHt = static_cast<int>(pitchRange.range * zoomHt + 100);
        SetVirtualSize(maxWd,maxHt);
        SetScrollRate(20, 20);
    }

    void TrackerSheet::onPaint(wxPaintEvent& evt)
    {
        wxAutoBufferedPaintDC dc(this);
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
        if(posY2 >= pitchRange.range)
            TrackerDrawing::drawNullSpace(dc, pixelX-1, static_cast<int>(pitchRange.range * zoomHt), pixelWd + pixelX, pixelHt + pixelY);

        // Draw right null space
        if(posX2 >= channel->score.getTotalLength())
            TrackerDrawing::drawNullSpace(dc, static_cast<int>(channel->score.getTotalLength() * zoomWd), pixelY - 1, pixelWd + pixelX, pixelHt + pixelY);

        auto tone = channel->score.getAtTime(posX1);
        while(tone != channel->score.end() && tone->first < posX2)
        {
            int pitchPos = song->host->driver->getPitchPos(tone->second, channel->name, song);
            if(pitchPos >= pitchRange.range)
                pitchPos = PitchPos::Unknown;
            
            int x  = static_cast<int>(tone->first * zoomWd);
            int x2 = static_cast<int>((tone->first + tone->second.length) * zoomWd);
            int y  = 0;
            int y2 = static_cast<int>(pitchRange.range * zoomHt);
            TrackerDrawing::drawMainBg(dc, x, y, x2, y2);

            if(pitchPos >= 0)
            {
                y  = static_cast<int>( (pitchRange.range - 1 - pitchPos)     * zoomHt );
                y2 = static_cast<int>( (pitchRange.range - 1 - pitchPos + 1) * zoomHt );
                TrackerDrawing::drawTone(dc, x, y, x2, y2);
            }
            ++tone;
        }
    }
}
