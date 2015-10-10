#ifndef GeneralLunaConfig_hhhhh
#define GeneralLunaConfig_hhhhh

#include <string>

class GeneralLunaConfig
{
    bool m_renderer_useLetterbox;

    std::wstring m_settingFilename;
public:
    GeneralLunaConfig();
    ~GeneralLunaConfig();

    void doDefaults();
    bool save();
    
    void loadOrDefault();
    bool load();

    inline void setFilename(const std::wstring& filename) { m_settingFilename = filename; }
    inline std::wstring filename() const { return m_settingFilename; }

    inline void setRendererUseLetterbox(bool rendererUseLetterbox) { m_renderer_useLetterbox = rendererUseLetterbox; }
    inline bool getRendererUseLetterbox() const { return m_renderer_useLetterbox; }
};

#endif