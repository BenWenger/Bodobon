
#ifndef BODOBEEP_GUI_TRACKERSHEET_H_INCLUDED
#define BODOBEEP_GUI_TRACKERSHEET_H_INCLUDED

#include "types.h"
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
        
        double              zoomWd          = 4.0;          // number of pixels for each update column (x-axis)
        double              zoomHt          = 12.0;         // number of pixels for each tone row (y-axis)

        PitchRange          pitchRange;
        ToneFieldDetails    toneFieldDetails;


        void                paintRect(wxDC& dc, int pixelX, int pixelY, int pixelWd, int pixelHt);

        void                recalcScroll();
    };

}


#endif
