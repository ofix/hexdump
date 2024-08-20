///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb 17 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __HexDumpDialog__
#define __HexDumpDialog__

// Define WX_GCH in order to support precompiled headers with GCC compiler.
// You have to create the header "wx_pch.h" and include all files needed
// for compile your gui inside it.
// Then, compile it and place the file "wx_pch.h.gch" into the same
// directory that "wx_pch.h".
#ifdef WX_GCH
#include <wx_pch.h>
#else
#include <wx/wx.h>
#endif

#include <wx/button.h>
#include <wx/statline.h>
#include <wx/combobox.h>
#include "HexDumpPanel.h"
#include "TagWindow.h"

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class HexDumpDialog
///////////////////////////////////////////////////////////////////////////////
class HexDumpDialog : public wxDialog
{
public:
    HexDumpDialog( wxWindow* parent, int id = wxID_ANY, wxString title = wxT("对话框"), wxPoint pos = wxDefaultPosition, wxSize size = wxDefaultSize, int style = wxDEFAULT_DIALOG_STYLE );
    void OnPcieDeviceChange(wxCommandEvent& event);
    void OnSaveBtn(wxCommandEvent& event);
private:
    wxComboBox* m_listBoxDevices;
    HexDumpPanel* m_panel;
    wxButton* m_saveBtn;
    wxTextCtrl* m_textCtrlRootPath;
    wxTextCtrl* m_textCtrlHighlightAddr;
    TagWindow* m_tagWnd;

    DECLARE_EVENT_TABLE()
};

#endif //__HexDumpDialog__
