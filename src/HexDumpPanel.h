#ifndef HEXDUMPPANEL_H
#define HEXDUMPPANEL_H

#include <wx/colour.h>
#include <wx/vscroll.h>
#include <string>
#include <vector>

struct word {
    wxColor clr;
    std::string hex;
};

struct HilightAddr {
    int offset;
    int size;
    wxColor color;
};

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
    bool HilightWords(size_t pos, size_t word_count, wxColor background_clr = wxColor(255, 255, 0));
    void SetHilightAddrs(const std::vector<HilightAddr>& hilight_addrs);
    std::string ToHex(int num, int nWidth = 2);
    int GetHexDigits(int num);
    size_t GetOffsetX();
    size_t GetOffsetY();
    wxRect GetCellRect(size_t iRow, size_t iCol);
    void OnPaint(wxPaintEvent& event);
    virtual wxCoord OnGetRowHeight(size_t row) const;

   private:
    static std::vector<HilightAddr> m_hilightCells;
    std::string m_dirRoot;
    size_t m_visualSize;  // 最大显示的区域大小，默认等于PCIe 配置文件大小，允许动态调节
    size_t m_offsetX;
    std::vector<std::string> m_devices;
    std::vector<std::string> m_configFiles;
    std::vector<word> m_words;  // 16 进制数据
};

#endif  // HEXDUMPPANEL_H
