#include "SaveManager.h"
#include "PerkManager.h"

void SaveManager::RevertCallback(const F4SESerializationInterface* intfc) {
    PerkManager::m_PlayerTags = PerkManager::m_DefaultTags;
    PerkManager::m_LevelUp = false;
    PerkManager::m_CurrentLevel = 1;
}

void SaveManager::LoadCallback(const F4SESerializationInterface* intfc) {
    UInt32 type, version, length;

    while (intfc->GetNextRecordInfo(&type, &version, &length)) {
        switch (type) {
        case 'NLVL':
            if (length == sizeof(bool))
                intfc->ReadRecordData(&PerkManager::m_LevelUp, sizeof(bool));
            break;

        case 'PLVL':
            if (length == sizeof(int))
                intfc->ReadRecordData(&PerkManager::m_CurrentLevel, sizeof(int));
            break;

        case 'PTAG':
            if (length == sizeof(int))
                intfc->ReadRecordData(&PerkManager::m_PlayerTags, sizeof(PerkManager::m_PlayerTags));
            break;
        }
    }

    if (PerkManager::m_LevelUp)
        PerkManager::LevelUp_Hook();
}

void SaveManager::SaveCallback(const F4SESerializationInterface* intfc) {
    if (g_Serialization->OpenRecord('NLVL', 1))
        g_Serialization->WriteRecordData(&PerkManager::m_LevelUp, sizeof(bool));

    if (g_Serialization->OpenRecord('PLVL', 1))
        g_Serialization->WriteRecordData(&PerkManager::m_CurrentLevel, sizeof(int));

    if (g_Serialization->OpenRecord('PTAG', 1))
        g_Serialization->WriteRecordData(&PerkManager::m_PlayerTags, sizeof(PerkManager::m_PlayerTags));
}

bool SaveManager::Hook_Commit(PluginHandle g_pluginHandle) {
    if (g_Serialization) {
        g_Serialization->SetUniqueID(g_pluginHandle, 'RTNG');
        g_Serialization->SetRevertCallback(g_pluginHandle, RevertCallback);
        g_Serialization->SetLoadCallback(g_pluginHandle, LoadCallback);
        g_Serialization->SetSaveCallback(g_pluginHandle, SaveCallback);
        return true;
    }

    return false;
}