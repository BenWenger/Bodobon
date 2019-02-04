
#include "wxpch.h"
#include "songsheet.h"
#include "trackersheet.h"

namespace bodobeep
{
    wxBEGIN_EVENT_TABLE(TrackerSheet, wxScrolledCanvas)
        EVT_PAINT(TrackerSheet::onPaint)
    wxEND_EVENT_TABLE()

    TrackerSheet::TrackerSheet(SongSheet* parent, Song* theSong, Song::Channel* theChannel)
        : wxScrolledCanvas(parent)
        , song(theSong)
        , channel(theChannel)
    {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
    }


    void TrackerSheet::onPaint(wxPaintEvent& evt)
    {
        wxBufferedPaintDC dc(this);

        wxRegionIterator iter(GetUpdateRegion());
        while(iter)
            paintRect(dc, iter.GetRect());
    }

    void TrackerSheet::paintRect(wxDC& dc, const wxRect& rect)
    {
    }

}
