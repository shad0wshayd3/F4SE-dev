#include "FormLoader.h"

#include "ISettings.h"

void FormLoader::SetPluginName(const char* pluginName) {
    m_PluginName = ISettings::GetString("General:PluginName", pluginName);
}

std::string FormLoader::GetPluginName() {
    return m_PluginName;
}

bool FormLoader::OpenPlugin() {
    m_PluginInfo = (*g_dataHandler)->LookupModByName(m_PluginName.c_str());
    if (m_PluginInfo)
        if (m_PluginInfo->IsActive())
            return true;

    return false;
}

TESForm* FormLoader::LookupFromBase(UInt32 FormID) {
    return LookupFormByID(FormID);
}

TESForm* FormLoader::LookupFromBase(UInt32 FormID, std::string settingName) {
    return LookupFormByID(ISettings::GetInteger(settingName, FormID));
}

TESForm* FormLoader::LookupFromName(std::string pluginName, UInt32 FormID) {
    const ModInfo* pluginInfo = (*g_dataHandler)->LookupModByName(pluginName.c_str());
    if (pluginInfo)
        if (pluginInfo->IsActive())
            return LookupFormByID(pluginInfo->GetFormID(FormID));
    return nullptr;
}

TESForm* FormLoader::LookupFromName(std::string pluginName, UInt32 FormID, std::string settingName) {
    const ModInfo* pluginInfo = (*g_dataHandler)->LookupModByName(pluginName.c_str());
    if (pluginInfo)
        if (pluginInfo->IsActive())
            return LookupFormByID(pluginInfo->GetFormID(ISettings::GetInteger(settingName, FormID)));
    return nullptr;
}

TESForm* FormLoader::LookupFromPlugin(UInt32 FormID) {
    return LookupFormByID(m_PluginInfo->GetFormID(FormID));
}

TESForm* FormLoader::LookupFromPlugin(UInt32 FormID, std::string settingName) {
    return LookupFormByID(m_PluginInfo->GetFormID(ISettings::GetInteger(settingName, FormID)));
}

// Static Defines
std::string FormLoader::m_PluginName;
const ModInfo* FormLoader::m_PluginInfo;