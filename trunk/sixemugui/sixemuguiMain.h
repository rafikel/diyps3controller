/***************************************************************
 * Name:      sixemuguiMain.h
 * Purpose:   Defines Application Frame
 * Author:    Matlo (mat.lau@laposte.net)
 * Created:   2011-01-12
 * Copyright: Matlo (http://diy-machine.blogspot.com/)
 * License:
 **************************************************************/

#ifndef SIXEMUGUIMAIN_H
#define SIXEMUGUIMAIN_H

//(*Headers(sixemuguiFrame)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/menu.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/statusbr.h>
//*)

#include <usb.h>
#include <list>

class sixemuguiFrame: public wxFrame
{
    public:

        sixemuguiFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~sixemuguiFrame();

    private:

        //(*Handlers(sixemuguiFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnSelectSixaxisBdaddr(wxCommandEvent& event);
        void OnChoice2Select(wxCommandEvent& event);
        void OnSelectPS3Bdaddr(wxCommandEvent& event);
        void OnSelectBtDongle(wxCommandEvent& event);
        void OnChoice4Select(wxCommandEvent& event);
        void OnSelectRefresh(wxCommandEvent& event);
        void OnButton2Click(wxCommandEvent& event);
        void OnChoice5Select(wxCommandEvent& event);
        void OnChoice6Select(wxCommandEvent& event);
        void OnChoice7Select(wxCommandEvent& event);
        void OnButton1Click(wxCommandEvent& event);
        //*)

        void readSixaxis();
        void readDongles();
        void setDongleAddress();
        void refresh();

        //(*Identifiers(sixemuguiFrame)
        static const long ID_STATICTEXT1;
        static const long ID_CHOICE1;
        static const long ID_STATICTEXT2;
        static const long ID_CHOICE2;
        static const long ID_STATICTEXT3;
        static const long ID_CHOICE3;
        static const long ID_STATICTEXT4;
        static const long ID_CHOICE5;
        static const long ID_STATICTEXT6;
        static const long ID_CHOICE6;
        static const long ID_STATICTEXT7;
        static const long ID_CHOICE7;
        static const long ID_STATICTEXT8;
        static const long ID_BUTTON2;
        static const long ID_STATICTEXT5;
        static const long ID_CHOICE4;
        static const long ID_STATICTEXT10;
        static const long ID_STATICTEXT9;
        static const long ID_TEXTCTRL1;
        static const long ID_BUTTON1;
        static const long ID_PANEL1;
        static const long ID_MENUITEM1;
        static const long ID_MENUITEM5;
        static const long idMenuQuit;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        //*)

        //(*Declarations(sixemuguiFrame)
        wxStaticText* StaticText10;
        wxChoice* Choice5;
        wxStaticText* StaticText9;
        wxMenuItem* MenuItem7;
        wxStaticText* StaticText2;
        wxChoice* Choice3;
        wxButton* Button1;
        wxStaticText* StaticText6;
        wxStaticText* StaticText8;
        wxPanel* Panel1;
        wxStaticText* StaticText1;
        wxStaticText* StaticText3;
        wxChoice* Choice7;
        wxButton* Button2;
        wxMenuItem* MenuItem3;
        wxStaticText* StaticText5;
        wxStaticText* StaticText7;
        wxStatusBar* StatusBar1;
        wxChoice* Choice4;
        wxChoice* Choice6;
        wxTextCtrl* TextCtrl1;
        wxStaticText* StaticText4;
        wxChoice* Choice1;
        wxChoice* Choice2;
        //*)

        wxString m_SelectedSixaxis;
        wxString m_SelectedPS3;
        wxString m_SelectedBtDongle;
        int m_SelectedController;

        std::list<wxString> m_used_Sixaxis;

        DECLARE_EVENT_TABLE()
};

#endif // SIXEMUGUIMAIN_H
