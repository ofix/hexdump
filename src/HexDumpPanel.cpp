#include "HexDumpPanel.h"
#include "util/FileTool.h"
#include <wx/dcbuffer.h>
#include <wx/event.h>
#include <wx/colour.h>

///// 文件夹相关头文件
#include <dirent.h>
#include <sys/stat.h>
#include <cstring>
///// 16进制格式化相关头文件
#include <sstream>
#include <iomanip>  // 添加这一行

#ifdef _WIN32
#include <windows.h>
#endif

#define CELL_WIDTH 30
#define CELL_HEIGHT 30

#define DEFAULT_BACKGROUND wxColor(255,255,255)


HexDumpPanel::HexDumpPanel(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxString &name):
    wxVScrolledWindow(parent,id,pos,size,style,name)
{
    //ctor
    // 使用自动双缓冲
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT,&HexDumpPanel::OnPaint,this);
}

HexDumpPanel::~HexDumpPanel()
{
    //dtor
}

std::vector<std::string> HexDumpPanel::GetSubDirs(const std::string& directoryPath)
{
    std::vector<std::string> subDirectories;

    DIR* dir;
    struct dirent* ent;

    if ((dir = opendir(directoryPath.c_str()))!= NULL)
    {
        while ((ent = readdir(dir))!= NULL)
        {
            std::string entryName = ent->d_name;
            if (entryName!= "." && entryName!= "..")
            {
                std::string fullPath = directoryPath + "/" + entryName;

#ifdef _WIN32
                DWORD fileAttributes = GetFileAttributes(fullPath.c_str());
                if (fileAttributes!= INVALID_FILE_ATTRIBUTES && (fileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                {
#else
                struct stat st;
                if (stat(fullPath.c_str(), &st) == 0 && S_ISDIR(st.st_mode))
                {
#endif
                    subDirectories.push_back(entryName);
                }
            }
        }
        closedir(dir);
    }
    else
    {
        std::cerr << "无法打开目录: " << directoryPath << std::endl;
        return subDirectories;
    }

    return subDirectories;
}

void HexDumpPanel::SetPcieConfigDirRoot(const std::string dir_path)
{
    m_dirRoot = dir_path;
    // 查找所有子目录
    m_devices = GetSubDirs(m_dirRoot);
    for(auto& device: m_devices)
    {
        std::string config_file = m_dirRoot + "/"+device+"/config";
        m_configFiles.push_back(config_file);
    }
    if(m_configFiles.size() > 0)
    {
        LoadPcieConfigFile(m_configFiles[0]);
    }
    Refresh();
}

std::vector<std::string> HexDumpPanel::GetPcieDevices()
{
    return m_devices;
}

std::vector<std::string> HexDumpPanel::GetPcieConfigFiles()
{
    return m_configFiles;
}

std::string HexDumpPanel::GetConfigFilePath(size_t n){
   if(n>m_configFiles.size()-1){
      return "";
   }
   return m_configFiles[n];
}

void HexDumpPanel::SetVisualSize(size_t size)
{
    if(size <0 )
    {
        return ;
    }
    if(size > m_words.size())
    {
        m_visualSize = m_words.size();
    }
    else
    {
        m_visualSize = size;
    }
}

void HexDumpPanel::OnPaint(wxPaintEvent& event)
{
    wxAutoBufferedPaintDC dc(this);

    size_t iRowBegin = GetVisibleRowsBegin();
    size_t iRowEnd = GetVisibleRowsEnd();
    int nDigits = GetHexDigits(m_visualSize);
    for(size_t iRow=iRowBegin; iRow<iRowEnd; iRow++)
    {
        if(iRow==0)
        {
            for(size_t iCol=0; iCol<16; iCol++)
            {
                wxRect rc(GetOffsetX()+iCol*CELL_WIDTH,0,CELL_WIDTH,CELL_HEIGHT);
                dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
                dc.DrawLabel(std::to_string(iCol),rc,wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL);
            }
        }
        else
        {
            size_t iVisualRow = iRow - iRowBegin;
            std::string addr="0x"+ToHex((iRow-1)*16,nDigits);

            dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
            dc.SetTextForeground(wxColor(0,0,0));
            dc.DrawLabel(addr,wxRect(0,iVisualRow*30,GetOffsetX(),CELL_HEIGHT),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

            for(size_t iCol=0; iCol<16; iCol++)
            {
                word w=m_words[(iRow-1)*16+ iCol];
                wxRect cell = GetCellRect(iVisualRow-1,iCol);

                if(w.clr == DEFAULT_BACKGROUND)  // 白底黑字,没有高亮
                {
                    dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
                    dc.SetTextForeground(wxColor(0,0,0));
                }
                else
                {
                    wxBrush brush(w.clr);
                    dc.SetBrush(brush);
                    dc.SetPen(*wxTRANSPARENT_PEN);
                    dc.DrawRectangle(cell);
                    dc.SetTextBackground(w.clr);
                    dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
                    dc.SetTextForeground(wxColor(255,255,255));
                }

                dc.DrawLabel(w.hex,cell,wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL);
            }
        }
    }
    /*
      size_t iRow = 0;
      size_t iCol = 0;
      for(size_t i=0; i<m_visualSize; i++)
      {
          word w=m_words[i];
          iCol = i%16;
          if(iCol == 0)
          {
              std::string addr="0x"+ToHex(i);
              dc.SetBrush(*wxBLACK_BRUSH);
              dc.DrawLabel(addr,wxRect(0,iRow*30,20,30),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
              ///////
              wxRect cell = GetCellRect(iRow,iCol);
              wxBrush brush(w.clr);
              dc.SetBrush(brush);
              dc.DrawLabel(w.hex,cell,wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL);
              iRow++;
          }
          else
          {
              wxRect cell = GetCellRect(iRow,iCol);
              wxBrush brush(w.clr);
              dc.SetBrush(brush);
              dc.DrawLabel(w.hex,cell,wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL);
          }

      }
      */
}

size_t HexDumpPanel::GetOffsetX()
{
    static size_t offset_x = 20+GetHexDigits(m_visualSize)*10;
    return offset_x;
}

size_t HexDumpPanel::GetOffsetY()
{
    static size_t offset_y = 30;
    return offset_y;
}

wxCoord HexDumpPanel::OnGetRowHeight(size_t row) const
{
    return CELL_HEIGHT;
}

wxRect HexDumpPanel::GetCellRect(size_t iRow, size_t iCol)
{
    static size_t offset_x = GetOffsetX();
    static size_t offset_y = GetOffsetY();
    wxRect rc(offset_x + CELL_WIDTH* iCol, offset_y + CELL_HEIGHT* iRow, CELL_WIDTH,CELL_HEIGHT);
    return rc;
}

bool HexDumpPanel::LoadPcieConfigFile(const std::string& file_path)
{
    std::string data = FileTool::LoadFile(file_path);
    m_words.clear();
    m_words.reserve(data.length());
    for (char c : data)
    {
        word w;
        w.hex = ToHex(static_cast<int>(c) & 0xFF);
        w.clr = wxColor(255,255,255);
        m_words.push_back(w);
    }
    m_visualSize = m_words.size();
    size_t rows = m_visualSize / 16;
    SetRowCount(rows+1);
    for(HilightAddr& cell: m_hilightCells){
      HilightWords(cell.offset,cell.size,cell.color);
    }
    return true;
}

std::vector<HilightAddr> HexDumpPanel::m_hilightCells = {};

void HexDumpPanel::SetHilightAddrs(const std::vector<HilightAddr>& hilight_addrs){
   m_hilightCells = hilight_addrs;
   for(HilightAddr& cell: m_hilightCells){
      HilightWords(cell.offset,cell.size,cell.color);
   }
}

bool HexDumpPanel::HilightWords(size_t pos,size_t word_count,wxColor background_clr)
{
    if(pos <0 || pos > m_words.size() -1)
    {
        return false;
    }
    size_t end_pos = pos+word_count -1;
    if(end_pos < 0 || end_pos > m_words.size() -1 )
    {
        return  false;
    }
    for(size_t i=pos; i< pos+word_count; i++)
    {
        m_words[i].clr = background_clr;
    }
    return true;
}

std::string HexDumpPanel::ToHex(int num,int nWidth)
{
    std::stringstream ss;
    ss << std::uppercase << std::hex << std::setfill('0') <<std::setw(nWidth) << num;
    std::string result;
    ss >> result;
    return result;
}

int HexDumpPanel::GetHexDigits(int num)
{
    int count = 0;
    if (num == 0)
    {
        return 1;
    }
    while (num!= 0)
    {
        num >>= 4;  // 每次右移 4 位，相当于除以 16
        count++;
    }
    return count;
}


