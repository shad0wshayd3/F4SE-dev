#pragma once

#include "f4se/PluginAPI.h"

namespace Serialization {
    bool Hook_Init(F4SESerializationInterface* SerializationInterface);
    bool Hook_Commit(PluginHandle g_PluginHandle);

    void RevertCallback(const F4SESerializationInterface* intfc);
    void LoadCallback(const F4SESerializationInterface* intfc);
    void SaveCallback(const F4SESerializationInterface* intfc);
}
