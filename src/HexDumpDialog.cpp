///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb 17 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "HexDumpDialog.h"
#include <wx/imagpng.h>


///////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( HexDumpDialog, wxDialog )

END_EVENT_TABLE()

HexDumpDialog::HexDumpDialog( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style ) : wxDialog( parent, id, title, pos, size, style )
{
    wxImage::AddHandler(new wxPNGHandler);
    // 设置对话框的图标
    SetIcon(wxIcon(wxT("rocket.png"), wxBITMAP_TYPE_PNG));

    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    wxBoxSizer* bSizer1;
    bSizer1 = new wxBoxSizer( wxHORIZONTAL );
    m_panel = new HexDumpPanel(this,wxID_ANY,wxPoint(10,8),wxSize(560,480));
    m_panel->SetPcieConfigDirRoot("/home/greatwall/work/pcie/devices");
    m_panel->HilightWords(0x34,4,wxColor(214,62,195));
    bSizer1->Add(m_panel, 0, wxALL, 10);

    ///////////////////////////////////////////// 右列
    wxBoxSizer* bSizer2;
    bSizer2= new wxBoxSizer(wxVERTICAL);

    wxSize common_size = wxSize(240,40);
    // 添加PCIe设备列表
    // wxStaticText
    wxStaticText* staticTextPath = new wxStaticText(this,wxID_ANY,wxT("PCIe 设备根路径"));
    bSizer2->Add(staticTextPath,0,wxTOP|wxRIGHT,20);
    // wxTextCtrl
    m_textCtrlRootPath = new wxTextCtrl(this,wxID_ANY,wxT("/home/greatwall/work/pcie/devices"),wxDefaultPosition,common_size);
    m_textCtrlRootPath->SetBackgroundColour(wxColor(230,230,230));
    bSizer2->Add(m_textCtrlRootPath,0,wxTOP|wxRIGHT,6);
    // wxStaticText
    wxStaticText* staticTextDevices = new wxStaticText(this,wxID_ANY,wxT("PCIe 设备列表"));
    bSizer2->Add(staticTextDevices,0,wxTOP|wxRIGHT,20);
    // wxComboBox
    std::vector<std::string> devices = m_panel->GetPcieDevices();
    m_listBoxDevices = new wxComboBox(this,wxID_ANY,wxT(""),wxDefaultPosition, common_size,
                                      0, NULL, wxCB_DROPDOWN);
    for(auto& device: devices)
    {
        m_listBoxDevices->Append(device);
    }
    m_listBoxDevices->SetSelection(0);
    m_listBoxDevices->Bind(wxEVT_COMBOBOX,  &HexDumpDialog::OnPcieDeviceChange, this);

    bSizer2->Add( m_listBoxDevices, 0, wxTOP|wxRIGHT, 6);

    //TagWindow
    m_tagWnd = new TagWindow(this,wxID_ANY,wxPoint(10,8),wxSize(240,40));
    std::vector<std::string> tags{"0x34","0x40","0xFF00","0x0F22"};
    m_tagWnd->AddTags(tags);
    bSizer2->Add(m_tagWnd,0,wxTOP|wxRIGHT,6);

    // wxStaticText
    wxStaticText* staticTextAddr = new wxStaticText(this,wxID_ANY,wxT("高亮地址块"));
    bSizer2->Add(staticTextAddr,0,wxTOP|wxRIGHT,20);
    // wxTextCtrl
    m_textCtrlHighlightAddr = new wxTextCtrl(this,wxID_ANY,wxT(""),wxDefaultPosition,wxSize(240,140),wxTE_MULTILINE|wxHSCROLL);
    m_textCtrlHighlightAddr->SetBackgroundColour(wxColor(230,230,230));
    bSizer2->Add(m_textCtrlHighlightAddr,0,wxTOP|wxRIGHT,6);

    // Space
    // wxBoxSizer* bSizer3 = new wxBoxSizer(wxVERTICAL);
    //wxStaticText* staticTextSpace = new wxStaticText(this,wxID_ANY,wxT(""));
    //bSizer3->Add(staticTextSpace,0,wxTOP,10);
    // bSizer2->Add(bSizer3, 0, wxTOP, 10);

    wxButton* m_saveBtn = new wxButton(this,wxID_ANY,wxT("保存"),wxDefaultPosition,common_size);
    bSizer2->Add(m_saveBtn,0,wxTOP|wxRIGHT,20);
    m_saveBtn->Bind(wxEVT_BUTTON,&HexDumpDialog::OnSaveBtn,this);
    /////////////////////////////////////////////

    bSizer1->Add( bSizer2, 0, wxALL|wxEXPAND, 10);

    this->SetSizer( bSizer1 );
    this->Layout();
    bSizer1->Fit( this );
}

void HexDumpDialog::OnPcieDeviceChange(wxCommandEvent& event)
{
    int selection = event.GetSelection();
    wxString choice = m_listBoxDevices->GetString(selection);
    // std::cout<<"选择了: "<<choice<<std::endl;
}

void HexDumpDialog::OnSaveBtn(wxCommandEvent& event){
    int selection = m_listBoxDevices->GetSelection();
    std::string device_path = m_panel->GetConfigFilePath(selection);
    m_panel->LoadPcieConfigFile(device_path);
    wxString hilight_addrs = m_textCtrlHighlightAddr->GetValue();
    std::cout<<hilight_addrs<<std::endl;
    m_panel->RefreshAll();
}
