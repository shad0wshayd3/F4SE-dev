#pragma once

#include <string>
#include <unordered_map>

class ISettingsPath {
public:
    ISettingsPath(std::string settingPath);

    const char* GetSection();
    const char* GetSetting();

private:
    std::string section;
    std::string setting;
};

class ISettings {
public:
    static void Open(const char* filePath);

    static int GetInteger(std::string settingPath, int settingDefault);
    static float GetFloat(std::string settingPath, float settingDefault);
    static std::string GetString(std::string settingPath, std::string settingDefault);
    static bool GetBool(std::string settingPath, bool settingDefault);

    static void SetInteger(std::string settingPath, int settingValue);
    static void SetFloat(std::string settingPath, float settingValue);
    static void SetString(std::string settingPath, std::string settingValue);
    static void SetBool(std::string settingPath, bool settingValue);

private:
    static const char* GetConfigPath();

    static std::string configPath;
};