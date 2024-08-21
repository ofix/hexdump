#ifndef PADDINGTEXTCTRL_H
#define PADDINGTEXTCTRL_H

#include <wx/textctrl.h>
#include <wx/panel.h>


class PaddingTextCtrl : public wxPanel
{
   public:
      PaddingTextCtrl(wxWindow *parent, wxWindowID id=wxID_ANY, const wxPoint &pos=wxDefaultPosition,
      const wxSize &size=wxDefaultSize, long style=wxTAB_TRAVERSAL, const wxString &name=wxPanelNameStr);
      virtual ~PaddingTextCtrl();

      void SetPadding(const wxSize& padding);

      wxString GetValue();
      void AppendText(const wxString& text);

   private:
   wxTextCtrl* m_pTextCtrl;
   wxSize m_padding;
};

#endif // PADDINGTEXTCTRL_H
