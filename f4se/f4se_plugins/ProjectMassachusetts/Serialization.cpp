#include "Serialization.h"

#include "Data.h"

namespace Serialization {
    void RevertCallback(const F4SESerializationInterface* intfc) {
        Perks::PlayerTags   = Perks::DefaultTags;
        Perks::NeedLevelUp  = false;
        Perks::PlayerLevel  = 1;
    }

    void LoadCallback(const F4SESerializationInterface* intfc) {
        UInt32 type, version, length;

        while (intfc->GetNextRecordInfo(&type, &version, &length)) {
            switch (type) {
            case 'NLVL':
                if (length == sizeof(bool))
                    intfc->ReadRecordData(&Perks::NeedLevelUp, sizeof(bool));
                break;

            case 'PLVL':
                if (length == sizeof(int))
                    intfc->ReadRecordData(&Perks::PlayerLevel, sizeof(int));
                break;

            case 'PTAG':
                if (length == sizeof(int))
                    intfc->ReadRecordData(&Perks::PlayerTags, sizeof(Perks::PlayerTags));
                break;
            }
        }

        if (Perks::NeedLevelUp)
            Perks::LUPrompt_Hook();
    }

    void SaveCallback(const F4SESerializationInterface* intfc) {
        if (g_Serialization->OpenRecord('NLVL', 1))
            g_Serialization->WriteRecordData(&Perks::NeedLevelUp, sizeof(bool));

        if (g_Serialization->OpenRecord('PLVL', 1))
            g_Serialization->WriteRecordData(&Perks::PlayerLevel, sizeof(int));

        if (g_Serialization->OpenRecord('PTAG', 1))
            g_Serialization->WriteRecordData(&Perks::PlayerTags, sizeof(Perks::PlayerTags));
    }

    bool Hook_Init(F4SESerializationInterface* SerializationInterface) {
        g_Serialization = SerializationInterface;
        if (!g_Serialization)
            return false;

        return true;
    }

    bool Hook_Commit(PluginHandle g_pluginHandle) {
        if (g_Serialization) {
            g_Serialization->SetUniqueID(g_pluginHandle, 'RTNG');
            g_Serialization->SetRevertCallback(g_pluginHandle, RevertCallback);
            g_Serialization->SetLoadCallback(g_pluginHandle, LoadCallback);
            g_Serialization->SetSaveCallback(g_pluginHandle, SaveCallback);
            return true;
        }

        return false;
    }
}
