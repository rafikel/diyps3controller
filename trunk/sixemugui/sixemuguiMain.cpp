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

#include <glib.h>

#include <algorithm>
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
const long sixemuguiFrame::ID_STATICTEXT10 = wxNewId();
const long sixemuguiFrame::ID_STATICTEXT9 = wxNewId();
const long sixemuguiFrame::ID_TEXTCTRL1 = wxNewId();
const long sixemuguiFrame::ID_BUTTON1 = wxNewId();
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

static int readCommandResults(const char * argv[], int nb_params, wxString params[], int nb_repeat, wxString results[])
{
    int exit_status = 0;
    char* output = NULL;
    char* err = NULL;
    GError *error = NULL;
    gboolean test;
    char* line;
    char* end;
    int ret = 0;
    char c_param[256];

    test = g_spawn_sync(NULL, (gchar**)argv, NULL, (GSpawnFlags)G_SPAWN_SEARCH_PATH, NULL, NULL, &output, &err, &exit_status, &error);

    if(test)
    {
        line = output;

        printf("%s\n", output);

        for(int i=0; i<nb_params*nb_repeat; ++i)
        {
            strncpy(c_param, params[i%nb_params].mb_str(), sizeof(c_param));

            if((line = strstr(line, c_param)))
            {
                if((end = strstr(line, "\n")))
                {
                    *end = '\0';
                }
                results[i] = wxString(line+strlen(c_param), wxConvUTF8);
                line=end+1;
            }
            else
            {
                if(nb_repeat == 1)
                {
                    ret = -1;
                }
                break;
            }
        }
    }
    else
    {
        ret = -1;
    }

    if(output) g_free(output);
    if(err) g_free(err);
    if(error) g_free(error);

    return ret;
}

static char hci[7];
static char bad[18];

static const char *sixaxis_device[] = { "sudo", "sixaddr", NULL };

static const char *hciconfig_all[] = { "hciconfig", "-a", hci, NULL };
static const char *hciconfig_revision[] = { "sudo", "hciconfig", hci, "revision", NULL };

static const char *bdaddr[] = { "bdaddr", "-i", hci, NULL };
static const char *bdaddr_modify[] = { "sudo", "bdaddr", "-r", "-i", hci, bad, NULL };

void sixemuguiFrame::readSixaxis()
{
    wxString params[2] = {_("Current Bluetooth master: "), _("Current Bluetooth Device Address: ")};
    wxString results[14];

    int res = readCommandResults(sixaxis_device, 2, params, 7, results);

    if(res != -1)
    {
        for(int i=0; i<13; i+=2)
        {
            if(results[i].IsEmpty())
            {
                break;
            }
            Choice2->Append(results[i].MakeUpper());
            Choice1->Append(results[i+1].MakeUpper());
        }
    }
}

void sixemuguiFrame::readDongles()
{
    wxString params1[3] = {_("BD Address: "), _("Name: "), _("Manufacturer: ")};
    wxString results1[3];
    wxString params2[1] = {_("Chip version: ")};
    wxString results2[1];
    int res;

    for(int i=0; i<256; ++i)
    {
        snprintf(hci, sizeof(hci), "hci%d", i);
        res = readCommandResults(hciconfig_all, 3, params1, 1, results1);

        if(res != -1)
        {
            Choice3->Append(results1[0].Left(17));

            Choice5->Append(results1[1]);

            Choice6->Append(results1[2]);

            res = readCommandResults(hciconfig_revision, 1, params2, 1, results2);

            if(res != -1)
            {
                Choice7->Append(results2[0]);
            }
            else
            {
                Choice7->Append(_(""));
            }
        }
        else
        {
            break;
        }
    }
}

int sixemuguiFrame::setDongleAddress()
{
    int i;
    int j = 0;
    unsigned int k;
    wxString params1[1] = {_("Address changed - ")};
    wxString results1[1];
    wxString params2[1] = {_("Device address: ")};
    wxString results2[1];
    int res;

    if(Choice1->GetStringSelection().IsEmpty())
    {
        wxMessageBox( wxT("Please select a Sixaxis Address!"), wxT("Error"), wxICON_ERROR);
        return -1;
    }
    else if(Choice3->GetStringSelection().IsEmpty())
    {
        wxMessageBox( wxT("Please select a Bluetooth Dongle!"), wxT("Error"), wxICON_ERROR);
        return -1;
    }

    for(k=0; k<Choice3->GetCount(); ++k)
    {
        if(Choice3->GetString(k) == Choice1->GetStringSelection())
        {
            wxMessageBox( wxT("Address already used!"), wxT("Error"), wxICON_ERROR);
            return -1;
        }
    }

    i = Choice3->GetSelection();
    snprintf(hci, sizeof(hci), "hci%d", i);
    strncpy( bad, Choice1->GetStringSelection().mb_str(), 18 );

    res = readCommandResults(bdaddr_modify, 1, params1, 1, results1);

    if(res != -1)
    {
        wxMessageBox( results1[0], wxT("Success"), wxICON_INFORMATION);
    }

    while(readCommandResults(bdaddr, 1, params2, 1, results2) == -1 && j<50)
    {
        usleep(100000);
        j++;
    }

    if(results2[0] !=  Choice1->GetStringSelection())
    {
        wxMessageBox( wxT("Read address after set: ko!"), wxT("Error"), wxICON_ERROR);
    }
    else
    {
        wxMessageBox( wxT("Read address after set: seems ok!"), wxT("Success"), wxICON_INFORMATION);
    }

    return 0;
}

void sixemuguiFrame::refresh()
{
    Choice1->Clear();
    Choice2->Clear();
    Choice3->Clear();
    Choice5->Clear();
    Choice6->Clear();
    Choice7->Clear();
    readSixaxis();
    readDongles();
    if(Choice1->GetCount() == 0)
    {
        wxMessageBox( wxT("No Sixaxis Detected!\nSixaxis usb wire plugged?"), wxT("Error"), wxICON_ERROR);
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
            Choice5->SetSelection(0);
            Choice6->SetSelection(0);
            Choice7->SetSelection(0);
        }
    }
    Refresh();
    Layout();
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
    wxGridSizer* GridSizer1;
    wxBoxSizer* BoxSizer1;
    wxMenuBar* MenuBar1;
    wxStaticBoxSizer* StaticBoxSizer1;
    wxMenu* Menu2;

    Create(parent, wxID_ANY, _("Sixemugui"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    GridSizer1 = new wxGridSizer(1, 1, 0, 0);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Sixaxis"));
    FlexGridSizer2 = new wxFlexGridSizer(2, 2, 0, 0);
    StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Address"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    FlexGridSizer2->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice1 = new wxChoice(this, ID_CHOICE1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
    FlexGridSizer2->Add(Choice1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("PS3 address"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    FlexGridSizer2->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice2 = new wxChoice(this, ID_CHOICE2, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE2"));
    FlexGridSizer2->Add(Choice2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer1->Add(FlexGridSizer2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    GridSizer1->Add(StaticBoxSizer1, 1, wxALL|wxSHAPED|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Dongle"));
    FlexGridSizer3 = new wxFlexGridSizer(5, 2, 0, 0);
    StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Address"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    FlexGridSizer3->Add(StaticText3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice3 = new wxChoice(this, ID_CHOICE3, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE3"));
    FlexGridSizer3->Add(Choice3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Name"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
    FlexGridSizer3->Add(StaticText4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice5 = new wxChoice(this, ID_CHOICE5, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE5"));
    FlexGridSizer3->Add(Choice5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText6 = new wxStaticText(this, ID_STATICTEXT6, _("Manufacturer"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
    FlexGridSizer3->Add(StaticText6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice6 = new wxChoice(this, ID_CHOICE6, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE6"));
    FlexGridSizer3->Add(Choice6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText7 = new wxStaticText(this, ID_STATICTEXT7, _("Chip version"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
    FlexGridSizer3->Add(StaticText7, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Choice7 = new wxChoice(this, ID_CHOICE7, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE7"));
    FlexGridSizer3->Add(Choice7, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText8 = new wxStaticText(this, ID_STATICTEXT8, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT8"));
    FlexGridSizer3->Add(StaticText8, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button2 = new wxButton(this, ID_BUTTON2, _("Set Dongle Address"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    FlexGridSizer3->Add(Button2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer2->Add(FlexGridSizer3, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    GridSizer1->Add(StaticBoxSizer2, 1, wxALL|wxSHAPED|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer3 = new wxStaticBoxSizer(wxHORIZONTAL, this, wxEmptyString);
    FlexGridSizer4 = new wxFlexGridSizer(1, 3, 0, 0);
    StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("Controller"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
    FlexGridSizer4->Add(StaticText5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    Choice4 = new wxChoice(this, ID_CHOICE4, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE4"));
    Choice4->SetSelection( Choice4->Append(_("1")) );
    Choice4->Append(_("2"));
    Choice4->Append(_("3"));
    Choice4->Append(_("4"));
    Choice4->Append(_("5"));
    Choice4->Append(_("6"));
    Choice4->Append(_("7"));
    Choice4->Disable();
    FlexGridSizer4->Add(Choice4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText10 = new wxStaticText(this, ID_STATICTEXT10, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT10"));
    FlexGridSizer4->Add(StaticText10, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText9 = new wxStaticText(this, ID_STATICTEXT9, _("Base Port"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT9"));
    FlexGridSizer4->Add(StaticText9, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrl1 = new wxTextCtrl(this, ID_TEXTCTRL1, _("21313"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    FlexGridSizer4->Add(TextCtrl1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button1 = new wxButton(this, ID_BUTTON1, _("Connect"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    FlexGridSizer4->Add(Button1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer3->Add(FlexGridSizer4, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    GridSizer1->Add(StaticBoxSizer3, 1, wxALL|wxSHAPED|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    GridSizer1->Add(BoxSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(GridSizer1);
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
    GridSizer1->Fit(this);
    GridSizer1->SetSizeHints(this);

    Connect(ID_CHOICE1,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&sixemuguiFrame::OnSelectSixaxisBdaddr);
    Connect(ID_CHOICE2,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&sixemuguiFrame::OnSelectPS3Bdaddr);
    Connect(ID_CHOICE3,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&sixemuguiFrame::OnSelectBtDongle);
    Connect(ID_CHOICE5,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&sixemuguiFrame::OnChoice5Select);
    Connect(ID_CHOICE6,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&sixemuguiFrame::OnChoice6Select);
    Connect(ID_CHOICE7,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&sixemuguiFrame::OnChoice7Select);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&sixemuguiFrame::OnButton2Click);
    Connect(ID_CHOICE4,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&sixemuguiFrame::OnChoice4Select);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&sixemuguiFrame::OnButton1Click);
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
    Choice5->SetSelection(Choice3->GetSelection());
    Choice6->SetSelection(Choice3->GetSelection());
    Choice7->SetSelection(Choice3->GetSelection());
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
        if(setDongleAddress() != -1)
        {
            refresh();
        }
    }
    else if (answer == wxNO)
    {
        wxMessageBox(_("Please save it!"), _("Info"));
    }
}

void sixemuguiFrame::OnChoice5Select(wxCommandEvent& event)
{
    Choice3->SetSelection(Choice5->GetSelection());
    Choice6->SetSelection(Choice5->GetSelection());
    Choice7->SetSelection(Choice5->GetSelection());
}

void sixemuguiFrame::OnChoice6Select(wxCommandEvent& event)
{
    Choice3->SetSelection(Choice6->GetSelection());
    Choice5->SetSelection(Choice6->GetSelection());
    Choice7->SetSelection(Choice6->GetSelection());
}

void sixemuguiFrame::OnChoice7Select(wxCommandEvent& event)
{
    Choice3->SetSelection(Choice7->GetSelection());
    Choice5->SetSelection(Choice7->GetSelection());
    Choice6->SetSelection(Choice7->GetSelection());
}

void sixemuguiFrame::OnButton1Click(wxCommandEvent& event)
{

}
