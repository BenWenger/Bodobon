
#ifndef BODOBEEP_GUI_TRACKERSHEET_H_INCLUDED
#define BODOBEEP_GUI_TRACKERSHEET_H_INCLUDED

#include "data.h"

namespace bodobeep
{
    class SongSheet;

    class TrackerSheet : public wxScrolledCanvas
    {
    public:
        TrackerSheet(SongSheet* parent, Song* theSong, Song::Channel* theChannel);

    private:
        wxDECLARE_EVENT_TABLE();

        void                onPaint(wxPaintEvent& evt);


        Song*               song            = nullptr;
        Song::Channel*      channel         = nullptr;
        
        static const int    naturalZoomX    = 8;
        int                 zoomX           = naturalZoomX;
        static const int    naturalZoomY    = 8;
        int                 zoomY           = naturalZoomY;

        int                 maxYSlots       = 64;


        void                paintRect(wxDC& dc, const wxRect& rect);
    };

}


#endif
