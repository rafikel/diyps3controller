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
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/statusbr.h>
//*)

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
        void OnButton3Click(wxCommandEvent& event);
        void OnCheckBox1Click(wxCommandEvent& event);
        void OnSave(wxCommandEvent& event);
        void OnCheckBox2Click(wxCommandEvent& event);
        void OnCheckBox3Click(wxCommandEvent& event);
        //*)

        void readSixaxis();
        void readDongles();
        int setDongleAddress();
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
        static const long ID_BUTTON1;
        static const long ID_CHECKBOX1;
        static const long ID_CHECKBOX4;
        static const long ID_CHECKBOX2;
        static const long ID_CHECKBOX3;
        static const long ID_CHOICE4;
        static const long ID_BUTTON3;
        static const long ID_PANEL1;
        static const long ID_MENUITEM1;
        static const long ID_MENUITEM2;
        static const long idMenuQuit;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        //*)

        //(*Declarations(sixemuguiFrame)
        wxChoice* Choice5;
        wxStaticText* StaticText2;
        wxChoice* Choice3;
        wxButton* Button1;
        wxStaticText* StaticText6;
        wxMenuItem* MenuItem4;
        wxStaticText* StaticText8;
        wxCheckBox* CheckBox3;
        wxCheckBox* CheckBox2;
        wxPanel* Panel1;
        wxStaticText* StaticText1;
        wxStaticText* StaticText3;
        wxChoice* Choice7;
        wxButton* Button2;
        wxCheckBox* CheckBox1;
        wxMenuItem* MenuItem3;
        wxButton* Button3;
        wxStaticText* StaticText7;
        wxStatusBar* StatusBar1;
        wxChoice* Choice4;
        wxCheckBox* CheckBox4;
        wxChoice* Choice6;
        wxStaticText* StaticText4;
        wxChoice* Choice1;
        wxChoice* Choice2;
        //*)

        wxString m_SelectedSixaxis;
        wxString m_SelectedPS3;
        wxString m_SelectedBtDongle;
        int m_SelectedController;

        bool launched;

        DECLARE_EVENT_TABLE()
};

#endif // SIXEMUGUIMAIN_H