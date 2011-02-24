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
#include <stdio.h>
 #include <sys/types.h>
 #include <pwd.h>

//(*InternalHeaders(sixaxis_emu_guiFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

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
const long sixaxis_emu_guiFrame::ID_STATICTEXT35 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT27 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT36 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT37 = wxNewId();
const long sixaxis_emu_guiFrame::ID_BUTTON1 = wxNewId();
const long sixaxis_emu_guiFrame::ID_PANEL1 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT4 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT29 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT10 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT5 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT6 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT8 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT20 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT7 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT38 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT30 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT39 = wxNewId();
const long sixaxis_emu_guiFrame::ID_CHOICE4 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT40 = wxNewId();
const long sixaxis_emu_guiFrame::ID_TEXTCTRL3 = wxNewId();
const long sixaxis_emu_guiFrame::ID_BUTTON8 = wxNewId();
const long sixaxis_emu_guiFrame::ID_CHOICE5 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICLINE1 = wxNewId();
const long sixaxis_emu_guiFrame::ID_GRID1 = wxNewId();
const long sixaxis_emu_guiFrame::ID_BUTTON4 = wxNewId();
const long sixaxis_emu_guiFrame::ID_BUTTON6 = wxNewId();
const long sixaxis_emu_guiFrame::ID_PANEL2 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT11 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT31 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT12 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT13 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT14 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT21 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT16 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT15 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT17 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT18 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT41 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT32 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT42 = wxNewId();
const long sixaxis_emu_guiFrame::ID_CHOICE7 = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATICTEXT43 = wxNewId();
const long sixaxis_emu_guiFrame::ID_BUTTON9 = wxNewId();
const long sixaxis_emu_guiFrame::ID_CHOICE8 = wxNewId();
const long sixaxis_emu_guiFrame::ID_TEXTCTRL8 = wxNewId();
const long sixaxis_emu_guiFrame::ID_TEXTCTRL9 = wxNewId();
const long sixaxis_emu_guiFrame::ID_TEXTCTRL10 = wxNewId();
const long sixaxis_emu_guiFrame::ID_GRID2 = wxNewId();
const long sixaxis_emu_guiFrame::ID_BUTTON3 = wxNewId();
const long sixaxis_emu_guiFrame::ID_BUTTON7 = wxNewId();
const long sixaxis_emu_guiFrame::ID_PANEL3 = wxNewId();
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
const long sixaxis_emu_guiFrame::idMenuAbout = wxNewId();
const long sixaxis_emu_guiFrame::ID_STATUSBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(sixaxis_emu_guiFrame,wxFrame)
    //(*EventTable(sixaxis_emu_guiFrame)
    //*)
END_EVENT_TABLE()


void sixaxis_emu_guiFrame::fillButtonAxisChoice(wxChoice* choice)
{
    choice->Clear();
    choice->SetSelection(choice->Append(_("")));
    choice->Append(_("rstick left"));
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
    choice->SetSelection(choice->Append(_("")));
    choice->Append(_("rstick x"));
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

char* username;

sixaxis_emu_guiFrame::sixaxis_emu_guiFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(sixaxis_emu_guiFrame)
    wxFlexGridSizer* FlexGridSizer4;
    wxMenuItem* MenuItem2;
    wxFlexGridSizer* FlexGridSizer10;
    wxFlexGridSizer* FlexGridSizer3;
    wxMenuItem* MenuItem1;
    wxFlexGridSizer* FlexGridSizer5;
    wxFlexGridSizer* FlexGridSizer9;
    wxFlexGridSizer* FlexGridSizer7;
    wxGridSizer* GridSizer1;
    wxFlexGridSizer* FlexGridSizer8;
    wxMenuItem* MenuItem6;
    wxFlexGridSizer* FlexGridSizer13;
    wxMenuBar* MenuBar1;
    wxFlexGridSizer* FlexGridSizer6;
    wxFlexGridSizer* FlexGridSizer1;
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
    StaticText35 = new wxStaticText(Panel1, ID_STATICTEXT35, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT35"));
    FlexGridSizer13->Add(StaticText35, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText27 = new wxStaticText(Panel1, ID_STATICTEXT27, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT27"));
    FlexGridSizer13->Add(StaticText27, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText36 = new wxStaticText(Panel1, ID_STATICTEXT36, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT36"));
    FlexGridSizer13->Add(StaticText36, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText37 = new wxStaticText(Panel1, ID_STATICTEXT37, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT37"));
    FlexGridSizer13->Add(StaticText37, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
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
    StaticText20 = new wxStaticText(Panel2, ID_STATICTEXT20, wxEmptyString, wxDefaultPosition, wxSize(31,21), 0, _T("ID_STATICTEXT20"));
    FlexGridSizer1->Add(StaticText20, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText7 = new wxStaticText(Panel2, ID_STATICTEXT7, _("Button"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
    FlexGridSizer1->Add(StaticText7, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText38 = new wxStaticText(Panel2, ID_STATICTEXT38, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT38"));
    FlexGridSizer1->Add(StaticText38, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText30 = new wxStaticText(Panel2, ID_STATICTEXT30, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT30"));
    FlexGridSizer1->Add(StaticText30, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText39 = new wxStaticText(Panel2, ID_STATICTEXT39, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT39"));
    FlexGridSizer1->Add(StaticText39, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice4 = new wxChoice(Panel2, ID_CHOICE4, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE4"));
    Choice4->SetSelection( Choice4->Append(_("button")) );
    Choice4->Append(_("axis up"));
    Choice4->Append(_("axis down"));
    FlexGridSizer1->Add(Choice4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText40 = new wxStaticText(Panel2, ID_STATICTEXT40, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT40"));
    FlexGridSizer1->Add(StaticText40, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrl3 = new wxTextCtrl(Panel2, ID_TEXTCTRL3, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL3"));
    TextCtrl3->Disable();
    FlexGridSizer1->Add(TextCtrl3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button8 = new wxButton(Panel2, ID_BUTTON8, _("Auto detect"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON8"));
    FlexGridSizer1->Add(Button8, 1, wxALL|wxALIGN_BOTTOM|wxALIGN_CENTER_HORIZONTAL, 5);
    Choice5 = new wxChoice(Panel2, ID_CHOICE5, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE5"));
    FlexGridSizer1->Add(Choice5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticLine1 = new wxStaticLine(Panel2, ID_STATICLINE1, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("ID_STATICLINE1"));
    FlexGridSizer1->Add(StaticLine1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
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
    StaticText21 = new wxStaticText(Panel3, ID_STATICTEXT21, wxEmptyString, wxDefaultPosition, wxSize(30,17), 0, _T("ID_STATICTEXT21"));
    FlexGridSizer3->Add(StaticText21, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText16 = new wxStaticText(Panel3, ID_STATICTEXT16, _("Axis"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT16"));
    FlexGridSizer3->Add(StaticText16, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText15 = new wxStaticText(Panel3, ID_STATICTEXT15, _("Dead zone"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT15"));
    FlexGridSizer3->Add(StaticText15, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText17 = new wxStaticText(Panel3, ID_STATICTEXT17, _("Multiplier"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT17"));
    FlexGridSizer3->Add(StaticText17, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText18 = new wxStaticText(Panel3, ID_STATICTEXT18, _("Exponent"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT18"));
    FlexGridSizer3->Add(StaticText18, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText41 = new wxStaticText(Panel3, ID_STATICTEXT41, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT41"));
    FlexGridSizer3->Add(StaticText41, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText32 = new wxStaticText(Panel3, ID_STATICTEXT32, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT32"));
    FlexGridSizer3->Add(StaticText32, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText42 = new wxStaticText(Panel3, ID_STATICTEXT42, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT42"));
    FlexGridSizer3->Add(StaticText42, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice7 = new wxChoice(Panel3, ID_CHOICE7, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE7"));
    Choice7->SetSelection( Choice7->Append(_("axis")) );
    Choice7->Append(_("button"));
    FlexGridSizer3->Add(Choice7, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText43 = new wxStaticText(Panel3, ID_STATICTEXT43, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT43"));
    FlexGridSizer3->Add(StaticText43, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button9 = new wxButton(Panel3, ID_BUTTON9, _("Auto detect"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON9"));
    FlexGridSizer3->Add(Button9, 1, wxALL|wxALIGN_BOTTOM|wxALIGN_CENTER_HORIZONTAL, 5);
    Choice8 = new wxChoice(Panel3, ID_CHOICE8, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE8"));
    FlexGridSizer3->Add(Choice8, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrl8 = new wxTextCtrl(Panel3, ID_TEXTCTRL8, _("8"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL8"));
    FlexGridSizer3->Add(TextCtrl8, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrl9 = new wxTextCtrl(Panel3, ID_TEXTCTRL9, _("4"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL9"));
    FlexGridSizer3->Add(TextCtrl9, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrl10 = new wxTextCtrl(Panel3, ID_TEXTCTRL10, _("1"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL10"));
    FlexGridSizer3->Add(TextCtrl10, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
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
    Notebook1->AddPage(Panel1, _("Configuration trigger"), false);
    Notebook1->AddPage(Panel2, _("Button configuration"), false);
    Notebook1->AddPage(Panel3, _("Axis configuration"), false);
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
    MenuItem4->Enable(false);
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
    
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnButton1Click1);
    Connect(ID_CHOICE4,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnChoice4Select1);
    Connect(ID_BUTTON8,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnButton8Click);
    Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnButtonAdd1Click);
    Connect(ID_BUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnButton6Click);
    Connect(ID_CHOICE7,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnChoice4Select);
    Connect(ID_BUTTON9,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnButton9Click);
    Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnButton3Click);
    Connect(ID_BUTTON7,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnButton7Click);
    Connect(idMenuNew,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnMenuItemNew);
    Connect(idMenuOpen,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnMenuOpen);
    Connect(idMenuSave,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnMenuSave);
    Connect(idMenuSaveAs,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnMenuSaveAs);
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
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&sixaxis_emu_guiFrame::OnAbout);
    //*)

    currentController = 0;
    currentConfiguration = 0;

    username = getpwuid(getuid())->pw_name;

    wxString default_directory = _("/home/");
    wxString user(username, wxConvUTF8);
    default_directory << user << _("/.emuclient/config");

    FileDialog1->SetDirectory(default_directory);

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
    configFile = ConfigurationFile();

    currentController = 0;
    currentConfiguration = 0;
    Menu3->Check(ID_MENUITEM1, true);
    Menu4->Check(ID_MENUITEM8, true);
    load_current();
    refresh_gui();
    Menu1->Enable(idMenuSave, false);
}

void sixaxis_emu_guiFrame::OnButtonAdd1Click(wxCommandEvent& event)
{
    if(Choice5->GetStringSelection() == _(""))
    {
        wxMessageBox( wxT("Please select a Button!"), wxT("Error"), wxICON_ERROR);
        return;
    }
    Grid1->InsertRows();
    Grid1->SetCellValue(0, 0, StaticText38->GetLabel());
    Grid1->SetCellValue(0, 1, StaticText30->GetLabel());
    Grid1->SetCellValue(0, 2, StaticText39->GetLabel());
    Grid1->SetCellValue(0, 3, Choice4->GetStringSelection());
    Grid1->SetCellValue(0, 4, StaticText40->GetLabel());
    Grid1->SetCellValue(0, 5, TextCtrl3->GetValue());
    Grid1->SetCellValue(0, 6, Choice5->GetStringSelection());
    Grid1->AutoSizeColumns();
    Panel2->Layout();
}

void sixaxis_emu_guiFrame::OnButton3Click(wxCommandEvent& event)
{
    if(Choice8->GetStringSelection() == _(""))
    {
        wxMessageBox( wxT("Please select an Axis!"), wxT("Error"), wxICON_ERROR);
        return;
    }
    Grid2->InsertRows();
    Grid2->SetCellValue(0, 0, StaticText41->GetLabel());
    Grid2->SetCellValue(0, 1, StaticText32->GetLabel());
    Grid2->SetCellValue(0, 2, StaticText42->GetLabel());
    Grid2->SetCellValue(0, 3, Choice7->GetStringSelection());
    Grid2->SetCellValue(0, 4, StaticText43->GetLabel());
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
    StaticText41->SetLabel(_(""));
    StaticText32->SetLabel(_(""));
    StaticText42->SetLabel(_(""));
    StaticText43->SetLabel(_(""));
    Choice8->Clear();

    if(Choice7->GetStringSelection() == _("button"))
    {
        TextCtrl8->Disable();
        TextCtrl8->SetValue(_(""));
        TextCtrl9->Disable();
        TextCtrl9->SetValue(_(""));
        TextCtrl10->Disable();
        TextCtrl10->SetValue(_(""));
    }
    else
    {
        TextCtrl8->Enable();
        TextCtrl8->SetValue(_("8"));
        TextCtrl9->Enable();
        TextCtrl9->SetValue(_("4"));
        TextCtrl10->Enable();
        TextCtrl10->SetValue(_("1"));
    }
    Panel3->Layout();
}

void sixaxis_emu_guiFrame::OnChoice4Select1(wxCommandEvent& event)
{
    StaticText38->SetLabel(_(""));
    StaticText30->SetLabel(_(""));
    StaticText39->SetLabel(_(""));
    StaticText40->SetLabel(_(""));
    Choice5->Clear();

    if(Choice4->GetStringSelection() == _("button"))
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

void sixaxis_emu_guiFrame::auto_detect(event_catcher* evcatch, wxStaticText* device_type, wxStaticText* device_name, wxStaticText* device_id, wxString event_type, wxStaticText* event_id)
{
    evcatch->run(event_type);

    device_type->SetLabel(wxString(evcatch->GetDeviceType()));

    device_name->SetLabel(wxString(evcatch->GetDeviceName()));

    device_id->SetLabel( wxString(evcatch->GetDeviceId()));

    event_id->SetLabel( wxString(evcatch->GetEventId()));
}

void sixaxis_emu_guiFrame::OnButton1Click1(wxCommandEvent& event)
{
    auto_detect(&evcatch, StaticText35, StaticText27, StaticText36, _("button"), StaticText37);

    Panel1->Layout();
}

void sixaxis_emu_guiFrame::OnButton8Click(wxCommandEvent& event)
{
    auto_detect(&evcatch, StaticText38, StaticText30, StaticText39, Choice4->GetStringSelection(), StaticText40);

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

    Choice5->Clear();
    Choice5->SetSelection(Choice5->Append(_("")));
    Choice5->Append(_("up"));
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

    Panel2->Layout();
}

void sixaxis_emu_guiFrame::OnButton9Click(wxCommandEvent& event)
{
    auto_detect(&evcatch, StaticText41, StaticText32, StaticText42, Choice7->GetStringSelection(), StaticText43);

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

void sixaxis_emu_guiFrame::save_current()
{
    std::list<ButtonMapper>* buttonMappers;
    std::list<AxisMapper>* axisMappers;
    //Save Trigger
    configFile.GetController(currentController)->GetConfiguration(currentConfiguration)->GetTrigger()->GetDevice()->SetType(StaticText35->GetLabel());
    configFile.GetController(currentController)->GetConfiguration(currentConfiguration)->GetTrigger()->GetDevice()->SetName(StaticText27->GetLabel());
    configFile.GetController(currentController)->GetConfiguration(currentConfiguration)->GetTrigger()->GetDevice()->SetId(StaticText36->GetLabel());
    configFile.GetController(currentController)->GetConfiguration(currentConfiguration)->GetTrigger()->GetEvent()->SetId(StaticText37->GetLabel());
    //Save ButtonMappers
    buttonMappers = configFile.GetController(currentController)->GetConfiguration(currentConfiguration)->GetButtonMapperList();
    for(std::list<ButtonMapper>::iterator it = buttonMappers->begin(); it!=buttonMappers->end(); it = buttonMappers->erase(it)) {}
    for(int i=0; i<Grid1->GetNumberRows(); i++)
    {
        //ButtonMapper(wxString dtype, wxString did, wxString dname, wxString etype, wxString eid, wxString threshold, wxString button)
        buttonMappers->push_front(ButtonMapper(Grid1->GetCellValue(i, 0), Grid1->GetCellValue(i, 2), Grid1->GetCellValue(i, 1), Grid1->GetCellValue(i, 3), Grid1->GetCellValue(i, 4), Grid1->GetCellValue(i, 5), Grid1->GetCellValue(i, 6)));
    }
    //Save AxisMappers
    axisMappers = configFile.GetController(currentController)->GetConfiguration(currentConfiguration)->GetAxisMapperList();
    for(std::list<AxisMapper>::iterator it = axisMappers->begin(); it!=axisMappers->end(); it = axisMappers->erase(it)) {}
    for(int i=0; i<Grid2->GetNumberRows(); i++)
    {
        //AxisMapper(wxString dtype, wxString did, wxString dname, wxString etype, wxString eid, wxString axis, wxString deadZone, wxString multiplier, wxString exponent);
        axisMappers->push_front(AxisMapper(Grid2->GetCellValue(i, 0), Grid2->GetCellValue(i, 2), Grid2->GetCellValue(i, 1), Grid2->GetCellValue(i, 3), Grid2->GetCellValue(i, 4), Grid2->GetCellValue(i, 5), Grid2->GetCellValue(i, 6), Grid2->GetCellValue(i, 7), Grid2->GetCellValue(i, 8)));
    }

}

void sixaxis_emu_guiFrame::load_current()
{
    std::list<ButtonMapper>* buttonMappers;
    std::list<AxisMapper>* axisMappers;
    //Load Trigger
    StaticText35->SetLabel(configFile.GetController(currentController)->GetConfiguration(currentConfiguration)->GetTrigger()->GetDevice()->GetType());
    StaticText27->SetLabel(configFile.GetController(currentController)->GetConfiguration(currentConfiguration)->GetTrigger()->GetDevice()->GetName());
    StaticText36->SetLabel(configFile.GetController(currentController)->GetConfiguration(currentConfiguration)->GetTrigger()->GetDevice()->GetId());
    StaticText37->SetLabel(configFile.GetController(currentController)->GetConfiguration(currentConfiguration)->GetTrigger()->GetEvent()->GetId());
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
        Grid2->SetCellValue(0, 6, it->GetEvent()->GetDeadZone());
        Grid2->SetCellValue(0, 7, it->GetEvent()->GetMultiplier());
        Grid2->SetCellValue(0, 8, it->GetEvent()->GetExponent());
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
    Menu1->Enable(idMenuSave, true);
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

void sixaxis_emu_guiFrame::OnMenuSave(wxCommandEvent& event)
{
    save_current();
    configFile.WriteConfigFile();
}

void sixaxis_emu_guiFrame::OnMenuSaveAs(wxCommandEvent& event)
{
    if ( FileDialog1->ShowModal() != wxID_OK ) return;

    wxString FileName = FileDialog1->GetPath();
    if ( FileName.IsEmpty() ) return;

    configFile.SetFilePath(FileName);

    OnMenuSave(event);
}
