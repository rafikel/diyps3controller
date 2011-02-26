/***************************************************************
 * Name:      sixaxis_emu_guiMain.h
 * Purpose:   Defines Application Frame
 * Author:    Matlo (mat.lau@laposte.net)
 * Created:   2010-11-09
 * Copyright: Matlo (http://diy-machine.blogspot.com/)
 * License:
 **************************************************************/

#ifndef SIXAXIS_EMU_GUIMAIN_H
#define SIXAXIS_EMU_GUIMAIN_H

//(*Headers(sixaxis_emu_guiFrame)
#include <wx/notebook.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/menu.h>
#include <wx/textctrl.h>
#include <wx/statline.h>
#include <wx/panel.h>
#include <wx/grid.h>
#include <wx/filedlg.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/statusbr.h>
//*)

#include <event_catcher.h>
#include <ConfigurationFile.h>

class sixaxis_emu_guiFrame: public wxFrame
{
    public:

        sixaxis_emu_guiFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~sixaxis_emu_guiFrame();

        void save_current();
        void load_current();
        void refresh_gui();

    private:

        //(*Handlers(sixaxis_emu_guiFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnButton1Click(wxCommandEvent& event);
        void OnMenuItemNew(wxCommandEvent& event);
        void OnGrid1CellLeftClick(wxGridEvent& event);
        void OnGrid1CellLeftClick1(wxGridEvent& event);
        void OnButton2Click(wxCommandEvent& event);
        void OnButtonAdd1Click(wxCommandEvent& event);
        void OnGrid1CellLeftClick2(wxGridEvent& event);
        void OnGrid1CellLeftClick3(wxGridEvent& event);
        void OnChoice3Select(wxCommandEvent& event);
        void OnChoice4Select(wxCommandEvent& event);
        void OnTextCtrl1Text(wxCommandEvent& event);
        void OnChoice8Select(wxCommandEvent& event);
        void OnButton3Click(wxCommandEvent& event);
        void OnButton6Click(wxCommandEvent& event);
        void OnButton7Click(wxCommandEvent& event);
        void OnChoice4Select1(wxCommandEvent& event);
        void OnChoice6Select(wxCommandEvent& event);
        void OnGrid1CellLeftClick4(wxGridEvent& event);
        void OnChoice2Select(wxCommandEvent& event);
        void OnChoice2Select1(wxCommandEvent& event);
        void OnChoice3Select1(wxCommandEvent& event);
        void OnChoice6Select1(wxCommandEvent& event);
        void OnButton1Click1(wxCommandEvent& event);
        void OnGrid3CellLeftClick(wxGridEvent& event);
        void OnChoice15Select(wxCommandEvent& event);
        void OnGrid3CellLeftClick1(wxGridEvent& event);
        void OnChoice17Select(wxCommandEvent& event);
        void OnChoice18Select(wxCommandEvent& event);
        void OnButton11Click(wxCommandEvent& event);
        void OnGrid3CellLeftClick2(wxGridEvent& event);
        void OnCheckBox2Click(wxCommandEvent& event);
        void OnCheckBox1Click(wxCommandEvent& event);
        void OnButton13Click(wxCommandEvent& event);
        void OnButton8Click(wxCommandEvent& event);
        void OnButton9Click(wxCommandEvent& event);
        void OnButton10Click(wxCommandEvent& event);
        void OnMenuOpen(wxCommandEvent& event);
        void OnMenuItemController1(wxCommandEvent& event);
        void OnMenuItemController2(wxCommandEvent& event);
        void OnMenuItemController3(wxCommandEvent& event);
        void OnMenuItemController4(wxCommandEvent& event);
        void OnMenuItemController5(wxCommandEvent& event);
        void OnMenuItemController6(wxCommandEvent& event);
        void OnMenuItemController7(wxCommandEvent& event);
        void OnMenuItemConfiguration1(wxCommandEvent& event);
        void OnMenuItemConfiguration2(wxCommandEvent& event);
        void OnMenuItemConfiguration3(wxCommandEvent& event);
        void OnMenuItemConfiguration4(wxCommandEvent& event);
        void OnMenuItemExpert(wxCommandEvent& event);
        void OnMenuSave(wxCommandEvent& event);
        void OnMenuSaveAs(wxCommandEvent& event);
        //*)
        void DeleteSelectedRows(wxGrid* grid);
        void fillDeviceTypeButtonChoice(wxChoice* choice);
        void fillDeviceIdButtonChoice(wxChoice* choice);
        void fillMouseAxisChoice(wxChoice* choice);
        void fillMouseButtonChoice(wxChoice* choice);
        void fillJoystickChoice(wxChoice* choice);
        void fillButtonAxisChoice(wxChoice* choice);
        void fillAxisAxisChoice(wxChoice* choice);
        void fillKeyboardButtonChoice(wxChoice* choice);
        void auto_detect(event_catcher* evcatch, wxStaticText* device_type, wxStaticText* device_name, wxStaticText* device_id, wxString event_type, wxStaticText* event_id);

        //(*Identifiers(sixaxis_emu_guiFrame)
        static const long ID_STATICTEXT1;
        static const long ID_STATICTEXT19;
        static const long ID_STATICTEXT9;
        static const long ID_STATICTEXT3;
        static const long ID_STATICTEXT2;
        static const long ID_STATICTEXT35;
        static const long ID_STATICTEXT27;
        static const long ID_STATICTEXT36;
        static const long ID_STATICTEXT37;
        static const long ID_BUTTON1;
        static const long ID_PANEL1;
        static const long ID_STATICTEXT4;
        static const long ID_STATICTEXT29;
        static const long ID_STATICTEXT10;
        static const long ID_STATICTEXT5;
        static const long ID_STATICTEXT6;
        static const long ID_STATICTEXT8;
        static const long ID_STATICTEXT20;
        static const long ID_STATICTEXT7;
        static const long ID_STATICTEXT38;
        static const long ID_STATICTEXT30;
        static const long ID_STATICTEXT39;
        static const long ID_CHOICE4;
        static const long ID_STATICTEXT40;
        static const long ID_TEXTCTRL3;
        static const long ID_BUTTON8;
        static const long ID_CHOICE5;
        static const long ID_STATICLINE1;
        static const long ID_GRID1;
        static const long ID_BUTTON4;
        static const long ID_BUTTON6;
        static const long ID_PANEL2;
        static const long ID_STATICTEXT11;
        static const long ID_STATICTEXT31;
        static const long ID_STATICTEXT12;
        static const long ID_STATICTEXT13;
        static const long ID_STATICTEXT14;
        static const long ID_STATICTEXT21;
        static const long ID_STATICTEXT16;
        static const long ID_STATICTEXT15;
        static const long ID_STATICTEXT17;
        static const long ID_STATICTEXT18;
        static const long ID_STATICTEXT22;
        static const long ID_STATICTEXT41;
        static const long ID_STATICTEXT32;
        static const long ID_STATICTEXT42;
        static const long ID_CHOICE7;
        static const long ID_STATICTEXT43;
        static const long ID_BUTTON9;
        static const long ID_CHOICE8;
        static const long ID_TEXTCTRL8;
        static const long ID_TEXTCTRL9;
        static const long ID_TEXTCTRL10;
        static const long ID_CHOICE1;
        static const long ID_GRID2;
        static const long ID_BUTTON3;
        static const long ID_BUTTON7;
        static const long ID_PANEL3;
        static const long ID_NOTEBOOK1;
        static const long idMenuNew;
        static const long idMenuOpen;
        static const long idMenuSave;
        static const long idMenuSaveAs;
        static const long idMenuQuit;
        static const long ID_MENUITEM1;
        static const long ID_MENUITEM2;
        static const long ID_MENUITEM3;
        static const long ID_MENUITEM4;
        static const long ID_MENUITEM5;
        static const long ID_MENUITEM6;
        static const long ID_MENUITEM7;
        static const long ID_MENUITEM8;
        static const long ID_MENUITEM9;
        static const long ID_MENUITEM10;
        static const long ID_MENUITEM11;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        //*)

        //(*Declarations(sixaxis_emu_guiFrame)
        wxChoice* Choice5;
        wxStaticText* StaticText10;
        wxStaticText* StaticText22;
        wxStaticText* StaticText9;
        wxMenuItem* MenuItem8;
        wxStaticText* StaticText20;
        wxMenuItem* MenuItem7;
        wxButton* Button4;
        wxStaticText* StaticText29;
        wxNotebook* Notebook1;
        wxStaticText* StaticText37;
        wxMenuItem* MenuItem5;
        wxStaticText* StaticText13;
        wxStaticText* StaticText2;
        wxStaticText* StaticText14;
        wxStaticText* StaticText30;
        wxMenu* Menu3;
        wxButton* Button1;
        wxStaticText* StaticText6;
        wxMenuItem* MenuItem4;
        wxStaticText* StaticText40;
        wxMenuItem* MenuItem14;
        wxMenuItem* MenuItem11;
        wxStaticText* StaticText42;
        wxStaticText* StaticText32;
        wxStaticText* StaticText19;
        wxStaticText* StaticText38;
        wxStaticText* StaticText8;
        wxStaticText* StaticText11;
        wxStaticText* StaticText18;
        wxMenuItem* MenuItem15;
        wxStaticText* StaticText31;
        wxPanel* Panel1;
        wxStaticText* StaticText1;
        wxStaticText* StaticText27;
        wxFileDialog* FileDialog1;
        wxMenuItem* MenuItem17;
        wxStaticText* StaticText3;
        wxMenuItem* MenuItem13;
        wxGrid* Grid1;
        wxMenu* Menu1;
        wxChoice* Choice7;
        wxChoice* Choice8;
        wxMenuItem* MenuItem10;
        wxPanel* Panel3;
        wxStaticText* StaticText21;
        wxStaticText* StaticText39;
        wxButton* Button6;
        wxMenuItem* MenuItem12;
        wxGrid* Grid2;
        wxMenuItem* MenuItem3;
        wxButton* Button3;
        wxStaticText* StaticText5;
        wxButton* Button7;
        wxStaticText* StaticText7;
        wxStatusBar* StatusBar1;
        wxChoice* Choice4;
        wxButton* Button9;
        wxTextCtrl* TextCtrl8;
        wxStaticLine* StaticLine1;
        wxStaticText* StaticText43;
        wxStaticText* StaticText41;
        wxStaticText* StaticText15;
        wxStaticText* StaticText12;
        wxTextCtrl* TextCtrl9;
        wxStaticText* StaticText35;
        wxPanel* Panel2;
        wxMenuItem* MenuItem16;
        wxMenuItem* MenuItem9;
        wxStaticText* StaticText17;
        wxStaticText* StaticText4;
        wxStaticText* StaticText36;
        wxTextCtrl* TextCtrl3;
        wxTextCtrl* TextCtrl10;
        wxChoice* Choice1;
        wxStaticText* StaticText16;
        wxButton* Button8;
        wxMenu* Menu4;
        //*)

        event_catcher evcatch;
        ConfigurationFile configFile;
        unsigned int currentController;
        unsigned int currentConfiguration;

        DECLARE_EVENT_TABLE()
};

#endif // SIXAXIS_EMU_GUIMAIN_H
