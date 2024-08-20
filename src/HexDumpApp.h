/***************************************************************
 * Name:      hexdumpApp.h
 * Purpose:   Defines Application Class
 * Author:    ofix (shb8845369@gmail.com)
 * Created:   2024-08-19
 * Copyright: ofix ()
 * License:
 **************************************************************/

#ifndef HEXDUMPAPP_H
#define HEXDUMPAPP_H

#include <wx/app.h>

class HexDumpApp : public wxApp
{
    public:
        virtual bool OnInit();
};

#endif // HEXDUMPAPP_H
