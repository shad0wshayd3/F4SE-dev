#include "Forms.h"
#include "Values.h"

#define CheckForm(Form, FormType, Name)\
    if (##Form##) {\
        _LogMessageNT("Variable: %s", #Form);\
        _DebugIndent();\
        _DebugMessageNT("%s is FormType: %i. Should be %i.", #Form, ##Form##->formType, FormType);\
        if (##Form##->formType == ##FormType##) {\
            _DebugMessageNT("%s reporting name as: %s", #Form, ##Form##->##Name##);\
        }\
        _DebugOutdent();\
        _DebugMessageNT("");\
    }\
    else {\
        return false;\
    }\

#define LookupType(FormID, SettingName, Type)\
    (##Type*) Runtime_DynamicCast((void*)(Lookup(ISettings::GetInteger(##SettingName##, ##FormID##))), RTTI_TESForm, RTTI_##Type)

// ------------------------------------------------------------------------------------------------
// Public
// ------------------------------------------------------------------------------------------------

bool Forms::Load() {
    m_PluginName = ISettings::GetString("General:PluginName", DEFAULT_PLUGIN_NAME);
    _LogMessage("Forms::Load - Game Plugin: %s.", m_PluginName.c_str());

    if (!CheckForPlugin()) {
        _LogError("Forms::Load - Game Plugin is not loaded.");
        return false;
    }

    // ------------------------------------------------------------------------
    // Fallout4.esm Forms
    // ------------------------------------------------------------------------
    Strength                = LookupTypeByID(0x0002C2,                                  ActorValueInfo);
    Perception              = LookupTypeByID(0x0002C3,                                  ActorValueInfo);
    Endurance               = LookupTypeByID(0x0002C4,                                  ActorValueInfo);
    Charisma                = LookupTypeByID(0x0002C5,                                  ActorValueInfo);
    Intelligence            = LookupTypeByID(0x0002C6,                                  ActorValueInfo);
    Agility                 = LookupTypeByID(0x0002C7,                                  ActorValueInfo);
    Luck                    = LookupTypeByID(0x0002C8,                                  ActorValueInfo);
    Experience              = LookupTypeByID(0x0002C9,                                  ActorValueInfo);
    Health                  = LookupTypeByID(0x0002D4,                                  ActorValueInfo);
    ActionPoints            = LookupTypeByID(0x0002D5,                                  ActorValueInfo);
    Rads                    = LookupTypeByID(0x0002E1,                                  ActorValueInfo);
    PoisonResistance        = LookupTypeByID(0x0002E4,                                  ActorValueInfo);
	RadResistIngestion      = LookupTypeByID(0x0002E9,                                  ActorValueInfo);
    RadResistExposure       = LookupTypeByID(0x0002EA,                                  ActorValueInfo);
    RadHealthMax            = LookupTypeByID(0x0002EE,                                  ActorValueInfo);
    HeadCondition           = LookupTypeByID(0x00036C,                                  ActorValueInfo);
    TorsoCondition          = LookupTypeByID(0x00036D,                                  ActorValueInfo);
    LeftArmCondition        = LookupTypeByID(0x00036E,                                  ActorValueInfo);
    RightArmCondition       = LookupTypeByID(0x00036F,                                  ActorValueInfo);
    LeftLegCondition        = LookupTypeByID(0x000370,                                  ActorValueInfo);
    RightLegCondition       = LookupTypeByID(0x000371,                                  ActorValueInfo);
    RadX                    = LookupTypeByID(0x024057,                                  AlchemyItem);

	ObjectTypes.push_back(	{ LookupTypeByID(0x055ECC,									BGSKeyword),		56 });
	ObjectTypes.push_back(	{ LookupTypeByID(0x0F4AED,									BGSKeyword),		53 });
	ObjectTypes.push_back(	{ LookupTypeByID(0x10C416,									BGSKeyword),		57 });
	ObjectTypes.push_back(	{ LookupTypeByID(0x249F30,									BGSKeyword),		39 });
	ObjectTypes.push_back(	{ LookupTypeByID(0x0F4AE7,									BGSKeyword),		54 });
	ObjectTypes.push_back(	{ LookupTypeByID(0x0842A1,									BGSKeyword),		58 });
	ObjectTypes.push_back(	{ LookupTypeByID(0x0F4AEB,									BGSKeyword),		55 });
	ObjectTypes.push_back(	{ LookupTypeByID(0x1D4A70,									BGSKeyword),		46 });

    // ------------------------------------------------------------------------
    // ProjectMassachusetts.esm Forms
    // ------------------------------------------------------------------------
    Barter                  = LookupType(0x000800,  "Skill:Barter",                     ActorValueInfo);
    EnergyWeapons           = LookupType(0x000801,  "Skill:EnergyWeapons",              ActorValueInfo);
    Explosives              = LookupType(0x000802,  "Skill:Explosives",                 ActorValueInfo);
    Guns                    = LookupType(0x000803,  "Skill:Guns",                       ActorValueInfo);
    Lockpick                = LookupType(0x000804,  "Skill:Lockpick",                   ActorValueInfo);
    Medicine                = LookupType(0x000805,  "Skill:Medicine",                   ActorValueInfo);
    MeleeWeapons            = LookupType(0x000806,  "Skill:MeleeWeapons",               ActorValueInfo);
    Repair                  = LookupType(0x000807,  "Skill:Repair",                     ActorValueInfo);
    Science                 = LookupType(0x000808,  "Skill:Science",                    ActorValueInfo);
    Sneak                   = LookupType(0x000809,  "Skill:Sneak",                      ActorValueInfo);
    Speech                  = LookupType(0x00080A,  "Skill:Speech",                     ActorValueInfo);
    Survival                = LookupType(0x00080B,  "Skill:Survival",                   ActorValueInfo);
    Unarmed                 = LookupType(0x00080C,  "Skill:Unarmed",                    ActorValueInfo);

    Barter_BuyMod           = LookupType(0x000FA6,  "SkillMod:Barter_BuyMod",           ActorValueInfo);
    Barter_SellMod          = LookupType(0x000FA7,  "SkillMod:Barter_SellMod",          ActorValueInfo);
    EnergyWeapons_Damage    = LookupType(0x000FA8,  "SkillMod:EnergyWeapons_Damage",    ActorValueInfo);
    EnergyWeapons_Accuracy  = LookupType(0x000FA9,  "SkillMod:EnergyWeapons_Accuracy",  ActorValueInfo);
    Explosives_Damage       = LookupType(0x000FAA,  "SkillMod:Explosives_Damage",       ActorValueInfo);
    Explosives_Accuracy     = LookupType(0x000FAB,  "SkillMod:Explosives_Accuracy",     ActorValueInfo);
    Guns_Damage             = LookupType(0x000FAC,  "SkillMod:Guns_Damage",             ActorValueInfo);
    Guns_Accuracy           = LookupType(0x000FAD,  "SkillMod:Guns_Accuracy",           ActorValueInfo);
    Medicine_UsageMod       = LookupType(0x000FB2,  "SkillMod:Medicine_UsageMod",       ActorValueInfo);
    MeleeWeapons_Damage     = LookupType(0x000FB3,  "SkillMod:MeleeWeapons_Damage",     ActorValueInfo);
    MeleeWeapons_Secondary  = LookupType(0x000FB4,  "SkillMod:MeleeWeapons_Secondary",  ActorValueInfo);
    Sneak_SkillMod          = LookupType(0x000FB6,  "SkillMod:Sneak_SkillMod",          ActorValueInfo);
    Sneak_DetectionMod      = LookupType(0x000FB7,  "SkillMod:Sneak_DetectionMod",      ActorValueInfo);
    Sneak_PickpocketMod     = LookupType(0x000FB8,  "SkillMod:Sneak_PickpocketMod",     ActorValueInfo);
    Survival_UsageMod       = LookupType(0x000FB9,  "SkillMod:Survival_UsageMod",       ActorValueInfo);
    Unarmed_Damage          = LookupType(0x000FBA,  "SkillMod:Unarmed_Damage",          ActorValueInfo);
    Unarmed_Secondary       = LookupType(0x000FBB,  "SkillMod:Unarmed_Secondary",       ActorValueInfo);

    Dehydration             = LookupType(0x000900,  "Hardcore:Dehydration",             ActorValueInfo);
    Starvation              = LookupType(0x000901,  "Hardcore:Starvation",              ActorValueInfo);
    SleepDeprivation        = LookupType(0x000902,  "Hardcore:SleepDeprivation",        ActorValueInfo);

	ActionPointsMult        = LookupType(0x000B00,  "MiscAV:ActionPointsMult",          ActorValueInfo);
    InventoryWeight         = LookupType(0x000B01,  "MiscAV:InventoryWeight",           ActorValueInfo);
	PipboyLightEnabled		= LookupType(0x000000,	"MiscAV:PipboyLightEnabled",		ActorValueInfo);
	RadiationSourceCount	= LookupType(0x000000,	"MiscAV:RadiationSourceCount",		ActorValueInfo);
	SkillPointBonus         = LookupType(0x000B02,  "MiscAV:SkillPointBonus",           ActorValueInfo);

	ItemCondMaxHealth       = LookupType(0x000C00,  "Condition:ConditionMax",           ActorValueInfo);
    ItemCondMinHealth       = LookupType(0x000C01,  "Condition:ConditionMin",           ActorValueInfo);
    ItemCondStartCond       = LookupType(0x000C02,  "Condition:ConditionInitial",       ActorValueInfo);

    StimpakOrder            = LookupType(0x02F9B8,  "MiscForm:StimpakOrder",            BGSListForm);
    DoctorsBag              = LookupType(0x0AD82B,  "MiscForm:DoctorsBag",              AlchemyItem);

    Perk_Barter             = LookupType(0x00268F,  "SkillPerk:Barter",                 BGSPerk);
    Perk_EnergyWeapons      = LookupType(0x002690,  "SkillPerk:EnergyWeapons",          BGSPerk);
    Perk_Explosives         = LookupType(0x002691,  "SkillPerk:Explosives",             BGSPerk);
    Perk_Guns               = LookupType(0x002692,  "SkillPerk:Guns",                   BGSPerk);
    Perk_Lockpick           = LookupType(0x002693,  "SkillPerk:Lockpick",               BGSPerk);
    Perk_Medicine           = LookupType(0x002694,  "SkillPerk:Medicine",               BGSPerk);
    Perk_MeleeWeapons       = LookupType(0x002695,  "SkillPerk:MeleeWeapons",           BGSPerk);
    Perk_Repair             = LookupType(0x002696,  "SkillPerk:Repair",                 BGSPerk);
    Perk_Science            = LookupType(0x002698,  "SkillPerk:Science",                BGSPerk);
    Perk_Sneak              = LookupType(0x002698,  "SkillPerk:Sneak",                  BGSPerk);
    Perk_Speech             = LookupType(0x002699,  "SkillPerk:Speech",                 BGSPerk);
    Perk_Survival           = LookupType(0x00269A,  "SkillPerk:Survival",               BGSPerk);
    Perk_Unarmed            = LookupType(0x00269B,  "SkillPerk:Unarmed",                BGSPerk);
    Perk_Hardcore           = LookupType(0x004C55,  "SkillPerk:Hardcore",               BGSPerk);

    // ------------------------------------------------------------------------
    // Create Lists
    // ------------------------------------------------------------------------
    ListSpecial.emplace_back(Strength);
    ListSpecial.emplace_back(Perception);
    ListSpecial.emplace_back(Endurance);
    ListSpecial.emplace_back(Charisma);
    ListSpecial.emplace_back(Intelligence);
    ListSpecial.emplace_back(Agility);
    ListSpecial.emplace_back(Luck);

    ListSkills.emplace_back(Barter);
    ListSkills.emplace_back(EnergyWeapons);
    ListSkills.emplace_back(Explosives);
    ListSkills.emplace_back(Guns);
    ListSkills.emplace_back(Lockpick);
    ListSkills.emplace_back(Medicine);
    ListSkills.emplace_back(MeleeWeapons);
    ListSkills.emplace_back(Repair);
    ListSkills.emplace_back(Science);
    ListSkills.emplace_back(Sneak);
    ListSkills.emplace_back(Speech);
    ListSkills.emplace_back(Survival);
    ListSkills.emplace_back(Unarmed);

    ListSkillMods.emplace_back(Barter_BuyMod);
    ListSkillMods.emplace_back(Barter_SellMod);
    ListSkillMods.emplace_back(EnergyWeapons_Damage);
    ListSkillMods.emplace_back(EnergyWeapons_Accuracy);
    ListSkillMods.emplace_back(Explosives_Damage);
    ListSkillMods.emplace_back(Explosives_Accuracy);
    ListSkillMods.emplace_back(Guns_Damage);
    ListSkillMods.emplace_back(Guns_Accuracy);
    ListSkillMods.emplace_back(Medicine_UsageMod);
    ListSkillMods.emplace_back(MeleeWeapons_Damage);
    ListSkillMods.emplace_back(MeleeWeapons_Secondary);
    ListSkillMods.emplace_back(Sneak_SkillMod);
    ListSkillMods.emplace_back(Sneak_DetectionMod);
    ListSkillMods.emplace_back(Sneak_PickpocketMod);
    ListSkillMods.emplace_back(Survival_UsageMod);
    ListSkillMods.emplace_back(Unarmed_Damage);
    ListSkillMods.emplace_back(Unarmed_Secondary);

    ListSkillPerks.emplace_back(Perk_Barter);
    ListSkillPerks.emplace_back(Perk_EnergyWeapons);
    ListSkillPerks.emplace_back(Perk_Explosives);
    ListSkillPerks.emplace_back(Perk_Guns);
    ListSkillPerks.emplace_back(Perk_Lockpick);
    ListSkillPerks.emplace_back(Perk_Medicine);
    ListSkillPerks.emplace_back(Perk_MeleeWeapons);
    ListSkillPerks.emplace_back(Perk_Repair);
    ListSkillPerks.emplace_back(Perk_Science);
    ListSkillPerks.emplace_back(Perk_Sneak);
    ListSkillPerks.emplace_back(Perk_Speech);
    ListSkillPerks.emplace_back(Perk_Survival);
    ListSkillPerks.emplace_back(Perk_Unarmed);
    ListSkillPerks.emplace_back(Perk_Hardcore);

    tArray<BGSPerk*> Perks = (*g_dataHandler)->arrPERK;
    for (int i = 0; i < Perks.count; i++) {
        if (!Perks[i]->hidden && (Perks[i]->numRanks > 0)) {
            ((Perks[i]->trait) ? ListTraits : ListPerks).emplace_back(Perks[i]);
            ListMaster.emplace_back(Perks[i]);
        }
    }

    // ------------------------------------------------------------------------
    // Create Perk Sorting Order
    // ------------------------------------------------------------------------
    ListSortOrder.emplace_back(StringVector{}); // Level
    ListSortOrder[0].emplace_back("Level");

    ListSortOrder.emplace_back(StringVector{}); // SPECIAL
    for (auto iter : ListSpecial)
        ListSortOrder[1].emplace_back(iter->GetFullName());

    ListSortOrder.emplace_back(StringVector{}); // Skills
    for (auto iter : ListSkills)
        ListSortOrder[2].emplace_back(iter->GetFullName());

    ListSortOrder.emplace_back(StringVector{}); // Perks
    ListSortOrder[3].emplace_back("Perk");

    ListSortOrder.emplace_back(StringVector{}); // Anything Else
	ListSortOrder[4].emplace_back(std::string{});

	// ------------------------------------------------------------------------
	// Create Effect Sorting Order
	// ------------------------------------------------------------------------
	EffectSortOrder.emplace_back(UInt32Vector{});
	for (auto iter : ListSpecial)
		EffectSortOrder[0].emplace_back(iter->formID);

	EffectSortOrder.emplace_back(UInt32Vector{});
	for (auto iter : ListSkills)
		EffectSortOrder[1].emplace_back(iter->formID);

	EffectSortOrder.emplace_back(UInt32Vector{});
	EffectSortOrder[2].emplace_back(Health->formID);
	EffectSortOrder[2].emplace_back(0x2D7);					// HealRate
	EffectSortOrder[2].emplace_back(ActionPoints->formID);	
	EffectSortOrder[2].emplace_back(0x2D8);					// ActionPointsRate
	EffectSortOrder[2].emplace_back(0x2DC);					// CarryWeight

	EffectSortOrder.emplace_back(UInt32Vector{});
	EffectSortOrder[3].emplace_back(0);

    // ------------------------------------------------------------------------
    // Check Forms
    // ------------------------------------------------------------------------
    if (ISettings::GetBool("General:CheckLoadedForms", false)) {
        if (!CheckLoadedForms()) {
            _LogError("Forms::Load - Error while checking Forms.");
            return false;
        }
    }

    // ------------------------------------------------------------------------
    // End
    // ------------------------------------------------------------------------
    _LogMessage("Forms::Load - Finished.");
    return true;
}

void Forms::Unload() {
    _LogMessage("Forms::Unload - Stopping PlayerThread...");

    Values::RunPlayerThread = false;
    while (!Values::PlayerThreadDidExit)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    _LogMessage("Forms::Unload - Stopped PlayerThread.");

    ClearMaps();

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
    RadX                    = nullptr;

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
	InventoryWeight         = nullptr;
	PipboyLightEnabled		= nullptr;
	RadiationSourceCount	= nullptr;
    SkillPointBonus         = nullptr;
    
    Dehydration             = nullptr;
    Starvation              = nullptr;
    SleepDeprivation        = nullptr;

    StimpakOrder            = nullptr;
    DoctorsBag              = nullptr;

    Perk_Barter             = nullptr;
    Perk_EnergyWeapons      = nullptr;
    Perk_Explosives         = nullptr;
    Perk_Guns               = nullptr;
    Perk_Lockpick           = nullptr;
    Perk_Medicine           = nullptr;
    Perk_MeleeWeapons       = nullptr;
    Perk_Repair             = nullptr;
    Perk_Science            = nullptr;
    Perk_Sneak              = nullptr;
    Perk_Speech             = nullptr;
    Perk_Survival           = nullptr;
    Perk_Unarmed            = nullptr;
    Perk_Hardcore           = nullptr;

    PlayerTags              = 0;

    PlayerLevelUp           = false;
    PlayerLevel             = 1;

    _LogMessage("Forms::Unload - Finished.");
}

TESForm* Forms::Lookup(UInt32 FormID) {
    return LookupFormFromMod(m_PluginInfo, FormID);
}

void Forms::AddDependent(ActorValueInfo* Parent, ActorValueInfo* Child) {
    if (Parent->numDependentAVs < sizeof(Parent->dependentAVs) / 8) {
        Parent->dependentAVs[Parent->numDependentAVs++] = Child;
        RegisteredDependents.emplace(Child, Parent);
        return;
    }

    _LogWarning("Forms::AddDependent: Failed to add %s as a dependent to %s.", Child->GetFullName(), Parent->GetFullName());
}

ActorValueInfo* Forms::GetDependent(ActorValueInfo* Child) {
    auto search = RegisteredDependents.find(Child);
    if (search != RegisteredDependents.end())
        return search->second;

    return nullptr;
}

ActorValueInfo* Forms::GetSkillByName(const char* name) {
    for (auto iter : ListSkills)
        if (!_stricmp(name, iter->avName))
            return iter;

    return nullptr;
}

// ------------------------------------------------------------------------------------------------
// Private
// ------------------------------------------------------------------------------------------------

bool Forms::CheckForPlugin() {
    m_PluginInfo = (*g_dataHandler)->LookupLoadedModByName(m_PluginName.c_str());
    return (m_PluginInfo != nullptr);
}

bool Forms::CheckLoadedForms() {
    _LogMessage("Forms::CheckLoadedForms() - Starting.");
    _LogMessage("Look for \"Finished.\" to know when the check is complete.");
    _DebugMessageNT("");

    ITimeKeeper CheckLoadedFormsTimer = ITimeKeeper();

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
    CheckForm(InventoryWeight,          kFormType_AVIF, avName);
	CheckForm(PipboyLightEnabled,       kFormType_AVIF, avName);
	CheckForm(RadiationSourceCount,     kFormType_AVIF, avName);
	CheckForm(SkillPointBonus,          kFormType_AVIF, avName);
    CheckForm(Dehydration,              kFormType_AVIF, avName);
    CheckForm(Starvation,               kFormType_AVIF, avName);
    CheckForm(SleepDeprivation,         kFormType_AVIF, avName);
    CheckForm(StimpakOrder,             kFormType_FLST, GetEditorID());
    CheckForm(DoctorsBag,               kFormType_ALCH, GetFullName());

    _LogMessage("Forms::CheckLoadedForms() - Finished.");
    _LogMessage("Time Elapsed: %fms", CheckLoadedFormsTimer.Format(ITimeKeeper::kDuration_Milli));
    return true;
}

void Forms::ClearMaps() {
    ListPerks.              clear();
    ListTraits.             clear();
    ListMaster.             clear();
    ListSkills.             clear();
    ListSpecial.            clear();
    ListSkillMods.          clear();
    ListSortOrder.          clear();
    ListSkillPerks.         clear();
	EffectSortOrder.		clear();
	ObjectTypes.			clear();
    RegisteredDependents.   clear();
}

// ------------------------------------------------------------------------------------------------
// Initialize Statics
// ------------------------------------------------------------------------------------------------

ActorValueInfo*     Forms::Strength                     = nullptr;
ActorValueInfo*     Forms::Perception                   = nullptr;
ActorValueInfo*     Forms::Endurance                    = nullptr;
ActorValueInfo*     Forms::Charisma                     = nullptr;
ActorValueInfo*     Forms::Intelligence                 = nullptr;
ActorValueInfo*     Forms::Agility                      = nullptr;
ActorValueInfo*     Forms::Luck                         = nullptr;
ActorValueInfo*     Forms::Experience                   = nullptr;
ActorValueInfo*     Forms::Health                       = nullptr;
ActorValueInfo*     Forms::ActionPoints                 = nullptr;
ActorValueInfo*     Forms::Rads                         = nullptr;
ActorValueInfo*     Forms::PoisonResistance             = nullptr;
ActorValueInfo*     Forms::RadResistExposure            = nullptr;
ActorValueInfo*     Forms::RadResistIngestion           = nullptr;
ActorValueInfo*     Forms::RadHealthMax                 = nullptr;
ActorValueInfo*     Forms::HeadCondition                = nullptr;
ActorValueInfo*     Forms::TorsoCondition               = nullptr;
ActorValueInfo*     Forms::LeftArmCondition             = nullptr;
ActorValueInfo*     Forms::RightArmCondition            = nullptr;
ActorValueInfo*     Forms::LeftLegCondition             = nullptr;
ActorValueInfo*     Forms::RightLegCondition            = nullptr;
AlchemyItem*        Forms::RadX                         = nullptr;
ActorValueInfo*     Forms::Barter                       = nullptr;
ActorValueInfo*     Forms::EnergyWeapons                = nullptr;
ActorValueInfo*     Forms::Explosives                   = nullptr;
ActorValueInfo*     Forms::Guns                         = nullptr;
ActorValueInfo*     Forms::Lockpick                     = nullptr;
ActorValueInfo*     Forms::Medicine                     = nullptr;
ActorValueInfo*     Forms::MeleeWeapons                 = nullptr;
ActorValueInfo*     Forms::Repair                       = nullptr;
ActorValueInfo*     Forms::Science                      = nullptr;
ActorValueInfo*     Forms::Sneak                        = nullptr;
ActorValueInfo*     Forms::Speech                       = nullptr;
ActorValueInfo*     Forms::Survival                     = nullptr;
ActorValueInfo*     Forms::Unarmed                      = nullptr;
ActorValueInfo*     Forms::Barter_BuyMod                = nullptr;
ActorValueInfo*     Forms::Barter_SellMod               = nullptr;
ActorValueInfo*     Forms::EnergyWeapons_Damage         = nullptr;
ActorValueInfo*     Forms::EnergyWeapons_Accuracy       = nullptr;
ActorValueInfo*     Forms::Explosives_Damage            = nullptr;
ActorValueInfo*     Forms::Explosives_Accuracy          = nullptr;
ActorValueInfo*     Forms::Guns_Damage                  = nullptr;
ActorValueInfo*     Forms::Guns_Accuracy                = nullptr;
ActorValueInfo*     Forms::Medicine_UsageMod            = nullptr;
ActorValueInfo*     Forms::MeleeWeapons_Damage          = nullptr;
ActorValueInfo*     Forms::MeleeWeapons_Secondary       = nullptr;
ActorValueInfo*     Forms::Sneak_SkillMod               = nullptr;
ActorValueInfo*     Forms::Sneak_DetectionMod           = nullptr;
ActorValueInfo*     Forms::Sneak_PickpocketMod          = nullptr;
ActorValueInfo*     Forms::Survival_UsageMod            = nullptr;
ActorValueInfo*     Forms::Unarmed_Damage               = nullptr;
ActorValueInfo*     Forms::Unarmed_Secondary            = nullptr;
ActorValueInfo*     Forms::ActionPointsMult             = nullptr;
ActorValueInfo*     Forms::PipboyLightEnabled           = nullptr;
ActorValueInfo*     Forms::RadiationSourceCount         = nullptr;
ActorValueInfo*     Forms::InventoryWeight              = nullptr;
ActorValueInfo*     Forms::SkillPointBonus              = nullptr;
ActorValueInfo*     Forms::Dehydration                  = nullptr;
ActorValueInfo*     Forms::Starvation                   = nullptr;
ActorValueInfo*     Forms::SleepDeprivation             = nullptr;
BGSListForm*        Forms::StimpakOrder                 = nullptr;
AlchemyItem*        Forms::DoctorsBag                   = nullptr;
BGSPerk*            Forms::Perk_Barter                  = nullptr;
BGSPerk*            Forms::Perk_EnergyWeapons           = nullptr;
BGSPerk*            Forms::Perk_Explosives              = nullptr;
BGSPerk*            Forms::Perk_Guns                    = nullptr;
BGSPerk*            Forms::Perk_Lockpick                = nullptr;
BGSPerk*            Forms::Perk_Medicine                = nullptr;
BGSPerk*            Forms::Perk_MeleeWeapons            = nullptr;
BGSPerk*            Forms::Perk_Repair                  = nullptr;
BGSPerk*            Forms::Perk_Science                 = nullptr;
BGSPerk*            Forms::Perk_Sneak                   = nullptr;
BGSPerk*            Forms::Perk_Speech                  = nullptr;
BGSPerk*            Forms::Perk_Survival                = nullptr;
BGSPerk*            Forms::Perk_Unarmed                 = nullptr;
BGSPerk*            Forms::Perk_Hardcore                = nullptr;

ActorValueInfo*     Forms::ItemCondMaxHealth            = nullptr;
ActorValueInfo*     Forms::ItemCondMinHealth            = nullptr;
ActorValueInfo*     Forms::ItemCondStartCond            = nullptr;

AVVector            Forms::ListSpecial;
AVVector            Forms::ListSkills;
AVVector            Forms::ListSkillMods;

PerkVector          Forms::ListPerks;
PerkVector          Forms::ListTraits;
PerkVector          Forms::ListSkillPerks;
PerkVector          Forms::ListMaster;

StringVectorVector  Forms::ListSortOrder;
UInt32VectorVector	Forms::EffectSortOrder;

KeywordMap			Forms::ObjectTypes;

ActorValueMap       Forms::RegisteredDependents;
PerkRankMap         Forms::RegisteredActors;

UInt16              Forms::PlayerTags;

bool                Forms::PlayerLevelUp                = false;
UInt16              Forms::PlayerLevel                  = 1;

std::string         Forms::m_PluginName;
const ModInfo*      Forms::m_PluginInfo;