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
#include <wx/snglinst.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/utils.h>
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
        void OnSelectRefresh(wxCommandEvent& event);
        void OnButton1Click(wxCommandEvent& event);
        void OnButton3Click(wxCommandEvent& event);
        void OnCheckBox1Click(wxCommandEvent& event);
        void OnSave(wxCommandEvent& event);
        void OnCheckBox2Click(wxCommandEvent& event);
        void OnCheckBox3Click(wxCommandEvent& event);
        void OnChoice8Select(wxCommandEvent& event);
        void OnCheckBoxCalibrate(wxCommandEvent& event);
        //*)

        void readDevices();
        void readFrequency();
        void refresh();

        //(*Identifiers(sixemuguiFrame)
        static const long ID_STATICTEXT3;
        static const long ID_CHOICE3;
        static const long ID_CHECKBOX5;
        static const long ID_CHOICE8;
        static const long ID_BUTTON1;
        static const long ID_STATICTEXT5;
        static const long ID_STATICTEXT9;
        static const long ID_STATICTEXT10;
        static const long ID_STATICTEXT11;
        static const long ID_STATICTEXT12;
        static const long ID_STATICTEXT13;
        static const long ID_STATICTEXT14;
        static const long ID_STATICTEXT1;
        static const long ID_CHOICE1;
        static const long ID_STATICTEXT2;
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
        wxStaticText* StaticText10;
        wxStaticText* StaticText9;
        wxStaticText* StaticText13;
        wxStaticText* StaticText2;
        wxStaticText* StaticText14;
        wxChoice* Choice3;
        wxButton* Button1;
        wxMenuItem* MenuItem4;
        wxStaticText* StaticText11;
        wxCheckBox* CheckBox3;
        wxCheckBox* CheckBox2;
        wxPanel* Panel1;
        wxStaticText* StaticText1;
        wxStaticText* StaticText3;
        wxChoice* Choice8;
        wxCheckBox* CheckBox1;
        wxSingleInstanceChecker SingleInstanceChecker1;
        wxMenuItem* MenuItem3;
        wxButton* Button3;
        wxStaticText* StaticText5;
        wxStatusBar* StatusBar1;
        wxChoice* Choice4;
        wxCheckBox* CheckBox4;
        wxStaticText* StaticText12;
        wxCheckBox* CheckBox5;
        wxChoice* Choice1;
        //*)

        wxString m_SelectedSixaxis;
        wxString m_SelectedPS3;
        wxString m_SelectedBtDongle;
        int m_SelectedController;

        int pid[7];
        bool serialDeviceInUse[256];
        int dongle[7];

        DECLARE_EVENT_TABLE()
};

#endif // SIXEMUGUIMAIN_H