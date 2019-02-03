
#ifndef WXPCH_H_INCLUDED
#define WXPCH_H_INCLUDED

#include <wx/wx.h>


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
//        #pragma comment(lib, "wxmsw31ud_aui.lib")
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
        #pragma comment(lib, "wxmsw31u_core.lib")

        #pragma comment(lib, "wxpng.lib")
        #pragma comment(lib, "wxzlib.lib")
    #endif
#endif

#endif