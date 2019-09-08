#include "Seralize.h"
#include "Scaleform.h"

void RevertCallback(const F4SESerializationInterface* intfc) {
    Forms::PlayerTags       = 0;

    Forms::PlayerLevelUp    = false;
    Forms::PlayerLevel      = 1;
}

void LoadCallback(const F4SESerializationInterface* intfc) {
    UInt32 type, version, length;

    while (intfc->GetNextRecordInfo(&type, &version, &length)) {
        switch (type) {
        case 'NLVL':
            if (length == sizeof(bool))
                intfc->ReadRecordData(&Forms::PlayerLevelUp, sizeof(bool));
            break;
        
        case 'PLVL':
            if (length == sizeof(UInt16))
                intfc->ReadRecordData(&Forms::PlayerLevel, sizeof(UInt16));
            break;

        case 'PTAG':
            if (length == sizeof(UInt16))
                intfc->ReadRecordData(&Forms::PlayerTags, sizeof(UInt16));
            break;
        }
    }

    if (Forms::PlayerLevelUp)
        LevelUpMenuPrompt_Hook();
}

void SaveCallback(const F4SESerializationInterface* intfc) {
    if (g_Serialization->OpenRecord('NLVL', 1))
        g_Serialization->WriteRecordData(&Forms::PlayerLevelUp, sizeof(bool));
    
    if (g_Serialization->OpenRecord('PLVL', 1))
        g_Serialization->WriteRecordData(&Forms::PlayerLevel, sizeof(UInt16));

    if (g_Serialization->OpenRecord('PTAG', 1))
        g_Serialization->WriteRecordData(&Forms::PlayerTags, sizeof(UInt16));
}

bool Serialize::Commit() {
    if (g_Serialization) {
        g_Serialization->SetUniqueID(g_PluginHandle, 'RTNG');
        g_Serialization->SetRevertCallback(g_PluginHandle, RevertCallback);
        g_Serialization->SetLoadCallback(g_PluginHandle, LoadCallback);
        g_Serialization->SetSaveCallback(g_PluginHandle, SaveCallback);
        return true;
    }

    return false;
}