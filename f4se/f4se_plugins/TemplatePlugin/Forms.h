#pragma once

#include "Config.h"
#include "f4se/PluginAPI.h"
#include "f4se_globals/Globals.h"

extern PluginHandle                     g_PluginHandle;
extern F4SEMessagingInterface*          g_Messaging;
extern F4SEPapyrusInterface*            g_Papyrus;
extern F4SEScaleformInterface*          g_Scaleform;
extern F4SESerializationInterface*      g_Serialization;
extern F4SETaskInterface*               g_Tasks;

class Forms {
public:
    static bool Load();
    static void Unload();

private:
    static bool CheckLoadedForms();
};