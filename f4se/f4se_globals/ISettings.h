#pragma once

#include <string>

class ISettingsPath {
public:
                    ISettingsPath(std::string settingPath);
                    ~ISettingsPath();

    const char*     GetSection();
    const char*     GetSetting();

private:
    std::string     section;
    std::string     setting;
};

class ISettings {
public:
                    ISettings(const char* filePath);
                    ~ISettings();

    int             GetInteger(std::string settingPath, int settingDefault);
    float           GetFloat(std::string settingPath, float settingDefault);
    std::string     GetString(std::string settingPath, std::string settingDefault);
    bool            GetBool(std::string settingPath, bool settingDefault);

    void            SetInteger(std::string settingPath, int settingValue);
    void            SetFloat(std::string settingPath, float settingValue);
    void            SetString(std::string settingPath, std::string settingValue);
    void            SetBool(std::string settingPath, bool settingValue);

private:
    const char*     GetConfigPath();
    std::string     configPath;
};

extern ISettings g_Settings;