#include "TagWindow.h"
#include <wx/dc.h>
#include <wx/dcbuffer.h>
#include "util/Global.h"

#define TAG_SPACE 10
#define TAG_PADDING 6
#define TAG_MARGIN 10
#define TAG_HEIGHT 24

TagWindow::TagWindow(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxString &name):
    wxVScrolledWindow(parent,id,pos,size,style,name),m_enableClose(false),m_activeTagPos(0)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT,&TagWindow::OnPaint,this);
    Bind(wxEVT_LEFT_DOWN,&TagWindow::OnClick,this);
}

TagWindow::~TagWindow()
{
    //dtor
}

void TagWindow::OnClick(wxMouseEvent& event)
{
    wxPoint pt = event.GetPosition();
    size_t iRowBegin = GetVisibleRowsBegin();
    size_t iRowEnd = GetVisibleRowsEnd();
    size_t  pos=0;
    int newActiveTagPos = -1;
    for(size_t iRow = 0; iRow<iRowBegin; iRow++)
    {
        pos += m_visibleTags[iRow].size();
    }

    for(size_t iRow=iRowBegin; iRow<iRowEnd; iRow++)
    {
        for(size_t iCol =0; iCol<m_visibleTags[iRow].size(); iCol++)
        {
            Tag tag = m_visibleTags[iRow][iCol];
            wxRect visible_rect(tag.rect.x,tag.rect.y - (iRowBegin)*(TAG_HEIGHT+TAG_MARGIN),tag.rect.width,tag.rect.height);
            wxRect close_rect(visible_rect.x + TAG_PADDING + tag.name_width + 12,visible_rect.y+8, 8,tag.rect.height-16);
            // 优先检查是否是关闭按钮
            if(m_enableClose && close_rect.Contains(pt))
            {
                // 检查删除的是否是活动标签，如果不是，默认第一个高亮
                RemoveTag(pos);
                break;
            }
            else if(visible_rect.Contains(pt))
            {
                m_visibleTags[iRow][iCol].active = true;
                m_tags[pos].active = true;
                newActiveTagPos = pos;
                break;
            }
            pos += 1;
        }
    }

    if(newActiveTagPos != -1 && newActiveTagPos != m_activeTagPos)
    {
        pos = 0;
        for(size_t iRow=0; iRow<m_visibleTags.size(); iRow++)
        {
            for(size_t iCol =0; iCol<m_visibleTags[iRow].size(); iCol++)
            {

                if(pos == m_activeTagPos)
                {
                    m_visibleTags[iRow][iCol].active = false;
                    m_tags[m_activeTagPos].active = false;
                    // 取消之前的的高亮
                    m_activeTagPos = newActiveTagPos;
                    break;
                }
                pos += 1;
            }
        }
    }
    // 创建并发送自定义事件
    TagChangedEvent tagEvent(wxEVT_TAG_CHANGED);
    tagEvent.SetAddr(hex_to_dec<int>(m_tags[m_activeTagPos].name));
    tagEvent.SetSelection(pos);
    // GetEventHandler()->AddPendingEvent(tagEvent);
    wxPostEvent(this->GetParent(), tagEvent);
    std::cout<<"发送消息: "<<newActiveTagPos<<std::endl;

    RefreshAll();
}

std::string TagWindow::GetActiveTagName()
{
    return m_tags[m_activeTagPos].name;
}

void TagWindow::OnPaint(wxPaintEvent& event)
{
    wxAutoBufferedPaintDC dc(this);
    size_t iRowBegin = GetVisibleRowsBegin();
    size_t iRowEnd = GetVisibleRowsEnd();

    for(size_t iRow=iRowBegin; iRow<iRowEnd; iRow++)
    {
        for(size_t iCol =0; iCol<m_visibleTags[iRow].size(); iCol++)
        {
            Tag tag = m_visibleTags[iRow][iCol];
            wxRect visible_rect(tag.rect.x,tag.rect.y - (iRowBegin)*(TAG_HEIGHT+TAG_MARGIN),tag.rect.width,tag.rect.height);
            wxRect text_rect(visible_rect.x+TAG_PADDING,visible_rect.y,tag.name_width,tag.rect.height);
            wxRect close_rect(visible_rect.x + TAG_PADDING + tag.name_width + 12,visible_rect.y+8, 8,tag.rect.height-16);
            if(!tag.active)
            {
                // 绘制圆角边框
                wxBrush activeBrush(wxColor(243, 243, 243),wxBRUSHSTYLE_SOLID);
                dc.SetBrush(activeBrush);
                wxPen inactivePen(wxColor(222,222,222),1,wxPENSTYLE_SOLID);
                dc.SetPen(inactivePen);
                dc.DrawRoundedRectangle(visible_rect,2);
                // 靠左绘制文本
                dc.SetTextForeground(wxColor(66,66,66));
                dc.DrawLabel(tag.name,text_rect,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
                // 绘制关闭按钮
                if(m_enableClose)
                {
                    DrawCloseBtn(&dc,close_rect,false);
                }
            }
            else
            {
                // 绘制圆角边框
                wxBrush activeBrush(wxColor(249, 219, 249),wxBRUSHSTYLE_SOLID);
                dc.SetBrush(activeBrush);
                wxPen activePen(wxColor(249,119,249),1,wxPENSTYLE_SOLID);
                dc.SetPen(activePen);
                dc.DrawRoundedRectangle(visible_rect,2);
                // 靠左绘制文本
                dc.SetTextForeground(wxColor(249, 54, 249));
                dc.DrawLabel(tag.name,text_rect,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
                // 绘制关闭按钮
                if(m_enableClose)
                {
                    DrawCloseBtn(&dc,close_rect,true);
                }
            }
        }
    }
}

void TagWindow::DrawCloseBtn(wxDC* pDC, wxRect& rect, bool active)
{
    wxPen pen = wxPen(wxColor(0,0,0),1,wxPENSTYLE_SOLID);
    if(active)
    {
        pen.SetColour(wxColor(255,0,0));
    }
    pDC->DrawLine(rect.x,rect.y,rect.x+rect.width,rect.y+rect.height);
    pDC->DrawLine(rect.x,rect.y+rect.height,rect.x+rect.width,rect.y);
    pDC->DrawPoint(rect.x+rect.width,rect.y+rect.height);
    pDC->DrawPoint(rect.x+rect.width,rect.y);
}

wxCoord TagWindow::OnGetRowHeight(size_t row) const
{
    return TAG_HEIGHT+TAG_MARGIN;
}

void TagWindow::AddTags(std::vector<std::string>& tags)
{
    if(tags.size() == 0)
    {
        return;
    }
    m_tags.clear();
    for(auto& tag_name: tags)
    {
        Tag tag;
        tag.name = tag_name;
        tag.rect = wxRect(0,0,0,0);
        tag.active = false;
        tag.name_width = 0;
        m_tags.push_back(tag);
    }
    m_tags[0].active = true;
    m_activeTagPos = 0;
    ReLayout();
    RefreshAll();
}

void TagWindow::AddTag(const std::string& tag_name)
{
    Tag tag;
    tag.name = tag_name;
    tag.rect = wxRect(0,0,0,0);
    tag.name_width = 0;
    tag.active = false;
    m_tags.push_back(tag);
    ReLayout();
}

void TagWindow::RemoveTag(const std::string& tag_name)
{
    size_t i=0;
    for(auto& tag:m_tags)
    {
        if(tag.name == tag_name)
        {
            m_tags.erase(m_tags.begin()+i);
        }
        i++;
    }
    EnsureOneActive();
    ReLayout();
}

void TagWindow::RemoveTag(size_t pos)
{
    if(pos > m_tags.size() -1|| pos < 0 || m_tags.size() == 0)
    {
        return;
    }
    m_tags.erase(m_tags.begin()+pos);
    EnsureOneActive();
    ReLayout();
}

void TagWindow::EnsureOneActive()
{
    bool has_active = false;
    for(auto& tag:m_tags)
    {
        if(tag.active)
        {
            has_active = true;
        }
    }
    if(!has_active && m_tags.size() >0)
    {
        m_tags[0].active = true;
    }
}

void TagWindow::ReLayout()
{
    // Flex 布局算法
    size_t offset_x=0;
    size_t offset_y=TAG_MARGIN;
    size_t MAX_WIDTH = this->GetSize().GetWidth();
    wxMemoryDC dc;
    size_t nrows = 0;
    size_t last_cell = 0;
    m_visibleTags.clear();
    std::vector<Tag> row;
    for(size_t i=0; i<m_tags.size(); i++)
    {
        size_t name_width = dc.GetTextExtent(m_tags[i].name).GetWidth();
        size_t width = (name_width+TAG_PADDING*2) + (m_enableClose?24:0);
        if(offset_x+width> MAX_WIDTH)
        {
            offset_y += TAG_MARGIN+TAG_HEIGHT; // 另起一行
            offset_x = TAG_MARGIN;
            nrows++;
            last_cell = i-1;
            m_visibleTags.push_back(row); //完成一行的添加
            row.clear();
        }
        m_tags[i].rect.x = offset_x;
        m_tags[i].rect.y = offset_y;
        m_tags[i].name_width = name_width;
        m_tags[i].rect.width = width;
        std::cout<<"name_width: "<<name_width<<" width: "<<width<<std::endl;
        m_tags[i].rect.height = TAG_HEIGHT;
        row.push_back(m_tags[i]);
        offset_x += width + TAG_MARGIN;
    }
    if(last_cell != m_tags.size())
    {
        m_visibleTags.push_back(row);
        nrows++;
    }
    SetRowCount(nrows);
}
