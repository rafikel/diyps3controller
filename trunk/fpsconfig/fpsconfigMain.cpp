/***************************************************************
 * Name:      fpsconfigMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Matlo (mat.lau@laposte.net)
 * Created:   2011-08-06
 * Copyright: Matlo (http://diy-machine.blogspot.com/)
 * License:
 **************************************************************/

#include "wx_pch.h"
#include "fpsconfigMain.h"
#include <wx/msgdlg.h>
#include <ConfigurationFile.h>

#ifndef WIN32
#include <pwd.h>
#endif

//(*InternalHeaders(fpsconfigFrame)
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include "background.png.cpp"
#include <wx/mstream.h>

using namespace std;

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

#ifndef WIN32
char* homedir;
#endif

//(*IdInit(fpsconfigFrame)
const long fpsconfigFrame::ID_SPINCTRL8 = wxNewId();
const long fpsconfigFrame::ID_SPINCTRL7 = wxNewId();
const long fpsconfigFrame::ID_SPINCTRL6 = wxNewId();
const long fpsconfigFrame::ID_SPINCTRL5 = wxNewId();
const long fpsconfigFrame::ID_SPINCTRL4 = wxNewId();
const long fpsconfigFrame::ID_SPINCTRL3 = wxNewId();
const long fpsconfigFrame::ID_BUTTON10 = wxNewId();
const long fpsconfigFrame::ID_SPINCTRL1 = wxNewId();
const long fpsconfigFrame::ID_SPINCTRL2 = wxNewId();
const long fpsconfigFrame::ID_TEXTCTRL24 = wxNewId();
const long fpsconfigFrame::ID_TEXTCTRL23 = wxNewId();
const long fpsconfigFrame::ID_TEXTCTRL26 = wxNewId();
const long fpsconfigFrame::ID_TEXTCTRL22 = wxNewId();
const long fpsconfigFrame::ID_STATICTEXT1 = wxNewId();
const long fpsconfigFrame::ID_STATICTEXT7 = wxNewId();
const long fpsconfigFrame::ID_STATICTEXT2 = wxNewId();
const long fpsconfigFrame::ID_STATICTEXT3 = wxNewId();
const long fpsconfigFrame::ID_STATICTEXT4 = wxNewId();
const long fpsconfigFrame::ID_STATICTEXT5 = wxNewId();
const long fpsconfigFrame::ID_STATICTEXT6 = wxNewId();
const long fpsconfigFrame::ID_BUTTON1 = wxNewId();
const long fpsconfigFrame::ID_BUTTON2 = wxNewId();
const long fpsconfigFrame::ID_BUTTON3 = wxNewId();
const long fpsconfigFrame::ID_BUTTON4 = wxNewId();
const long fpsconfigFrame::ID_BUTTON5 = wxNewId();
const long fpsconfigFrame::ID_BUTTON6 = wxNewId();
const long fpsconfigFrame::ID_BUTTON7 = wxNewId();
const long fpsconfigFrame::ID_BUTTON8 = wxNewId();
const long fpsconfigFrame::ID_BUTTON9 = wxNewId();
const long fpsconfigFrame::ID_BUTTON11 = wxNewId();
const long fpsconfigFrame::ID_BUTTON12 = wxNewId();
const long fpsconfigFrame::ID_BUTTON13 = wxNewId();
const long fpsconfigFrame::ID_BUTTON14 = wxNewId();
const long fpsconfigFrame::ID_BUTTON15 = wxNewId();
const long fpsconfigFrame::ID_BUTTON16 = wxNewId();
const long fpsconfigFrame::ID_BUTTON17 = wxNewId();
const long fpsconfigFrame::ID_BUTTON18 = wxNewId();
const long fpsconfigFrame::ID_BUTTON19 = wxNewId();
const long fpsconfigFrame::ID_BUTTON20 = wxNewId();
const long fpsconfigFrame::ID_BUTTON21 = wxNewId();
const long fpsconfigFrame::ID_CHOICE2 = wxNewId();
const long fpsconfigFrame::ID_CHOICE1 = wxNewId();
const long fpsconfigFrame::ID_TEXTCTRL1 = wxNewId();
const long fpsconfigFrame::ID_TEXTCTRL25 = wxNewId();
const long fpsconfigFrame::ID_PANEL1 = wxNewId();
const long fpsconfigFrame::ID_MENUITEM1 = wxNewId();
const long fpsconfigFrame::ID_MENUITEM4 = wxNewId();
const long fpsconfigFrame::ID_MENUITEM2 = wxNewId();
const long fpsconfigFrame::ID_MENUITEM3 = wxNewId();
const long fpsconfigFrame::idMenuQuit = wxNewId();
const long fpsconfigFrame::idMenuAbout = wxNewId();
const long fpsconfigFrame::ID_STATUSBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(fpsconfigFrame,wxFrame)
    //(*EventTable(fpsconfigFrame)
    //*)
END_EVENT_TABLE()

class wxBackgroundBitmap : public wxEvtHandler {
    typedef wxEvtHandler Inherited;
public:
    wxBackgroundBitmap(const wxBitmap &B) : Bitmap(B), wxEvtHandler() { }
    virtual bool        ProcessEvent(wxEvent &Event);
protected:
    wxBitmap            Bitmap;
};

bool wxBackgroundBitmap::ProcessEvent(wxEvent &Event)
{
    if (Event.GetEventType() == wxEVT_ERASE_BACKGROUND) {
        wxEraseEvent &EraseEvent = dynamic_cast<wxEraseEvent &>(Event);
        wxDC *DC = EraseEvent.GetDC();
        DC->DrawBitmap(Bitmap, 7, 7, false);
        return true;
    } else return Inherited::ProcessEvent(Event);
}

const char* button_labels[BI_MAX] =
{
    "undef",
    "select", "start", "PS",
    "up", "right", "down", "left",
    "triangle", "circle", "cross", "square",
    "l1", "r1",
    "l2", "r2",
    "l3", "r3"
};

const char* axis_labels[AI_MAX] =
{
    "undef",
    "lstick up",
    "lstick right",
    "lstick down",
    "lstick left"
};

fpsconfigFrame::fpsconfigFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(fpsconfigFrame)
    wxMenuItem* MenuItem2;
    wxMenuItem* MenuItem1;
    wxMenu* Menu1;
    wxMenuBar* MenuBar1;
    wxMenu* Menu2;

    Create(parent, wxID_ANY, _("fpsconfig"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    SetClientSize(wxSize(620,410));
    SetBackgroundColour(wxColour(255,255,255));
    Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(0,0), wxSize(620,372), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    Panel1->SetBackgroundColour(wxColour(255,255,255));
    SpinCtrl8 = new wxSpinCtrl(Panel1, ID_SPINCTRL8, _T("100"), wxPoint(592,352), wxSize(19,-1), 0, -10000, 10000, 100, _T("ID_SPINCTRL8"));
    SpinCtrl8->SetValue(_T("100"));
    SpinCtrl7 = new wxSpinCtrl(Panel1, ID_SPINCTRL7, _T("100"), wxPoint(592,320), wxSize(19,-1), 0, -10000, 10000, 100, _T("ID_SPINCTRL7"));
    SpinCtrl7->SetValue(_T("100"));
    SpinCtrl6 = new wxSpinCtrl(Panel1, ID_SPINCTRL6, _T("100"), wxPoint(496,352), wxSize(19,-1), 0, -10000, 10000, 100, _T("ID_SPINCTRL6"));
    SpinCtrl6->SetValue(_T("100"));
    SpinCtrl5 = new wxSpinCtrl(Panel1, ID_SPINCTRL5, _T("100"), wxPoint(496,320), wxSize(19,-1), 0, -10000, 10000, 100, _T("ID_SPINCTRL5"));
    SpinCtrl5->SetValue(_T("100"));
    SpinCtrl4 = new wxSpinCtrl(Panel1, ID_SPINCTRL4, _T("100"), wxPoint(400,352), wxSize(19,-1), 0, -10000, 10000, 100, _T("ID_SPINCTRL4"));
    SpinCtrl4->SetValue(_T("100"));
    SpinCtrl3 = new wxSpinCtrl(Panel1, ID_SPINCTRL3, _T("100"), wxPoint(400,320), wxSize(19,-1), 0, -10000, 10000, 100, _T("ID_SPINCTRL3"));
    SpinCtrl3->SetValue(_T("100"));
    stickright = new wxButton(Panel1, ID_BUTTON10, wxEmptyString, wxPoint(256,222), wxSize(50,-1), 0, wxDefaultValidator, _T("ID_BUTTON10"));
    wxFont stickrightFont(6,wxDEFAULT,wxFONTSTYLE_NORMAL,wxNORMAL,false,wxEmptyString,wxFONTENCODING_DEFAULT);
    stickright->SetFont(stickrightFont);
    SpinCtrl1 = new wxSpinCtrl(Panel1, ID_SPINCTRL1, _T("20"), wxPoint(184,320), wxSize(50,-1), 0, 0, 64, 20, _T("ID_SPINCTRL1"));
    SpinCtrl1->SetValue(_T("20"));
    SpinCtrl2 = new wxSpinCtrl(Panel1, ID_SPINCTRL2, _T("20"), wxPoint(184,352), wxSize(50,-1), 0, 0, 64, 20, _T("ID_SPINCTRL2"));
    SpinCtrl2->SetValue(_T("20"));
    TextCtrl24 = new wxTextCtrl(Panel1, ID_TEXTCTRL24, _("1.00"), wxPoint(328,352), wxSize(73,-1), 0, wxDefaultValidator, _T("ID_TEXTCTRL24"));
    TextCtrl23 = new wxTextCtrl(Panel1, ID_TEXTCTRL23, _("1.00"), wxPoint(328,320), wxSize(73,-1), 0, wxDefaultValidator, _T("ID_TEXTCTRL23"));
    TextCtrl26 = new wxTextCtrl(Panel1, ID_TEXTCTRL26, _("1.00"), wxPoint(424,352), wxSize(73,-1), 0, wxDefaultValidator, _T("ID_TEXTCTRL26"));
    TextCtrl22 = new wxTextCtrl(Panel1, ID_TEXTCTRL22, _("1.00"), wxPoint(424,320), wxSize(73,-1), 0, wxDefaultValidator, _T("ID_TEXTCTRL22"));
    StaticText1 = new wxStaticText(Panel1, ID_STATICTEXT1, _("primary"), wxPoint(120,328), wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    StaticText7 = new wxStaticText(Panel1, ID_STATICTEXT7, _("dz shape"), wxPoint(256,296), wxDefaultSize, 0, _T("ID_STATICTEXT7"));
    StaticText2 = new wxStaticText(Panel1, ID_STATICTEXT2, _("secondary"), wxPoint(104,360), wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    StaticText3 = new wxStaticText(Panel1, ID_STATICTEXT3, _("dz value"), wxPoint(176,296), wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    StaticText4 = new wxStaticText(Panel1, ID_STATICTEXT4, _("sensitivity"), wxPoint(336,296), wxDefaultSize, 0, _T("ID_STATICTEXT4"));
    StaticText5 = new wxStaticText(Panel1, ID_STATICTEXT5, _("acceleration"), wxPoint(424,296), wxDefaultSize, 0, _T("ID_STATICTEXT5"));
    StaticText6 = new wxStaticText(Panel1, ID_STATICTEXT6, _("x/y ratio"), wxPoint(536,296), wxDefaultSize, 0, _T("ID_STATICTEXT6"));
    l2 = new wxButton(Panel1, ID_BUTTON1, wxEmptyString, wxPoint(136,0), wxSize(50,-1), 0, wxDefaultValidator, _T("ID_BUTTON1"));
    wxFont l2Font(6,wxDEFAULT,wxFONTSTYLE_NORMAL,wxNORMAL,false,wxEmptyString,wxFONTENCODING_DEFAULT);
    l2->SetFont(l2Font);
    l1 = new wxButton(Panel1, ID_BUTTON2, wxEmptyString, wxPoint(136,32), wxSize(50,-1), 0, wxDefaultValidator, _T("ID_BUTTON2"));
    wxFont l1Font(6,wxDEFAULT,wxFONTSTYLE_NORMAL,wxNORMAL,false,wxEmptyString,wxFONTENCODING_DEFAULT);
    l1->SetFont(l1Font);
    up = new wxButton(Panel1, ID_BUTTON3, wxEmptyString, wxPoint(104,96), wxSize(50,-1), 0, wxDefaultValidator, _T("ID_BUTTON3"));
    wxFont upFont(6,wxDEFAULT,wxFONTSTYLE_NORMAL,wxNORMAL,false,wxEmptyString,wxFONTENCODING_DEFAULT);
    up->SetFont(upFont);
    left = new wxButton(Panel1, ID_BUTTON4, wxEmptyString, wxPoint(80,128), wxSize(50,-1), 0, wxDefaultValidator, _T("ID_BUTTON4"));
    wxFont leftFont(6,wxDEFAULT,wxFONTSTYLE_NORMAL,wxNORMAL,false,wxEmptyString,wxFONTENCODING_DEFAULT);
    left->SetFont(leftFont);
    right = new wxButton(Panel1, ID_BUTTON5, wxEmptyString, wxPoint(128,128), wxSize(50,-1), 0, wxDefaultValidator, _T("ID_BUTTON5"));
    wxFont rightFont(6,wxDEFAULT,wxFONTSTYLE_NORMAL,wxNORMAL,false,wxEmptyString,wxFONTENCODING_DEFAULT);
    right->SetFont(rightFont);
    down = new wxButton(Panel1, ID_BUTTON6, wxEmptyString, wxPoint(104,160), wxSize(50,-1), 0, wxDefaultValidator, _T("ID_BUTTON6"));
    wxFont downFont(6,wxDEFAULT,wxFONTSTYLE_NORMAL,wxNORMAL,false,wxEmptyString,wxFONTENCODING_DEFAULT);
    down->SetFont(downFont);
    l3 = new wxButton(Panel1, ID_BUTTON7, wxEmptyString, wxPoint(194,222), wxSize(50,-1), 0, wxDefaultValidator, _T("ID_BUTTON7"));
    wxFont l3Font(6,wxDEFAULT,wxFONTSTYLE_NORMAL,wxNORMAL,false,wxEmptyString,wxFONTENCODING_DEFAULT);
    l3->SetFont(l3Font);
    stickup = new wxButton(Panel1, ID_BUTTON8, wxEmptyString, wxPoint(194,176), wxSize(50,-1), 0, wxDefaultValidator, _T("ID_BUTTON8"));
    wxFont stickupFont(6,wxDEFAULT,wxFONTSTYLE_NORMAL,wxNORMAL,false,wxEmptyString,wxFONTENCODING_DEFAULT);
    stickup->SetFont(stickupFont);
    stickleft = new wxButton(Panel1, ID_BUTTON9, wxEmptyString, wxPoint(130,222), wxSize(50,-1), 0, wxDefaultValidator, _T("ID_BUTTON9"));
    wxFont stickleftFont(6,wxDEFAULT,wxFONTSTYLE_NORMAL,wxNORMAL,false,wxEmptyString,wxFONTENCODING_DEFAULT);
    stickleft->SetFont(stickleftFont);
    stickdown = new wxButton(Panel1, ID_BUTTON11, wxEmptyString, wxPoint(194,264), wxSize(50,-1), 0, wxDefaultValidator, _T("ID_BUTTON11"));
    wxFont stickdownFont(6,wxDEFAULT,wxFONTSTYLE_NORMAL,wxNORMAL,false,wxEmptyString,wxFONTENCODING_DEFAULT);
    stickdown->SetFont(stickdownFont);
    select = new wxButton(Panel1, ID_BUTTON12, wxEmptyString, wxPoint(256,144), wxSize(50,-1), 0, wxDefaultValidator, _T("ID_BUTTON12"));
    wxFont selectFont(6,wxDEFAULT,wxFONTSTYLE_NORMAL,wxNORMAL,false,wxEmptyString,wxFONTENCODING_DEFAULT);
    select->SetFont(selectFont);
    start = new wxButton(Panel1, ID_BUTTON13, wxEmptyString, wxPoint(312,144), wxSize(50,-1), 0, wxDefaultValidator, _T("ID_BUTTON13"));
    wxFont startFont(6,wxDEFAULT,wxFONTSTYLE_NORMAL,wxNORMAL,false,wxEmptyString,wxFONTENCODING_DEFAULT);
    start->SetFont(startFont);
    ps = new wxButton(Panel1, ID_BUTTON14, wxEmptyString, wxPoint(284,176), wxSize(50,-1), 0, wxDefaultValidator, _T("ID_BUTTON14"));
    wxFont psFont(6,wxDEFAULT,wxFONTSTYLE_NORMAL,wxNORMAL,false,wxEmptyString,wxFONTENCODING_DEFAULT);
    ps->SetFont(psFont);
    r2 = new wxButton(Panel1, ID_BUTTON15, wxEmptyString, wxPoint(424,0), wxSize(50,-1), 0, wxDefaultValidator, _T("ID_BUTTON15"));
    wxFont r2Font(6,wxDEFAULT,wxFONTSTYLE_NORMAL,wxNORMAL,false,wxEmptyString,wxFONTENCODING_DEFAULT);
    r2->SetFont(r2Font);
    r1 = new wxButton(Panel1, ID_BUTTON16, wxEmptyString, wxPoint(424,32), wxSize(50,-1), 0, wxDefaultValidator, _T("ID_BUTTON16"));
    wxFont r1Font(6,wxDEFAULT,wxFONTSTYLE_NORMAL,wxNORMAL,false,wxEmptyString,wxFONTENCODING_DEFAULT);
    r1->SetFont(r1Font);
    triangle = new wxButton(Panel1, ID_BUTTON17, wxEmptyString, wxPoint(488,56), wxSize(50,-1), 0, wxDefaultValidator, _T("ID_BUTTON17"));
    wxFont triangleFont(6,wxDEFAULT,wxFONTSTYLE_NORMAL,wxNORMAL,false,wxEmptyString,wxFONTENCODING_DEFAULT);
    triangle->SetFont(triangleFont);
    square = new wxButton(Panel1, ID_BUTTON18, wxEmptyString, wxPoint(384,104), wxSize(50,-1), 0, wxDefaultValidator, _T("ID_BUTTON18"));
    wxFont squareFont(6,wxDEFAULT,wxFONTSTYLE_NORMAL,wxNORMAL,false,wxEmptyString,wxFONTENCODING_DEFAULT);
    square->SetFont(squareFont);
    circle = new wxButton(Panel1, ID_BUTTON19, wxEmptyString, wxPoint(544,112), wxSize(50,-1), 0, wxDefaultValidator, _T("ID_BUTTON19"));
    wxFont circleFont(6,wxDEFAULT,wxFONTSTYLE_NORMAL,wxNORMAL,false,wxEmptyString,wxFONTENCODING_DEFAULT);
    circle->SetFont(circleFont);
    cross = new wxButton(Panel1, ID_BUTTON20, wxEmptyString, wxPoint(496,192), wxSize(50,-1), 0, wxDefaultValidator, _T("ID_BUTTON20"));
    wxFont crossFont(6,wxDEFAULT,wxFONTSTYLE_NORMAL,wxNORMAL,false,wxEmptyString,wxFONTENCODING_DEFAULT);
    cross->SetFont(crossFont);
    r3 = new wxButton(Panel1, ID_BUTTON21, wxEmptyString, wxPoint(368,222), wxSize(50,-1), 0, wxDefaultValidator, _T("ID_BUTTON21"));
    wxFont r3Font(6,wxDEFAULT,wxFONTSTYLE_NORMAL,wxNORMAL,false,wxEmptyString,wxFONTENCODING_DEFAULT);
    r3->SetFont(r3Font);
    Choice2 = new wxChoice(Panel1, ID_CHOICE2, wxPoint(240,320), wxSize(80,-1), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE2"));
    Choice2->SetSelection( Choice2->Append(_("Circle")) );
    Choice2->Append(_("Rectangle"));
    Choice1 = new wxChoice(Panel1, ID_CHOICE1, wxPoint(240,352), wxSize(80,-1), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
    Choice1->SetSelection( Choice1->Append(_("Circle")) );
    Choice1->Append(_("Rectangle"));
    TextCtrl1 = new wxTextCtrl(Panel1, ID_TEXTCTRL1, _("1.00"), wxPoint(520,320), wxSize(73,-1), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    TextCtrl25 = new wxTextCtrl(Panel1, ID_TEXTCTRL25, _("1.00"), wxPoint(520,352), wxSize(73,-1), 0, wxDefaultValidator, _T("ID_TEXTCTRL25"));
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem3 = new wxMenuItem(Menu1, ID_MENUITEM1, _("New"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuItem3);
    MenuItem6 = new wxMenuItem(Menu1, ID_MENUITEM4, _("Open"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuItem6);
    MenuItem4 = new wxMenuItem(Menu1, ID_MENUITEM2, _("Save"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuItem4);
    MenuItem4->Enable(false);
    MenuItem5 = new wxMenuItem(Menu1, ID_MENUITEM3, _("Save As"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuItem5);
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
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

    Connect(ID_SPINCTRL8,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&fpsconfigFrame::OnSpinCtrlChange);
    Connect(ID_SPINCTRL7,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&fpsconfigFrame::OnSpinCtrlChange);
    Connect(ID_SPINCTRL6,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&fpsconfigFrame::OnSpinCtrlChange);
    Connect(ID_SPINCTRL5,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&fpsconfigFrame::OnSpinCtrlChange);
    Connect(ID_SPINCTRL4,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&fpsconfigFrame::OnSpinCtrlChange);
    Connect(ID_SPINCTRL3,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&fpsconfigFrame::OnSpinCtrlChange);
    Connect(ID_BUTTON10,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&fpsconfigFrame::OnButtonClick);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&fpsconfigFrame::OnButtonClick);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&fpsconfigFrame::OnButtonClick);
    Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&fpsconfigFrame::OnButtonClick);
    Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&fpsconfigFrame::OnButtonClick);
    Connect(ID_BUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&fpsconfigFrame::OnButtonClick);
    Connect(ID_BUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&fpsconfigFrame::OnButtonClick);
    Connect(ID_BUTTON7,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&fpsconfigFrame::OnButtonClick);
    Connect(ID_BUTTON8,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&fpsconfigFrame::OnButtonClick);
    Connect(ID_BUTTON9,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&fpsconfigFrame::OnButtonClick);
    Connect(ID_BUTTON11,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&fpsconfigFrame::OnButtonClick);
    Connect(ID_BUTTON12,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&fpsconfigFrame::OnButtonClick);
    Connect(ID_BUTTON13,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&fpsconfigFrame::OnButtonClick);
    Connect(ID_BUTTON14,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&fpsconfigFrame::OnButtonClick);
    Connect(ID_BUTTON15,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&fpsconfigFrame::OnButtonClick);
    Connect(ID_BUTTON16,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&fpsconfigFrame::OnButtonClick);
    Connect(ID_BUTTON17,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&fpsconfigFrame::OnButtonClick);
    Connect(ID_BUTTON18,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&fpsconfigFrame::OnButtonClick);
    Connect(ID_BUTTON19,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&fpsconfigFrame::OnButtonClick);
    Connect(ID_BUTTON20,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&fpsconfigFrame::OnButtonClick);
    Connect(ID_BUTTON21,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&fpsconfigFrame::OnButtonClick);
    Connect(ID_MENUITEM1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&fpsconfigFrame::OnMenuNew);
    Connect(ID_MENUITEM4,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&fpsconfigFrame::OnMenuOpen);
    Connect(ID_MENUITEM2,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&fpsconfigFrame::OnMenuSave);
    Connect(ID_MENUITEM3,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&fpsconfigFrame::OnMenuSaveAs);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&fpsconfigFrame::OnQuit);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&fpsconfigFrame::OnAbout);
    //*)

    wxMemoryInputStream istream(background_png, sizeof background_png);
    wxImage background_img(istream, wxBITMAP_TYPE_PNG);
    wxBackgroundBitmap* ToolBarBackground = new wxBackgroundBitmap(wxBitmap(background_img));
    Panel1->PushEventHandler(ToolBarBackground);

#ifndef WIN32
    if(!getuid())
    {
    	int answer = wxMessageBox(_("It's not recommended to run as root user. Continue?"), _("Confirm"), wxYES_NO);
		if (answer == wxNO)
		{
			exit(0);
		}
    }

    homedir = getpwuid(getuid())->pw_dir;

    string cmd = "";
    cmd.append("test -d ");
    cmd.append(homedir);
    cmd.append("/.emuclient || cp -r /etc/emuclient ");
    cmd.append(homedir);
    cmd.append("/.emuclient");
    if(system(cmd.c_str()) < 0)
    {
        wxMessageBox( wxT("Cannot open emuclient config directory!"), wxT("Error"), wxICON_ERROR);
    }

    cmd.erase();
    cmd.append(homedir);
    cmd.append("/.emuclient/config");

#else
    string cmd = "config";
#endif

    wxString default_directory = wxString(cmd.c_str(), wxConvUTF8);

    FileDialog1->SetDirectory(default_directory);
}

fpsconfigFrame::~fpsconfigFrame()
{
    //(*Destroy(fpsconfigFrame)
    //*)
}

void fpsconfigFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void fpsconfigFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

void updateTextCtrlValue(wxTextCtrl* text, wxSpinEvent& event)
{
    double value;
    wxString str;
    value = event.GetInt() * 0.01;
    str = wxString::Format(wxT("%.02f"), value);
    str.Replace(_(","), _("."));
    text->SetValue(str);
}

void fpsconfigFrame::OnSpinCtrlChange(wxSpinEvent& event)
{
    wxSpinCtrl* spin = (wxSpinCtrl*)event.GetEventObject();

    if(spin == SpinCtrl3)
    {
        updateTextCtrlValue(TextCtrl23, event);
    }
    else if(spin == SpinCtrl4)
    {
        updateTextCtrlValue(TextCtrl24, event);
    }
    else if(spin == SpinCtrl5)
    {
        updateTextCtrlValue(TextCtrl22, event);
    }
    else if(spin == SpinCtrl6)
    {
        updateTextCtrlValue(TextCtrl26, event);
    }
    else if(spin == SpinCtrl7)
    {
        updateTextCtrlValue(TextCtrl1, event);
    }
    else if(spin == SpinCtrl8)
    {
        updateTextCtrlValue(TextCtrl25, event);
    }
}

e_button_index fpsconfigFrame::getButtonIndex(wxButton* button)
{
    if(button == select)
    {
        return bi_select;
    }
    else if(button == start)
    {
        return bi_start;
    }
    else if(button == ps)
    {
        return bi_ps;
    }
    else if(button == up)
    {
        return bi_up;
    }
    else if(button == down)
    {
        return bi_down;
    }
    else if(button == right)
    {
        return bi_right;
    }
    else if(button == left)
    {
        return bi_left;
    }
    else if(button == square)
    {
        return bi_square;
    }
    else if(button == cross)
    {
        return bi_cross;
    }
    else if(button == circle)
    {
        return bi_circle;
    }
    else if(button == triangle)
    {
        return bi_triangle;
    }
    else if(button == l1)
    {
        return bi_l1;
    }
    else if(button == l2)
    {
        return bi_l2;
    }
    else if(button == l3)
    {
        return bi_l3;
    }
    else if(button == r1)
    {
        return bi_r1;
    }
    else if(button == r2)
    {
        return bi_r2;
    }
    else if(button == r3)
    {
        return bi_r3;
    }
    return bi_undef;//not supposed to occur...
}

wxButton* fpsconfigFrame::getButtonButton(wxString blabel)
{
    if(blabel == wxString(button_labels[bi_select], wxConvUTF8))
    {
        return select;
    }
    else if(blabel == wxString(button_labels[bi_start], wxConvUTF8))
    {
        return start;
    }
    else if(blabel == wxString(button_labels[bi_ps], wxConvUTF8))
    {
        return ps;
    }
    else if(blabel == wxString(button_labels[bi_up], wxConvUTF8))
    {
        return up;
    }
    else if(blabel == wxString(button_labels[bi_down], wxConvUTF8))
    {
        return down;
    }
    else if(blabel == wxString(button_labels[bi_right], wxConvUTF8))
    {
        return right;
    }
    else if(blabel == wxString(button_labels[bi_left], wxConvUTF8))
    {
        return left;
    }
    else if(blabel == wxString(button_labels[bi_square], wxConvUTF8))
    {
        return square;
    }
    else if(blabel == wxString(button_labels[bi_cross], wxConvUTF8))
    {
        return cross;
    }
    else if(blabel == wxString(button_labels[bi_circle], wxConvUTF8))
    {
        return circle;
    }
    else if(blabel == wxString(button_labels[bi_triangle], wxConvUTF8))
    {
        return triangle;
    }
    else if(blabel == wxString(button_labels[bi_l1], wxConvUTF8))
    {
        return l1;
    }
    else if(blabel == wxString(button_labels[bi_l2], wxConvUTF8))
    {
        return l2;
    }
    else if(blabel == wxString(button_labels[bi_l3], wxConvUTF8))
    {
        return l3;
    }
    else if(blabel == wxString(button_labels[bi_r1], wxConvUTF8))
    {
        return r1;
    }
    else if(blabel == wxString(button_labels[bi_r2], wxConvUTF8))
    {
        return r2;
    }
    else if(blabel == wxString(button_labels[bi_r3], wxConvUTF8))
    {
        return r3;
    }
    return NULL;//not supposed to occur...
}

wxButton* fpsconfigFrame::getAxisButton(wxString alabel)
{
    if(alabel == wxString(axis_labels[ai_ls_down], wxConvUTF8))
    {
        return stickdown;
    }
    else if(alabel == wxString(axis_labels[ai_ls_left], wxConvUTF8))
    {
        return stickleft;
    }
    else if(alabel == wxString(axis_labels[ai_ls_right], wxConvUTF8))
    {
        return stickright;
    }
    else if(alabel == wxString(axis_labels[ai_ls_up], wxConvUTF8))
    {
        return stickup;
    }
    return NULL;//not supposed to occur...
}

e_axis_index fpsconfigFrame::getAxisIndex(wxButton* button)
{
    if(button == stickdown)
    {
        return ai_ls_down;
    }
    else if(button == stickleft)
    {
        return ai_ls_left;
    }
    else if(button == stickright)
    {
        return ai_ls_right;
    }
    else if(button == stickup)
    {
        return ai_ls_up;
    }
    return ai_undef;//not supposed to occur...
}

void fpsconfigFrame::OnButtonClick(wxCommandEvent& event)
{
    e_button_index bindex;
    e_axis_index aindex;

    evcatch.run();
    ((wxButton*)event.GetEventObject())->SetLabel(evcatch.GetEventId());
    ((wxButton*)event.GetEventObject())->SetToolTip(evcatch.GetEventId());

    bindex = getButtonIndex((wxButton*)event.GetEventObject());

    if(bindex != bi_undef)
    {
        buttons[bindex].SetDevice(Device(evcatch.GetDeviceType(), _("0"), _("")));
        buttons[bindex].SetEvent(Event(_("button"), evcatch.GetEventId()));
        buttons[bindex].SetButton(wxString(button_labels[bindex], wxConvUTF8));
    }
    else
    {
        aindex = getAxisIndex((wxButton*)event.GetEventObject());

        if(aindex != ai_undef)
        {
            axes[aindex].SetDevice(Device(evcatch.GetDeviceType(), _("0"), _("")));
            axes[aindex].SetEvent(Event(_("button"), evcatch.GetEventId()));
            axes[aindex].SetAxis(wxString(axis_labels[aindex], wxConvUTF8));
        }
    }
}

void fpsconfigFrame::OnMenuNew(wxCommandEvent& event)
{
    wxButton* button;

    for(int i=bi_select; i<BI_MAX; i++)
    {
        buttons[i] = ButtonMapper();
        button = getButtonButton(wxString(button_labels[i], wxConvUTF8));
        if(button != NULL)
        {
            button->SetLabel(_(""));
            button->SetToolTip(_(""));
        }
    }

    for(int i=ai_ls_up; i<AI_MAX; i++)
    {
        axes[i] = AxisMapper();
        button = getAxisButton(wxString(axis_labels[i], wxConvUTF8));
        if(button != NULL)
        {
            button->SetLabel(_(""));
            button->SetToolTip(_(""));
        }
    }

    SpinCtrl1->SetValue(20);
    SpinCtrl2->SetValue(20);
    Choice1->SetSelection(0);
    Choice2->SetSelection(0);
    TextCtrl23->SetValue(_("1.00"));
    TextCtrl24->SetValue(_("1.00"));
    TextCtrl22->SetValue(_("1.00"));
    TextCtrl26->SetValue(_("1.00"));
    TextCtrl1->SetValue(_("1.00"));
    TextCtrl25->SetValue(_("1.00"));

    configFile = ConfigurationFile();

    MenuItem4->Enable(false);
}

void fpsconfigFrame::OnMenuSaveAs(wxCommandEvent& event)
{
    wxFileDialog saveFileDialog(this, _T("Save Config file"), _T(""), _T(""), _T("XML files (*.xml)|*.xml"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

#ifndef WIN32
    string cmd = "";
    cmd.append(homedir);
    cmd.append("/.emuclient/config");
#else
    string cmd = "config";
#endif
    wxString default_directory = wxString(cmd.c_str(), wxConvUTF8);
    saveFileDialog.SetDirectory(default_directory);

    if ( saveFileDialog.ShowModal() == wxID_CANCEL ) return;

    wxString FileName = saveFileDialog.GetPath();

    if ( FileName.IsEmpty() ) return;

    configFile.SetFilePath(FileName);

    OnMenuSave(event);
}

void fpsconfigFrame::OnMenuSave(wxCommandEvent& event)
{
    std::list<ButtonMapper>* buttonMappers;
    std::list<AxisMapper>* axisMappers;
    double mx, my;
    double xyratio;
    wxString wsmx, wsmy, wsxyratio;
    /*
     * Save primary config.
     */
    //Save ButtonMappers
    buttonMappers = configFile.GetController(0)->GetConfiguration(0)->GetButtonMapperList();
    for(std::list<ButtonMapper>::iterator it = buttonMappers->begin(); it!=buttonMappers->end(); it = buttonMappers->erase(it)) {}
    for(int i=bi_select; i<BI_MAX; i++)
    {
        if(!buttons[i].GetDevice()->GetType().IsEmpty())
        {
            buttonMappers->push_front(buttons[i]);
        }
    }
    //Save AxisMappers
    axisMappers = configFile.GetController(0)->GetConfiguration(0)->GetAxisMapperList();
    for(std::list<AxisMapper>::iterator it = axisMappers->begin(); it!=axisMappers->end(); it = axisMappers->erase(it)) {}
    for(int i=ai_ls_up; i<AI_MAX; i++)
    {
        if(!axes[i].GetDevice()->GetType().IsEmpty())
        {
            axisMappers->push_front(axes[i]);
        }
    }
    axisMappers->push_front(AxisMapper(_("mouse"), _("0"), _(""), _("axis"), _("x"), _("rstick x"), wxString::Format(wxT("%i"),SpinCtrl1->GetValue()), TextCtrl23->GetValue(), TextCtrl22->GetValue(), Choice2->GetStringSelection()));
    wsmx = TextCtrl23->GetValue();
    wsxyratio = TextCtrl1->GetValue();
    wsmx.Replace(_("."), _(","));
    wsxyratio.Replace(_("."), _(","));
    if(wsmx.ToDouble(&mx) && wsxyratio.ToDouble(&xyratio))
    {
        my = mx * xyratio;
    }
    else
    {
        my = mx;
    }
    wsmy = wxString::Format(wxT("%.02f"), my);
    wsmy.Replace(_(","), _("."));
    axisMappers->push_front(AxisMapper(_("mouse"), _("0"), _(""), _("axis"), _("y"), _("rstick y"), wxString::Format(wxT("%i"),SpinCtrl1->GetValue()), wsmy, TextCtrl22->GetValue(), Choice2->GetStringSelection()));
    /*
     * Save secondary config.
     */
    //Save Trigger
    configFile.GetController(0)->GetConfiguration(1)->GetTrigger()->GetDevice()->SetType(buttons[bi_l1].GetDevice()->GetType());
    configFile.GetController(0)->GetConfiguration(1)->GetTrigger()->GetDevice()->SetName(buttons[bi_l1].GetDevice()->GetName());
    configFile.GetController(0)->GetConfiguration(1)->GetTrigger()->GetDevice()->SetId(buttons[bi_l1].GetDevice()->GetId());
    configFile.GetController(0)->GetConfiguration(1)->GetTrigger()->GetEvent()->SetId(buttons[bi_l1].GetEvent()->GetId());
    configFile.GetController(0)->GetConfiguration(1)->GetTrigger()->SetSwitchBack(_("yes"));
    configFile.GetController(0)->GetConfiguration(1)->GetTrigger()->SetDelay(0);
    //Save ButtonMappers
    buttonMappers = configFile.GetController(0)->GetConfiguration(1)->GetButtonMapperList();
    for(std::list<ButtonMapper>::iterator it = buttonMappers->begin(); it!=buttonMappers->end(); it = buttonMappers->erase(it)) {}
    for(int i=bi_select; i<BI_MAX; i++)
    {
        if(!buttons[i].GetDevice()->GetType().IsEmpty())
        {
            buttonMappers->push_front(buttons[i]);
        }
    }
    //Save AxisMappers
    axisMappers = configFile.GetController(0)->GetConfiguration(1)->GetAxisMapperList();
    for(std::list<AxisMapper>::iterator it = axisMappers->begin(); it!=axisMappers->end(); it = axisMappers->erase(it)) {}
    for(int i=ai_ls_up; i<AI_MAX; i++)
    {
        if(!axes[i].GetDevice()->GetType().IsEmpty())
        {
            axisMappers->push_front(axes[i]);
        }
    }
    axisMappers->push_front(AxisMapper(_("mouse"), _("0"), _(""), _("axis"), _("x"), _("rstick x"), wxString::Format(wxT("%i"),SpinCtrl2->GetValue()), TextCtrl24->GetValue(), TextCtrl26->GetValue(), Choice2->GetStringSelection()));
    wsmx = TextCtrl24->GetValue();
    wsxyratio = TextCtrl25->GetValue();
    wsmx.Replace(_("."), _(","));
    wsxyratio.Replace(_("."), _(","));
    if(wsmx.ToDouble(&mx) && wsxyratio.ToDouble(&xyratio))
    {
        my = mx * xyratio;
    }
    else
    {
        my = mx;
    }
    wsmy = wxString::Format(wxT("%.02f"), my);
    wsmy.Replace(_(","), _("."));
    axisMappers->push_front(AxisMapper(_("mouse"), _("0"), _(""), _("axis"), _("y"), _("rstick y"), wxString::Format(wxT("%i"),SpinCtrl2->GetValue()), wsmy, TextCtrl26->GetValue(), Choice1->GetStringSelection()));

    configFile.WriteConfigFile();
}

void fpsconfigFrame::OnMenuOpen(wxCommandEvent& event)
{
    std::list<ButtonMapper>* buttonMappers;
    std::list<AxisMapper>* axisMappers;
    e_button_index bindex;
    e_axis_index aindex;
    bool warning = false;
    wxButton* button;

    double mx, my, exp;
    double xyratio;
    wxString wsmx, wsmy, wsexp, wsxyratio;

    if ( FileDialog1->ShowModal() != wxID_OK ) return;

    wxString FileName = FileDialog1->GetPath();
    if ( FileName.IsEmpty() ) return;

    configFile.ReadConfigFile(FileName);

    /*
     * Load primary config.
     */
    //Load ButtonMappers
    for(int i=bi_select; i<BI_MAX; i++)
    {
        buttons[i] = ButtonMapper();
        button = getButtonButton(wxString(button_labels[i], wxConvUTF8));
        if(button != NULL)
        {
            button->SetLabel(_(""));
            button->SetToolTip(_(""));
        }
    }
    buttonMappers = configFile.GetController(0)->GetConfiguration(0)->GetButtonMapperList();
    for(std::list<ButtonMapper>::iterator it = buttonMappers->begin(); it!=buttonMappers->end(); it++)
    {
        button = getButtonButton(it->GetButton());

        if(button == NULL)
        {
            continue;
        }

        bindex = getButtonIndex(button);

        if(bindex == bi_undef)
        {
            continue;
        }

        if(button->GetLabel().IsEmpty())
        {
            buttons[bindex] = *it;
            button->SetLabel(it->GetEvent()->GetId());
            button->SetToolTip(it->GetEvent()->GetId());
        }
        else
        {
            warning = true;
        }
    }
    //Load AxisMappers
    for(int i=ai_ls_up; i<AI_MAX; i++)
    {
        axes[i] = AxisMapper();
        button = getAxisButton(wxString(axis_labels[i], wxConvUTF8));
        if(button != NULL)
        {
            button->SetLabel(_(""));
            button->SetToolTip(_(""));
        }
    }
    wsmx.erase();
    wsmy.erase();
    mx = 0;
    my = 0;
    axisMappers = configFile.GetController(0)->GetConfiguration(0)->GetAxisMapperList();
    for(std::list<AxisMapper>::iterator it = axisMappers->begin(); it!=axisMappers->end(); it++)
    {
        button = getAxisButton(it->GetAxis());

        if(button == NULL)
        {
            if(it->GetAxis() == _("rstick x"))
            {
                SpinCtrl1->SetValue(wxAtoi(it->GetEvent()->GetDeadZone()));
                Choice2->SetStringSelection(it->GetEvent()->GetShape());

                wsmx = it->GetEvent()->GetMultiplier();
                wsmx.Replace(_("."), _(","));
                if(wsmx.ToDouble(&mx))
                {
                    TextCtrl23->SetValue(wsmx);
                    SpinCtrl3->SetValue(mx*100);
                    if(my && mx)
                    {
                        xyratio = my / mx;
                        wsxyratio = wxString::Format(wxT("%.02f"), xyratio);
                        wsxyratio.Replace(_(","), _("."));
                        TextCtrl1->SetValue(wsxyratio);
                        SpinCtrl7->SetValue(xyratio*100);
                    }
                }

                wsexp = it->GetEvent()->GetExponent();
                wsexp.Replace(_("."), _(","));
                if(wsexp.ToDouble(&exp))
                {
                  TextCtrl22->SetValue(it->GetEvent()->GetExponent());
                  SpinCtrl5->SetValue(exp*100);
                }
            }
            else if(it->GetAxis() == _("rstick y"))
            {
                wsmy = it->GetEvent()->GetMultiplier();
                wsmy.Replace(_("."), _(","));
                if(wsmy.ToDouble(&my) && mx && my)
                {
                    xyratio = my / mx;
                    wsxyratio = wxString::Format(wxT("%.02f"), xyratio);
                    wsxyratio.Replace(_(","), _("."));
                    TextCtrl1->SetValue(wsxyratio);
                    SpinCtrl7->SetValue(xyratio*100);
                }
            }
            else
            {
                warning = true;
            }
        }
        else
        {
            aindex = getAxisIndex(button);

            if(aindex == ai_undef)
            {
                continue;
            }

            if(button->GetLabel().IsEmpty())
            {
                axes[aindex] = *it;
                button->SetLabel(it->GetEvent()->GetId());
                button->SetToolTip(it->GetEvent()->GetId());
            }
            else
            {
                warning = true;
            }
        }
    }

    /*
     * Load secondary config.
     */
    wsmx.erase();
    wsmy.erase();
    mx = 0;
    my = 0;
    axisMappers = configFile.GetController(0)->GetConfiguration(1)->GetAxisMapperList();
    for(std::list<AxisMapper>::iterator it = axisMappers->begin(); it!=axisMappers->end(); it++)
    {
        button = getAxisButton(it->GetAxis());

        if(button == NULL)
        {
            if(it->GetAxis() == _("rstick x"))
            {
                SpinCtrl2->SetValue(wxAtoi(it->GetEvent()->GetDeadZone()));
                Choice1->SetStringSelection(it->GetEvent()->GetShape());

                wsmx = it->GetEvent()->GetMultiplier();
                wsmx.Replace(_("."), _(","));
                if(wsmx.ToDouble(&mx))
                {
                    TextCtrl24->SetValue(wsmx);
                    SpinCtrl4->SetValue(mx*100);
                    if(my && mx)
                    {
                        xyratio = my / mx;
                        wsxyratio = wxString::Format(wxT("%.02f"), xyratio);
                        wsxyratio.Replace(_(","), _("."));
                        TextCtrl25->SetValue(wsxyratio);
                        SpinCtrl8->SetValue(xyratio*100);
                    }
                }

                wsexp = it->GetEvent()->GetExponent();
                wsexp.Replace(_("."), _(","));
                if(wsexp.ToDouble(&exp))
                {
                  TextCtrl26->SetValue(it->GetEvent()->GetExponent());
                  SpinCtrl6->SetValue(exp*100);
                }
            }
            else if(it->GetAxis() == _("rstick y"))
            {
                wsmy = it->GetEvent()->GetMultiplier();
                wsmy.Replace(_("."), _(","));
                if(wsmy.ToDouble(&my) && mx && my)
                {
                    xyratio = my / mx;
                    wsxyratio = wxString::Format(wxT("%.02f"), xyratio);
                    wsxyratio.Replace(_(","), _("."));
                    TextCtrl25->SetValue(wsxyratio);
                    SpinCtrl8->SetValue(xyratio*100);
                }
            }
            else
            {
                warning = true;
            }
        }
    }

    if(warning)
    {
        wxMessageBox( wxT("Some parts of this config file are not supported\nand will be lost if the file is edited.\n"), wxT("Info"), wxICON_INFORMATION);
    }

    MenuItem4->Enable(true);
}
