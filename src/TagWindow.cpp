#include "TagWindow.h"
#include <wx/dc.h>
#include <wx/dcbuffer.h>


#define TAG_SPACE 10
#define TAG_PADDING 4
#define TAG_MARGIN 10
#define TAG_HEIGHT 24

TagWindow::TagWindow(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxString &name):
    wxVScrolledWindow(parent,id,pos,size,style,name),m_activeTagPos(0)
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
    std::cout<<"click at: "<<pt.x<<","<<pt.y<<std::endl;
    size_t iRowBegin = GetVisibleRowsBegin();
    size_t iRowEnd = GetVisibleRowsEnd();
    size_t  pos=0;
    int newActiveTagPos = -1;
    for(size_t iRow = 0; iRow<iRowBegin; iRow++){
       pos += m_visibleTags[iRow].size();
    }

    for(size_t iRow=iRowBegin; iRow<iRowEnd; iRow++)
    {
        for(size_t iCol =0; iCol<m_visibleTags[iRow].size(); iCol++)
        {
            Tag tag = m_visibleTags[iRow][iCol];
             wxRect visible_rect(tag.rect.x,tag.rect.y - (iRowBegin)*(TAG_HEIGHT+TAG_MARGIN),tag.rect.width,tag.rect.height);
            if(visible_rect.Contains(pt))
            {
                m_visibleTags[iRow][iCol].active = true;
                m_tags[pos].active = true;
                newActiveTagPos = pos;
                std::cout<<"新高亮: "<<pos<<std::endl;
                break;
            }
            pos += 1;
        }
    }

    std::cout<<"老高亮: "<<m_activeTagPos<<std::endl;

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
                    std::cout<<"之前高亮取消: " <<m_activeTagPos<<std::endl;
                    m_activeTagPos = newActiveTagPos;
                    break;
                }
                pos += 1;
            }
        }
    }
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
            if(!tag.active)
            {
                // 绘制圆角边框
                wxPen inactivePen(wxColor(0,0,0),1,wxPENSTYLE_SOLID);
                dc.SetPen(inactivePen);
                dc.DrawRoundedRectangle(visible_rect,2);
                // 居中绘制文本
                dc.SetTextForeground(wxColor(0,0,0));
                dc.DrawLabel(tag.name,visible_rect,wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL);
                //
            }
            else
            {
                // 绘制圆角边框
                wxPen activePen(wxColor(255,0,0),1,wxPENSTYLE_SOLID);
                dc.SetPen(activePen);
                dc.DrawRoundedRectangle(visible_rect,2);
                // 居中绘制文本
                dc.SetTextForeground(wxColor(255,0,0));
                dc.DrawLabel(tag.name,visible_rect,wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL);
            }
        }
    }
}


wxCoord TagWindow::OnGetRowHeight(size_t row) const
{
    return TAG_HEIGHT+TAG_MARGIN;
}

void TagWindow::AddTags(std::vector<std::string>& tags)
{
    for(auto& tag_name: tags)
    {
        Tag tag;
        tag.name = tag_name;
        tag.rect = wxRect(0,0,0,0);
        tag.active = false;
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
    ReLayout();
}

void TagWindow::RemoveTag(size_t pos)
{
    if(pos > m_tags.size() -1|| pos < 0 || m_tags.size() == 0)
    {
        return;
    }
    m_tags.erase(m_tags.begin()+pos);
    ReLayout();
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
        size_t width = dc.GetTextExtent(m_tags[i].name).GetWidth()+TAG_PADDING*2+24;
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
        m_tags[i].rect.width = width;
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
