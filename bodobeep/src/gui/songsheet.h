
#ifndef BODOBEEP_GUI_SONGSHEET_H_INCLUDED
#define BODOBEEP_GUI_SONGSHEET_H_INCLUDED


namespace bodobeep
{
    class SongNotebook;
    class Song;

    class SongSheet : public wxAuiNotebook
    {
    public:
        SongSheet(SongNotebook* parent, Song* theSong);

    private:
        Song*               song;
    };

}


#endif
