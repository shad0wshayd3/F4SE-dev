#include "IGlobalSettings.h"

#include "ILog.h"

#include "f4se/GameData.h"
#include "f4se/GameRTTI.h"

void IGlobalSettings::Init(std::string GlobalPrefix) {
    tArray<TESForm*> Globals = (*g_dataHandler)->arrGLOB;

    for (int i = 0; i < Globals.count; i++) {
        std::string GlobalName = Globals[i]->GetEditorID();
            
        auto index = GlobalName.find(GlobalPrefix);
        if (index != std::string::npos)
            m_SettingMap.emplace(GlobalName.substr(GlobalPrefix.size(), std::string::npos), Globals[i]->formID);
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