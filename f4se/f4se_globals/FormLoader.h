#pragma once

#include "f4se/GameData.h"
#include "f4se_globals/Utilities.h"

#define CheckForm(Form, FormType, Name)                                                             \
    if (##Form##) {                                                                                 \
        _LogMessageNT("Variable: %s", #Form);                                                       \
        _DebugIndent();                                                                             \
        _DebugMessageNT("%s is FormType: %i. Should be %i.", #Form, ##Form##->formType, FormType);  \
        if (##Form##->formType == ##FormType##) {                                                   \
            _DebugMessageNT("%s reporting name as: %s", #Form, ##Form##->##Name##);                 \
        }                                                                                           \
        _DebugOutdent();                                                                            \
        _DebugMessageNT("");                                                                        \
    } else {                                                                                        \
        return false;                                                                               \
    }

#define ToType(Form, Type) (##Type*)Runtime_DynamicCast((void*)(Form), RTTI_TESForm, RTTI_##Type)

class FormLoader {
public:
    static void SetPluginName(const char* pluginName);
    static std::string GetPluginName();

    static bool OpenPlugin();

    static TESForm* LookupFromBase(UInt32 FormID);
    static TESForm* LookupFromBase(UInt32 FormID, std::string settingName);

    static TESForm* LookupFromName(std::string pluginName, UInt32 FormID);
    static TESForm* LookupFromName(std::string pluginName, UInt32 FormID, std::string settingName);

    static TESForm* LookupFromPlugin(UInt32 FormID);
    static TESForm* LookupFromPlugin(UInt32 FormID, std::string settingName);

private:
    static std::string m_PluginName;
    static const ModInfo* m_PluginInfo;
};
