#include "ISettings.h"

#include "f4se_common/Utilities.h"

// ------------------------------------------------------------------------------------------------
// ISettingsPath
// ------------------------------------------------------------------------------------------------

ISettingsPath::ISettingsPath(std::string settingPath) {
    auto delimiter = settingPath.find_first_of(":");
    setting = settingPath.substr(delimiter + 1);
    section = settingPath.substr(0, delimiter);
}

const char* ISettingsPath::GetSection() {
    return section.c_str();
}

const char* ISettingsPath::GetSetting() {
    return setting.c_str();
}

// ------------------------------------------------------------------------------------------------
// ISettings
// ------------------------------------------------------------------------------------------------

void ISettings::Open(const char* filePath) {
    char tempConfigPath[MAX_PATH];
    sprintf_s(tempConfigPath, sizeof(tempConfigPath), "%s%s", GetRuntimeDirectory().c_str(), filePath);
    configPath = std::string(tempConfigPath);
}

int ISettings::GetInteger(std::string settingPath, int settingDefault) {
    ISettingsPath Path = ISettingsPath(settingPath);
    return GetPrivateProfileInt(Path.GetSection(), Path.GetSetting(), settingDefault, GetConfigPath());
}

float ISettings::GetFloat(std::string settingPath, float settingDefault) {
    ISettingsPath Path = ISettingsPath(settingPath);

    char Result[10], Default[10];
    sprintf_s(Default, sizeof(Default), "%f", settingDefault);
    GetPrivateProfileString(Path.GetSection(), Path.GetSetting(), Default, Result, sizeof(Result), GetConfigPath());
    return strtof(Result, NULL);
}

std::string ISettings::GetString(std::string settingPath, std::string settingDefault) {
    ISettingsPath Path = ISettingsPath(settingPath);

    char Result[1024];
    GetPrivateProfileString(Path.GetSection(), Path.GetSetting(), settingDefault.c_str(), Result, sizeof(Result), GetConfigPath());
    return std::string(Result);
}

bool ISettings::GetBool(std::string settingPath, bool settingDefault) {
    ISettingsPath Path = ISettingsPath(settingPath);
    return (GetPrivateProfileInt(Path.GetSection(), Path.GetSetting(), settingDefault, GetConfigPath()) >= 1);
}

void ISettings::SetInteger(std::string settingPath, int settingValue) {
    ISettingsPath Path = ISettingsPath(settingPath);

    char Value[10];
    sprintf_s(Value, sizeof(Value), "%i", settingValue);
    WritePrivateProfileString(Path.GetSection(), Path.GetSetting(), Value, GetConfigPath());
}

void ISettings::SetFloat(std::string settingPath, float settingValue) {
    ISettingsPath Path = ISettingsPath(settingPath);

    char Value[10];
    sprintf_s(Value, sizeof(Value), "%f", settingValue);
    WritePrivateProfileString(Path.GetSection(), Path.GetSetting(), Value, GetConfigPath());
}

void ISettings::SetString(std::string settingPath, std::string settingValue) {
    ISettingsPath Path = ISettingsPath(settingPath);
    WritePrivateProfileString(Path.GetSection(), Path.GetSetting(), settingValue.c_str(), GetConfigPath());
}

void ISettings::SetBool(std::string settingPath, bool settingValue) {
    ISettingsPath Path = ISettingsPath(settingPath);

    char Value[10];
    sprintf_s(Value, sizeof(Value), "%i", settingValue);
    WritePrivateProfileString(Path.GetSection(), Path.GetSetting(), Value, GetConfigPath());
}

// ------------------------------------------------------------------------------------------------
// Internal Functions
// ------------------------------------------------------------------------------------------------

const char* ISettings::GetConfigPath() {
    return configPath.c_str();
}

// ------------------------------------------------------------------------------------------------
// Initialize
// ------------------------------------------------------------------------------------------------

std::string ISettings::configPath = "";