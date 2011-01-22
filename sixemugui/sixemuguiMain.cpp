/***************************************************************
 * Name:      sixemuguiMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Matlo (mat.lau@laposte.net)
 * Created:   2011-01-12
 * Copyright: Matlo (http://diy-machine.blogspot.com/)
 * License:
 **************************************************************/

#include "wx_pch.h"
#include "sixemuguiMain.h"
#include <wx/msgdlg.h>

//(*InternalHeaders(sixemuguiFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include <algorithm>
#include <fstream>
using namespace std;

//helper functions
enum wxbuildinfoformat
{
    short_f, long_f
};

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

//(*IdInit(sixemuguiFrame)
const long sixemuguiFrame::ID_STATICTEXT1 = wxNewId();
const long sixemuguiFrame::ID_CHOICE1 = wxNewId();
const long sixemuguiFrame::ID_STATICTEXT2 = wxNewId();
const long sixemuguiFrame::ID_CHOICE2 = wxNewId();
const long sixemuguiFrame::ID_STATICTEXT3 = wxNewId();
const long sixemuguiFrame::ID_CHOICE3 = wxNewId();
const long sixemuguiFrame::ID_STATICTEXT4 = wxNewId();
const long sixemuguiFrame::ID_CHOICE5 = wxNewId();
const long sixemuguiFrame::ID_STATICTEXT6 = wxNewId();
const long sixemuguiFrame::ID_CHOICE6 = wxNewId();
const long sixemuguiFrame::ID_STATICTEXT7 = wxNewId();
const long sixemuguiFrame::ID_CHOICE7 = wxNewId();
const long sixemuguiFrame::ID_STATICTEXT8 = wxNewId();
const long sixemuguiFrame::ID_BUTTON2 = wxNewId();
const long sixemuguiFrame::ID_STATICTEXT5 = wxNewId();
const long sixemuguiFrame::ID_CHOICE4 = wxNewId();
const long sixemuguiFrame::ID_BUTTON1 = wxNewId();
const long sixemuguiFrame::ID_PANEL1 = wxNewId();
const long sixemuguiFrame::ID_MENUITEM1 = wxNewId();
const long sixemuguiFrame::ID_MENUITEM5 = wxNewId();
const long sixemuguiFrame::idMenuQuit = wxNewId();
const long sixemuguiFrame::idMenuAbout = wxNewId();
const long sixemuguiFrame::ID_STATUSBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(sixemuguiFrame,wxFrame)
    //(*EventTable(sixemuguiFrame)
    //*)
END_EVENT_TABLE()

/*
 * Copy-paste from bluez...
 */
int bachk(const char *str)
{
	char tmp[18], *ptr = tmp;

	if (!str)
		return -1;

	if (strlen(str) != 17)
		return -1;

	memcpy(tmp, str, 18);

	while (*ptr) {
		*ptr = toupper(*ptr);
		if (*ptr < '0'|| (*ptr > '9' && *ptr < 'A') || *ptr > 'F')
			return -1;
		ptr++;

		*ptr = toupper(*ptr);
		if (*ptr < '0'|| (*ptr > '9' && *ptr < 'A') || *ptr > 'F')
			return -1;
		ptr++;

		*ptr = toupper(*ptr);
		if (*ptr == 0)
			break;
		if (*ptr != ':')
			return -1;
		ptr++;
	}

	return 0;
}

#define MASTER "Current Bluetooth master:"
#define DEVICE "Current Bluetooth Device Address:"

void sixemuguiFrame::readSixaxis()
{
    ifstream inFile;
    char line[256];

    system("sixaddr > /tmp/.sixaddr");

    inFile.open("/tmp/.sixaddr");

    if (!inFile)
    {
        return;
    }

    while (inFile.getline(line, 256))
    {
        if(strstr(line, MASTER) == line)
        {
            if(bachk(line+sizeof(MASTER)) != -1)
            {
                Choice2->Append(wxString(line+sizeof(MASTER), wxConvUTF8).MakeUpper());
            }
        }
        else if(strstr(line, DEVICE) == line)
        {
            if(bachk(line+sizeof(DEVICE)) != -1)
            {
                Choice1->Append(wxString(line+sizeof(DEVICE), wxConvUTF8).MakeUpper());
            }
        }
    }

    inFile.close();

    system("rm /tmp/.sixaddr");
}

#define DONGLE "Device address:"

void sixemuguiFrame::readDongles()
{
    ifstream inFile;
    char line[256];
    char command[256];

    for(int i=0; i<256; ++i)
    {
        sprintf(command, "bdaddr -i hci%d 2&>> /tmp/.bdaddr", i);
        if(system(command) != 0)
        {
            break;
        }
    }

    inFile.open("/tmp/.bdaddr");

    if (!inFile)
    {
        return;
    }

    while (inFile.getline(line, 256))
    {
        if(strstr(line, DONGLE) == line)
        {
            if(bachk(line+sizeof(DONGLE)) != -1)
            {
                Choice3->Append(wxString(line+sizeof(DONGLE), wxConvUTF8).MakeUpper());
            }
        }
    }

    inFile.close();

    system("rm /tmp/.bdaddr");
}

void sixemuguiFrame::setDongleAddress()
{
    ifstream inFile;
    char line[256];
    char status[256];
    char command[256];
    char bdaddr[256];
    int i, j, k;

    if(Choice1->GetStringSelection() == _(""))
    {
        wxMessageBox( wxT("Please select a Sixaxis Address!"), wxT("Error"), wxICON_ERROR);
        return;
    }
    else if(Choice3->GetStringSelection() == _(""))
    {
        wxMessageBox( wxT("Please select a Bluetooth Dongle!"), wxT("Error"), wxICON_ERROR);
        return;
    }

    for(k=0; k<Choice3->GetCount(); ++k)
    {
        if(Choice3->GetString(k) == Choice1->GetStringSelection())
        {
            wxMessageBox( wxT("Address already used!"), wxT("Error"), wxICON_ERROR);
            return;
        }
    }

    i = Choice3->GetSelection();
    strncpy( bdaddr, Choice1->GetStringSelection().mb_str(), 18 );

    snprintf(command, sizeof(command), "bdaddr -r -i hci%d %s 2&>> /tmp/.bdaddr", i, bdaddr);

    if(system(command) != 0)
    {
        wxMessageBox( wxT("Dongle Address not changed!"), wxT("Error"), wxICON_ERROR);
        return;
    }

    inFile.open("/tmp/.bdaddr");

    if (!inFile)
    {
        wxMessageBox( wxT("Reading back Bluetooth Dongle Address failed!"), wxT("Error"), wxICON_ERROR);
        return;
    }

    while (inFile.getline(line, 256))
    {
        strncpy(status, line, 256);
    }

    wxMessageBox( wxString(status, wxConvUTF8), wxT("Success"), wxICON_INFORMATION);

    inFile.close();

    system("rm /tmp/.bdaddr");

    snprintf(command, sizeof(command), "bdaddr -i hci%d 2&>> /dev/null", i);

    for(j=0; j<10; ++j)
    {
        usleep(500000);
        if(system(command) == 0)
        {
            break;
        }
    }

    snprintf(command, sizeof(command), "bdaddr -i hci%d 2&>> /tmp/.bdaddr", i);

    if(system(command) != 0)
    {
        wxMessageBox( wxT("Reading back Dongle Address failed!"), wxT("Error"), wxICON_ERROR);
        return;
    }

    inFile.open("/tmp/.bdaddr");

    if (!inFile)
    {
        wxMessageBox( wxT("Reading back Bluetooth Dongle Address failed!"), wxT("Error"), wxICON_ERROR);
        return;
    }

    while (inFile.getline(line, 256))
    {
        if(strstr(line, DONGLE) == line)
        {
            if(bachk(line+sizeof(DONGLE)) != -1)
            {
                if(strstr(line+sizeof(DONGLE), bdaddr) != line+sizeof(DONGLE))
                {
                    wxMessageBox( wxT("Read address after set: ko!"), wxT("Error"), wxICON_ERROR);
                }
                else
                {
                    wxMessageBox( wxT("Read address after set: seems ok!"), wxT("Success"), wxICON_INFORMATION);
                }
            }
            break;
        }
    }

    inFile.close();

    system("rm /tmp/.bdaddr");
}

void sixemuguiFrame::refresh()
{
    Choice1->Clear();
    Choice2->Clear();
    Choice3->Clear();
    readSixaxis();
    readDongles();
    if(Choice1->GetCount() == 0)
    {
        wxMessageBox( wxT("No Sixaxis Detected!\nAre you root?\nSixaxis usb wire plugged?"), wxT("Error"), wxICON_ERROR);
    }
    else
    {
        if(Choice1->GetSelection() < 0)
        {
            Choice1->SetSelection(0);
        }
        if(Choice2->GetSelection() < 0)
        {
            Choice2->SetSelection(Choice1->GetSelection());
        }
    }
    if(Choice3->GetCount() == 0)
    {
        wxMessageBox( wxT("No Bluetooth Dongle Detected!"), wxT("Error"), wxICON_ERROR);
    }
    else
    {
        if(Choice3->GetSelection() < 0)
        {
            Choice3->SetSelection(0);
        }
    }
    Panel1->Layout();
}

sixemuguiFrame::sixemuguiFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(sixemuguiFrame)
    wxStaticBoxSizer* StaticBoxSizer2;
    wxFlexGridSizer* FlexGridSizer4;
    wxMenuItem* MenuItem2;
    wxFlexGridSizer* FlexGridSizer3;
    wxMenuItem* MenuItem1;
    wxFlexGridSizer* FlexGridSizer2;
    wxMenu* Menu1;
    wxStaticBoxSizer* StaticBoxSizer3;
    wxMenuBar* MenuBar1;
    wxStaticBoxSizer* StaticBoxSizer1;
    wxFlexGridSizer* FlexGridSizer1;
    wxMenu* Menu2;

    Create(parent, wxID_ANY, _("Sixemugui"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    Panel1 = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    FlexGridSizer1 = new wxFlexGridSizer(3, 1, 0, 0);
    FlexGridSizer1->AddGrowableCol(1);
    FlexGridSizer1->AddGrowableRow(1);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, Panel1, _("Sixaxis"));
    FlexGridSizer2 = new wxFlexGridSizer(2, 2, 0, 0);
    StaticText1 = new wxStaticText(Panel1, ID_STATICTEXT1, _("Address"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    FlexGridSizer2->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice1 = new wxChoice(Panel1, ID_CHOICE1, wxDefaultPosition, wxSize(160,-1), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
    FlexGridSizer2->Add(Choice1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText2 = new wxStaticText(Panel1, ID_STATICTEXT2, _("PS3 address"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    FlexGridSizer2->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice2 = new wxChoice(Panel1, ID_CHOICE2, wxDefaultPosition, wxSize(160,-1), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE2"));
    FlexGridSizer2->Add(Choice2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer1->Add(FlexGridSizer2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(StaticBoxSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, Panel1, _("Dongle"));
    FlexGridSizer3 = new wxFlexGridSizer(5, 2, 0, 0);
    StaticText3 = new wxStaticText(Panel1, ID_STATICTEXT3, _("Address"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    FlexGridSizer3->Add(StaticText3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice3 = new wxChoice(Panel1, ID_CHOICE3, wxDefaultPosition, wxSize(160,-1), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE3"));
    FlexGridSizer3->Add(Choice3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText4 = new wxStaticText(Panel1, ID_STATICTEXT4, _("Name"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
    FlexGridSizer3->Add(StaticText4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice5 = new wxChoice(Panel1, ID_CHOICE5, wxDefaultPosition, wxSize(160,-1), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE5"));
    FlexGridSizer3->Add(Choice5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText6 = new wxStaticText(Panel1, ID_STATICTEXT6, _("Manufacturer"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
    FlexGridSizer3->Add(StaticText6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice6 = new wxChoice(Panel1, ID_CHOICE6, wxDefaultPosition, wxSize(160,-1), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE6"));
    FlexGridSizer3->Add(Choice6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText7 = new wxStaticText(Panel1, ID_STATICTEXT7, _("Chip version"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
    FlexGridSizer3->Add(StaticText7, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice7 = new wxChoice(Panel1, ID_CHOICE7, wxDefaultPosition, wxSize(160,-1), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE7"));
    FlexGridSizer3->Add(Choice7, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText8 = new wxStaticText(Panel1, ID_STATICTEXT8, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT8"));
    FlexGridSizer3->Add(StaticText8, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button2 = new wxButton(Panel1, ID_BUTTON2, _("Set Dongle Address"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    FlexGridSizer3->Add(Button2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer2->Add(FlexGridSizer3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(StaticBoxSizer2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer3 = new wxStaticBoxSizer(wxHORIZONTAL, Panel1, wxEmptyString);
    FlexGridSizer4 = new wxFlexGridSizer(1, 3, 0, 0);
    StaticText5 = new wxStaticText(Panel1, ID_STATICTEXT5, _("Controller"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
    FlexGridSizer4->Add(StaticText5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    Choice4 = new wxChoice(Panel1, ID_CHOICE4, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE4"));
    Choice4->SetSelection( Choice4->Append(_("1")) );
    Choice4->Append(_("2"));
    Choice4->Append(_("3"));
    Choice4->Append(_("4"));
    Choice4->Append(_("5"));
    Choice4->Append(_("6"));
    Choice4->Append(_("7"));
    Choice4->Disable();
    FlexGridSizer4->Add(Choice4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button1 = new wxButton(Panel1, ID_BUTTON1, _("Connect"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    FlexGridSizer4->Add(Button1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer3->Add(FlexGridSizer4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(StaticBoxSizer3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Panel1->SetSizer(FlexGridSizer1);
    FlexGridSizer1->Fit(Panel1);
    FlexGridSizer1->SetSizeHints(Panel1);
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem3 = new wxMenuItem(Menu1, ID_MENUITEM1, _("Refresh"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuItem3);
    MenuItem7 = new wxMenuItem(Menu1, ID_MENUITEM5, _("Start client"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuItem7);
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

    Connect(ID_CHOICE1,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&sixemuguiFrame::OnSelectSixaxisBdaddr);
    Connect(ID_CHOICE2,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&sixemuguiFrame::OnSelectPS3Bdaddr);
    Connect(ID_CHOICE3,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&sixemuguiFrame::OnSelectBtDongle);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&sixemuguiFrame::OnButton2Click);
    Connect(ID_CHOICE4,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&sixemuguiFrame::OnChoice4Select);
    Connect(ID_MENUITEM1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&sixemuguiFrame::OnSelectRefresh);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&sixemuguiFrame::OnQuit);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&sixemuguiFrame::OnAbout);
    //*)

    refresh();
}

sixemuguiFrame::~sixemuguiFrame()
{
    //(*Destroy(sixemuguiFrame)
    //*)
}

void sixemuguiFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void sixemuguiFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

void sixemuguiFrame::OnSelectSixaxisBdaddr(wxCommandEvent& event)
{
    Choice2->SetSelection(Choice1->GetSelection());
}

void sixemuguiFrame::OnSelectPS3Bdaddr(wxCommandEvent& event)
{
    Choice1->SetSelection(Choice2->GetSelection());
}

void sixemuguiFrame::OnSelectBtDongle(wxCommandEvent& event)
{

}

void sixemuguiFrame::OnChoice4Select(wxCommandEvent& event)
{
    m_SelectedController = wxAtoi(Choice4->GetStringSelection());
}

void sixemuguiFrame::OnSelectRefresh(wxCommandEvent& event)
{
    refresh();
}

void sixemuguiFrame::OnButton2Click(wxCommandEvent& event)
{
    int answer = wxMessageBox(_("Did you saved your dongle address?"), _("Confirm"), wxYES_NO | wxCANCEL);
    if (answer == wxYES)
    {
        setDongleAddress();
        refresh();
    }
    else if (answer == wxNO)
    {
        wxMessageBox(_("Please save it!"), _("Info"));
    }
}
