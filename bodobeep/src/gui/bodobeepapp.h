
#ifndef BODOBEEPAPP_H_INCLUDED
#define BODOBEEPAPP_H_INCLUDED


namespace bodobeep
{
    class BodoBeepApp : public wxApp
    {
    public:
        virtual bool        OnInit() override;
    };

    wxDECLARE_APP(BodoBeepApp);
}


#endif
