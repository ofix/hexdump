///////////////////////////////////////////////////////////////////////////////
// Name:        src/ui/events/RichRadioEvent.cpp
// Purpose:     custom radio control selection event
// Author:      songhuabiao
// Modified by:
// Created:     2024-07-13 19:16
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "events/TagChangedEvent.h"

wxDEFINE_EVENT(wxEVT_TAG_CHANGED, TagChangedEvent);

TagChangedEvent::TagChangedEvent(wxEventType type, int id) : wxEvent(id, type) {
}

// 自定义数据必须在这里进行设置，否则事件接收者无法正确收到
TagChangedEvent::TagChangedEvent(const TagChangedEvent& event) : wxEvent(event) {
    this->SetSelection(event.GetSelection());
    this->SetAddr(event.GetAddr());
}

int TagChangedEvent::GetSelection() const {
    return m_selection;
}
void TagChangedEvent::SetSelection(int n) {
    m_selection = n;
}

void TagChangedEvent::SetAddr(int addr) {
    m_addr = addr;
}

int TagChangedEvent::GetAddr() const {
    return m_addr;
}
