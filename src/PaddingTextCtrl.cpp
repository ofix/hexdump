#include "PaddingTextCtrl.h"

PaddingTextCtrl::PaddingTextCtrl(wxWindow *parent, wxWindowID id, const wxPoint &pos,
                                 const wxSize &size, long style, const wxString &name)
    : wxPanel(parent,id,pos,size,style,name), m_padding(10, 10)
{
    //ctor
    wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
    m_pTextCtrl = new wxTextCtrl(this,wxID_ANY,wxT(""),wxDefaultPosition,wxSize(240,140),wxTE_MULTILINE|wxHSCROLL|wxNO_BORDER);
    pSizer->Add(m_pTextCtrl,1,wxEXPAND|wxALL,m_padding.x);
    m_pTextCtrl->SetBackgroundColour(wxColor(230,230,230));
    this->SetSizer( pSizer );
    this->Layout();
    pSizer->Fit( this );
}

wxString PaddingTextCtrl::GetValue()
{
    return m_pTextCtrl->GetValue();
}

void PaddingTextCtrl::AppendText(const wxString& text)
{
   m_pTextCtrl->AppendText(text);
}

void PaddingTextCtrl::SetPadding(const wxSize& padding)
{
    this->m_padding = padding;
}

PaddingTextCtrl::~PaddingTextCtrl()
{
    //dtor
}
