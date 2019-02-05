
#include "wxpch.h"
#include "songnotebook.h"
#include "songsheet.h"
#include "data.h"
#include "trackersheet.h"

namespace
{
    const int notebookstyle = 
        wxAUI_NB_TAB_SPLIT |
        wxAUI_NB_SCROLL_BUTTONS |
        wxAUI_NB_BOTTOM;
}

namespace bodobeep
{
    SongSheet::SongSheet(SongNotebook* parent, Song* theSong)
        : wxAuiNotebook(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, notebookstyle)
        , song(theSong)
    {
        parent->AddPage(this, wxT("booger"), true, NO_IMAGE);
        for(auto& i : song->channels)
            AddPage( new TrackerSheet(this, song, &i), wxT(i.name), false, NO_IMAGE );
    }
}