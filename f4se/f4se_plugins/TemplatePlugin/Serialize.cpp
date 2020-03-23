#include "Serialize.h"

void RevertCallback(const F4SESerializationInterface* intfc) {

}

void LoadCallback(const F4SESerializationInterface* intfc) {
    UInt32 type, version, length;

    while (intfc->GetNextRecordInfo(&type, &version, &length)) {
        // switch (type) {
        // case 'EXPL':
        //     if (length == sizeof(UInt16))
        //         intfc->ReadRecordData(0, sizeof(UInt16));
        //     break;
        // }
    }
}

void SaveCallback(const F4SESerializationInterface* intfc) {
    // if (g_Serialization->OpenRecord('EXPL', 1))
    //     g_Serialization->WriteRecordData(0, sizeof(UInt16));
}

bool Serialize::Hook_Commit() {
    if (g_Serialization) {
        // g_Serialization->SetUniqueID(g_PluginHandle, 'EXPL');
        // g_Serialization->SetRevertCallback(g_PluginHandle, RevertCallback);
        // g_Serialization->SetLoadCallback(g_PluginHandle, LoadCallback);
        // g_Serialization->SetSaveCallback(g_PluginHandle, SaveCallback);
        return true;
    }

    return false;
}