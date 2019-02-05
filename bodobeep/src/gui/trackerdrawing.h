
#ifndef BODOBEEP_GUI_TRACKERDRAWING_H_INCLUDED
#define BODOBEEP_GUI_TRACKERDRAWING_H_INCLUDED


namespace bodobeep
{
    class TrackerDrawing
    {
    public:
        static void             drawNullSpace(wxDC& dc, int x, int y, int x2, int y2);
        static void             drawMainBg(wxDC& dc, int x, int y, int x2, int y2);
        static void             drawTone(wxDC& dc, int x, int y, int x2, int y2);
        
    private:
        TrackerDrawing() = delete;
    };

}


#endif
