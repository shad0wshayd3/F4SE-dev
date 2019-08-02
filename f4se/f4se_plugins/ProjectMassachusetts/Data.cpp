#include "Data.h"

#include "Config.h"
#include "f4se_globals/Globals.h"

#include "f4se/GameData.h"

float GetPermValue(TESObjectREFR* thisObj, ActorValueInfo* avif) {
    return (thisObj->actorValueOwner.GetBase(avif) + thisObj->actorValueOwner.GetMod(1, avif));
}

// ------------------------------------------------------------------------------------------------
// GameSettings::Public
// ------------------------------------------------------------------------------------------------

void GameSettings::Init() {
    tArray<TESForm*> Globals = (*g_dataHandler)->arrGLOB;
    SettingsMap.clear();

    for (int i = 0; i < Globals.count; i++) {
        TESGlobal* thisGlobal = DYNAMIC_CAST(Globals[i], TESForm, TESGlobal);

        if (thisGlobal) {
            std::string EDID = thisGlobal->editorID;
            auto search = EDID.find("PM_");

            if ((search != 0) || (search == std::string::npos))
                continue;

            SettingsMap.emplace(EDID.substr(3, std::string::npos), thisGlobal->formID);
        }
    }
}

bool GameSettings::GetBool(std::string settingName) {
    auto search = SettingsMap.find(settingName);
    if (search != SettingsMap.end()) {
        TESGlobal* Setting = LookupTypeByID(search->second, TESGlobal);
        if (Setting)
            return (Setting->value == 1.0);
    }

    return false;
}

float GameSettings::GetFloat(std::string settingName) {
    auto search = SettingsMap.find(settingName);
    if (search != SettingsMap.end()) {
        TESGlobal* Setting = LookupTypeByID(search->second, TESGlobal);
        if (Setting)
            return Setting->value;
    }

    return 0.0;
}

// ------------------------------------------------------------------------------------------------
// DataManager::Public
// ------------------------------------------------------------------------------------------------

bool DataManager::Init() {
    m_pluginName = g_Settings.GetString("sPlugin:General", DEFAULT_PLUGIN_NAME);
    g_Log.LogMessage("Using Game Plugin: %s", m_pluginName.c_str());

    if (GetGamePlugin()) {
        g_Log.LogMessage("Found Game Plugin.");

        /* Vanilla Actor Values */ {
        Strength                = LookupTypeByID(0x2C2,     ActorValueInfo);
        Perception              = LookupTypeByID(0x2C3,     ActorValueInfo);
        Endurance               = LookupTypeByID(0x2C4,     ActorValueInfo);
        Charisma                = LookupTypeByID(0x2C5,     ActorValueInfo);
        Intelligence            = LookupTypeByID(0x2C6,     ActorValueInfo);
        Agility                 = LookupTypeByID(0x2C7,     ActorValueInfo);
        Luck                    = LookupTypeByID(0x2C8,     ActorValueInfo);
        Experience              = LookupTypeByID(0x2C9,     ActorValueInfo);
        Health                  = LookupTypeByID(0x2D4,     ActorValueInfo);
        ActionPoints            = LookupTypeByID(0x2D5,     ActorValueInfo);
        Rads                    = LookupTypeByID(0x2E1,     ActorValueInfo);
        RadResistExposure       = LookupTypeByID(0x2EA,     ActorValueInfo);
        RadResistIngestion      = LookupTypeByID(0x2EA,     ActorValueInfo);
        RadHealthMax            = LookupTypeByID(0x2EE,     ActorValueInfo);
        HeadCondition           = LookupTypeByID(0x36C,     ActorValueInfo);
        TorsoCondition          = LookupTypeByID(0x36D,     ActorValueInfo);
        LeftArmCondition        = LookupTypeByID(0x36E,     ActorValueInfo);
        RightArmCondition       = LookupTypeByID(0x36F,     ActorValueInfo);
        LeftLegCondition        = LookupTypeByID(0x370,     ActorValueInfo);
        RightLegCondition       = LookupTypeByID(0x371,     ActorValueInfo);
        }

        /* Vanilla Misc Types */ {
        PipboyMenuImod          = LookupTypeByID(0x12BC08,  TESImageSpaceModifier);
        ObjectTypeStimpak       = LookupFormByID(0xF4AEB);
        RadX                    = LookupFormByID(0x24057);
        }

        /* Mod Actor Values */ {
        Barter                  = LookupTypeFromPlugin(GetFormID(0x0F99,    "iBarter:FormID"),                  ActorValueInfo);
        EnergyWeapons           = LookupTypeFromPlugin(GetFormID(0x0F9A,    "iEnergyWeapons:FormID"),           ActorValueInfo);
        Explosives              = LookupTypeFromPlugin(GetFormID(0x0F9B,    "iExplosives:FormID"),              ActorValueInfo);
        Guns                    = LookupTypeFromPlugin(GetFormID(0x0F9C,    "iGuns:FormID"),                    ActorValueInfo);
        Lockpick                = LookupTypeFromPlugin(GetFormID(0x0F9D,    "iLockpick:FormID"),                ActorValueInfo);
        Medicine                = LookupTypeFromPlugin(GetFormID(0x0F9E,    "iMedicine:FormID"),                ActorValueInfo);
        MeleeWeapons            = LookupTypeFromPlugin(GetFormID(0x0F9F,    "iMeleeWeapons:FormID"),            ActorValueInfo);
        Repair                  = LookupTypeFromPlugin(GetFormID(0x0FA0,    "iRepair:FormID"),                  ActorValueInfo);
        Science                 = LookupTypeFromPlugin(GetFormID(0x0FA1,    "iScience:FormID"),                 ActorValueInfo);
        Sneak                   = LookupTypeFromPlugin(GetFormID(0x0FA2,    "iSneak:FormID"),                   ActorValueInfo);
        Speech                  = LookupTypeFromPlugin(GetFormID(0x0FA3,    "iSpeech:FormID"),                  ActorValueInfo);
        Survival                = LookupTypeFromPlugin(GetFormID(0x0FA4,    "iSurvival:FormID"),                ActorValueInfo);
        Unarmed                 = LookupTypeFromPlugin(GetFormID(0x0FA5,    "iUnarmed:FormID"),                 ActorValueInfo);
        Barter_BuyMod           = LookupTypeFromPlugin(GetFormID(0x0FA6,    "iBarter_BuyMod:FormID"),           ActorValueInfo);
        Barter_SellMod          = LookupTypeFromPlugin(GetFormID(0x0FA7,    "iBarter_SellMod:FormID"),          ActorValueInfo);
        EnergyWeapons_Damage    = LookupTypeFromPlugin(GetFormID(0x0FA8,    "iEnergyWeapons_Damage:FormID"),    ActorValueInfo);
        EnergyWeapons_Accuracy  = LookupTypeFromPlugin(GetFormID(0x0FA9,    "iEnergyWeapons_Accuracy:FormID"),  ActorValueInfo);
        Explosives_Damage       = LookupTypeFromPlugin(GetFormID(0x0FAA,    "iExplosives_Damage:FormID"),       ActorValueInfo);
        Explosives_Accuracy     = LookupTypeFromPlugin(GetFormID(0x0FAB,    "iExplosives_Accuracy:FormID"),     ActorValueInfo);
        Guns_Damage             = LookupTypeFromPlugin(GetFormID(0x0FAC,    "iGuns_Damage:FormID"),             ActorValueInfo);
        Guns_Accuracy           = LookupTypeFromPlugin(GetFormID(0x0FAD,    "iGuns_Accuracy:FormID"),           ActorValueInfo);
        Medicine_UsageMod       = LookupTypeFromPlugin(GetFormID(0x0FB2,    "iMedicine_UsageMod:FormID"),       ActorValueInfo);
        MeleeWeapons_Damage     = LookupTypeFromPlugin(GetFormID(0x0FB3,    "iMeleeWeapons_Damage:FormID"),     ActorValueInfo);
        MeleeWeapons_Secondary  = LookupTypeFromPlugin(GetFormID(0x0FB4,    "iMeleeWeapons_Secondary:FormID"),  ActorValueInfo);
        Sneak_SkillMod          = LookupTypeFromPlugin(GetFormID(0x0FB6,    "iSneak_SkillMod:FormID"),          ActorValueInfo);
        Sneak_DetectionMod      = LookupTypeFromPlugin(GetFormID(0x0FB7,    "iSneak_DetectionMod:FormID"),      ActorValueInfo);
        Sneak_PickpocketMod     = LookupTypeFromPlugin(GetFormID(0x0FB8,    "iSneak_PickpocketMod:FormID"),     ActorValueInfo);
        Survival_UsageMod       = LookupTypeFromPlugin(GetFormID(0x0FB9,    "iSurvival_UsageMod:FormID"),       ActorValueInfo);
        Unarmed_Damage          = LookupTypeFromPlugin(GetFormID(0x0FBA,    "iUnarmed_Damage:FormID"),          ActorValueInfo);
        Unarmed_Secondary       = LookupTypeFromPlugin(GetFormID(0x0FBB,    "iUnarmed_Secondary:FormID"),       ActorValueInfo);
        ActionPointsMult        = LookupTypeFromPlugin(GetFormID(0x0FBD,    "iActionPointsMult:FormID"),        ActorValueInfo);
        SkillOffset             = LookupTypeFromPlugin(GetFormID(0x0FBC,    "iSkillOffset:FormID"),             ActorValueInfo);
        CurrentLevel            = LookupTypeFromPlugin(GetFormID(0x0FBE,    "iCurrentLevel:FormID"),            ActorValueInfo);
        Dehydration             = LookupTypeFromPlugin(GetFormID(0x4C50,    "iDehydration:FormID"),             ActorValueInfo);
        Starvation              = LookupTypeFromPlugin(GetFormID(0x4C51,    "iStarvation:FormID"),              ActorValueInfo);
        SleepDeprivation        = LookupTypeFromPlugin(GetFormID(0x4C52,    "iSleepDeprivation:FormID"),        ActorValueInfo);
        }

        /* Mod Globals */ {
        DialogueBarter          = LookupTypeFromPlugin(GetFormID(0x00FD0,   "iDialogueBarter:FormID"),          TESGlobal);
        DialogueEnergyWeapons   = LookupTypeFromPlugin(GetFormID(0x00FD1,   "iDialogueEnergyWeapons:FormID"),   TESGlobal);
        DialogueExplosives      = LookupTypeFromPlugin(GetFormID(0x00FD2,   "iDialogueExplosives:FormID"),      TESGlobal);
        DialogueGuns            = LookupTypeFromPlugin(GetFormID(0x00FD3,   "iDialogueGuns:FormID"),            TESGlobal);
        DialogueLockpick        = LookupTypeFromPlugin(GetFormID(0x00FD4,   "iDialogueLockpick:FormID"),        TESGlobal);
        DialogueMedicine        = LookupTypeFromPlugin(GetFormID(0x00FD5,   "iDialogueMedicine:FormID"),        TESGlobal);
        DialogueMeleeWeapons    = LookupTypeFromPlugin(GetFormID(0x00FD6,   "iDialogueMeleeWeapons:FormID"),    TESGlobal);
        DialogueRepair          = LookupTypeFromPlugin(GetFormID(0x00FD7,   "iDialogueRepair:FormID"),          TESGlobal);
        DialogueScience         = LookupTypeFromPlugin(GetFormID(0x00FD8,   "iDialogueScience:FormID"),         TESGlobal);
        DialogueSneak           = LookupTypeFromPlugin(GetFormID(0x00FD9,   "iDialogueSneak:FormID"),           TESGlobal);
        DialogueSpeech          = LookupTypeFromPlugin(GetFormID(0x00FDA,   "iDialogueSpeech:FormID"),          TESGlobal);
        DialogueSurvival        = LookupTypeFromPlugin(GetFormID(0x00FDB,   "iDialogueSurvival:FormID"),        TESGlobal);
        DialogueUnarmed         = LookupTypeFromPlugin(GetFormID(0x00FDC,   "iDialogueUnarmed:FormID"),         TESGlobal);
        LimbTarget              = LookupTypeFromPlugin(GetFormID(0x4B273,   "iLimbTarget:FormID"),              TESGlobal);
        }

        /* Mod Misc Types */ {
        StimpakOrder            = LookupTypeFromPlugin(GetFormID(0x2F9B8,   "iStimpakOrder:FormID"),            BGSListForm);
        PerksQuest              = LookupTypeFromPlugin(GetFormID(0x04C87,   "iPerksQuest:FormID"),              TESQuest);
        DoctorsBag              = LookupFormFromPlugin(GetFormID(0xAD82B,   "iDoctorsBag:FormID"));
        }
    }
    else {
        g_Log.LogError("Game Plugin is not loaded.");
        return false;
    }

    if (g_Settings.GetBool("bDoSelfTest:General", false)) {
        m_Wait = true;
        DoSelfCheck();
    }

    while (m_Wait) {}
    return true;
}

// ------------------------------------------------------------------------------------------------
// DataManager::Private
// ------------------------------------------------------------------------------------------------

bool DataManager::GetGamePlugin() {
    m_ModInfo = (*g_dataHandler)->LookupLoadedModByName(m_pluginName.c_str());
    return (m_ModInfo != nullptr);
}

TESForm* DataManager::LookupFormFromPlugin(UInt32 FormID) {
    FormID |= (m_ModInfo->modIndex) << 24;
    return LookupFormByID(FormID);
}

void DataManager::DoSelfCheck() {
    g_Log.LogMessage("Perfoming data self check...");
    g_Log.LogMessage("(If the game crashes, that's a pretty good indication that a form is incorrect.)");
    g_Log.LogMessageNT("");

    ITimeKeeper CheckTimer = ITimeKeeper();
    CheckTimer.Start();

    CheckForm(Barter,                   98, avName);
    CheckForm(EnergyWeapons,            98, avName);
    CheckForm(Explosives,               98, avName);
    CheckForm(Guns,                     98, avName);
    CheckForm(Lockpick,                 98, avName);
    CheckForm(Medicine,                 98, avName);
    CheckForm(MeleeWeapons,             98, avName);
    CheckForm(Repair,                   98, avName);
    CheckForm(Science,                  98, avName);
    CheckForm(Sneak,                    98, avName);
    CheckForm(Speech,                   98, avName);
    CheckForm(Survival,                 98, avName);
    CheckForm(Unarmed,                  98, avName);
    CheckForm(Barter_BuyMod,            98, avName);
    CheckForm(Barter_SellMod,           98, avName);
    CheckForm(EnergyWeapons_Damage,     98, avName);
    CheckForm(EnergyWeapons_Accuracy,   98, avName);
    CheckForm(Explosives_Damage,        98, avName);
    CheckForm(Explosives_Accuracy,      98, avName);
    CheckForm(Guns_Damage,              98, avName);
    CheckForm(Guns_Accuracy,            98, avName);
    CheckForm(Medicine_UsageMod,        98, avName);
    CheckForm(MeleeWeapons_Damage,      98, avName);
    CheckForm(MeleeWeapons_Secondary,   98, avName);
    CheckForm(Sneak_SkillMod,           98, avName);
    CheckForm(Sneak_DetectionMod,       98, avName);
    CheckForm(Sneak_PickpocketMod,      98, avName);
    CheckForm(Survival_UsageMod,        98, avName);
    CheckForm(Unarmed_Damage,           98, avName);
    CheckForm(Unarmed_Secondary,        98, avName);
    CheckForm(ActionPointsMult,         98, avName);
    CheckForm(SkillOffset,              98, avName);
    CheckForm(CurrentLevel,             98, avName);
    CheckForm(Dehydration,              98, avName);
    CheckForm(Starvation,               98, avName);
    CheckForm(SleepDeprivation,         98, avName);
    CheckForm(DialogueBarter,           11, editorID);
    CheckForm(DialogueEnergyWeapons,    11, editorID);
    CheckForm(DialogueExplosives,       11, editorID);
    CheckForm(DialogueGuns,             11, editorID);
    CheckForm(DialogueLockpick,         11, editorID);
    CheckForm(DialogueMedicine,         11, editorID);
    CheckForm(DialogueMeleeWeapons,     11, editorID);
    CheckForm(DialogueRepair,           11, editorID);
    CheckForm(DialogueScience,          11, editorID);
    CheckForm(DialogueSneak,            11, editorID);
    CheckForm(DialogueSpeech,           11, editorID);
    CheckForm(DialogueSurvival,         11, editorID);
    CheckForm(DialogueUnarmed,          11, editorID);
    CheckForm(LimbTarget,               11, editorID);
    CheckForm(StimpakOrder,             94, forms.count);
    CheckForm(PerksQuest,               80, GetEditorID());
    CheckForm(DoctorsBag,               48, GetFullName());

    g_Log.LogMessage("Self Check Finished. Time: %fms", CheckTimer.Format(ITimeKeeper::Milli));
    m_Wait = false;
}
