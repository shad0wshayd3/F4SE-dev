#pragma once

#include <unordered_map>

class IGlobalSettings {
public:
    void                                        Init(std::string GlobalPrefix);

    bool                                        GetBool(std::string settingName);
    float                                       GetFloat(std::string settingName);

private:
    std::unordered_map<std::string, UInt32>     m_SettingMap;
};

extern IGlobalSettings g_GlobalSettings;
