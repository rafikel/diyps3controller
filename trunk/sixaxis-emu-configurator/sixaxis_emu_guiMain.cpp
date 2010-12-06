/***************************************************************
 * Name:      sixaxis_emu_guiMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Matlo (mat.lau@laposte.net)
 * Created:   2010-11-09
 * Copyright: Matlo (http://diy-machine.blogspot.com/)
 * License:
 **************************************************************/

#include "wx_pch.h"
#include "sixaxis_emu_guiMain.h"
#include <wx/msgdlg.h>

//(*InternalHeaders(sixaxis_emu_guiFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

#define MAX_DEVICE_PER_TYPE 7
#define MAX_JOYSTICK_BUTTON 32


//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

//(*IdInit(sixaxis_emu_guiFrame)
const long sixaxis_emu_guiFrame::ID_STATICTEXT1 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT19 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT9 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT3 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT2 = wxNewId();
const long sixaxis_emu_guiFrame::ID_CHOICE2 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT27 = wxNewId();
const long sixaxis_emu_guiFrame::ID_CHOICE9 = wxNewId();
const long sixaxis_emu_guiFrame::ID_CHOICE10 = wxNewId();
const long sixaxis_emu_guiFrame::ID_BUTTON1 = wxNewId();
const long sixaxis_emu_guiFrame::ID_PANEL1 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT4 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT29 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT10 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT5 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT6 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT8 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT7 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT20 = wxNewId();
const long sixaxis_emu_guiFrame::ID_CHOICE3 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT30 = wxNewId();
const long sixaxis_emu_guiFrame::ID_CHOICE12 = wxNewId();
const long sixaxis_emu_guiFrame::ID_CHOICE4 = wxNewId();
const long sixaxis_emu_guiFrame::ID_CHOICE11 = wxNewId();
const long sixaxis_emu_guiFrame::ID_TEXTCTRL3 = wxNewId();
const long sixaxis_emu_guiFrame::ID_CHOICE5 = wxNewId();
const long sixaxis_emu_guiFrame::ID_BUTTON8 = wxNewId();
const long sixaxis_emu_guiFrame::ID_GRID1 = wxNewId();
const long sixaxis_emu_guiFrame::ID_BUTTON4 = wxNewId();
const long sixaxis_emu_guiFrame::ID_BUTTON6 = wxNewId();
const long sixaxis_emu_guiFrame::ID_PANEL2 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT11 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT31 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT12 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT13 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT14 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT16 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT15 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT17 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT18 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT21 = wxNewId();
const long sixaxis_emu_guiFrame::ID_CHOICE6 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT32 = wxNewId();
const long sixaxis_emu_guiFrame::ID_CHOICE13 = wxNewId();
const long sixaxis_emu_guiFrame::ID_CHOICE7 = wxNewId();
const long sixaxis_emu_guiFrame::ID_CHOICE14 = wxNewId();
const long sixaxis_emu_guiFrame::ID_CHOICE8 = wxNewId();
const long sixaxis_emu_guiFrame::ID_TEXTCTRL8 = wxNewId();
const long sixaxis_emu_guiFrame::ID_TEXTCTRL9 = wxNewId();
const long sixaxis_emu_guiFrame::ID_TEXTCTRL10 = wxNewId();
const long sixaxis_emu_guiFrame::ID_BUTTON9 = wxNewId();
const long sixaxis_emu_guiFrame::ID_GRID2 = wxNewId();
const long sixaxis_emu_guiFrame::ID_BUTTON3 = wxNewId();
const long sixaxis_emu_guiFrame::ID_BUTTON7 = wxNewId();
const long sixaxis_emu_guiFrame::ID_PANEL3 = wxNewId();
const long sixaxis_emu_guiFrame::ID_CHECKBOX1 = wxNewId();
const long sixaxis_emu_guiFrame::ID_TEXTCTRL1 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT26 = wxNewId();
const long sixaxis_emu_guiFrame::ID_CHECKBOX2 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT22 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT33 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT23 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT24 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT25 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT28 = wxNewId();
const long sixaxis_emu_guiFrame::ID_CHOICE15 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT34 = wxNewId();
const long sixaxis_emu_guiFrame::ID_CHOICE16 = wxNewId();
const long sixaxis_emu_guiFrame::ID_CHOICE17 = wxNewId();
const long sixaxis_emu_guiFrame::ID_CHOICE18 = wxNewId();
const long sixaxis_emu_guiFrame::ID_BUTTON10 = wxNewId();
const long sixaxis_emu_guiFrame::ID_GRID3 = wxNewId();
const long sixaxis_emu_guiFrame::ID_BUTTON11 = wxNewId();
const long sixaxis_emu_guiFrame::ID_BUTTON13 = wxNewId();
const long sixaxis_emu_guiFrame::ID_PANEL4 = wxNewId();
const long sixaxis_emu_guiFrame::ID_NOTEBOOK1 = wxNewId();
const long sixaxis_emu_guiFrame::idMenuNew = wxNewId();
const long sixaxis_emu_guiFrame::idMenuOpen = wxNewId();
const long sixaxis_emu_guiFrame::idMenuSave = wxNewId();
const long sixaxis_emu_guiFrame::idMenuSaveAs = wxNewId();
const long sixaxis_emu_guiFrame::idMenuQuit = wxNewId();
const long sixaxis_emu_guiFrame::ID_MENUITEM1 = wxNewId();
const long sixaxis_emu_guiFrame::ID_MENUITEM2 = wxNewId();
const long sixaxis_emu_guiFrame::ID_MENUITEM3 = wxNewId();
const long sixaxis_emu_guiFrame::ID_MENUITEM4 = wxNewId();
const long sixaxis_emu_guiFrame::ID_MENUITEM5 = wxNewId();
const long sixaxis_emu_guiFrame::ID_MENUITEM6 = wxNewId();
const long sixaxis_emu_guiFrame::ID_MENUITEM7 = wxNewId();
const long sixaxis_emu_guiFrame::ID_MENUITEM8 = wxNewId();
const long sixaxis_emu_guiFrame::ID_MENUITEM9 = wxNewId();
const long sixaxis_emu_guiFrame::ID_MENUITEM10 = wxNewId();
const long sixaxis_emu_guiFrame::ID_MENUITEM11 = wxNewId();
const long sixaxis_emu_guiFrame::ID_MENUITEM12 = wxNewId();
const long sixaxis_emu_guiFrame::idMenuAbout = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATUSBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(sixaxis_emu_guiFrame,wxFrame)
    //(*EventTable(sixaxis_emu_guiFrame)
    //*)
END_EVENT_TABLE()


void sixaxis_emu_guiFrame::fillDeviceTypeButtonChoice(wxChoice* choice)
{
    choice->Clear();
    if(!expertMode) return;
    choice->SetSelection( choice->Append(_("keyboard")) );
    choice->Append(_("mouse"));
    choice->Append(_("joystick"));
}

void sixaxis_emu_guiFrame::fillDeviceIdButtonChoice(wxChoice* choice)
{
    choice->Clear();
    if(!expertMode) return;
    choice->SetSelection( choice->Append(_("1")) );
    for(int i = 2; i <= MAX_DEVICE_PER_TYPE; ++i)
    {
        choice->Append(wxString::Format(wxT("%i"),i));
    }
}

void sixaxis_emu_guiFrame::fillMouseAxisChoice(wxChoice* choice)
{
    choice->Clear();
    if(!expertMode) return;
    choice->SetSelection( choice->Append(_("x")) );
    choice->Append(_("y"));
}

void sixaxis_emu_guiFrame::fillMouseButtonChoice(wxChoice* choice)
{
    choice->Clear();
    if(!expertMode) return;
    choice->SetSelection( choice->Append(_("BUTTON_LEFT")) );
	choice->Append(_("BUTTON_RIGHT"));
	choice->Append(_("BUTTON_MIDDLE"));
	choice->Append(_("BUTTON_WHEELUP"));
	choice->Append(_("BUTTON_WHEELDOWN"));
	choice->Append(_("BUTTON_X1"));
	choice->Append(_("BUTTON_X2"));
	choice->Append(_("BUTTON_X3"));
}

void sixaxis_emu_guiFrame::fillJoystickChoice(wxChoice* choice)
{
    choice->Clear();
    if(!expertMode) return;
    choice->SetSelection( choice->Append(_("1")) );
	for(int i = 2; i<=MAX_JOYSTICK_BUTTON; ++i)
	{
	    choice->Append(wxString::Format(wxT("%i"),i));
	}
}

void sixaxis_emu_guiFrame::fillButtonAxisChoice(wxChoice* choice)
{
    choice->Clear();
    choice->SetSelection( choice->Append(_("rstick left")) );
    choice->Append(_("rstick right"));
    choice->Append(_("rstick up"));
    choice->Append(_("rstick down"));
    choice->Append(_("lstick left"));
    choice->Append(_("lstick right"));
    choice->Append(_("lstick up"));
    choice->Append(_("lstick down"));
    choice->Append(_("up"));
    choice->Append(_("down"));
    choice->Append(_("right"));
    choice->Append(_("left"));
    choice->Append(_("r1"));
    choice->Append(_("r2"));
    choice->Append(_("l1"));
    choice->Append(_("l2"));
    choice->Append(_("circle"));
    choice->Append(_("square"));
    choice->Append(_("cross"));
    choice->Append(_("triangle"));
}

void sixaxis_emu_guiFrame::fillAxisAxisChoice(wxChoice* choice)
{
    choice->Clear();
    choice->SetSelection( choice->Append(_("rstick x")) );
    choice->Append(_("rstick y"));
    choice->Append(_("lstick x"));
    choice->Append(_("lstick y"));
    choice->Append(_("up"));
    choice->Append(_("down"));
    choice->Append(_("right"));
    choice->Append(_("left"));
    choice->Append(_("r1"));
    choice->Append(_("r2"));
    choice->Append(_("l1"));
    choice->Append(_("l2"));
    choice->Append(_("circle"));
    choice->Append(_("square"));
    choice->Append(_("cross"));
    choice->Append(_("triangle"));
}

void sixaxis_emu_guiFrame::fillKeyboardButtonChoice(wxChoice* choice)
{
    choice->Clear();
    if(!expertMode) return;
    choice->Append(_("BACKSPACE"));
    choice->Append(_("TAB"));
    choice->Append(_("CLEAR"));
    choice->Append(_("RETURN"));
    choice->Append(_("PAUSE"));
    choice->Append(_("ESCAPE"));
    choice->Append(_("SPACE"));
    choice->Append(_("EXCLAIM"));
    choice->Append(_("QUOTEDBL"));
    choice->Append(_("HASH"));
    choice->Append(_("DOLLAR"));
    choice->Append(_("AMPERSAND"));
    choice->Append(_("QUOTE"));
    choice->Append(_("LEFTPAREN"));
    choice->Append(_("RIGHTPAREN"));
    choice->Append(_("ASTERISK"));
    choice->Append(_("PLUS"));
    choice->Append(_("COMMA"));
    choice->Append(_("MINUS"));
    choice->Append(_("PERIOD"));
    choice->Append(_("SLASH"));
    choice->Append(_("0"));
    choice->Append(_("1"));
    choice->Append(_("2"));
    choice->Append(_("3"));
    choice->Append(_("4"));
    choice->Append(_("5"));
    choice->Append(_("6"));
    choice->Append(_("7"));
    choice->Append(_("8"));
    choice->Append(_("9"));
    choice->Append(_("COLON"));
    choice->Append(_("SEMICOLON"));
    choice->Append(_("LESS"));
    choice->Append(_("EQUALS"));
    choice->Append(_("GREATER"));
    choice->Append(_("QUESTION"));
    choice->Append(_("AT"));
    choice->Append(_("LEFTBRACKET"));
    choice->Append(_("BACKSLASH"));
    choice->Append(_("RIGHTBRACKET"));
    choice->Append(_("CARET"));
    choice->Append(_("UNDERSCORE"));
    choice->Append(_("BACKQUOTE"));
    choice->SetSelection( choice->Append(_("a")) );
    choice->Append(_("b"));
    choice->Append(_("c"));
    choice->Append(_("d"));
    choice->Append(_("e"));
    choice->Append(_("f"));
    choice->Append(_("g"));
    choice->Append(_("h"));
    choice->Append(_("i"));
    choice->Append(_("j"));
    choice->Append(_("k"));
    choice->Append(_("l"));
    choice->Append(_("m"));
    choice->Append(_("n"));
    choice->Append(_("o"));
    choice->Append(_("p"));
    choice->Append(_("q"));
    choice->Append(_("r"));
    choice->Append(_("s"));
    choice->Append(_("t"));
    choice->Append(_("u"));
    choice->Append(_("v"));
    choice->Append(_("w"));
    choice->Append(_("x"));
    choice->Append(_("y"));
    choice->Append(_("z"));
    choice->Append(_("DELETE"));
    choice->Append(_("WORLD_0"));
    choice->Append(_("WORLD_1"));
    choice->Append(_("WORLD_2"));
    choice->Append(_("WORLD_3"));
    choice->Append(_("WORLD_4"));
    choice->Append(_("WORLD_5"));
    choice->Append(_("WORLD_6"));
    choice->Append(_("WORLD_7"));
    choice->Append(_("WORLD_8"));
    choice->Append(_("WORLD_9"));
    choice->Append(_("WORLD_10"));
    choice->Append(_("WORLD_11"));
    choice->Append(_("WORLD_12"));
    choice->Append(_("WORLD_13"));
    choice->Append(_("WORLD_14"));
    choice->Append(_("WORLD_15"));
    choice->Append(_("WORLD_16"));
    choice->Append(_("WORLD_17"));
    choice->Append(_("WORLD_18"));
    choice->Append(_("WORLD_19"));
    choice->Append(_("WORLD_20"));
    choice->Append(_("WORLD_21"));
    choice->Append(_("WORLD_22"));
    choice->Append(_("WORLD_23"));
    choice->Append(_("WORLD_24"));
    choice->Append(_("WORLD_25"));
    choice->Append(_("WORLD_26"));
    choice->Append(_("WORLD_27"));
    choice->Append(_("WORLD_28"));
    choice->Append(_("WORLD_29"));
    choice->Append(_("WORLD_30"));
    choice->Append(_("WORLD_31"));
    choice->Append(_("WORLD_32"));
    choice->Append(_("WORLD_33"));
    choice->Append(_("WORLD_34"));
    choice->Append(_("WORLD_35"));
    choice->Append(_("WORLD_36"));
    choice->Append(_("WORLD_37"));
    choice->Append(_("WORLD_38"));
    choice->Append(_("WORLD_39"));
    choice->Append(_("WORLD_40"));
    choice->Append(_("WORLD_41"));
    choice->Append(_("WORLD_42"));
    choice->Append(_("WORLD_43"));
    choice->Append(_("WORLD_44"));
    choice->Append(_("WORLD_45"));
    choice->Append(_("WORLD_46"));
    choice->Append(_("WORLD_47"));
    choice->Append(_("WORLD_48"));
    choice->Append(_("WORLD_49"));
    choice->Append(_("WORLD_50"));
    choice->Append(_("WORLD_51"));
    choice->Append(_("WORLD_52"));
    choice->Append(_("WORLD_53"));
    choice->Append(_("WORLD_54"));
    choice->Append(_("WORLD_55"));
    choice->Append(_("WORLD_56"));
    choice->Append(_("WORLD_57"));
    choice->Append(_("WORLD_58"));
    choice->Append(_("WORLD_59"));
    choice->Append(_("WORLD_60"));
    choice->Append(_("WORLD_61"));
    choice->Append(_("WORLD_62"));
    choice->Append(_("WORLD_63"));
    choice->Append(_("WORLD_64"));
    choice->Append(_("WORLD_65"));
    choice->Append(_("WORLD_66"));
    choice->Append(_("WORLD_67"));
    choice->Append(_("WORLD_68"));
    choice->Append(_("WORLD_69"));
    choice->Append(_("WORLD_70"));
    choice->Append(_("WORLD_71"));
    choice->Append(_("WORLD_72"));
    choice->Append(_("WORLD_73"));
    choice->Append(_("WORLD_74"));
    choice->Append(_("WORLD_75"));
    choice->Append(_("WORLD_76"));
    choice->Append(_("WORLD_77"));
    choice->Append(_("WORLD_78"));
    choice->Append(_("WORLD_79"));
    choice->Append(_("WORLD_80"));
    choice->Append(_("WORLD_81"));
    choice->Append(_("WORLD_82"));
    choice->Append(_("WORLD_83"));
    choice->Append(_("WORLD_84"));
    choice->Append(_("WORLD_85"));
    choice->Append(_("WORLD_86"));
    choice->Append(_("WORLD_87"));
    choice->Append(_("WORLD_88"));
    choice->Append(_("WORLD_89"));
    choice->Append(_("WORLD_90"));
    choice->Append(_("WORLD_91"));
    choice->Append(_("WORLD_92"));
    choice->Append(_("WORLD_93"));
    choice->Append(_("WORLD_94"));
    choice->Append(_("WORLD_95"));
    choice->Append(_("KP0"));
    choice->Append(_("KP1"));
    choice->Append(_("KP2"));
    choice->Append(_("KP3"));
    choice->Append(_("KP4"));
    choice->Append(_("KP5"));
    choice->Append(_("KP6"));
    choice->Append(_("KP7"));
    choice->Append(_("KP8"));
    choice->Append(_("KP9"));
    choice->Append(_("KP_PERIOD"));
    choice->Append(_("KP_DIVIDE"));
    choice->Append(_("KP_MULTIPLY"));
    choice->Append(_("KP_MINUS"));
    choice->Append(_("KP_PLUS"));
    choice->Append(_("KP_ENTER"));
    choice->Append(_("KP_EQUALS"));
    choice->Append(_("UP"));
    choice->Append(_("DOWN"));
    choice->Append(_("RIGHT"));
    choice->Append(_("LEFT"));
    choice->Append(_("INSERT"));
    choice->Append(_("HOME"));
    choice->Append(_("END"));
    choice->Append(_("PAGEUP"));
    choice->Append(_("PAGEDOWN"));
    choice->Append(_("F1"));
    choice->Append(_("F2"));
    choice->Append(_("F3"));
    choice->Append(_("F4"));
    choice->Append(_("F5"));
    choice->Append(_("F6"));
    choice->Append(_("F7"));
    choice->Append(_("F8"));
    choice->Append(_("F9"));
    choice->Append(_("F10"));
    choice->Append(_("F11"));
    choice->Append(_("F12"));
    choice->Append(_("F13"));
    choice->Append(_("F14"));
    choice->Append(_("F15"));
    choice->Append(_("NUMLOCK"));
    choice->Append(_("CAPSLOCK"));
    choice->Append(_("SCROLLOCK"));
    choice->Append(_("RSHIFT"));
    choice->Append(_("LSHIFT"));
    choice->Append(_("RCTRL"));
    choice->Append(_("LCTRL"));
    choice->Append(_("RALT"));
    choice->Append(_("LALT"));
    choice->Append(_("RMETA"));
    choice->Append(_("LMETA"));
    choice->Append(_("LSUPER"));
    choice->Append(_("RSUPER"));
    choice->Append(_("MODE"));
    choice->Append(_("COMPOSE"));
    choice->Append(_("HELP"));
    choice->Append(_("PRINT"));
    choice->Append(_("SYSREQ"));
    choice->Append(_("BREAK"));
    choice->Append(_("MENU"));
    choice->Append(_("POWER"));
    choice->Append(_("EURO"));
    choice->Append(_("UNDO"));
}

sixaxis_emu_guiFrame::sixaxis_emu_guiFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(sixaxis_emu_guiFrame)
    wxFlexGridSizer* FlexGridSizer4;
    wxFlexGridSizer* FlexGridSizer16;
    wxMenuItem* MenuItem2;
    wxFlexGridSizer* FlexGridSizer10;
    wxFlexGridSizer* FlexGridSizer3;
    wxMenuItem* MenuItem1;
    wxFlexGridSizer* FlexGridSizer5;
    wxFlexGridSizer* FlexGridSizer9;
    wxFlexGridSizer* FlexGridSizer2;
    wxMenu* Menu1;
    wxFlexGridSizer* FlexGridSizer7;
    wxGridSizer* GridSizer1;
    wxFlexGridSizer* FlexGridSizer15;
    wxFlexGridSizer* FlexGridSizer8;
    wxMenuItem* MenuItem6;
    wxFlexGridSizer* FlexGridSizer14;
    wxFlexGridSizer* FlexGridSizer13;
    wxFlexGridSizer* FlexGridSizer12;
    wxMenuBar* MenuBar1;
    wxFlexGridSizer* FlexGridSizer6;
    wxFlexGridSizer* FlexGridSizer1;
    wxFlexGridSizer* FlexGridSizer11;
    wxMenu* Menu2;

    Create(parent, wxID_ANY, _("Sixaxis emulator customizer"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    GridSizer1 = new wxGridSizer(1, 1, 0, 0);
    Notebook1 = new wxNotebook(this, ID_NOTEBOOK1, wxDefaultPosition, wxSize(-1,480), 0, _T("ID_NOTEBOOK1"));
    Panel1 = new wxPanel(Notebook1, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    FlexGridSizer10 = new wxFlexGridSizer(1, 1, 0, 0);
    FlexGridSizer13 = new wxFlexGridSizer(2, 5, 0, 0);
    StaticText1 = new wxStaticText(Panel1, ID_STATICTEXT1, _("Device type"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    FlexGridSizer13->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText19 = new wxStaticText(Panel1, ID_STATICTEXT19, _("Device name"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT19"));
    FlexGridSizer13->Add(StaticText19, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText9 = new wxStaticText(Panel1, ID_STATICTEXT9, _("Device id"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT9"));
    FlexGridSizer13->Add(StaticText9, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText3 = new wxStaticText(Panel1, ID_STATICTEXT3, _("Button id"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    FlexGridSizer13->Add(StaticText3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText2 = new wxStaticText(Panel1, ID_STATICTEXT2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    FlexGridSizer13->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice2 = new wxChoice(Panel1, ID_CHOICE2, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE2"));
    FlexGridSizer13->Add(Choice2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText27 = new wxStaticText(Panel1, ID_STATICTEXT27, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT27"));
    FlexGridSizer13->Add(StaticText27, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice9 = new wxChoice(Panel1, ID_CHOICE9, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE9"));
    FlexGridSizer13->Add(Choice9, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice10 = new wxChoice(Panel1, ID_CHOICE10, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE10"));
    FlexGridSizer13->Add(Choice10, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button1 = new wxButton(Panel1, ID_BUTTON1, _("Auto detect"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    FlexGridSizer13->Add(Button1, 1, wxALL|wxALIGN_BOTTOM|wxALIGN_CENTER_HORIZONTAL, 5);
    FlexGridSizer10->Add(FlexGridSizer13, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Panel1->SetSizer(FlexGridSizer10);
    FlexGridSizer10->Fit(Panel1);
    FlexGridSizer10->SetSizeHints(Panel1);
    Panel2 = new wxPanel(Notebook1, ID_PANEL2, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL2"));
    FlexGridSizer9 = new wxFlexGridSizer(2, 1, 0, 0);
    FlexGridSizer9->AddGrowableRow(1);
    FlexGridSizer1 = new wxFlexGridSizer(2, 8, 0, 0);
    StaticText4 = new wxStaticText(Panel2, ID_STATICTEXT4, _("Device type"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
    FlexGridSizer1->Add(StaticText4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText29 = new wxStaticText(Panel2, ID_STATICTEXT29, _("Device name"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT29"));
    FlexGridSizer1->Add(StaticText29, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText10 = new wxStaticText(Panel2, ID_STATICTEXT10, _("Device id"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT10"));
    FlexGridSizer1->Add(StaticText10, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText5 = new wxStaticText(Panel2, ID_STATICTEXT5, _("Event type"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
    FlexGridSizer1->Add(StaticText5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText6 = new wxStaticText(Panel2, ID_STATICTEXT6, _("Event id"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
    FlexGridSizer1->Add(StaticText6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText8 = new wxStaticText(Panel2, ID_STATICTEXT8, _("Threshold"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT8"));
    FlexGridSizer1->Add(StaticText8, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText7 = new wxStaticText(Panel2, ID_STATICTEXT7, _("Button"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
    FlexGridSizer1->Add(StaticText7, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText20 = new wxStaticText(Panel2, ID_STATICTEXT20, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT20"));
    FlexGridSizer1->Add(StaticText20, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice3 = new wxChoice(Panel2, ID_CHOICE3, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE3"));
    FlexGridSizer1->Add(Choice3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText30 = new wxStaticText(Panel2, ID_STATICTEXT30, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT30"));
    FlexGridSizer1->Add(StaticText30, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice12 = new wxChoice(Panel2, ID_CHOICE12, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE12"));
    FlexGridSizer1->Add(Choice12, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice4 = new wxChoice(Panel2, ID_CHOICE4, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE4"));
    FlexGridSizer1->Add(Choice4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice11 = new wxChoice(Panel2, ID_CHOICE11, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE11"));
    FlexGridSizer1->Add(Choice11, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrl3 = new wxTextCtrl(Panel2, ID_TEXTCTRL3, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL3"));
    TextCtrl3->Disable();
    FlexGridSizer1->Add(TextCtrl3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice5 = new wxChoice(Panel2, ID_CHOICE5, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE5"));
    FlexGridSizer1->Add(Choice5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button8 = new wxButton(Panel2, ID_BUTTON8, _("Auto detect"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON8"));
    FlexGridSizer1->Add(Button8, 1, wxALL|wxALIGN_BOTTOM|wxALIGN_CENTER_HORIZONTAL, 5);
    FlexGridSizer9->Add(FlexGridSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer8 = new wxFlexGridSizer(1, 2, 0, 0);
    Grid1 = new wxGrid(Panel2, ID_GRID1, wxDefaultPosition, wxDefaultSize, wxVSCROLL, _T("ID_GRID1"));
    Grid1->CreateGrid(0,7);
    Grid1->EnableEditing(false);
    Grid1->EnableGridLines(true);
    Grid1->SetDefaultColSize(125, true);
    Grid1->SetColLabelValue(0, _("Device type"));
    Grid1->SetColLabelValue(1, _("Device name"));
    Grid1->SetColLabelValue(2, _("Device id"));
    Grid1->SetColLabelValue(3, _("Event type"));
    Grid1->SetColLabelValue(4, _("Event id"));
    Grid1->SetColLabelValue(5, _("Threshold"));
    Grid1->SetColLabelValue(6, _("Button id"));
    Grid1->SetDefaultCellFont( Grid1->GetFont() );
    Grid1->SetDefaultCellTextColour( Grid1->GetForegroundColour() );
    FlexGridSizer8->Add(Grid1, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer6 = new wxFlexGridSizer(2, 1, 0, 0);
    Button4 = new wxButton(Panel2, ID_BUTTON4, _("Add"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON4"));
    FlexGridSizer6->Add(Button4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button6 = new wxButton(Panel2, ID_BUTTON6, _("Remove"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON6"));
    FlexGridSizer6->Add(Button6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer8->Add(FlexGridSizer6, 1, wxALL|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    FlexGridSizer9->Add(FlexGridSizer8, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Panel2->SetSizer(FlexGridSizer9);
    FlexGridSizer9->Fit(Panel2);
    FlexGridSizer9->SetSizeHints(Panel2);
    Panel3 = new wxPanel(Notebook1, ID_PANEL3, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL3"));
    FlexGridSizer5 = new wxFlexGridSizer(2, 1, 0, 0);
    FlexGridSizer5->AddGrowableRow(1);
    FlexGridSizer3 = new wxFlexGridSizer(2, 10, 0, 0);
    StaticText11 = new wxStaticText(Panel3, ID_STATICTEXT11, _("Device type"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT11"));
    FlexGridSizer3->Add(StaticText11, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText31 = new wxStaticText(Panel3, ID_STATICTEXT31, _("Device name"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT31"));
    FlexGridSizer3->Add(StaticText31, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText12 = new wxStaticText(Panel3, ID_STATICTEXT12, _("Device id"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT12"));
    FlexGridSizer3->Add(StaticText12, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText13 = new wxStaticText(Panel3, ID_STATICTEXT13, _("Event type"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT13"));
    FlexGridSizer3->Add(StaticText13, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText14 = new wxStaticText(Panel3, ID_STATICTEXT14, _("Event id"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT14"));
    FlexGridSizer3->Add(StaticText14, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText16 = new wxStaticText(Panel3, ID_STATICTEXT16, _("Axis"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT16"));
    FlexGridSizer3->Add(StaticText16, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText15 = new wxStaticText(Panel3, ID_STATICTEXT15, _("Dead zone"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT15"));
    FlexGridSizer3->Add(StaticText15, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText17 = new wxStaticText(Panel3, ID_STATICTEXT17, _("Multiplier"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT17"));
    FlexGridSizer3->Add(StaticText17, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText18 = new wxStaticText(Panel3, ID_STATICTEXT18, _("Exponent"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT18"));
    FlexGridSizer3->Add(StaticText18, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText21 = new wxStaticText(Panel3, ID_STATICTEXT21, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT21"));
    FlexGridSizer3->Add(StaticText21, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice6 = new wxChoice(Panel3, ID_CHOICE6, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE6"));
    FlexGridSizer3->Add(Choice6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText32 = new wxStaticText(Panel3, ID_STATICTEXT32, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT32"));
    FlexGridSizer3->Add(StaticText32, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice13 = new wxChoice(Panel3, ID_CHOICE13, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE13"));
    FlexGridSizer3->Add(Choice13, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice7 = new wxChoice(Panel3, ID_CHOICE7, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE7"));
    FlexGridSizer3->Add(Choice7, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice14 = new wxChoice(Panel3, ID_CHOICE14, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE14"));
    FlexGridSizer3->Add(Choice14, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice8 = new wxChoice(Panel3, ID_CHOICE8, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE8"));
    FlexGridSizer3->Add(Choice8, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrl8 = new wxTextCtrl(Panel3, ID_TEXTCTRL8, _("8"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL8"));
    FlexGridSizer3->Add(TextCtrl8, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrl9 = new wxTextCtrl(Panel3, ID_TEXTCTRL9, _("4"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL9"));
    FlexGridSizer3->Add(TextCtrl9, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrl10 = new wxTextCtrl(Panel3, ID_TEXTCTRL10, _("1"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL10"));
    FlexGridSizer3->Add(TextCtrl10, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button9 = new wxButton(Panel3, ID_BUTTON9, _("Auto detect"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON9"));
    FlexGridSizer3->Add(Button9, 1, wxALL|wxALIGN_BOTTOM|wxALIGN_CENTER_HORIZONTAL, 5);
    FlexGridSizer5->Add(FlexGridSizer3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer4 = new wxFlexGridSizer(1, 2, 0, 0);
    Grid2 = new wxGrid(Panel3, ID_GRID2, wxDefaultPosition, wxDefaultSize, 0, _T("ID_GRID2"));
    Grid2->CreateGrid(0,9);
    Grid2->EnableEditing(false);
    Grid2->EnableGridLines(true);
    Grid2->SetDefaultColSize(100, true);
    Grid2->SetColLabelValue(0, _("Device type"));
    Grid2->SetColLabelValue(1, _("Device name"));
    Grid2->SetColLabelValue(2, _("Device id"));
    Grid2->SetColLabelValue(3, _("Event type"));
    Grid2->SetColLabelValue(4, _("Event id"));
    Grid2->SetColLabelValue(5, _("Axis id"));
    Grid2->SetColLabelValue(6, _("Dead zone"));
    Grid2->SetColLabelValue(7, _("Multiplier"));
    Grid2->SetColLabelValue(8, _("Exponent"));
    Grid2->SetDefaultCellFont( Grid2->GetFont() );
    Grid2->SetDefaultCellTextColour( Grid2->GetForegroundColour() );
    FlexGridSizer4->Add(Grid2, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    FlexGridSizer7 = new wxFlexGridSizer(2, 1, 0, 0);
    Button3 = new wxButton(Panel3, ID_BUTTON3, _("Add"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
    FlexGridSizer7->Add(Button3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button7 = new wxButton(Panel3, ID_BUTTON7, _("Remove"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON7"));
    FlexGridSizer7->Add(Button7, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer4->Add(FlexGridSizer7, 1, wxALL|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    FlexGridSizer5->Add(FlexGridSizer4, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Panel3->SetSizer(FlexGridSizer5);
    FlexGridSizer5->Fit(Panel3);
    FlexGridSizer5->SetSizeHints(Panel3);
    Panel4 = new wxPanel(Notebook1, ID_PANEL4, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL4"));
    Panel4->Hide();
    FlexGridSizer2 = new wxFlexGridSizer(3, 1, 0, 0);
    FlexGridSizer2->AddGrowableRow(1);
    FlexGridSizer15 = new wxFlexGridSizer(0, 3, 0, 0);
    FlexGridSizer16 = new wxFlexGridSizer(2, 3, 0, 0);
    CheckBox1 = new wxCheckBox(Panel4, ID_CHECKBOX1, _("Delay"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
    CheckBox1->SetValue(false);
    FlexGridSizer16->Add(CheckBox1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrl1 = new wxTextCtrl(Panel4, ID_TEXTCTRL1, _("50"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    TextCtrl1->Disable();
    FlexGridSizer16->Add(TextCtrl1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText26 = new wxStaticText(Panel4, ID_STATICTEXT26, _("ms"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT26"));
    FlexGridSizer16->Add(StaticText26, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    CheckBox2 = new wxCheckBox(Panel4, ID_CHECKBOX2, _("Event"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
    CheckBox2->SetValue(false);
    FlexGridSizer16->Add(CheckBox2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer15->Add(FlexGridSizer16, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer11 = new wxFlexGridSizer(2, 6, 0, 0);
    StaticText22 = new wxStaticText(Panel4, ID_STATICTEXT22, _("Device type"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT22"));
    FlexGridSizer11->Add(StaticText22, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText33 = new wxStaticText(Panel4, ID_STATICTEXT33, _("Device name"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT33"));
    FlexGridSizer11->Add(StaticText33, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText23 = new wxStaticText(Panel4, ID_STATICTEXT23, _("Device id"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT23"));
    FlexGridSizer11->Add(StaticText23, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText24 = new wxStaticText(Panel4, ID_STATICTEXT24, _("Event type"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT24"));
    FlexGridSizer11->Add(StaticText24, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText25 = new wxStaticText(Panel4, ID_STATICTEXT25, _("Event id"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT25"));
    FlexGridSizer11->Add(StaticText25, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText28 = new wxStaticText(Panel4, ID_STATICTEXT28, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT28"));
    FlexGridSizer11->Add(StaticText28, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice15 = new wxChoice(Panel4, ID_CHOICE15, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE15"));
    Choice15->Disable();
    FlexGridSizer11->Add(Choice15, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText34 = new wxStaticText(Panel4, ID_STATICTEXT34, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT34"));
    FlexGridSizer11->Add(StaticText34, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice16 = new wxChoice(Panel4, ID_CHOICE16, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE16"));
    Choice16->Disable();
    FlexGridSizer11->Add(Choice16, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice17 = new wxChoice(Panel4, ID_CHOICE17, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE17"));
    Choice17->SetSelection( Choice17->Append(_("button")) );
    Choice17->Append(_("button down"));
    Choice17->Append(_("button up"));
    Choice17->Disable();
    FlexGridSizer11->Add(Choice17, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice18 = new wxChoice(Panel4, ID_CHOICE18, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE18"));
    Choice18->Disable();
    FlexGridSizer11->Add(Choice18, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button10 = new wxButton(Panel4, ID_BUTTON10, _("Auto detect"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON10"));
    Button10->Disable();
    FlexGridSizer11->Add(Button10, 1, wxALL|wxALIGN_BOTTOM|wxALIGN_CENTER_HORIZONTAL, 5);
    FlexGridSizer15->Add(FlexGridSizer11, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer2->Add(FlexGridSizer15, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer12 = new wxFlexGridSizer(1, 2, 0, 0);
    Grid3 = new wxGrid(Panel4, ID_GRID3, wxDefaultPosition, wxDefaultSize, wxVSCROLL, _T("ID_GRID3"));
    Grid3->CreateGrid(0,6);
    Grid3->EnableEditing(false);
    Grid3->EnableGridLines(true);
    Grid3->SetDefaultColSize(125, true);
    Grid3->SetColLabelValue(0, _("Delay"));
    Grid3->SetColLabelValue(1, _("Device type"));
    Grid3->SetColLabelValue(2, _("Device name"));
    Grid3->SetColLabelValue(3, _("Device id"));
    Grid3->SetColLabelValue(4, _("Event type"));
    Grid3->SetColLabelValue(5, _("Event id"));
    Grid3->SetDefaultCellFont( Grid3->GetFont() );
    Grid3->SetDefaultCellTextColour( Grid3->GetForegroundColour() );
    FlexGridSizer12->Add(Grid3, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer14 = new wxFlexGridSizer(2, 1, 0, 0);
    Button11 = new wxButton(Panel4, ID_BUTTON11, _("Add"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON11"));
    FlexGridSizer14->Add(Button11, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button13 = new wxButton(Panel4, ID_BUTTON13, _("Remove"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON13"));
    FlexGridSizer14->Add(Button13, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer12->Add(FlexGridSizer14, 1, wxALL|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    FlexGridSizer2->Add(FlexGridSizer12, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Panel4->SetSizer(FlexGridSizer2);
    FlexGridSizer2->Fit(Panel4);
    FlexGridSizer2->SetSizeHints(Panel4);
    Notebook1->AddPage(Panel1, _("Configuration trigger"), false);
    Notebook1->AddPage(Panel2, _("Button configuration"), false);
    Notebook1->AddPage(Panel3, _("Axis configuration"), false);
    Notebook1->AddPage(Panel4, _("Macros"), false);
    GridSizer1->Add(Notebook1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(GridSizer1);
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem3 = new wxMenuItem(Menu1, idMenuNew, _("New\tCtrl-N"), _("Create a new configuration"), wxITEM_NORMAL);
    Menu1->Append(MenuItem3);
    MenuItem6 = new wxMenuItem(Menu1, idMenuOpen, _("Open\tCtrl-o"), _("Open a configuration file"), wxITEM_NORMAL);
    Menu1->Append(MenuItem6);
    MenuItem4 = new wxMenuItem(Menu1, idMenuSave, _("Save\tCtrl-s"), _("Save configuration"), wxITEM_NORMAL);
    Menu1->Append(MenuItem4);
    MenuItem5 = new wxMenuItem(Menu1, idMenuSaveAs, _("Save As..."), _("Save configuration to specified file"), wxITEM_NORMAL);
    Menu1->Append(MenuItem5);
    Menu1->AppendSeparator();
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu3 = new wxMenu();
    MenuItem7 = new wxMenuItem(Menu3, ID_MENUITEM1, _("1"), wxEmptyString, wxITEM_RADIO);
    Menu3->Append(MenuItem7);
    MenuItem8 = new wxMenuItem(Menu3, ID_MENUITEM2, _("2"), wxEmptyString, wxITEM_RADIO);
    Menu3->Append(MenuItem8);
    MenuItem9 = new wxMenuItem(Menu3, ID_MENUITEM3, _("3"), wxEmptyString, wxITEM_RADIO);
    Menu3->Append(MenuItem9);
    MenuItem10 = new wxMenuItem(Menu3, ID_MENUITEM4, _("4"), wxEmptyString, wxITEM_RADIO);
    Menu3->Append(MenuItem10);
    MenuItem11 = new wxMenuItem(Menu3, ID_MENUITEM5, _("5"), wxEmptyString, wxITEM_RADIO);
    Menu3->Append(MenuItem11);
    MenuItem12 = new wxMenuItem(Menu3, ID_MENUITEM6, _("6"), wxEmptyString, wxITEM_RADIO);
    Menu3->Append(MenuItem12);
    MenuItem13 = new wxMenuItem(Menu3, ID_MENUITEM7, _("7"), wxEmptyString, wxITEM_RADIO);
    Menu3->Append(MenuItem13);
    MenuBar1->Append(Menu3, _("Controller"));
    Menu4 = new wxMenu();
    MenuItem14 = new wxMenuItem(Menu4, ID_MENUITEM8, _("1"), wxEmptyString, wxITEM_RADIO);
    Menu4->Append(MenuItem14);
    MenuItem15 = new wxMenuItem(Menu4, ID_MENUITEM9, _("2"), wxEmptyString, wxITEM_RADIO);
    Menu4->Append(MenuItem15);
    MenuItem17 = new wxMenuItem(Menu4, ID_MENUITEM10, _("3"), wxEmptyString, wxITEM_RADIO);
    Menu4->Append(MenuItem17);
    MenuItem16 = new wxMenuItem(Menu4, ID_MENUITEM11, _("4"), wxEmptyString, wxITEM_RADIO);
    Menu4->Append(MenuItem16);
    MenuBar1->Append(Menu4, _("Configuration"));
    Menu5 = new wxMenu();
    MenuItem18 = new wxMenuItem(Menu5, ID_MENUITEM12, _("Enable"), wxEmptyString, wxITEM_CHECK);
    Menu5->Append(MenuItem18);
    MenuBar1->Append(Menu5, _("Expert"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("Help"));
    SetMenuBar(MenuBar1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);
    FileDialog1 = new wxFileDialog(this, _("Select file"), wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr, wxFD_DEFAULT_STYLE, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    GridSizer1->Fit(this);
    GridSizer1->SetSizeHints(this);

    Connect(ID_CHOICE2,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnChoice2Select);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnButton1Click1);
    Connect(ID_CHOICE3,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnChoice3Select1);
    Connect(ID_CHOICE4,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnChoice4Select1);
    Connect(ID_BUTTON8,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnButton8Click);
    Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnButtonAdd1Click);
    Connect(ID_BUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnButton6Click);
    Connect(ID_CHOICE6,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnChoice6Select1);
    Connect(ID_CHOICE7,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnChoice4Select);
    Connect(ID_BUTTON9,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnButton9Click);
    Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnButton3Click);
    Connect(ID_BUTTON7,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnButton7Click);
    Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnCheckBox1Click);
    Connect(ID_CHECKBOX2,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnCheckBox2Click);
    Connect(ID_CHOICE15,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnChoice15Select);
    Connect(ID_BUTTON10,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnButton10Click);
    Connect(ID_BUTTON11,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnButton11Click);
    Connect(ID_BUTTON13,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnButton13Click);
    Connect(idMenuNew,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnMenuItemNew);
    Connect(idMenuOpen,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnMenuOpen);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnQuit);
    Connect(ID_MENUITEM1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnMenuItemController1);
    Connect(ID_MENUITEM2,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnMenuItemController2);
    Connect(ID_MENUITEM3,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnMenuItemController3);
    Connect(ID_MENUITEM4,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnMenuItemController4);
    Connect(ID_MENUITEM5,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnMenuItemController5);
    Connect(ID_MENUITEM6,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnMenuItemController6);
    Connect(ID_MENUITEM7,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnMenuItemController7);
    Connect(ID_MENUITEM8,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnMenuItemConfiguration1);
    Connect(ID_MENUITEM9,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnMenuItemConfiguration2);
    Connect(ID_MENUITEM10,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnMenuItemConfiguration3);
    Connect(ID_MENUITEM11,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnMenuItemConfiguration4);
    Connect(ID_MENUITEM12,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnMenuItemExpert);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnAbout);
    //*)

    expertMode = false;

    currentController = 0;
    currentConfiguration = 0;

    Grid1->AutoSizeColumns();
    Grid2->AutoSizeColumns();
}

sixaxis_emu_guiFrame::~sixaxis_emu_guiFrame()
{
    //(*Destroy(sixaxis_emu_guiFrame)
    //*)
}

void sixaxis_emu_guiFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void sixaxis_emu_guiFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

void sixaxis_emu_guiFrame::OnMenuItemNew(wxCommandEvent& event)
{
}

void sixaxis_emu_guiFrame::OnButtonAdd1Click(wxCommandEvent& event)
{
    Grid1->InsertRows();
    Grid1->SetCellValue(0, 0, Choice3->GetStringSelection());
    Grid1->SetCellValue(0, 1, StaticText30->GetLabel());
    Grid1->SetCellValue(0, 2, Choice12->GetStringSelection());
    Grid1->SetCellValue(0, 3, Choice4->GetStringSelection());
    Grid1->SetCellValue(0, 4, Choice11->GetStringSelection());
    Grid1->SetCellValue(0, 5, TextCtrl3->GetValue());
    Grid1->SetCellValue(0, 6, Choice5->GetStringSelection());
    Grid1->AutoSizeColumns();
    Panel2->Layout();
}

void sixaxis_emu_guiFrame::OnButton3Click(wxCommandEvent& event)
{
    Grid2->InsertRows();
    Grid2->SetCellValue(0, 0, Choice6->GetStringSelection());
    Grid2->SetCellValue(0, 1, StaticText32->GetLabel());
    Grid2->SetCellValue(0, 2, Choice13->GetStringSelection());
    Grid2->SetCellValue(0, 3, Choice7->GetStringSelection());
    Grid2->SetCellValue(0, 4, Choice14->GetStringSelection());
    Grid2->SetCellValue(0, 5, Choice8->GetStringSelection());
    Grid2->SetCellValue(0, 6, TextCtrl8->GetValue());
    Grid2->SetCellValue(0, 7, TextCtrl9->GetValue());
    Grid2->SetCellValue(0, 8, TextCtrl10->GetValue());
    Grid2->AutoSizeColumns();
    Panel3->Layout();
}

void sixaxis_emu_guiFrame::DeleteSelectedRows(wxGrid* grid)
{
    unsigned int first;
    unsigned int nbRows;
    wxArrayInt array = grid->GetSelectedRows();

    if(array.GetCount())
    {
        first = array[0];
    }
    else
    {
        return;
    }

    while(array.GetCount())
    {
        grid->DeleteRows(array.Item(0), 1);
        array = grid->GetSelectedRows();
    }

    nbRows = grid->GetNumberRows();

    if(nbRows > first)
    {
        grid->SelectRow(first);
    }
    else if(nbRows)
    {
        grid->SelectRow(nbRows-1);
    }

    grid->AutoSizeColumns();
}

void sixaxis_emu_guiFrame::OnButton6Click(wxCommandEvent& event)
{
    sixaxis_emu_guiFrame::DeleteSelectedRows(Grid1);
    Panel2->Layout();
}

void sixaxis_emu_guiFrame::OnButton7Click(wxCommandEvent& event)
{
    sixaxis_emu_guiFrame::DeleteSelectedRows(Grid2);
    Panel3->Layout();
}

void sixaxis_emu_guiFrame::OnChoice4Select(wxCommandEvent& event)
{

    if(Choice7->GetStringSelection() != _("axis"))
    {
        TextCtrl8->Disable();
        TextCtrl8->SetValue(_(""));
        TextCtrl9->Disable();
        TextCtrl9->SetValue(_(""));
        TextCtrl10->Disable();
        TextCtrl10->SetValue(_(""));

        if(Choice6->GetStringSelection() == _("keyboard"))
        {
            fillKeyboardButtonChoice(Choice14);
            fillButtonAxisChoice(Choice8);
        }
        else if(Choice6->GetStringSelection() == _("mouse"))
        {
            fillMouseButtonChoice(Choice14);
            fillButtonAxisChoice(Choice8);
        }
        else if(Choice6->GetStringSelection() == _("joystick"))
        {
            fillJoystickChoice(Choice14);
            fillButtonAxisChoice(Choice8);
        }
    }
    else
    {
        TextCtrl8->Enable();
        TextCtrl8->SetValue(_("8"));
        TextCtrl9->Enable();
        TextCtrl9->SetValue(_("4"));
        TextCtrl10->Enable();
        TextCtrl10->SetValue(_("1"));

        if(Choice6->GetStringSelection() == _("mouse"))
        {
            fillMouseAxisChoice(Choice14);
            fillAxisAxisChoice(Choice8);
        }
        else if(Choice6->GetStringSelection() == _("joystick"))
        {
            fillJoystickChoice(Choice14);
            fillAxisAxisChoice(Choice8);
        }
    }
    Panel3->Layout();
}

void sixaxis_emu_guiFrame::OnChoice4Select1(wxCommandEvent& event)
{
    if(Choice4->GetStringSelection() == _("button"))
    {
        TextCtrl3->Disable();
        TextCtrl3->SetValue(_(""));
        if(Choice3->GetStringSelection() == _("keyboard"))
        {
            fillKeyboardButtonChoice(Choice11);
        }
        else if(Choice3->GetStringSelection() == _("mouse"))
        {
            fillMouseButtonChoice(Choice11);
        }
        else if(Choice3->GetStringSelection() == _("joystick"))
        {
            fillJoystickChoice(Choice11);
        }
    }
    else
    {
        TextCtrl3->Enable();
        TextCtrl3->SetValue(_("10"));
        if(Choice3->GetStringSelection() == _("mouse"))
        {
            fillMouseAxisChoice(Choice11);
        }
        else if(Choice3->GetStringSelection() == _("joystick"))
        {
            fillJoystickChoice(Choice11);
        }
    }
    Panel2->Layout();
}

void sixaxis_emu_guiFrame::OnChoice2Select(wxCommandEvent& event)
{
    StaticText27->SetLabel(_(""));
    if(Choice2->GetStringSelection() == _("keyboard"))
    {

        fillKeyboardButtonChoice(Choice10);
    }
    else if(Choice2->GetStringSelection() == _("mouse"))
    {
        fillMouseButtonChoice(Choice10);
    }
    else
    {
        fillJoystickChoice(Choice10);
    }
    fillDeviceIdButtonChoice(Choice9);
    Panel1->Layout();
}

void sixaxis_emu_guiFrame::OnChoice3Select1(wxCommandEvent& event)
{
    Choice4->Clear();
    if(!expertMode) return;
    Choice4->SetSelection( Choice4->Append(_("button")) );
    if(Choice3->GetStringSelection() == _("keyboard"))
    {
        fillKeyboardButtonChoice(Choice11);
    }
    else
    {
        Choice4->Append(_("axis up"));
        Choice4->Append(_("axis down"));
        if(Choice3->GetStringSelection() == _("mouse"))
        {
            fillMouseButtonChoice(Choice11);
        }
        else
        {
            fillJoystickChoice(Choice11);
        }
    }
    fillDeviceIdButtonChoice(Choice12);
    OnChoice4Select1(event);
}

void sixaxis_emu_guiFrame::OnChoice6Select1(wxCommandEvent& event)
{
    Choice7->Clear();
    if(!expertMode) return;
    if(Choice6->GetStringSelection() == _("keyboard"))
    {
        Choice7->SetSelection( Choice7->Append(_("button")) );
        fillKeyboardButtonChoice(Choice14);

    }
    else
    {
        Choice7->SetSelection( Choice7->Append(_("axis")) );
        Choice7->Append(_("button"));
        if(Choice6->GetStringSelection() == _("mouse"))
        {
            fillMouseButtonChoice(Choice14);
        }
        else
        {
            fillJoystickChoice(Choice14);
        }
    }
    StaticText32->SetLabel(_(""));
    fillDeviceIdButtonChoice(Choice13);
    OnChoice4Select(event);
}

void sixaxis_emu_guiFrame::OnChoice15Select(wxCommandEvent& event)
{
    Choice17->Clear();
    Choice17->SetSelection( Choice17->Append(_("button")) );
    Choice17->Append(_("button down"));
    Choice17->Append(_("button up"));
    if(Choice15->GetStringSelection() == _("keyboard"))
    {
        fillKeyboardButtonChoice(Choice18);
    }
    else if(Choice15->GetStringSelection() == _("mouse"))
    {
        fillMouseButtonChoice(Choice18);
    }
    else if(Choice15->GetStringSelection() == _("joystick"))
    {
        fillJoystickChoice(Choice18);
    }
    fillDeviceIdButtonChoice(Choice16);
    Panel4->Layout();
}

void sixaxis_emu_guiFrame::OnButton11Click(wxCommandEvent& event)
{
    if(CheckBox1->IsChecked() || CheckBox2->IsChecked())
    {
        Grid3->InsertRows();
        if(CheckBox1->IsChecked())
        {
            Grid3->SetCellValue(0, 0, TextCtrl1->GetValue());
        }
        if(CheckBox2->IsChecked())
        {
            Grid3->SetCellValue(0, 1, Choice15->GetStringSelection());
            Grid3->SetCellValue(0, 2, StaticText34->GetLabel());
            Grid3->SetCellValue(0, 3, Choice16->GetStringSelection());
            Grid3->SetCellValue(0, 4, Choice17->GetStringSelection());
            Grid3->SetCellValue(0, 5, Choice18->GetStringSelection());
        }
        Grid3->AutoSizeColumns();
        Panel4->Layout();
    }
}

void sixaxis_emu_guiFrame::OnCheckBox2Click(wxCommandEvent& event)
{
    if(CheckBox2->IsChecked())
    {
        Choice15->Enable();
        Choice16->Enable();
        Choice17->Enable();
        Choice18->Enable();
        Button10->Enable();
    }
    else
    {
        Choice15->Disable();
        Choice16->Disable();
        Choice17->Disable();
        Choice18->Disable();
        Button10->Disable();
    }
}

void sixaxis_emu_guiFrame::OnCheckBox1Click(wxCommandEvent& event)
{
    if(CheckBox1->IsChecked())
    {
        TextCtrl1->Enable();
    }
    else
    {
        TextCtrl1->Disable();
    }
}

void sixaxis_emu_guiFrame::OnButton13Click(wxCommandEvent& event)
{
    sixaxis_emu_guiFrame::DeleteSelectedRows(Grid3);
    Panel4->Layout();
}

void sixaxis_emu_guiFrame::auto_detect(event_catcher* evcatch, wxChoice* device_type, wxStaticText* device_name, wxChoice* device_id, wxChoice* event_type, wxChoice* event_id, bool button, bool motion)
{
    evcatch->run(button, motion);

    device_type->Clear();
    device_type->SetSelection( device_type->Append(wxString(evcatch->GetDeviceType())));
    if(evcatch->GetDeviceType() == _("keyboard"))
    {
        device_name->SetLabel(_(""));
        if(expertMode)
        {
            device_type->Append(_("mouse"));
            device_type->Append(_("joystick"));
        }
    }
    else if(evcatch->GetDeviceType() == _("mouse"))
    {
        device_name->SetLabel(_(""));
        if(expertMode)
        {
            device_type->Append(_("keyboard"));
            device_type->Append(_("joystick"));
        }
    }
    else if(evcatch->GetDeviceType() == _("joystick"))
    {
        device_name->SetLabel(wxString(evcatch->GetDeviceName()));
        if(expertMode)
        {
            device_type->Append(_("mouse"));
            device_type->Append(_("keyboard"));
        }
    }
    device_id->Clear();
    device_id->SetSelection( device_id->Append(wxString(evcatch->GetDeviceId())));
    if(event_type)
    {
        event_type->Clear();
        event_type->SetSelection( event_type->Append(wxString(evcatch->GetEventType())));
    }
    event_id->Clear();
    event_id->SetSelection( event_id->Append(wxString(evcatch->GetEventId())));
}

void sixaxis_emu_guiFrame::OnButton1Click1(wxCommandEvent& event)
{
    auto_detect(&evcatch, Choice2, StaticText27, Choice9, NULL, Choice10, true, false);

    Panel1->Layout();
}

void sixaxis_emu_guiFrame::OnButton8Click(wxCommandEvent& event)
{
    auto_detect(&evcatch, Choice3, StaticText30, Choice12, Choice4, Choice11, true, true);

    if(evcatch.GetEventType() == _("button"))
    {
        TextCtrl3->Disable();
        TextCtrl3->SetValue(_(""));
    }
    else
    {
        TextCtrl3->Enable();
        TextCtrl3->SetValue(_("10"));
    }

    Panel2->Layout();
}

void sixaxis_emu_guiFrame::OnButton9Click(wxCommandEvent& event)
{
    auto_detect(&evcatch, Choice6, StaticText32, Choice13, Choice7, Choice14, true, true);

    if(evcatch.GetEventType() == _("button"))
    {
        TextCtrl8->Disable();
        TextCtrl8->SetValue(_(""));
        TextCtrl9->Disable();
        TextCtrl9->SetValue(_(""));
        TextCtrl10->Disable();
        TextCtrl10->SetValue(_(""));
        fillButtonAxisChoice(Choice8);
    }
    else
    {
        TextCtrl8->Enable();
        TextCtrl8->SetValue(_("8"));
        TextCtrl9->Enable();
        TextCtrl9->SetValue(_("4"));
        TextCtrl10->Enable();
        TextCtrl10->SetValue(_("1"));
        fillAxisAxisChoice(Choice8);
    }

    Panel3->Layout();
}

void sixaxis_emu_guiFrame::OnButton10Click(wxCommandEvent& event)
{
    auto_detect(&evcatch, Choice15, StaticText34, Choice16, Choice17, Choice18, true, false);

    Panel4->Layout();
}

void sixaxis_emu_guiFrame::save_current()
{
    std::list<ButtonMapper>* buttonMappers;
    std::list<AxisMapper>* axisMappers;
    //Save Trigger
    configFile.GetController(currentController)->GetConfiguration(currentConfiguration)->GetTrigger()->GetDevice()->SetType(Choice2->GetStringSelection());
    configFile.GetController(currentController)->GetConfiguration(currentConfiguration)->GetTrigger()->GetDevice()->SetName(StaticText27->GetLabel());
    configFile.GetController(currentController)->GetConfiguration(currentConfiguration)->GetTrigger()->GetDevice()->SetId(Choice9->GetStringSelection());
    configFile.GetController(currentController)->GetConfiguration(currentConfiguration)->GetTrigger()->GetEvent()->SetId(Choice10->GetStringSelection());
    //Save ButtonMappers
    buttonMappers = configFile.GetController(currentController)->GetConfiguration(currentConfiguration)->GetButtonMapperList();
    for(std::list<ButtonMapper>::iterator it = buttonMappers->begin(); it!=buttonMappers->end(); it = buttonMappers->erase(it)) {}
    for(int i=0; i<Grid1->GetNumberRows(); i++)
    {
        //ButtonMapper(wxString dtype, wxString did, wxString dname, wxString etype, wxString eid, wxString threshold, wxString button)
        buttonMappers->push_back(ButtonMapper(Grid1->GetCellValue(i, 0), Grid1->GetCellValue(i, 2), Grid1->GetCellValue(i, 1), Grid1->GetCellValue(i, 3), Grid1->GetCellValue(i, 4), Grid1->GetCellValue(i, 5), Grid1->GetCellValue(i, 6)));
    }
    //Save AxisMappers
    axisMappers = configFile.GetController(currentController)->GetConfiguration(currentConfiguration)->GetAxisMapperList();
    for(std::list<AxisMapper>::iterator it = axisMappers->begin(); it!=axisMappers->end(); it = axisMappers->erase(it)) {}
    for(int i=0; i<Grid2->GetNumberRows(); i++)
    {
        //AxisMapper(wxString dtype, wxString did, wxString dname, wxString etype, wxString eid, wxString axis, wxString deadZone, wxString multiplier, wxString exponent);
        axisMappers->push_back(AxisMapper(Grid2->GetCellValue(i, 0), Grid2->GetCellValue(i, 2), Grid2->GetCellValue(i, 1), Grid2->GetCellValue(i, 3), Grid2->GetCellValue(i, 4), Grid2->GetCellValue(i, 5), Grid2->GetCellValue(i, 6), Grid2->GetCellValue(i, 7), Grid2->GetCellValue(i, 8)));
    }

}

void sixaxis_emu_guiFrame::load_current()
{
    std::list<ButtonMapper>* buttonMappers;
    std::list<AxisMapper>* axisMappers;
    //Load Trigger
    Choice2->Clear();
    Choice2->SetSelection(Choice2->Append(configFile.GetController(currentController)->GetConfiguration(currentConfiguration)->GetTrigger()->GetDevice()->GetType()));
    StaticText27->SetLabel(configFile.GetController(currentController)->GetConfiguration(currentConfiguration)->GetTrigger()->GetDevice()->GetName());
    Choice9->Clear();
    Choice9->SetSelection(Choice9->Append(configFile.GetController(currentController)->GetConfiguration(currentConfiguration)->GetTrigger()->GetDevice()->GetId()));
    Choice10->Clear();
    Choice10->SetSelection(Choice10->Append(configFile.GetController(currentController)->GetConfiguration(currentConfiguration)->GetTrigger()->GetEvent()->GetId()));
    //Load ButtonMappers
    Grid1->DeleteRows(0, Grid1->GetNumberRows());
    buttonMappers = configFile.GetController(currentController)->GetConfiguration(currentConfiguration)->GetButtonMapperList();
    for(std::list<ButtonMapper>::iterator it = buttonMappers->begin(); it!=buttonMappers->end(); it++)
    {
        Grid1->InsertRows();
        Grid1->SetCellValue(0, 0, it->GetDevice()->GetType());
        Grid1->SetCellValue(0, 1, it->GetDevice()->GetName());
        Grid1->SetCellValue(0, 2, it->GetDevice()->GetId());
        Grid1->SetCellValue(0, 3, it->GetEvent()->GetType());
        Grid1->SetCellValue(0, 4, it->GetEvent()->GetId());
        Grid1->SetCellValue(0, 5, it->GetEvent()->GetThreshold());
        Grid1->SetCellValue(0, 6, it->GetButton());
        Grid1->AutoSizeColumns();
    }
    //Load AxisMappers
    Grid2->DeleteRows(0, Grid2->GetNumberRows());
    axisMappers = configFile.GetController(currentController)->GetConfiguration(currentConfiguration)->GetAxisMapperList();
    for(std::list<AxisMapper>::iterator it = axisMappers->begin(); it!=axisMappers->end(); it++)
    {
        Grid2->InsertRows();
        Grid2->SetCellValue(0, 0, it->GetDevice()->GetType());
        Grid2->SetCellValue(0, 1, it->GetDevice()->GetName());
        Grid2->SetCellValue(0, 2, it->GetDevice()->GetId());
        Grid2->SetCellValue(0, 3, it->GetEvent()->GetType());
        Grid2->SetCellValue(0, 4, it->GetEvent()->GetId());
        Grid2->SetCellValue(0, 5, it->GetAxis());
        Grid2->SetCellValue(0, 6, it->GetDeadZone());
        Grid2->SetCellValue(0, 7, it->GetMultiplier());
        Grid2->SetCellValue(0, 8, it->GetExponent());
        Grid2->AutoSizeColumns();
    }
}

void sixaxis_emu_guiFrame::refresh_gui()
{
    Panel1->Layout();
    Panel2->Layout();
    Panel3->Layout();
}

void sixaxis_emu_guiFrame::OnMenuOpen(wxCommandEvent& event)
{
    if ( FileDialog1->ShowModal() != wxID_OK ) return;

    wxString FileName = FileDialog1->GetPath();
    if ( FileName.IsEmpty() ) return;

    configFile.ReadConfigFile(FileName);

    currentController = 0;
    currentConfiguration = 0;
    Menu3->Check(ID_MENUITEM1, true);
    Menu4->Check(ID_MENUITEM8, true);
    load_current();
    refresh_gui();
}

void sixaxis_emu_guiFrame::OnMenuItemController1(wxCommandEvent& event)
{
    save_current();
    currentController = 0;
    currentConfiguration = 0;
    Menu4->Check(ID_MENUITEM8, true);
    load_current();
    refresh_gui();
}

void sixaxis_emu_guiFrame::OnMenuItemController2(wxCommandEvent& event)
{
    save_current();
    currentController = 1;
    currentConfiguration = 0;
    Menu4->Check(ID_MENUITEM8, true);
    load_current();
    refresh_gui();
}

void sixaxis_emu_guiFrame::OnMenuItemController3(wxCommandEvent& event)
{
    save_current();
    currentController = 2;
    currentConfiguration = 0;
    Menu4->Check(ID_MENUITEM8, true);
    load_current();
    refresh_gui();
}

void sixaxis_emu_guiFrame::OnMenuItemController4(wxCommandEvent& event)
{
    save_current();
    currentController = 3;
    currentConfiguration = 0;
    Menu4->Check(ID_MENUITEM8, true);
    load_current();
    refresh_gui();
}

void sixaxis_emu_guiFrame::OnMenuItemController5(wxCommandEvent& event)
{
    save_current();
    currentController = 4;
    currentConfiguration = 0;
    Menu4->Check(ID_MENUITEM8, true);
    load_current();
    refresh_gui();
}

void sixaxis_emu_guiFrame::OnMenuItemController6(wxCommandEvent& event)
{
    save_current();
    currentController = 5;
    currentConfiguration = 0;
    Menu4->Check(ID_MENUITEM8, true);
    load_current();
    refresh_gui();
}

void sixaxis_emu_guiFrame::OnMenuItemController7(wxCommandEvent& event)
{
    save_current();
    currentController = 6;
    currentConfiguration = 0;
    Menu4->Check(ID_MENUITEM8, true);
    load_current();
    refresh_gui();
}

void sixaxis_emu_guiFrame::OnMenuItemConfiguration1(wxCommandEvent& event)
{
    save_current();
    currentConfiguration = 0;
    load_current();
    refresh_gui();
}

void sixaxis_emu_guiFrame::OnMenuItemConfiguration2(wxCommandEvent& event)
{
    save_current();
    currentConfiguration = 1;
    load_current();
    refresh_gui();
}

void sixaxis_emu_guiFrame::OnMenuItemConfiguration3(wxCommandEvent& event)
{
    save_current();
    currentConfiguration = 2;
    load_current();
    refresh_gui();
}

void sixaxis_emu_guiFrame::OnMenuItemConfiguration4(wxCommandEvent& event)
{
    save_current();
    currentConfiguration = 3;
    load_current();
    refresh_gui();
}

void sixaxis_emu_guiFrame::OnMenuItemExpert(wxCommandEvent& event)
{
    if(MenuItem18->IsChecked())
    {
        expertMode = true;
    }
    else
    {
        expertMode = false;
    }

    Choice6->Clear();
    Choice17->Clear();
    Choice14->Clear();
    Choice8->Clear();
    Choice4->Clear();
    Choice5->Clear();

    fillKeyboardButtonChoice(Choice10);
    fillKeyboardButtonChoice(Choice11);
    fillKeyboardButtonChoice(Choice18);
    fillDeviceIdButtonChoice(Choice16);
    fillDeviceTypeButtonChoice(Choice2);
    fillDeviceTypeButtonChoice(Choice15);
    fillDeviceIdButtonChoice(Choice13);
    fillDeviceTypeButtonChoice(Choice3);
    fillDeviceIdButtonChoice(Choice9);
    fillDeviceIdButtonChoice(Choice12);

    if(expertMode)
    {
        Choice6->SetSelection( Choice6->Append(_("mouse")) );
        Choice6->Append(_("joystick"));
        Choice6->Append(_("keyboard"));

        Choice17->SetSelection( Choice17->Append(_("button")) );
        Choice17->Append(_("button down"));
        Choice17->Append(_("button up"));

        Choice14->SetSelection( Choice14->Append(_("x")) );
        Choice14->Append(_("y"));

        Choice8->SetSelection( Choice8->Append(_("rstick x")) );
        Choice8->Append(_("rstick y"));
        Choice8->Append(_("lstick x"));
        Choice8->Append(_("lstick y"));
        Choice8->Append(_("up"));
        Choice8->Append(_("down"));
        Choice8->Append(_("right"));
        Choice8->Append(_("left"));
        Choice8->Append(_("r1"));
        Choice8->Append(_("r2"));
        Choice8->Append(_("l1"));
        Choice8->Append(_("l2"));
        Choice8->Append(_("circle"));
        Choice8->Append(_("square"));
        Choice8->Append(_("cross"));
        Choice8->Append(_("triangle"));

        Choice4->SetSelection( Choice4->Append(_("button")) );

        Choice5->SetSelection( Choice5->Append(_("up")) );
        Choice5->Append(_("down"));
        Choice5->Append(_("right"));
        Choice5->Append(_("left"));
        Choice5->Append(_("r1"));
        Choice5->Append(_("r2"));
        Choice5->Append(_("r3"));
        Choice5->Append(_("l1"));
        Choice5->Append(_("l2"));
        Choice5->Append(_("l3"));
        Choice5->Append(_("circle"));
        Choice5->Append(_("square"));
        Choice5->Append(_("cross"));
        Choice5->Append(_("triangle"));
        Choice5->Append(_("start"));
        Choice5->Append(_("select"));
        Choice5->Append(_("PS"));
    }

    OnChoice6Select1(event);

    refresh_gui();
}
