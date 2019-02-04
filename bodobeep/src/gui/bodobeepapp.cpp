
#include "wxpch.h"
#include <dshfs.h>
#include "bodobeepapp.h"
#include "bodomainframe.h"
#include "songsheet.h"

namespace bodobeep
{

    wxIMPLEMENT_APP(BodoBeepApp);

    bool BodoBeepApp::OnInit()
    {
        if(!wxApp::OnInit())
            return false;

        mainFrame = new BodoMainFrame;
        SetTopWindow(mainFrame);
        mainFrame->Show();

        return true;
    }


    void BodoBeepApp::guiOpenSong()
    {
        wxFileDialog    dlg(mainFrame, wxT("Select song file(s) to open"), wxEmptyString, wxEmptyString,
                            wxT("Bodobeep Song Files (*.bbs.json)|*.bbs.json|All Files|*"), wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);

        if(dlg.ShowModal() != wxID_OK)
            return;

        wxArrayString       files;
        dlg.GetPaths(files);

        for(auto& i : files)
            loadSong( toStr(i) );
    }

    void BodoBeepApp::handleErr()
    {
        try     { throw;        }
        catch(std::exception& e)
        {
            wxMessageBox(wxT(e.what()), wxT("Error occurred"), wxOK | wxICON_ERROR);
        }
        catch(...)
        {
            wxMessageBox(wxT("An unknown error occurred"), wxT("Error occurred"), wxOK | wxICON_ERROR);
        }
    }


    /////////////////////////////////////////

    void BodoBeepApp::loadSong(const wxString& wxpath)
    {
        try
        {
            dshfs::Filename fn(toStr(wxpath));
            if(!fn.fullResolve())
                throw std::runtime_error("Unable to resolve path \"" + toStr(wxpath) + "\"");

            auto path = fn.getFullPath();

            auto i = songs.find(path);
            if(i == songs.end())
            {
                SongData dat;
                dat.songPtr =   std::move(data.loadSong(path));
                dat.page =      new SongSheet( mainFrame->getSongNotebook(), dat.songPtr.get() );

                i = songs.insert( std::make_pair(path, std::move(dat)) ).first;
            }

            // now, i.second points to the loaded song data
        }catch(...){handleErr();}
    }
}