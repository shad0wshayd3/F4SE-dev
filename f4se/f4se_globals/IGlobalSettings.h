#pragma once

#include <unordered_map>

class IGlobalSettings {
public:
    static void                                     Open(std::string settingPrefix);

    static bool                                     GetBool(std::string settingName);
    static float                                    GetFloat(std::string settingName);

private:
    static std::unordered_map<std::string, UInt32>  m_SettingMap;
};
