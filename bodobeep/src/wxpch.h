
#ifndef WXPCH_H_INCLUDED
#define WXPCH_H_INCLUDED

#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/scrolwin.h>
#include <wx/aui/auibook.h>
#include <wx/dcbuffer.h>

// Wide strings are stupid and useless -- ditch wxWidget's conversion to wide strings
//   and instead use UTF-8.  Why this isn't the default for EVERYTHING EVER still baffles me
#ifdef wxT
#undef wxT
#endif
inline wxString     wxT(const char* c)          { return wxString::FromUTF8(c);             }
inline wxString     wxT(const std::string& s)   { return wxString::FromUTF8(s.c_str());     }
inline std::string  toStr(const wxString& s)    { return std::string( s.ToUTF8() );         }



#ifdef _MSC_VER
    #pragma comment(lib, "winmm.lib")
    #pragma comment(lib, "comctl32.lib")
    #pragma comment(lib, "rpcrt4.lib")

    #ifdef _DEBUG
        #pragma comment(lib, "wxbase31ud.lib")
//        #pragma comment(lib, "wxbase31ud_net.lib")
//        #pragma comment(lib, "wxbase31ud_xml.lib")


//        #pragma comment(lib, "wxexpatd.lib")
//        #pragma comment(lib, "wxjpegd.lib")

//        #pragma comment(lib, "wxmsw31ud_adv.lib")
        #pragma comment(lib, "wxmsw31ud_aui.lib")
        #pragma comment(lib, "wxmsw31ud_core.lib")
//        #pragma comment(lib, "wxmsw31ud_gl.lib")
//        #pragma comment(lib, "wxmsw31ud_html.lib")
//        #pragma comment(lib, "wxmsw31ud_media.lib")
//        #pragma comment(lib, "wxmsw31ud_propgrid.lib")
//        #pragma comment(lib, "wxmsw31ud_qa.lib")
//        #pragma comment(lib, "wxmsw31ud_ribbon.lib")
//        #pragma comment(lib, "wxmsw31ud_richtext.lib")
//        #pragma comment(lib, "wxmsw31ud_stc.lib")
//        #pragma comment(lib, "wxmsw31ud_webview.lib")
//        #pragma comment(lib, "wxmsw31ud_xrc.lib")

        #pragma comment(lib, "wxpngd.lib")
//        #pragma comment(lib, "wxregexud.lib")
//        #pragma comment(lib, "wxscintillad.lib")
//        #pragma comment(lib, "wxtiffd.lib")
        #pragma comment(lib, "wxzlibd.lib")

    #else               // Release
        #pragma comment(lib, "wxbase31u.lib")
        #pragma comment(lib, "wxmsw31u_aui.lib")
        #pragma comment(lib, "wxmsw31u_core.lib")

        #pragma comment(lib, "wxpng.lib")
        #pragma comment(lib, "wxzlib.lib")
    #endif
#endif

#endif