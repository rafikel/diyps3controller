/***************************************************************
 * Name:      sixaxis_emu_guiApp.h
 * Purpose:   Defines Application Class
 * Author:    Matlo (mat.lau@laposte.net)
 * Created:   2010-11-09
 * Copyright: Matlo (http://diy-machine.blogspot.com/)
 * License:
 **************************************************************/

#ifndef SIXAXIS_EMU_GUIAPP_H
#define SIXAXIS_EMU_GUIAPP_H

#include <wx/app.h>

class sixaxis_emu_guiApp : public wxApp
{
    public:
        virtual bool OnInit();
        virtual int OnExit();
};

#endif // SIXAXIS_EMU_GUIAPP_H
