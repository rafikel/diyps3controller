/***************************************************************
 * Name:      sixaxis_emu_guiApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Matlo (mat.lau@laposte.net)
 * Created:   2010-11-09
 * Copyright: Matlo (http://diy-machine.blogspot.com/)
 * License:
 **************************************************************/

#include "wx_pch.h"
#include "sixaxis_emu_guiApp.h"

//(*AppHeaders
#include "sixaxis_emu_guiMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(sixaxis_emu_guiApp);

bool sixaxis_emu_guiApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	sixaxis_emu_guiFrame* Frame = new sixaxis_emu_guiFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
