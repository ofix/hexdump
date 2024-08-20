#ifndef TAGWINDOW_H
#define TAGWINDOW_H

#include <wx/vscroll.h>
#include <vector>
#include <string>

struct Tag{
   std::string name;
   wxRect rect;
   bool active;
};

class TagWindow : public wxVScrolledWindow
{
   public:
      TagWindow(wxWindow *parent, wxWindowID id=wxID_ANY, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxTAB_TRAVERSAL, const wxString &name=wxPanelNameStr);
      virtual ~TagWindow();
      void OnPaint(wxPaintEvent& event);
      void OnClick(wxMouseEvent& event);
      // 辅助函数
      void AddTags(std::vector<std::string>& tags);
      void AddTag(const std::string& tag_name);
      void RemoveTag(const std::string& tag_name);
      void RemoveTag(size_t pos);
      void ReLayout();
      std::string GetActiveTagName();
      virtual wxCoord OnGetRowHeight(size_t row) const;

   protected:

   private:
   int m_activeTagPos;
   std::vector<Tag> m_tags;
   std::vector<std::vector<Tag>> m_visibleTags;

};

#endif // TAGWINDOW_H
