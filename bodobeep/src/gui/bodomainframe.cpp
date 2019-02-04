
#include "wxpch.h"
#include "bodobeepapp.h"
#include "bodomainframe.h"
#include "songnotebook.h"

namespace bodobeep
{

    wxBEGIN_EVENT_TABLE(BodoMainFrame, wxFrame)
        EVT_MENU(wxID_OPEN,     BodoMainFrame::onOpen)
        EVT_MENU(wxID_SAVE,     BodoMainFrame::onSave)
        EVT_MENU(wxID_SAVEAS,   BodoMainFrame::onSaveAs)
        EVT_MENU(wxID_EXIT,     BodoMainFrame::onExit)
    wxEND_EVENT_TABLE()

    BodoMainFrame::BodoMainFrame()
        : wxFrame(nullptr, wxID_ANY, wxT("Bodobeep"), wxDefaultPosition, wxSize(1200,800))
    {
        buildMenu();

        songNotebook = new SongNotebook(this);
    }

    void BodoMainFrame::buildMenu()
    {
        auto* fileMenu = new wxMenu;
        fileMenu->Append(wxID_OPEN);
        fileMenu->Append(wxID_SAVE);
        fileMenu->Append(wxID_SAVEAS);
        fileMenu->Append(wxID_EXIT, wxT("E&xit"));

        mainMenu = new wxMenuBar;
        mainMenu->Append(fileMenu, wxT("&File"));

        SetMenuBar(mainMenu);
    }

    void BodoMainFrame::onExit(wxCommandEvent& evt)     { Close(false);                     }
    void BodoMainFrame::onOpen(wxCommandEvent& evt)     { wxGetApp().guiOpenSong();         }
    void BodoMainFrame::onSave(wxCommandEvent& evt)     { /* TODO   */                      }
    void BodoMainFrame::onSaveAs(wxCommandEvent& evt)   { /* TODO   */                      }
}

