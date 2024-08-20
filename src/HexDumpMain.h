/***************************************************************
 * Name:      hexdumpMain.h
 * Purpose:   Defines Application Frame
 * Author:    ofix (shb8845369@gmail.com)
 * Created:   2024-08-19
 * Copyright: ofix ()
 * License:
 **************************************************************/

#ifndef HEXDUMPMAIN_H
#define HEXDUMPMAIN_H



#include "hexdumpApp.h"



#include "HexDumpDialog.h"

class hexdumpDialog: public HexDumpDialog
{
    public:
        hexdumpDialog(wxDialog *dlg);
        ~hexdumpDialog();
    private:
        virtual void OnClose(wxCloseEvent& event);
        virtual void OnQuit(wxCommandEvent& event);
        virtual void OnAbout(wxCommandEvent& event);
};
#endif // HEXDUMPMAIN_H
