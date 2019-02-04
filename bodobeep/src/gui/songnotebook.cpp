
#include "wxpch.h"
#include "songnotebook.h"

namespace
{
    const int notebookstyle = 
        wxAUI_NB_TAB_SPLIT |
        wxAUI_NB_TAB_MOVE |
        wxAUI_NB_SCROLL_BUTTONS |
        wxAUI_NB_WINDOWLIST_BUTTON |
        wxAUI_NB_CLOSE_ON_ALL_TABS |
        wxAUI_NB_TOP;
}

namespace bodobeep
{
    SongNotebook::SongNotebook(wxWindow* parent)
        : wxAuiNotebook(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, notebookstyle)
    {
    }
}