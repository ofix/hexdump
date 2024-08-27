#ifndef HEXDUMPPANEL_H
#define HEXDUMPPANEL_H

#include <wx/colour.h>
#include <wx/vscroll.h>
#include <string>
#include <vector>

struct word {
    wxColor clr;
    wxColor bk_clr;
    std::string hex;
};

struct HilightAddr {
    int offset;
    int size;
    wxColor bkColor;
    wxColor color;
};

struct BezierCurve{
    wxPoint ptStart;
    wxPoint ptEnd;
    wxPoint ptCtrl1;
    wxPoint ptCtrl2;
};

#define DEFAULT_HILIGHT_BKGROUND_COLOR wxColor(249, 219, 249)
#define DEFAULT_HILIGHT_COLOR wxColor(214, 62, 195)

class HexDumpPanel : public wxVScrolledWindow {
   public:
    HexDumpPanel(wxWindow* parent,
                 wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxTAB_TRAVERSAL,
                 const wxString& name = wxPanelNameStr);
    virtual ~HexDumpPanel();
    void SetPcieConfigDirRoot(const std::string dir_path = "/sys/bus/pci/devices");
    bool LoadPcieConfigFile(const std::string& file_path);
    std::vector<std::string> GetSubDirs(const std::string& directoryPath);
    std::vector<std::string> GetPcieDevices();
    std::vector<std::string> GetPcieConfigFiles();
    std::string GetConfigFilePath(size_t n);
    void SetVisualSize(size_t size);
    bool HilightWords(size_t pos, size_t word_count, wxColor background_clr = DEFAULT_HILIGHT_BKGROUND_COLOR,wxColor foreground_clr = DEFAULT_HILIGHT_COLOR);
    void SetHilightAddrs(const std::vector<HilightAddr>& hilight_addrs);
    std::string ToHex(int num, int nWidth = 2);
    int GetHexDigits(int num);
    size_t GetOffsetX();
    size_t GetOffsetY();
    wxRect GetCellRect(size_t iRow, size_t iCol);
    wxRect GetCellRect(size_t byte_offset);
    void OnPaint(wxPaintEvent& event);
    void OnDblClick(wxMouseEvent& event);
    void DrawTriangle(wxDC* pDC,wxPoint* ptEnd, wxPoint* pt1, wxPoint* pt2);
    void GetTrianglePoints(wxPoint& ptStart, wxPoint& ptEnd, wxPoint& pt1, wxPoint& pt2);
    virtual wxCoord OnGetRowHeight(size_t row) const;

    // 使用贝塞尔曲线绘制连接线
    static void SetJoinCurveBase(int i);
    void CalcJoinCurves();
    uint8_t GetHexVal(size_t offset);
    uint8_t GetPcieCapId(size_t offset);
    uint8_t GetPcieCapPtr(size_t offset);
    void hex_output(int id);
    std::vector<uint8_t> GetPhysicalSlot();

   private:
    static std::vector<HilightAddr> m_hilightCells;
    static int m_join_curve_base; // 连接线起始位置
    std::vector<BezierCurve> m_join_curves; // 需要绘制的贝塞尔曲线
    std::string m_dirRoot;
    size_t m_visualSize;  // 最大显示的区域大小，默认等于PCIe 配置文件大小，允许动态调节
    size_t m_offsetX;
    std::vector<std::string> m_devices;
    std::vector<std::string> m_configFiles;
    std::vector<word> m_words;  // 16 进制数据
    std::vector<uint8_t> m_slot;
};

#endif  // HEXDUMPPANEL_H
