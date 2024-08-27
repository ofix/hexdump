#include "HexDumpPanel.h"
#include "util/FileTool.h"
#include <algorithm>
#include <iostream>
#include <wx/colour.h>
#include <wx/dcbuffer.h>
#include <wx/event.h>
#include <wx/graphics.h>
#include <wx/dcgraph.h>
#include "util/Global.h"

///// 文件夹相关头文件
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>

///// 16进制格式化相关头文件
#include <iomanip> // 添加这一行
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#endif

#define CELL_WIDTH 30
#define CELL_HEIGHT 30

#define DEFAULT_BACKGROUND wxColor(255, 255, 255)

HexDumpPanel::HexDumpPanel(wxWindow *parent, wxWindowID id, const wxPoint &pos,
                           const wxSize &size, long style, const wxString &name)
    : wxVScrolledWindow(parent, id, pos, size, style, name) {
  m_slot = {};
  //  使用自动双缓冲
  SetBackgroundStyle(wxBG_STYLE_PAINT);
  Bind(wxEVT_PAINT, &HexDumpPanel::OnPaint, this);
  Bind(wxEVT_LEFT_DCLICK, & HexDumpPanel::OnDblClick,this);
}

HexDumpPanel::~HexDumpPanel() {
  // dtor
}

int HexDumpPanel::m_join_curve_base = -1;
std::vector<HilightAddr> HexDumpPanel::m_hilightCells = {};

std::vector<std::string>
HexDumpPanel::GetSubDirs(const std::string &directoryPath) {
  std::vector<std::string> subDirectories;
#ifdef _WIN32
  subDirectories.push_back("0000_00_01.0");
  // 文件句柄
  intptr_t hFile = -1;
  // 文件信息
  struct _finddata_t fileinfo; // 很少用的文件信息读取结构
  std::string p = directoryPath + "\\*";
  if ((hFile = _findfirst(p.c_str(), &fileinfo)) != -1) {
    do {
      if ((fileinfo.attrib & _A_SUBDIR)) // 判断是否为文件夹
      {
        if (strcmp(fileinfo.name, ".") != 0 &&
            strcmp(fileinfo.name, "..") != 0) {
          subDirectories.push_back(fileinfo.name); // 保存文件夹名字
        }
      }
    } while (_findnext(hFile, &fileinfo) == 0); // 寻找下一个，成功返回0，否则-1
    _findclose(hFile);
  }
#else
  DIR *dir;
  struct dirent *ent;

  if ((dir = opendir(directoryPath.c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      std::string entryName = ent->d_name;
      if (entryName != "." && entryName != "..") {
        std::string fullPath = directoryPath + "/" + entryName;
        struct stat st;
        if (stat(fullPath.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
          subDirectories.push_back(entryName);
        }
      }
    }
    closedir(dir);
  } else {
    std::cerr << "无法打开目录: " << directoryPath << std::endl;
    return subDirectories;
  }
#endif
  return subDirectories;
}

void HexDumpPanel::OnDblClick(wxMouseEvent& event){
  std::cout<<"panel double clicked!"<<std::endl;
  wxCommandEvent commandEvent(wxEVT_LEFT_DCLICK, GetId());
  commandEvent.SetEventObject(this);
  wxPostEvent(GetParent(), commandEvent);
  event.Skip();
}

std::vector<uint8_t> HexDumpPanel::GetPhysicalSlot(){
    return m_slot;
}

void HexDumpPanel::SetPcieConfigDirRoot(const std::string dir_path) {
  m_dirRoot = dir_path;
  // 查找所有子目录
  std::vector<std::string> sub_dirs = GetSubDirs(m_dirRoot);

  for (auto &sub_dir : sub_dirs) {
    std::string prefix = sub_dir.substr(0, 4);
    // if (prefix == "0000" || prefix == "0001" || prefix == "0002") {
      std::cout<<sub_dir<<std::endl;
      m_devices.push_back(sub_dir);
    //}
  }


  std::sort(m_devices.begin(), m_devices.end(),
            [](const std::string &a, const std::string &b) {
              std::vector<std::string> partsA, partsB;
              size_t startA = 0, startB = 0;
              size_t endA, endB;
              while ((endA = a.find(':', startA)) != std::string::npos) {
                partsA.push_back(a.substr(startA, endA - startA));
                startA = endA + 1;
              }
              partsA.push_back(a.substr(startA));

              while ((endB = b.find(':', startB)) != std::string::npos) {
                partsB.push_back(b.substr(startB, endB - startB));
                startB = endB + 1;
              }
              partsB.push_back(b.substr(startB));

              for (size_t i = 0; i < partsA.size() && i < partsB.size(); ++i) {
                if (partsA[i] < partsB[i])
                  return true;
                if (partsA[i] > partsB[i])
                  return false;
              }
              return partsA.size() < partsB.size();
            });

  for (auto &device : m_devices) {
    std::string config_file = m_dirRoot + "/" + device + "/config";
    m_configFiles.push_back(config_file);
  }
  if (m_configFiles.size() > 0) {
    LoadPcieConfigFile(m_configFiles[0]);
  }
  Refresh();
}

std::vector<std::string> HexDumpPanel::GetPcieDevices() { return m_devices; }

std::vector<std::string> HexDumpPanel::GetPcieConfigFiles() {
  return m_configFiles;
}

std::string HexDumpPanel::GetConfigFilePath(size_t n) {
  if (n > m_configFiles.size() - 1) {
    return "";
  }
  return m_configFiles[n];
}

void HexDumpPanel::SetVisualSize(size_t size) {
  if (size < 0) {
    return;
  }
  if (size > m_words.size()) {
    m_visualSize = m_words.size();
  } else {
    m_visualSize = size;
  }
}

void HexDumpPanel::OnPaint(wxPaintEvent &event) {
  wxAutoBufferedPaintDC dc(this);

  size_t iRowBegin = GetVisibleRowsBegin();
  size_t iRowEnd = GetVisibleRowsEnd();

#ifdef _WIN32
  dc.SetBrush(*wxWHITE_BRUSH);
  dc.DrawRectangle(-1, -1, GetSize().GetWidth(), GetRowCount() * CELL_HEIGHT);
#endif // _WIN32

  int nDigits = GetHexDigits(m_visualSize);
  // wxFont boldFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
  // wxFONTWEIGHT_BOLD); wxFont normalFont = dc.GetFont();
  for (size_t iRow = iRowBegin; iRow < iRowEnd; iRow++) {
    if (iRow == 0) {
      for (size_t iCol = 0; iCol < 16; iCol++) {
        wxRect rc(GetOffsetX() + iCol * CELL_WIDTH, 0, CELL_WIDTH, CELL_HEIGHT);
        dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
        dc.DrawLabel(std::to_string(iCol), rc,
                     wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL);
      }
    } else {
      size_t iVisualRow = iRow - iRowBegin;
      std::string addr = "0x" + ToHex((iRow - 1) * 16, nDigits);

      dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
      dc.SetTextForeground(wxColor(0, 0, 0));

      dc.DrawLabel(addr, wxRect(0, iVisualRow * 30, GetOffsetX(), CELL_HEIGHT),
                   wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

      for (size_t iCol = 0; iCol < 16; iCol++) {
        word w = m_words[(iRow - 1) * 16 + iCol];
        wxRect cell = GetCellRect(iVisualRow - 1, iCol);

        if (w.clr == DEFAULT_BACKGROUND) { // 白底黑字,没有高亮
          dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
          dc.SetTextForeground(wxColor(0, 0, 0));
        } else {
          wxBrush brush(w.bk_clr);
          dc.SetBrush(brush);
          dc.SetPen(*wxTRANSPARENT_PEN);
          dc.DrawRectangle(cell);
          dc.SetTextBackground(w.clr);
          dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
          dc.SetTextForeground(w.clr);
        }

        dc.DrawLabel(w.hex, cell, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL);
      }
    }
  }

  // 绘制贝塞尔曲线
  if (m_join_curves.size() > 0) {
    wxGraphicsContext *gc = wxGraphicsContext::Create( dc );
    if (gc) {
      wxPen redPen(DEFAULT_HILIGHT_COLOR, 1, wxPENSTYLE_SOLID);
      gc->SetPen(redPen);
      wxGraphicsPath path = gc->CreatePath();
      dc.SetPen(wxPen(DEFAULT_HILIGHT_COLOR,1,wxPENSTYLE_SOLID));
      dc.SetBrush(wxBrush(DEFAULT_HILIGHT_COLOR));

      int offsetY = iRowBegin * CELL_HEIGHT;
      for (BezierCurve &curve : m_join_curves) {
        path.MoveToPoint(curve.ptStart.x, curve.ptStart.y - offsetY);
        path.AddCurveToPoint(curve.ptCtrl1.x, curve.ptCtrl1.y - offsetY,
                             curve.ptCtrl2.x, curve.ptCtrl2.y - offsetY,
                             curve.ptEnd.x, curve.ptEnd.y - offsetY);
        gc->StrokePath(path);
        // 绘制箭头
        wxPoint pt1, pt2,ptStart,ptEnd;
        ptStart.x= curve.ptEnd.x;
        ptStart.y = curve.ptEnd.y - offsetY - 2-6;
        ptEnd.x = curve.ptEnd.x;
        ptEnd.y = curve.ptEnd.y - offsetY-2+ CELL_HEIGHT/2-6;
        GetTrianglePoints(ptStart,ptEnd,pt1,pt2);
        //std::cout<<std::dec<<"triangle: "<<ptEnd.x<<","<<ptEnd.y<<","<<pt1.x<<","<<pt1.y<<","<<pt2.x<<","<<pt2.y<<std::endl;
        DrawTriangle(&dc,&ptEnd,&pt1,&pt2);
      }
      delete gc;
    }
  }
}

void HexDumpPanel::DrawTriangle(wxDC* pDC,wxPoint* ptEnd, wxPoint* pt1, wxPoint* pt2){

   wxPointList* pPtList = new wxPointList();
   pPtList->Append(pt1);
   pPtList->Append(pt2);
   pPtList->Append(ptEnd);
   pDC->DrawPolygon(pPtList);
   // pDC->DrawLine(pt1->x,pt1->y,ptEnd->x,ptEnd->y);
   // pDC->DrawLine(pt2->x,pt2->y,ptEnd->x,ptEnd->y);
}

void HexDumpPanel::GetTrianglePoints(wxPoint& ptStart, wxPoint& ptEnd, wxPoint& pt1, wxPoint& pt2){
    double arrow_length = 8; // 斜线的长度
    double arrow_degree = 25*M_PI/180.0;
    double angle = atan2 (ptEnd.y - ptStart.y, ptEnd.x - ptStart.x) + M_PI;

    pt1.x = ptEnd.x + arrow_length * cos(angle - arrow_degree);
    pt1.y = ptEnd.y + arrow_length * sin(angle - arrow_degree);
    pt2.x = ptEnd.x + arrow_length * cos(angle + arrow_degree);
    pt2.y = ptEnd.y + arrow_length * sin(angle + arrow_degree);
}

size_t HexDumpPanel::GetOffsetX() {
  static size_t offset_x = 20 + GetHexDigits(m_visualSize) * 10;
  return offset_x;
}

size_t HexDumpPanel::GetOffsetY() {
  static size_t offset_y = 30;
  return offset_y;
}

wxCoord HexDumpPanel::OnGetRowHeight(size_t row) const { return CELL_HEIGHT; }

wxRect HexDumpPanel::GetCellRect(size_t iRow, size_t iCol) {
  static size_t offset_x = GetOffsetX();
  static size_t offset_y = GetOffsetY();
  wxRect rc(offset_x + CELL_WIDTH * iCol, offset_y + CELL_HEIGHT * iRow,
            CELL_WIDTH, CELL_HEIGHT);
  return rc;
}

wxRect HexDumpPanel::GetCellRect(size_t byte_offset) {
  size_t iRow = byte_offset / 16;
  size_t iCol = byte_offset % 16;
  std::cout<<"(i,j)= "<<iRow<<","<<iCol<<std::endl;
  return GetCellRect(iRow, iCol);
}

bool HexDumpPanel::LoadPcieConfigFile(const std::string &file_path) {
  std::string data = FileTool::LoadFile(file_path);
  m_words.clear();
  m_words.reserve(data.length());
  for (char c : data) {
    word w;
    w.hex = ToHex(static_cast<int>(c) & 0xFF);
    w.clr = wxColor(255, 255, 255);
    m_words.push_back(w);
  }
  m_visualSize = m_words.size();
  size_t rows = m_visualSize / 16;
  SetRowCount(rows + 1);
  for (HilightAddr &cell : m_hilightCells) {
    HilightWords(cell.offset, cell.size, cell.bkColor,cell.color);
  }
  if (m_join_curve_base != -1) {
    CalcJoinCurves();
  }
  // 高亮设备type=1的三个关键字段 Primary Bus Number / Secondary Bus Number / Subordinate Bus Number
  std::string type = m_words.at(0x0e).hex;
  if(type == "01"){
     HilightWords(0x19,3,wxColor(162,191,189),wxColor(66,92,90));
  }
  return true;
}

void HexDumpPanel::SetHilightAddrs(
    const std::vector<HilightAddr> &hilight_addrs) {
  m_hilightCells = hilight_addrs;
  for (HilightAddr &cell : m_hilightCells) {
    HilightWords(cell.offset, cell.size, cell.bkColor,cell.color);
  }
}

bool HexDumpPanel::HilightWords(size_t pos, size_t word_count,
                                wxColor background_clr,wxColor foreground_clr) {
  if (pos < 0 || pos > m_words.size() - 1) {
    return false;
  }
  size_t end_pos = pos + word_count - 1;
  if (end_pos < 0 || end_pos > m_words.size() - 1) {
    return false;
  }
  for (size_t i = pos; i < pos + word_count; i++) {
    m_words[i].bk_clr = background_clr;
    m_words[i].clr = foreground_clr;
  }
  return true;
}

std::string HexDumpPanel::ToHex(int num, int nWidth) {
  std::stringstream ss;
  ss << std::uppercase << std::hex << std::setfill('0') << std::setw(nWidth)
     << num;
  std::string result;
  ss >> result;
  return result;
}

int HexDumpPanel::GetHexDigits(int num) {
  int count = 0;
  if (num == 0) {
    return 1;
  }
  while (num != 0) {
    num >>= 4; // 每次右移 4 位，相当于除以 16
    count++;
  }
  return count;
}

void HexDumpPanel::SetJoinCurveBase(int addr) { m_join_curve_base = addr; }

uint8_t HexDumpPanel::GetHexVal(size_t offset) {
  std::string hex = m_words[offset].hex;
  return hex_to_dec<uint8_t>(hex);
}

uint8_t HexDumpPanel::GetPcieCapId(size_t offset) { return GetHexVal(offset); }

uint8_t HexDumpPanel::GetPcieCapPtr(size_t offset) { return GetHexVal(offset); }

void HexDumpPanel::hex_output(int id){
	std::cout<< std::hex << std::setiosflags(std::ios::uppercase) << std::setw(2) << std::setfill('0')
                      << static_cast<int>(id);
}

void HexDumpPanel::CalcJoinCurves() {
  if (m_join_curve_base == -1) {
    return;
  }
  m_join_curves.clear();
  std::vector<wxRect> m_rects;
  // 开始获取链表所有元素
  uint8_t cap_ptr = GetPcieCapPtr(m_join_curve_base); // 0x34
  uint16_t next_ptr = 0;
  uint8_t cap_id = 0;
  //uint16_t slot = 0xFFFF; // 最大值表示没有找到物理槽位
  wxRect rect_src = GetCellRect(m_join_curve_base);
  m_rects.push_back(rect_src);
  HilightWords(m_join_curve_base,1,DEFAULT_HILIGHT_BKGROUND_COLOR,DEFAULT_HILIGHT_COLOR);
  m_slot.clear();
  do {
    cap_id = GetPcieCapId(cap_ptr);        // 0x50
    next_ptr = GetPcieCapPtr(cap_ptr + 1); // 0x70
    HilightWords(cap_ptr,2,DEFAULT_HILIGHT_BKGROUND_COLOR,DEFAULT_HILIGHT_COLOR);
    hex_output(cap_id);
	 std::cout<<",";
    hex_output(next_ptr);
    std::cout<<std::endl;

    if (cap_id != 0x10 && next_ptr != 0x0) {
      wxRect rect_dst = GetCellRect(cap_ptr);
      wxRect rect_next_src = GetCellRect(cap_ptr+1);
      m_rects.push_back(rect_dst);
      m_rects.push_back(rect_next_src);
      cap_ptr = next_ptr;
    } else {
      wxRect rect_dst = GetCellRect(cap_ptr);
      m_rects.push_back(rect_dst);
      if (cap_id == 0x10) {
        HilightWords(cap_ptr+0x14,4,wxColor(193, 154, 234),wxColor(33, 4, 64));
        m_slot.push_back(GetHexVal(cap_ptr+0x14));
        m_slot.push_back(GetHexVal(cap_ptr+0x15));
        m_slot.push_back(GetHexVal(cap_ptr+0x16));
        m_slot.push_back(GetHexVal(cap_ptr+0x17));
        break;
        //uint32_t cap_slot = pciecfg_get32(pcfg, cap_ptr + 0x14);
        //slot = (cap_slot >> 18) & 0x1FFF; // 31位~19位为物理槽位
      }
    }
  } while (next_ptr != 0);

  // 计算贝塞尔曲线控制点

  for (size_t i = 0; i < m_rects.size();) {
    BezierCurve curve;
    // 贝塞尔曲线起点
    curve.ptStart.x = m_rects[i].x + m_rects[i].width / 2;
    curve.ptStart.y = m_rects[i].y + m_rects[i].height/2;
    // 贝塞尔曲线终点
    curve.ptEnd.x = m_rects[i + 1].x + m_rects[i + 1].width / 2;
    curve.ptEnd.y = m_rects[i + 1].y + 2;

    // 贝塞尔曲线控制点1
    curve.ptCtrl1.x = curve.ptStart.x;
    curve.ptCtrl1.y = curve.ptStart.y + (curve.ptEnd.y - curve.ptStart.y) / 2;
    // 贝塞尔曲线控制点2
    curve.ptCtrl2.x = curve.ptEnd.x;
    curve.ptCtrl2.y = curve.ptEnd.y - (curve.ptEnd.y - curve.ptStart.y) / 2;
    i += 2;
    m_join_curves.push_back(curve);
  }
}
