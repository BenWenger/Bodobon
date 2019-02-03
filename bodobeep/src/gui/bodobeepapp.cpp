
#include "wxpch.h"
#include "bodobeepapp.h"
#include "bodomainframe.h"

namespace bodobeep
{

    wxIMPLEMENT_APP(BodoBeepApp);

    bool BodoBeepApp::OnInit()
    {
        if(!wxApp::OnInit())
            return false;

        auto* win = new BodoMainFrame;
        SetTopWindow(win);
        win->Show();

        return true;
    }

}