#include "IGlobalSettings.h"

#include "f4se/GameData.h"
#include "f4se/GameRTTI.h"

// ------------------------------------------------------------------------------------------------
// IGlobalSettings
// ------------------------------------------------------------------------------------------------

void IGlobalSettings::Open(std::string settingPrefix) {
    tArray<TESForm*> Globals = (*g_dataHandler)->arrGLOB;

    for (int i = 0; i < Globals.count; i++) {
        std::string settingName = Globals[i]->GetEditorID();
            
        auto index = settingName.find(settingPrefix);
        if (index != std::string::npos)
            m_SettingMap.emplace(settingName.substr(settingPrefix.size(), std::string::npos), Globals[i]->formID);
    }
}

bool IGlobalSettings::GetBool(std::string settingName) {
    auto search = m_SettingMap.find(settingName);
    if (search != m_SettingMap.end()) {
        TESGlobal* Setting = DYNAMIC_CAST(LookupFormByID(search->second), TESForm, TESGlobal);
        if (Setting)
            return (Setting->value == 1.0);
    }

    return false;
}

float IGlobalSettings::GetFloat(std::string settingName) {
    auto search = m_SettingMap.find(settingName);
    if (search != m_SettingMap.end()) {
        TESGlobal* Setting = DYNAMIC_CAST(LookupFormByID(search->second), TESForm, TESGlobal);
        if (Setting)
            return Setting->value;
    }

    return 0.0;
}

// ------------------------------------------------------------------------------------------------
// Initialize
// ------------------------------------------------------------------------------------------------

std::unordered_map<std::string, UInt32> IGlobalSettings::m_SettingMap;