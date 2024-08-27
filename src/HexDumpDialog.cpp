///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb 17 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif  //__BORLANDC__

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif  // WX_PRECOMP

#include <wx/imagpng.h>
#include <regex>
#include "HexDumpDialog.h"
#include "util/Global.h"


const long HexDumpDialog::ID_HEXDUMP_PANEL = wxNewId();

///////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(HexDumpDialog, wxDialog)

END_EVENT_TABLE()

HexDumpDialog::HexDumpDialog(wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style)
    : wxDialog(parent, id, title, pos, size, style) {
    wxImage::AddHandler(new wxPNGHandler);
    // 设置对话框的图标
    SetIcon(wxIcon(wxT("rocket.png"), wxBITMAP_TYPE_PNG));

    this->SetSizeHints(wxDefaultSize, wxDefaultSize);

    wxBoxSizer* bSizer1;
    bSizer1 = new wxBoxSizer(wxHORIZONTAL);
    m_panel = new HexDumpPanel(this, ID_HEXDUMP_PANEL, wxPoint(10, 8), wxSize(560, 480));
#ifdef _WIN32
    m_panel->SetPcieConfigDirRoot("E:\\dev\\pcie\\devices");
#else
    m_panel->SetPcieConfigDirRoot("/home/greatwall/work/pcie/172.26.15.176/devices");
    m_panel->SetJoinCurveBase(0x34);
#endif

    bSizer1->Add(m_panel, 0, wxALL, 10);

    ///////////////////////////////////////////// 右列
    wxBoxSizer* bSizer2;
    bSizer2 = new wxBoxSizer(wxVERTICAL);

    wxSize common_size = wxSize(240, 40);
    // 添加PCIe设备列表
    // wxStaticText
    wxStaticText* staticTextPath = new wxStaticText(this, wxID_ANY, wxT("PCIe 设备根路径"));
    bSizer2->Add(staticTextPath, 0, wxTOP | wxRIGHT, 0);
    // wxTextCtrl
    m_textCtrlRootPath =
        new wxTextCtrl(this, wxID_ANY, wxT("/home/greatwall/work/pcie/172.26.15.176/devices"), wxDefaultPosition, common_size);
    m_textCtrlRootPath->SetBackgroundColour(wxColor(230, 230, 230));
    bSizer2->Add(m_textCtrlRootPath, 0, wxTOP | wxRIGHT, 6);
    // wxStaticText
    wxStaticText* staticTextDevices = new wxStaticText(this, wxID_ANY, wxT("PCIe 设备列表"));
    bSizer2->Add(staticTextDevices, 0, wxTOP | wxRIGHT, 20);
    // wxComboBox
    std::vector<std::string> devices = m_panel->GetPcieDevices();
    m_listBoxDevices = new wxComboBox(this, wxID_ANY, wxT(""), wxDefaultPosition, common_size, 0, NULL, wxCB_DROPDOWN);
    for (auto& device : devices) {
        m_listBoxDevices->Append(device);
    }
    m_listBoxDevices->SetSelection(0);
    m_listBoxDevices->Bind(wxEVT_COMBOBOX, &HexDumpDialog::OnPcieDeviceChange, this);

    bSizer2->Add(m_listBoxDevices, 0, wxTOP | wxRIGHT, 6);

    // TagWindow
    m_tagWnd = new TagWindow(this, wxID_ANY, wxPoint(10, 8), wxSize(240, 40));
    bSizer2->Add(m_tagWnd, 0, wxTOP | wxRIGHT, 6);

    // wxStaticText
    wxStaticText* staticTextAddr = new wxStaticText(this, wxID_ANY, wxT("高亮地址块"));
    bSizer2->Add(staticTextAddr, 0, wxTOP | wxRIGHT, 20);
    // wxTextCtrl
    m_textCtrlHighlightAddr = new PaddingTextCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(240, 140));
    m_textCtrlHighlightAddr->SetBackgroundColour(wxColor(230, 230, 230));
    bSizer2->Add(m_textCtrlHighlightAddr, 0, wxTOP | wxRIGHT, 6);

    // Space
    // wxBoxSizer* bSizer3 = new wxBoxSizer(wxVERTICAL);
    // wxStaticText* staticTextSpace = new wxStaticText(this,wxID_ANY,wxT(""));
    // bSizer3->Add(staticTextSpace,0,wxTOP,10);
    // bSizer2->Add(bSizer3, 0, wxTOP, 10);

    wxButton* m_saveBtn = new wxButton(this, wxID_ANY, wxT("保存"), wxDefaultPosition, common_size);
    bSizer2->Add(m_saveBtn, 0, wxTOP | wxRIGHT, 20);
    m_saveBtn->Bind(wxEVT_BUTTON, &HexDumpDialog::OnSaveBtn, this);
    /////////////////////////////////////////////


    bSizer1->Add(bSizer2, 0, wxALL | wxEXPAND, 10);

    this->SetSizer(bSizer1);
    this->Layout();
    bSizer1->Fit(this);

    Bind(wxEVT_TAG_CHANGED, &HexDumpDialog::OnTagChanged, this);
    Bind(wxEVT_LEFT_DCLICK,&HexDumpDialog::OnDblClick,this);

    // 初始化一些数据
    m_textCtrlHighlightAddr->AppendText("0x0e,1\n");
    OnInputChanged();
}

void HexDumpDialog::OnDblClick(wxMouseEvent& event){
  if(event.GetId() == ID_HEXDUMP_PANEL){
   std::vector<uint8_t> slot = m_panel->GetPhysicalSlot();
   SlotBinaryDialog slotDialog(0,wxID_ANY,wxT("物理槽位"),wxDefaultPosition, wxSize(640, 280));
   slotDialog.SetSlot(slot);
   slotDialog.ShowModal();
     // m_slotDialog->SetSlot(slot);
  }else{
      std::cout<<"[self] double clicked!"<<std::endl;
  }
}


void HexDumpDialog::OnTagChanged(TagChangedEvent& event) {
    int addr = event.GetAddr();
    int nRows = addr / 16;
    m_panel->ScrollToRow(nRows);
}

void HexDumpDialog::OnPcieDeviceChange(wxCommandEvent& event) {
    int selection = event.GetSelection();
    wxString choice = m_listBoxDevices->GetString(selection);
}

void HexDumpDialog::OnSaveBtn(wxCommandEvent& event) {
    int selection = m_listBoxDevices->GetSelection();
    std::string device_path = m_panel->GetConfigFilePath(selection);
    m_panel->LoadPcieConfigFile(device_path);
    wxString hilight_addrs = m_textCtrlHighlightAddr->GetValue();
    std::string addrs = hilight_addrs.ToStdString();
    std::vector<HilightAddr> result = ParseAddrs(addrs);
    m_panel->SetHilightAddrs(result);
    UpdateTagWnd(result);
    m_panel->RefreshAll();
}

void HexDumpDialog::OnInputChanged() {
    int selection = m_listBoxDevices->GetSelection();
    std::string device_path = m_panel->GetConfigFilePath(selection);
    m_panel->LoadPcieConfigFile(device_path);
    wxString hilight_addrs = m_textCtrlHighlightAddr->GetValue();
    std::string addrs = hilight_addrs.ToStdString();
    std::vector<HilightAddr> result = ParseAddrs(addrs);
    m_panel->SetHilightAddrs(result);
    UpdateTagWnd(result);
    m_panel->RefreshAll();
}

void HexDumpDialog::UpdateTagWnd(std::vector<HilightAddr>& addrs) {
    std::vector<std::string> vecHex;
    for (HilightAddr& addr : addrs) {
        std::string hex = dec_to_hex<int>(addr.offset);
        vecHex.push_back(hex);
    }
    m_tagWnd->AddTags(vecHex);
}

std::vector<HilightAddr> HexDumpDialog::ParseAddrs(const std::string& addrs) {
    std::vector<HilightAddr> result;
    std::vector<std::string> lines = split(addrs);
    std::regex pattern1(R"((0x[0-9A-Fa-f]+)\s*)");
    std::regex pattern2(R"((0x[0-9A-Fa-f]+)\s*,\s*([1-9]|[1-9][0-9]+)\s*)");
    std::regex pattern3(
        R"((0x[0-9A-Fa-f]+)\s*,\s*([1-9]|[1-9][0-9]+)\s*,\s*[rR][gG][bB]\((\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5]),(\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5]),(\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5])\)\s*)");

    for (auto& line : lines) {
        std::smatch matches;
        if (std::regex_match(line, matches, pattern1)) {
            HilightAddr addr;
            addr.offset = hex_to_dec<int>(matches[1]);
            addr.size = 1;
            addr.bkColor = DEFAULT_HILIGHT_BKGROUND_COLOR;
            addr.color = DEFAULT_HILIGHT_COLOR;
            result.push_back(addr);
        } else if (std::regex_match(line, matches, pattern2)) {
            HilightAddr addr;
            addr.offset = hex_to_dec<int>(matches[1]);
            addr.size = std::stoi(matches[2]);
            addr.bkColor = DEFAULT_HILIGHT_BKGROUND_COLOR;
            addr.color = DEFAULT_HILIGHT_COLOR;
            result.push_back(addr);
        } else if (std::regex_match(line, matches, pattern3)) {
            HilightAddr addr;
            addr.offset = hex_to_dec<int>(matches[1]);
            addr.size = std::stoi(matches[2]);
            addr.color = wxColor(std::stoi(matches[3]), std::stoi(matches[4]), std::stoi(matches[5]));
            result.push_back(addr);
        }
    }
    return result;
}
