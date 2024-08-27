#include "SlotBinaryDialog.h"
#include <wx/imagpng.h>
#include <wx/dcbuffer.h>
#include "util/Global.h"
#include <iostream>

BEGIN_EVENT_TABLE(SlotBinaryDialog, wxDialog)
EVT_PAINT(SlotBinaryDialog::OnPaint)
END_EVENT_TABLE()

SlotBinaryDialog::SlotBinaryDialog(wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style)
    : wxDialog(parent, id, title, pos, size, style)
{
    wxImage::AddHandler(new wxPNGHandler);
    // 设置对话框的图标
    SetIcon(wxIcon(wxT("rocket.png"), wxBITMAP_TYPE_PNG));

    this->SetSizeHints(wxDefaultSize, wxDefaultSize);
    m_slot = {};
    // 必须调用此函数，否则无法重绘wxDialog及其子类
    SetBackgroundStyle(wxBG_STYLE_PAINT);

}

SlotBinaryDialog::~SlotBinaryDialog()
{
    //dtor
}

std::string SlotBinaryDialog::toBinaryBits(uint8_t data)
{
    std::string binary;
    for (int i = 7; i >= 0; --i)
    {
        binary += ((data >> i) & 1)? '1' : '0';
    }
    return binary;
}

void SlotBinaryDialog::SetSlot(std::vector<uint8_t> slot){
   m_slot.assign(slot.begin(),slot.end());
   for(auto& c: slot){
     origin_bits.push_back(ToHex(static_cast<int>(c) & 0xFF));
   }

   for(int i=slot.size() -1;i >=0; --i){
      binary_bits.push_back(toBinaryBits(slot[i]));
   }

   // '000'+binary_bits[0].substr(1,7)+binary_bits[1].substr(0,6);
   slot_bits.push_back("000"+binary_bits[0].substr(0,5));
   slot_bits.push_back(binary_bits[0].substr(5,3)+binary_bits[1].substr(0,5));
   m_physical_slot = ((static_cast<uint16_t>(slot[3])<<8)|slot[2])>>3;
   std::cout<<"m_physical_slot = "<<m_physical_slot<<std::endl;
}

std::string SlotBinaryDialog::ToHex(int num, int nWidth) {
  std::stringstream ss;
  ss << std::uppercase << std::hex << std::setfill('0') << std::setw(nWidth)
     << num;
  std::string result;
  ss >> result;
  return result;
}

void SlotBinaryDialog::OnPaint(wxPaintEvent& event) {
    #define BIT_WIDTH 140
    #define HEX_WIDTH 140
    #define MARGIN 18
    #define HEIGHT 32
    wxAutoBufferedPaintDC dc(this);
    dc.SetTextForeground(wxColor(0,0,0));
    wxFont defaultFont(14, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, _T("微软雅黑"),
                       wxFONTENCODING_DEFAULT);
    dc.SetFont(defaultFont);
    wxRect rect(MARGIN,MARGIN,HEX_WIDTH,HEIGHT);
    for(size_t i=0; i<origin_bits.size() ;i++){
         DrawBinaryBits(&dc,rect,origin_bits[i]);
         rect.x += HEX_WIDTH+MARGIN;
    }

    rect.x = MARGIN ;
    rect.y += MARGIN+HEIGHT;
        for(int i=origin_bits.size()-1; i>=0;--i){
         DrawBinaryBits(&dc,rect,origin_bits[i]);
         rect.x += HEX_WIDTH+MARGIN;
    }

    rect.x = MARGIN;
    rect.y += MARGIN+HEIGHT;
    rect.width = BIT_WIDTH;

    dc.SetTextForeground(wxColor(255,0,0));
    dc.DrawText(binary_bits[0],wxPoint(rect.x+26,rect.y));
    dc.SetTextForeground(wxColor(0,0,255));
    dc.DrawText(binary_bits[1].substr(0,5),wxPoint(rect.x+BIT_WIDTH+MARGIN+26,rect.y));
    dc.SetTextForeground(wxColor(0,0,0));
    dc.DrawText(binary_bits[1].substr(5,3),wxPoint(rect.x+BIT_WIDTH+MARGIN+80,rect.y));

    rect.x += (BIT_WIDTH+MARGIN)*2;
    for(size_t i=2; i<binary_bits.size() ;i++){
         DrawBinaryBits(&dc,rect,binary_bits[i]);
         rect.x += BIT_WIDTH + MARGIN;
    }
    rect.x = MARGIN;
    rect.y += MARGIN+HEIGHT;

    dc.DrawText(slot_bits[0].substr(0,3),wxPoint(rect.x+26,rect.y));
    dc.SetTextForeground(wxColor(255,0,0));
    dc.DrawText(slot_bits[0].substr(3,5),wxPoint(rect.x+59,rect.y));
    dc.DrawText(slot_bits[1].substr(0,3),wxPoint(rect.x+BIT_WIDTH+MARGIN+26,rect.y));
    dc.SetTextForeground(wxColor(0,0,255));
    dc.DrawText(slot_bits[1].substr(3,5),wxPoint(rect.x+BIT_WIDTH+MARGIN+59,rect.y));

    dc.SetTextForeground(wxColor(0,0,0));
    rect.x += (BIT_WIDTH + MARGIN)*2;
    DrawBinaryBits(&dc,rect,"=");
    rect.x += BIT_WIDTH + MARGIN;
    DrawBinaryBits(&dc,rect, "槽位"+std::to_string(m_physical_slot));



    /*
    for(size_t i=1; i<slot_bits.size() ;i++){
         DrawBinaryBits(&dc,rect,slot_bits[i]);
         rect.x += BIT_WIDTH + MARGIN;
    }
    */
}

void SlotBinaryDialog::DrawBinaryBits(wxDC* pDC, wxRect rect,std::string bits){
   pDC->DrawLabel(wxString::FromUTF8(bits.c_str()),rect,wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL);
}

