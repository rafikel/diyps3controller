/***************************************************************
 * Name:      fpsconfigApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Matlo (mat.lau@laposte.net)
 * Created:   2011-08-06
 * Copyright: Matlo (http://diy-machine.blogspot.com/)
 * License:
 **************************************************************/

#include "wx_pch.h"
#include "fpsconfigApp.h"

//(*AppHeaders
#include "fpsconfigMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(fpsconfigApp);

bool fpsconfigApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
        fpsconfigFrame* Frame = new fpsconfigFrame(0, -1);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
