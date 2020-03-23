#pragma once

#include "Config.h"
#include "f4se/GameThreads.h"
#include "f4se/PluginAPI.h"
#include "f4se_globals/Globals.h"

extern PluginHandle             g_PluginHandle;
extern F4SEMessagingInterface*  g_Messaging;
extern F4SEScaleformInterface*  g_Scaleform;
extern F4SETaskInterface*       g_Tasks;

extern void GameLoadedCallback();

class Forms {
public:
    static bool Load();
    static void Unload();

    static ActorValueInfo*  PipboyLightActive;
    static ActorValueInfo*  RadiationSourceCount;
    static ActorValueInfo*  InventoryWeight;

private:
    static bool CheckLoadedForms();
};
