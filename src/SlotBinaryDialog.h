#ifndef SLOTBINARYDIALOG_H
#define SLOTBINARYDIALOG_H

#include <wx/dialog.h>
#include <vector>


class SlotBinaryDialog : public wxDialog
{
   public:
      SlotBinaryDialog(wxWindow* parent,
                  int id = wxID_ANY,
                  wxString title = wxT("物理槽位"),
                  wxPoint pos = wxDefaultPosition,
                  wxSize size = wxDefaultSize,
                  int style = wxDEFAULT_DIALOG_STYLE);
      virtual ~SlotBinaryDialog();

        std::string ToHex(int num, int nWidth = 2);

      void OnPaint(wxPaintEvent& event);
      void DrawBinaryBits(wxDC* pDC,wxRect rect,std::string bits);

      std::string toBinaryBits(uint8_t data);
      void SetSlot(std::vector<uint8_t> slot);
   protected:

   private:
    std::vector<uint8_t> m_slot;
    std::vector<std::string> origin_bits;
    std::vector<std::string> binary_bits;
    std::vector<std::string> slot_bits;
    int m_physical_slot;
    DECLARE_EVENT_TABLE()
};

#endif // SLOTBINARYDIALOG_H
