#ifndef GeneralLunaConfig_hhhhh
#define GeneralLunaConfig_hhhhh

#include <string>

class GeneralLunaConfig
{
public:
    enum GLMode
    {
        GLModeAuto=0,
        GLModeSoft,
        GLModeHard
    };

    enum VSyncMode
    {
        VSyncModeOff = 0,
        VSyncModeOn,
        VSyncModeAdaptive
    };

private:
    GLMode      m_renderer_opengl;
    VSyncMode   m_renderer_vsync;
    bool        m_renderer_useLetterbox;
    bool        m_renderer_forceDisableFullscreen;
    int         m_audio_samplerate;
    int         m_audio_bufferlen;
    std::string m_audio_driver;
    bool        m_lua_enable_http;

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

    inline GLMode getRendererOpenGL() const { return m_renderer_opengl; }
    inline VSyncMode getRendererVSync() const { return m_renderer_vsync; }
    inline void setRendererUseLetterbox(bool rendererUseLetterbox) { m_renderer_useLetterbox = rendererUseLetterbox; }
    inline bool getRendererUseLetterbox() const { return m_renderer_useLetterbox; }
    inline void setForceDisableFullscreen(bool forceDisableFullscreen) { m_renderer_forceDisableFullscreen = forceDisableFullscreen; }
    inline bool getForceDisableFullscreen() const { return m_renderer_forceDisableFullscreen; }
    inline int getAudioSampleRate() const { return m_audio_samplerate; }
    inline int getAudioBufferLength() const { return m_audio_bufferlen; }
    inline std::string getAudioDriver() const { return m_audio_driver; }
    bool getLuaEnableHTTP() const { return m_lua_enable_http; }
    void getLuaEnableHTTP(bool val) { m_lua_enable_http = val; }
};

#endif