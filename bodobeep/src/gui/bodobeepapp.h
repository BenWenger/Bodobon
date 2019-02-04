
#ifndef BODOBEEPAPP_H_INCLUDED
#define BODOBEEPAPP_H_INCLUDED

#include "data.h"


namespace bodobeep
{
    class BodoMainFrame;
    class SongSheet;

    class BodoBeepApp : public wxApp
    {
    public:
        virtual bool        OnInit() override;


        void                guiOpenSong();

    private:
        struct SongData
        {
            std::unique_ptr<Song>       songPtr;
            SongSheet*                  page = nullptr;
        };
        typedef std::unordered_map<std::string, SongData>       songMap_t;

        Data                data;
        BodoMainFrame*      mainFrame;
        songMap_t           songs;

        void                loadSong(const wxString& path);

        void                handleErr();
    };

    wxDECLARE_APP(BodoBeepApp);
}


#endif
