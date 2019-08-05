#include "DataManager.h"

ILog            g_Log               = ILog(PLUGIN_NAME_SHORT);
ISettings       g_Settings          = ISettings(INI_FILE_NAME);
IGlobalSettings g_GlobalSettings    = IGlobalSettings();
IObScript       g_ObScript;

float GetPermValue(TESObjectREFR* Obj, ActorValueInfo* AVIF) {
    return (Obj->actorValueOwner.GetBase(AVIF) + Obj->actorValueOwner.GetMod(1, AVIF));
}

// ------------------------------------------------------------------------------------------------
// Public
// ------------------------------------------------------------------------------------------------

bool DataManager::Load() {
    m_PluginName = g_Settings.GetString("sPluginName:General", DEFAULT_PLUGIN_NAME);
    _LOGMESSAGE("DataManager: Using Game Plugin: %s", m_PluginName.c_str());

    if (!CheckGamePlugin()) {
        _LOGERROR("DataManager: Game Plugin is not loaded.");
        return false;
    }

    /* Vanilla */ {
    Strength                = LookupTypeByID(0x02C2,                                                ActorValueInfo);
    Perception              = LookupTypeByID(0x02C3,                                                ActorValueInfo);
    Endurance               = LookupTypeByID(0x02C4,                                                ActorValueInfo);
    Charisma                = LookupTypeByID(0x02C5,                                                ActorValueInfo);
    Intelligence            = LookupTypeByID(0x02C6,                                                ActorValueInfo);
    Agility                 = LookupTypeByID(0x02C7,                                                ActorValueInfo);
    Luck                    = LookupTypeByID(0x02C8,                                                ActorValueInfo);
    Experience              = LookupTypeByID(0x02C9,                                                ActorValueInfo);
    Health                  = LookupTypeByID(0x02D4,                                                ActorValueInfo);
    ActionPoints            = LookupTypeByID(0x02D5,                                                ActorValueInfo);
    Rads                    = LookupTypeByID(0x02E1,                                                ActorValueInfo);
    PoisonResistance        = LookupTypeByID(0x02E4,                                                ActorValueInfo);
    RadResistExposure       = LookupTypeByID(0x02EA,                                                ActorValueInfo);
    RadResistIngestion      = LookupTypeByID(0x02EA,                                                ActorValueInfo);
    RadHealthMax            = LookupTypeByID(0x02EE,                                                ActorValueInfo);
    HeadCondition           = LookupTypeByID(0x036C,                                                ActorValueInfo);
    TorsoCondition          = LookupTypeByID(0x036D,                                                ActorValueInfo);
    LeftArmCondition        = LookupTypeByID(0x036E,                                                ActorValueInfo);
    RightArmCondition       = LookupTypeByID(0x036F,                                                ActorValueInfo);
    LeftLegCondition        = LookupTypeByID(0x0370,                                                ActorValueInfo);
    RightLegCondition       = LookupTypeByID(0x0371,                                                ActorValueInfo);
    PipboyMenuIMOD          = LookupTypeByID(0x12BC08,                                              TESImageSpaceModifier);
    ObjectTypeStimpak       = LookupTypeByID(0x0F4AEB,                                              BGSKeyword);
    RadX                    = LookupTypeByID(0x024057,                                              AlchemyItem);
    }

    /* Plugin */ {
    Barter                  = LookupTypeFromPlugin(0x00F99, "Skill:Barter",                         ActorValueInfo);
    EnergyWeapons           = LookupTypeFromPlugin(0x00F9A, "Skill:EnergyWeapons",                  ActorValueInfo);
    Explosives              = LookupTypeFromPlugin(0x00F9B, "Skill:Explosives",                     ActorValueInfo);
    Guns                    = LookupTypeFromPlugin(0x00F9C, "Skill:Guns",                           ActorValueInfo);
    Lockpick                = LookupTypeFromPlugin(0x00F9D, "Skill:Lockpick",                       ActorValueInfo);
    Medicine                = LookupTypeFromPlugin(0x00F9E, "Skill:Medicine",                       ActorValueInfo);
    MeleeWeapons            = LookupTypeFromPlugin(0x00F9F, "Skill:MeleeWeapons",                   ActorValueInfo);
    Repair                  = LookupTypeFromPlugin(0x00FA0, "Skill:Repair",                         ActorValueInfo);
    Science                 = LookupTypeFromPlugin(0x00FA1, "Skill:Science",                        ActorValueInfo);
    Sneak                   = LookupTypeFromPlugin(0x00FA2, "Skill:Sneak",                          ActorValueInfo);
    Speech                  = LookupTypeFromPlugin(0x00FA3, "Skill:Speech",                         ActorValueInfo);
    Survival                = LookupTypeFromPlugin(0x00FA4, "Skill:Survival",                       ActorValueInfo);
    Unarmed                 = LookupTypeFromPlugin(0x00FA5, "Skill:Unarmed",                        ActorValueInfo);
    Barter_BuyMod           = LookupTypeFromPlugin(0x00FA6, "SkillMod:Barter_BuyMod",               ActorValueInfo);
    Barter_SellMod          = LookupTypeFromPlugin(0x00FA7, "SkillMod:Barter_SellMod",              ActorValueInfo);
    EnergyWeapons_Damage    = LookupTypeFromPlugin(0x00FA8, "SkillMod:EnergyWeapons_Damage",        ActorValueInfo);
    EnergyWeapons_Accuracy  = LookupTypeFromPlugin(0x00FA9, "SkillMod:EnergyWeapons_Accuracy",      ActorValueInfo);
    Explosives_Damage       = LookupTypeFromPlugin(0x00FAA, "SkillMod:Explosives_Damage",           ActorValueInfo);
    Explosives_Accuracy     = LookupTypeFromPlugin(0x00FAB, "SkillMod:Explosives_Accuracy",         ActorValueInfo);
    Guns_Damage             = LookupTypeFromPlugin(0x00FAC, "SkillMod:Guns_Damage",                 ActorValueInfo);
    Guns_Accuracy           = LookupTypeFromPlugin(0x00FAD, "SkillMod:Guns_Accuracy",               ActorValueInfo);
    Medicine_UsageMod       = LookupTypeFromPlugin(0x00FB2, "SkillMod:Medicine_UsageMod",           ActorValueInfo);
    MeleeWeapons_Damage     = LookupTypeFromPlugin(0x00FB3, "SkillMod:MeleeWeapons_Damage",         ActorValueInfo);
    MeleeWeapons_Secondary  = LookupTypeFromPlugin(0x00FB4, "SkillMod:MeleeWeapons_Secondary",      ActorValueInfo);
    Sneak_SkillMod          = LookupTypeFromPlugin(0x00FB6, "SkillMod:Sneak_SkillMod",              ActorValueInfo);
    Sneak_DetectionMod      = LookupTypeFromPlugin(0x00FB7, "SkillMod:Sneak_DetectionMod",          ActorValueInfo);
    Sneak_PickpocketMod     = LookupTypeFromPlugin(0x00FB8, "SkillMod:Sneak_PickpocketMod",         ActorValueInfo);
    Survival_UsageMod       = LookupTypeFromPlugin(0x00FB9, "SkillMod:Survival_UsageMod",           ActorValueInfo);
    Unarmed_Damage          = LookupTypeFromPlugin(0x00FBA, "SkillMod:Unarmed_Damage",              ActorValueInfo);
    Unarmed_Secondary       = LookupTypeFromPlugin(0x00FBB, "SkillMod:Unarmed_Secondary",           ActorValueInfo);
    ActionPointsMult        = LookupTypeFromPlugin(0x00FBD, "MiscAV:ActionPointsMult",              ActorValueInfo);
    SkillOffset             = LookupTypeFromPlugin(0x00FBC, "MiscAV:SkillOffset",                   ActorValueInfo);
    InventoryWeight         = LookupTypeFromPlugin(0x15FC0, "MiscAV:InventoryWeight",               ActorValueInfo);
    Dehydration             = LookupTypeFromPlugin(0x04C50, "Hardcore:Dehydration",                 ActorValueInfo);
    Starvation              = LookupTypeFromPlugin(0x04C51, "Hardcore:Starvation",                  ActorValueInfo);
    SleepDeprivation        = LookupTypeFromPlugin(0x04C52, "Hardcore:SleepDeprivation",            ActorValueInfo);
    DialogueBarter          = LookupTypeFromPlugin(0x00FD0, "SkillGlobal:DialogueBarter",           TESGlobal);
    DialogueEnergyWeapons   = LookupTypeFromPlugin(0x00FD1, "SkillGlobal:DialogueEnergyWeapons",    TESGlobal);
    DialogueExplosives      = LookupTypeFromPlugin(0x00FD2, "SkillGlobal:DialogueExplosives",       TESGlobal);
    DialogueGuns            = LookupTypeFromPlugin(0x00FD3, "SkillGlobal:DialogueGuns",             TESGlobal);
    DialogueLockpick        = LookupTypeFromPlugin(0x00FD4, "SkillGlobal:DialogueLockpick",         TESGlobal);
    DialogueMedicine        = LookupTypeFromPlugin(0x00FD5, "SkillGlobal:DialogueMedicine",         TESGlobal);
    DialogueMeleeWeapons    = LookupTypeFromPlugin(0x00FD6, "SkillGlobal:DialogueMeleeWeapons",     TESGlobal);
    DialogueRepair          = LookupTypeFromPlugin(0x00FD7, "SkillGlobal:DialogueRepair",           TESGlobal);
    DialogueScience         = LookupTypeFromPlugin(0x00FD8, "SkillGlobal:DialogueScience",          TESGlobal);
    DialogueSneak           = LookupTypeFromPlugin(0x00FD9, "SkillGlobal:DialogueSneak",            TESGlobal);
    DialogueSpeech          = LookupTypeFromPlugin(0x00FDA, "SkillGlobal:DialogueSpeech",           TESGlobal);
    DialogueSurvival        = LookupTypeFromPlugin(0x00FDB, "SkillGlobal:DialogueSurvival",         TESGlobal);
    DialogueUnarmed         = LookupTypeFromPlugin(0x00FDC, "SkillGlobal:DialogueUnarmed",          TESGlobal);
    LimbTarget              = LookupTypeFromPlugin(0x4B273, "MiscForm:LimbTarget",                  TESGlobal);
    StimpakOrder            = LookupTypeFromPlugin(0x2F9B8, "MiscForm:StimpakOrder",                BGSListForm);
    DoctorsBag              = LookupTypeFromPlugin(0xAD82B, "MiscForm:DoctorsBag",                  AlchemyItem);
    PerksQuest              = LookupTypeFromPlugin(0x04C87, "MiscForm:PerksQuest",                  TESQuest);

    m_SkillPList.emplace_back(LookupTypeFromPlugin(0x0268F, "SkillPerk:Barter",                     BGSPerk));
    m_SkillPList.emplace_back(LookupTypeFromPlugin(0x02690, "SkillPerk:EnergyWeapons",              BGSPerk));
    m_SkillPList.emplace_back(LookupTypeFromPlugin(0x02691, "SkillPerk:Explosives",                 BGSPerk));
    m_SkillPList.emplace_back(LookupTypeFromPlugin(0x02692, "SkillPerk:Guns",                       BGSPerk));
    m_SkillPList.emplace_back(LookupTypeFromPlugin(0x02693, "SkillPerk:Lockpick",                   BGSPerk));
    m_SkillPList.emplace_back(LookupTypeFromPlugin(0x02694, "SkillPerk:Medicine",                   BGSPerk));
    m_SkillPList.emplace_back(LookupTypeFromPlugin(0x02695, "SkillPerk:MeleeWeapons",               BGSPerk));
    m_SkillPList.emplace_back(LookupTypeFromPlugin(0x02696, "SkillPerk:Repair",                     BGSPerk));
    m_SkillPList.emplace_back(LookupTypeFromPlugin(0x02698, "SkillPerk:Science",                    BGSPerk));
    m_SkillPList.emplace_back(LookupTypeFromPlugin(0x02698, "SkillPerk:Sneak",                      BGSPerk));
    m_SkillPList.emplace_back(LookupTypeFromPlugin(0x02699, "SkillPerk:Speech",                     BGSPerk));
    m_SkillPList.emplace_back(LookupTypeFromPlugin(0x0269A, "SkillPerk:Survival",                   BGSPerk));
    m_SkillPList.emplace_back(LookupTypeFromPlugin(0x0269B, "SkillPerk:Unarmed",                    BGSPerk));
    m_SkillPList.emplace_back(LookupTypeFromPlugin(0x04C55, "SkillPerk:Hardcore",                   BGSPerk));

    // SpecialList
    m_SpecialList.emplace_back(Strength);
    m_SpecialList.emplace_back(Perception);
    m_SpecialList.emplace_back(Endurance);
    m_SpecialList.emplace_back(Charisma);
    m_SpecialList.emplace_back(Intelligence);
    m_SpecialList.emplace_back(Agility);
    m_SpecialList.emplace_back(Luck);

    // SkillList
    m_SkillList.emplace_back(Barter);
    m_SkillList.emplace_back(EnergyWeapons);
    m_SkillList.emplace_back(Explosives);
    m_SkillList.emplace_back(Guns);
    m_SkillList.emplace_back(Lockpick);
    m_SkillList.emplace_back(Medicine);
    m_SkillList.emplace_back(MeleeWeapons);
    m_SkillList.emplace_back(Repair);
    m_SkillList.emplace_back(Science);
    m_SkillList.emplace_back(Sneak);
    m_SkillList.emplace_back(Speech);
    m_SkillList.emplace_back(Survival);
    m_SkillList.emplace_back(Unarmed);

    // SkillModList
    m_SkillModList.emplace_back(Barter_BuyMod);
    m_SkillModList.emplace_back(Barter_SellMod);
    m_SkillModList.emplace_back(EnergyWeapons_Damage);
    m_SkillModList.emplace_back(EnergyWeapons_Accuracy);
    m_SkillModList.emplace_back(Explosives_Damage);
    m_SkillModList.emplace_back(Explosives_Accuracy);
    m_SkillModList.emplace_back(Guns_Damage);
    m_SkillModList.emplace_back(Guns_Accuracy);
    m_SkillModList.emplace_back(Medicine_UsageMod);
    m_SkillModList.emplace_back(MeleeWeapons_Damage);
    m_SkillModList.emplace_back(MeleeWeapons_Secondary);
    m_SkillModList.emplace_back(Sneak_SkillMod);
    m_SkillModList.emplace_back(Sneak_DetectionMod);
    m_SkillModList.emplace_back(Sneak_PickpocketMod);
    m_SkillModList.emplace_back(Survival_UsageMod);
    m_SkillModList.emplace_back(Unarmed_Damage);
    m_SkillModList.emplace_back(Unarmed_Secondary);
    }

    if (g_Settings.GetBool("bCheckLoadedData:General", false)) {
        CheckLoadedData();
    }

    return true;
}

void DataManager::Unload() {
    m_ModInfo = nullptr;

    m_MasterPerkList.clear();
    m_PerkList.clear();
    m_TraitList.clear();
    m_SkillPList.clear();

    m_SkillList.clear();
    m_SkillModList.clear();
    m_SpecialList.clear();

    /* Vanilla */ {
    Strength                = nullptr;
    Perception              = nullptr;
    Endurance               = nullptr;
    Charisma                = nullptr;
    Intelligence            = nullptr;
    Agility                 = nullptr;
    Luck                    = nullptr;
    Experience              = nullptr;
    Health                  = nullptr;
    ActionPoints            = nullptr;
    Rads                    = nullptr;
    PoisonResistance        = nullptr;
    RadResistExposure       = nullptr;
    RadResistIngestion      = nullptr;
    RadHealthMax            = nullptr;
    HeadCondition           = nullptr;
    TorsoCondition          = nullptr;
    LeftArmCondition        = nullptr;
    RightArmCondition       = nullptr;
    LeftLegCondition        = nullptr;
    RightLegCondition       = nullptr;
    PipboyMenuIMOD          = nullptr;
    ObjectTypeStimpak       = nullptr;
    RadX                    = nullptr;
    }

    /* Plugin */ {
    Barter                  = nullptr;
    EnergyWeapons           = nullptr;
    Explosives              = nullptr;
    Guns                    = nullptr;
    Lockpick                = nullptr;
    Medicine                = nullptr;
    MeleeWeapons            = nullptr;
    Repair                  = nullptr;
    Science                 = nullptr;
    Sneak                   = nullptr;
    Speech                  = nullptr;
    Survival                = nullptr;
    Unarmed                 = nullptr;
    Barter_BuyMod           = nullptr;
    Barter_SellMod          = nullptr;
    EnergyWeapons_Damage    = nullptr;
    EnergyWeapons_Accuracy  = nullptr;
    Explosives_Damage       = nullptr;
    Explosives_Accuracy     = nullptr;
    Guns_Damage             = nullptr;
    Guns_Accuracy           = nullptr;
    Medicine_UsageMod       = nullptr;
    MeleeWeapons_Damage     = nullptr;
    MeleeWeapons_Secondary  = nullptr;
    Sneak_SkillMod          = nullptr;
    Sneak_DetectionMod      = nullptr;
    Sneak_PickpocketMod     = nullptr;
    Survival_UsageMod       = nullptr;
    Unarmed_Damage          = nullptr;
    Unarmed_Secondary       = nullptr;
    ActionPointsMult        = nullptr;
    SkillOffset             = nullptr;
    InventoryWeight         = nullptr;
    Dehydration             = nullptr;
    Starvation              = nullptr;
    SleepDeprivation        = nullptr;
    DialogueBarter          = nullptr;
    DialogueEnergyWeapons   = nullptr;
    DialogueExplosives      = nullptr;
    DialogueGuns            = nullptr;
    DialogueLockpick        = nullptr;
    DialogueMedicine        = nullptr;
    DialogueMeleeWeapons    = nullptr;
    DialogueRepair          = nullptr;
    DialogueScience         = nullptr;
    DialogueSneak           = nullptr;
    DialogueSpeech          = nullptr;
    DialogueSurvival        = nullptr;
    DialogueUnarmed         = nullptr;
    LimbTarget              = nullptr;
    StimpakOrder            = nullptr;
    DoctorsBag              = nullptr;
    PerksQuest              = nullptr;
    }
}

TESForm* DataManager::LookupFormFromPlugin(UInt32 FormID) {
    FormID |= (m_ModInfo->modIndex) << 24;
    return LookupFormByID(FormID);
}

ActorValueInfo* DataManager::GetSkillByName(std::string skillName) {
    for (auto iter : m_SkillList) {
        std::string thisSkill = iter->GetFullName();
        if (std::equal(thisSkill.begin(), thisSkill.end(), skillName.begin(), skillName.end(), [](char a, char b) { return tolower(a) == tolower(b); })) {
            return iter;
        }
    }

    return nullptr;
}

// ------------------------------------------------------------------------------------------------
// Private
// ------------------------------------------------------------------------------------------------

bool DataManager::CheckGamePlugin() {
    m_ModInfo = (*g_dataHandler)->LookupLoadedModByName(m_PluginName.c_str());
    return (m_ModInfo != nullptr);
}

bool DataManager::CheckLoadedData() {
    _LOGMESSAGE("DataManager: Starting CheckLoadedData()");
    _LOGMESSAGENT("");

    ITimeKeeper CheckLoadedDataTimer = ITimeKeeper();
    CheckLoadedDataTimer.Start();

    /* Data Check */ {
    CheckForm(Barter,                   kFormType_AVIF, avName);
    CheckForm(EnergyWeapons,            kFormType_AVIF, avName);
    CheckForm(Explosives,               kFormType_AVIF, avName);
    CheckForm(Guns,                     kFormType_AVIF, avName);
    CheckForm(Lockpick,                 kFormType_AVIF, avName);
    CheckForm(Medicine,                 kFormType_AVIF, avName);
    CheckForm(MeleeWeapons,             kFormType_AVIF, avName);
    CheckForm(Repair,                   kFormType_AVIF, avName);
    CheckForm(Science,                  kFormType_AVIF, avName);
    CheckForm(Sneak,                    kFormType_AVIF, avName);
    CheckForm(Speech,                   kFormType_AVIF, avName);
    CheckForm(Survival,                 kFormType_AVIF, avName);
    CheckForm(Unarmed,                  kFormType_AVIF, avName);
    CheckForm(Barter_BuyMod,            kFormType_AVIF, avName);
    CheckForm(Barter_SellMod,           kFormType_AVIF, avName);
    CheckForm(EnergyWeapons_Damage,     kFormType_AVIF, avName);
    CheckForm(EnergyWeapons_Accuracy,   kFormType_AVIF, avName);
    CheckForm(Explosives_Damage,        kFormType_AVIF, avName);
    CheckForm(Explosives_Accuracy,      kFormType_AVIF, avName);
    CheckForm(Guns_Damage,              kFormType_AVIF, avName);
    CheckForm(Guns_Accuracy,            kFormType_AVIF, avName);
    CheckForm(Medicine_UsageMod,        kFormType_AVIF, avName);
    CheckForm(MeleeWeapons_Damage,      kFormType_AVIF, avName);
    CheckForm(MeleeWeapons_Secondary,   kFormType_AVIF, avName);
    CheckForm(Sneak_SkillMod,           kFormType_AVIF, avName);
    CheckForm(Sneak_DetectionMod,       kFormType_AVIF, avName);
    CheckForm(Sneak_PickpocketMod,      kFormType_AVIF, avName);
    CheckForm(Survival_UsageMod,        kFormType_AVIF, avName);
    CheckForm(Unarmed_Damage,           kFormType_AVIF, avName);
    CheckForm(Unarmed_Secondary,        kFormType_AVIF, avName);
    CheckForm(ActionPointsMult,         kFormType_AVIF, avName);
    CheckForm(SkillOffset,              kFormType_AVIF, avName);
    CheckForm(InventoryWeight,          kFormType_AVIF, avName);
    CheckForm(Dehydration,              kFormType_AVIF, avName);
    CheckForm(Starvation,               kFormType_AVIF, avName);
    CheckForm(SleepDeprivation,         kFormType_AVIF, avName);
    CheckForm(DialogueBarter,           kFormType_GLOB, editorID);
    CheckForm(DialogueEnergyWeapons,    kFormType_GLOB, editorID);
    CheckForm(DialogueExplosives,       kFormType_GLOB, editorID);
    CheckForm(DialogueGuns,             kFormType_GLOB, editorID);
    CheckForm(DialogueLockpick,         kFormType_GLOB, editorID);
    CheckForm(DialogueMedicine,         kFormType_GLOB, editorID);
    CheckForm(DialogueMeleeWeapons,     kFormType_GLOB, editorID);
    CheckForm(DialogueRepair,           kFormType_GLOB, editorID);
    CheckForm(DialogueScience,          kFormType_GLOB, editorID);
    CheckForm(DialogueSneak,            kFormType_GLOB, editorID);
    CheckForm(DialogueSpeech,           kFormType_GLOB, editorID);
    CheckForm(DialogueSurvival,         kFormType_GLOB, editorID);
    CheckForm(DialogueUnarmed,          kFormType_GLOB, editorID);
    CheckForm(LimbTarget,               kFormType_GLOB, editorID);
    CheckForm(StimpakOrder,             kFormType_FLST, forms.count);
    CheckForm(PerksQuest,               kFormType_QUST, GetEditorID());
    CheckForm(DoctorsBag,               kFormType_ALCH, GetFullName());
    CheckForm(m_SkillPList[0],          kFormType_PERK, GetFullName());
    CheckForm(m_SkillPList[1],          kFormType_PERK, GetFullName());
    CheckForm(m_SkillPList[2],          kFormType_PERK, GetFullName());
    CheckForm(m_SkillPList[3],          kFormType_PERK, GetFullName());
    CheckForm(m_SkillPList[4],          kFormType_PERK, GetFullName());
    CheckForm(m_SkillPList[5],          kFormType_PERK, GetFullName());
    CheckForm(m_SkillPList[6],          kFormType_PERK, GetFullName());
    CheckForm(m_SkillPList[7],          kFormType_PERK, GetFullName());
    CheckForm(m_SkillPList[8],          kFormType_PERK, GetFullName());
    CheckForm(m_SkillPList[9],          kFormType_PERK, GetFullName());
    CheckForm(m_SkillPList[10],         kFormType_PERK, GetFullName());
    CheckForm(m_SkillPList[11],         kFormType_PERK, GetFullName());
    CheckForm(m_SkillPList[12],         kFormType_PERK, GetFullName());
    CheckForm(m_SkillPList[13],         kFormType_PERK, GetFullName());
    }

    _LOGMESSAGE("DataManager: Finished CheckLoadedData()");
    _LOGMESSAGE("Time Elapsed: %fms", CheckLoadedDataTimer.Format(ITimeKeeper::Milli));
    return true;
}

// ------------------------------------------------------------------------------------------------
// Initial
// ------------------------------------------------------------------------------------------------

std::string             DataManager::m_PluginName;
const ModInfo*          DataManager::m_ModInfo;

PerkList                DataManager::m_MasterPerkList;
PerkList                DataManager::m_PerkList;
PerkList                DataManager::m_TraitList;
PerkList                DataManager::m_SkillPList;

SkillList               DataManager::m_SkillList;
SkillList               DataManager::m_SkillModList;
SkillList               DataManager::m_SpecialList;

ActorValueInfo*         DataManager::Strength                   = nullptr;
ActorValueInfo*         DataManager::Perception                 = nullptr;
ActorValueInfo*         DataManager::Endurance                  = nullptr;
ActorValueInfo*         DataManager::Charisma                   = nullptr;
ActorValueInfo*         DataManager::Intelligence               = nullptr;
ActorValueInfo*         DataManager::Agility                    = nullptr;
ActorValueInfo*         DataManager::Luck                       = nullptr;
ActorValueInfo*         DataManager::Experience                 = nullptr;
ActorValueInfo*         DataManager::Health                     = nullptr;
ActorValueInfo*         DataManager::ActionPoints               = nullptr;
ActorValueInfo*         DataManager::Rads                       = nullptr;
ActorValueInfo*         DataManager::PoisonResistance           = nullptr;
ActorValueInfo*         DataManager::RadResistExposure          = nullptr;
ActorValueInfo*         DataManager::RadResistIngestion         = nullptr;
ActorValueInfo*         DataManager::RadHealthMax               = nullptr;
ActorValueInfo*         DataManager::HeadCondition              = nullptr;
ActorValueInfo*         DataManager::TorsoCondition             = nullptr;
ActorValueInfo*         DataManager::LeftArmCondition           = nullptr;
ActorValueInfo*         DataManager::RightArmCondition          = nullptr;
ActorValueInfo*         DataManager::LeftLegCondition           = nullptr;
ActorValueInfo*         DataManager::RightLegCondition          = nullptr;
TESImageSpaceModifier*  DataManager::PipboyMenuIMOD             = nullptr;
BGSKeyword*             DataManager::ObjectTypeStimpak          = nullptr;
AlchemyItem*            DataManager::RadX                       = nullptr;
ActorValueInfo*         DataManager::Barter                     = nullptr;
ActorValueInfo*         DataManager::EnergyWeapons              = nullptr;
ActorValueInfo*         DataManager::Explosives                 = nullptr;
ActorValueInfo*         DataManager::Guns                       = nullptr;
ActorValueInfo*         DataManager::Lockpick                   = nullptr;
ActorValueInfo*         DataManager::Medicine                   = nullptr;
ActorValueInfo*         DataManager::MeleeWeapons               = nullptr;
ActorValueInfo*         DataManager::Repair                     = nullptr;
ActorValueInfo*         DataManager::Science                    = nullptr;
ActorValueInfo*         DataManager::Sneak                      = nullptr;
ActorValueInfo*         DataManager::Speech                     = nullptr;
ActorValueInfo*         DataManager::Survival                   = nullptr;
ActorValueInfo*         DataManager::Unarmed                    = nullptr;
ActorValueInfo*         DataManager::Barter_BuyMod              = nullptr;
ActorValueInfo*         DataManager::Barter_SellMod             = nullptr;
ActorValueInfo*         DataManager::EnergyWeapons_Damage       = nullptr;
ActorValueInfo*         DataManager::EnergyWeapons_Accuracy     = nullptr;
ActorValueInfo*         DataManager::Explosives_Damage          = nullptr;
ActorValueInfo*         DataManager::Explosives_Accuracy        = nullptr;
ActorValueInfo*         DataManager::Guns_Damage                = nullptr;
ActorValueInfo*         DataManager::Guns_Accuracy              = nullptr;
ActorValueInfo*         DataManager::Medicine_UsageMod          = nullptr;
ActorValueInfo*         DataManager::MeleeWeapons_Damage        = nullptr;
ActorValueInfo*         DataManager::MeleeWeapons_Secondary     = nullptr;
ActorValueInfo*         DataManager::Sneak_SkillMod             = nullptr;
ActorValueInfo*         DataManager::Sneak_DetectionMod         = nullptr;
ActorValueInfo*         DataManager::Sneak_PickpocketMod        = nullptr;
ActorValueInfo*         DataManager::Survival_UsageMod          = nullptr;
ActorValueInfo*         DataManager::Unarmed_Damage             = nullptr;
ActorValueInfo*         DataManager::Unarmed_Secondary          = nullptr;
ActorValueInfo*         DataManager::ActionPointsMult           = nullptr;
ActorValueInfo*         DataManager::SkillOffset                = nullptr;
ActorValueInfo*         DataManager::InventoryWeight            = nullptr;
ActorValueInfo*         DataManager::Dehydration                = nullptr;
ActorValueInfo*         DataManager::Starvation                 = nullptr;
ActorValueInfo*         DataManager::SleepDeprivation           = nullptr;
TESGlobal*              DataManager::DialogueBarter             = nullptr;
TESGlobal*              DataManager::DialogueEnergyWeapons      = nullptr;
TESGlobal*              DataManager::DialogueExplosives         = nullptr;
TESGlobal*              DataManager::DialogueGuns               = nullptr;
TESGlobal*              DataManager::DialogueLockpick           = nullptr;
TESGlobal*              DataManager::DialogueMedicine           = nullptr;
TESGlobal*              DataManager::DialogueMeleeWeapons       = nullptr;
TESGlobal*              DataManager::DialogueRepair             = nullptr;
TESGlobal*              DataManager::DialogueScience            = nullptr;
TESGlobal*              DataManager::DialogueSneak              = nullptr;
TESGlobal*              DataManager::DialogueSpeech             = nullptr;
TESGlobal*              DataManager::DialogueSurvival           = nullptr;
TESGlobal*              DataManager::DialogueUnarmed            = nullptr;
TESGlobal*              DataManager::LimbTarget                 = nullptr;
BGSListForm*            DataManager::StimpakOrder               = nullptr;
AlchemyItem*            DataManager::DoctorsBag                 = nullptr;
TESQuest*               DataManager::PerksQuest                 = nullptr;