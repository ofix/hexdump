/***************************************************************
 * Name:      hexdumpApp.cpp
 * Purpose:   Code for Application Class
 * Author:    ofix (shb8845369@gmail.com)
 * Created:   2024-08-19
 * Copyright: ofix ()
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif  //__BORLANDC__

#include "HexDumpApp.h"
#include "HexDumpDialog.h"

IMPLEMENT_APP(HexDumpApp);

bool HexDumpApp::OnInit() {
    HexDumpDialog* dlg =
        new HexDumpDialog(0L, wxID_ANY, wxT("PCIe调试工具"), wxDefaultPosition, wxSize(1240, 960));
    dlg->Show();
    return true;
}
