/***************************************************************
 * Name:      fpsconfigMain.h
 * Purpose:   Defines Application Frame
 * Author:    Matlo (mat.lau@laposte.net)
 * Created:   2011-08-06
 * Copyright: Matlo (http://diy-machine.blogspot.com/)
 * License:
 **************************************************************/

#ifndef FPSCONFIGMAIN_H
#define FPSCONFIGMAIN_H

//(*Headers(fpsconfigFrame)
#include <wx/stattext.h>
#include <wx/menu.h>
#include <wx/textctrl.h>
#include <wx/spinctrl.h>
#include <wx/panel.h>
#include <wx/filedlg.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/spinbutt.h>
#include <wx/statusbr.h>
//*)

#include <event_catcher.h>
#include <ConfigurationFile.h>

typedef enum button_index {
    bi_undef = 0,
    bi_select, bi_start, bi_ps,
    bi_up, bi_right, bi_down, bi_left,
    bi_triangle, bi_circle, bi_cross, bi_square,
    bi_l1, bi_r1,
    bi_l2, bi_r2,
    bi_l3, bi_r3,
    BI_MAX
} e_button_index;

typedef enum axis_index {
    ai_undef = 0,
    ai_ls_up, ai_ls_right, ai_ls_down, ai_ls_left,
    AI_MAX
} e_axis_index;

class fpsconfigFrame: public wxFrame
{
    public:

        fpsconfigFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~fpsconfigFrame();

    private:

        //(*Handlers(fpsconfigFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnSpinButton4Change(wxSpinEvent& event);
        void OnSpinButton2Change(wxSpinEvent& event);
        void OnSpinButton1Change(wxSpinEvent& event);
        void OnSpinButton3Change(wxSpinEvent& event);
        void OnSpinButton5Change(wxSpinEvent& event);
        void OnButtonClick(wxCommandEvent& event);
        void OnSpinButton6Change(wxSpinEvent& event);
        void OnMenuSave(wxCommandEvent& event);
        void OnMenuNew(wxCommandEvent& event);
        void OnMenuSaveAs(wxCommandEvent& event);
        void OnMenuOpen(wxCommandEvent& event);
        //*)

        e_button_index getButtonIndex(wxButton* button);
        e_axis_index getAxisIndex(wxButton* button);
        wxButton* getButtonButton(wxString blabel);
        wxButton* getAxisButton(wxString blabel);

        //(*Identifiers(fpsconfigFrame)
        static const long ID_BUTTON10;
        static const long ID_SPINCTRL1;
        static const long ID_SPINCTRL2;
        static const long ID_TEXTCTRL24;
        static const long ID_TEXTCTRL23;
        static const long ID_TEXTCTRL26;
        static const long ID_TEXTCTRL1;
        static const long ID_TEXTCTRL25;
        static const long ID_TEXTCTRL22;
        static const long ID_SPINBUTTON3;
        static const long ID_SPINBUTTON2;
        static const long ID_SPINBUTTON5;
        static const long ID_SPINBUTTON6;
        static const long ID_SPINBUTTON4;
        static const long ID_SPINBUTTON1;
        static const long ID_STATICTEXT1;
        static const long ID_STATICTEXT7;
        static const long ID_STATICTEXT2;
        static const long ID_STATICTEXT3;
        static const long ID_STATICTEXT4;
        static const long ID_STATICTEXT5;
        static const long ID_STATICTEXT6;
        static const long ID_BUTTON1;
        static const long ID_BUTTON2;
        static const long ID_BUTTON3;
        static const long ID_BUTTON4;
        static const long ID_BUTTON5;
        static const long ID_BUTTON6;
        static const long ID_BUTTON7;
        static const long ID_BUTTON8;
        static const long ID_BUTTON9;
        static const long ID_BUTTON11;
        static const long ID_BUTTON12;
        static const long ID_BUTTON13;
        static const long ID_BUTTON14;
        static const long ID_BUTTON15;
        static const long ID_BUTTON16;
        static const long ID_BUTTON17;
        static const long ID_BUTTON18;
        static const long ID_BUTTON19;
        static const long ID_BUTTON20;
        static const long ID_BUTTON21;
        static const long ID_CHOICE2;
        static const long ID_CHOICE1;
        static const long ID_PANEL1;
        static const long ID_MENUITEM1;
        static const long ID_MENUITEM4;
        static const long ID_MENUITEM2;
        static const long ID_MENUITEM3;
        static const long idMenuQuit;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        //*)

        //(*Declarations(fpsconfigFrame)
        wxButton* stickdown;
        wxSpinButton* SpinButton5;
        wxButton* l1;
        wxButton* r3;
        wxSpinCtrl* SpinCtrl1;
        wxMenuItem* MenuItem5;
        wxButton* stickright;
        wxStaticText* StaticText2;
        wxButton* start;
        wxStaticText* StaticText6;
        wxTextCtrl* TextCtrl26;
        wxButton* stickleft;
        wxMenuItem* MenuItem4;
        wxButton* select;
        wxButton* down;
        wxSpinButton* SpinButton6;
        wxButton* stickup;
        wxPanel* Panel1;
        wxStaticText* StaticText1;
        wxFileDialog* FileDialog1;
        wxStaticText* StaticText3;
        wxButton* r2;
        wxButton* l2;
        wxSpinButton* SpinButton1;
        wxMenuItem* MenuItem3;
        wxTextCtrl* TextCtrl24;
        wxStaticText* StaticText5;
        wxStaticText* StaticText7;
        wxSpinButton* SpinButton2;
        wxSpinButton* SpinButton4;
        wxStatusBar* StatusBar1;
        wxMenuItem* MenuItem6;
        wxButton* left;
        wxTextCtrl* TextCtrl22;
        wxSpinCtrl* SpinCtrl2;
        wxButton* triangle;
        wxTextCtrl* TextCtrl1;
        wxTextCtrl* TextCtrl25;
        wxButton* cross;
        wxButton* square;
        wxTextCtrl* TextCtrl23;
        wxButton* ps;
        wxSpinButton* SpinButton3;
        wxButton* l3;
        wxButton* up;
        wxStaticText* StaticText4;
        wxButton* r1;
        wxChoice* Choice1;
        wxChoice* Choice2;
        wxButton* right;
        wxButton* circle;
        //*)

        event_catcher evcatch;
        ConfigurationFile configFile;

        ButtonMapper buttons[BI_MAX];
        AxisMapper axes[AI_MAX];

        DECLARE_EVENT_TABLE()
};

#endif // FPSCONFIGMAIN_H
