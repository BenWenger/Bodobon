
#ifndef BODOBEEP_GUI_BODOMAINFRAME_H_INCLUDED
#define BODOBEEP_GUI_BODOMAINFRAME_H_INCLUDED


namespace bodobeep
{
    class SongNotebook;

    class BodoMainFrame : public wxFrame
    {
    public:
        BodoMainFrame();

        SongNotebook*           getSongNotebook()       { return songNotebook;      }

    private:
        wxMenuBar*              mainMenu;
        SongNotebook*           songNotebook;

        void                    buildMenu();

        wxDECLARE_EVENT_TABLE();
        
        void                    onExit(wxCommandEvent& evt);
        void                    onOpen(wxCommandEvent& evt);
        void                    onSave(wxCommandEvent& evt);
        void                    onSaveAs(wxCommandEvent& evt);
    };
}


#endif
