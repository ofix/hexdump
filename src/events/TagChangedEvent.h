#ifndef RICH_RADIO_EVENT_H
#define RICH_RADIO_EVENT_H

#include <wx/event.h>

class TagChangedEvent;
wxDECLARE_EVENT(wxEVT_TAG_CHANGED, TagChangedEvent);
typedef void (wxEvtHandler::*TagChangedEventFunction)(TagChangedEvent&);
#define TagChangedEventHandler(func) wxEVENT_HANDLER_CAST(TagChangedEventFunction, func)
#define EVT_TAG_CHANGED(id, func) wx__DECLARE_EVT1(wxEVT_TAG_CHANGED, id, TagChangedEventHandler(func))

class TagChangedEvent : public wxEvent {
   public:
    TagChangedEvent(wxEventType type = wxEVT_TAG_CHANGED, int id = wxID_ANY);
    TagChangedEvent(const TagChangedEvent& event);
    wxEvent* Clone() const override {
        return new TagChangedEvent(*this);
    }
    int GetSelection() const;
    void SetSelection(int n);
    void SetAddr(int addr);
    int GetAddr() const;

   private:
    int m_selection;
    int m_addr;
};

#endif
