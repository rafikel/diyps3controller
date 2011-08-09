/***************************************************************
 * Name:      fpsconfigApp.h
 * Purpose:   Defines Application Class
 * Author:    Matlo (mat.lau@laposte.net)
 * Created:   2011-08-06
 * Copyright: Matlo (http://diy-machine.blogspot.com/)
 * License:
 **************************************************************/

#ifndef FPSCONFIGAPP_H
#define FPSCONFIGAPP_H

#include <wx/app.h>

class fpsconfigApp : public wxApp
{
    public:
        virtual bool OnInit();
};

#endif // FPSCONFIGAPP_H
