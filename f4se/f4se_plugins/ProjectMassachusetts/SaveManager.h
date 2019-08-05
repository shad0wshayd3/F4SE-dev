#pragma once
#include "DataManager.h"

class SaveManager : private DataManager {
public:
    static bool Hook_Commit(PluginHandle g_pluginHandle);

    static void RevertCallback(const F4SESerializationInterface* intfc);
    static void LoadCallback(const F4SESerializationInterface* intfc);
    static void SaveCallback(const F4SESerializationInterface* intfc);
};