#include "ValueManager.h"

#include "f4se/GameSettings.h"

#include <chrono>
#include <thread>

void ValueManager::Init() {
    // Register skills
    RegisterActorValue(Barter,          Charisma,       Luck,               DefaultSkillFormula);
    RegisterActorValue(EnergyWeapons,   Perception,     Luck,               DefaultSkillFormula);
    RegisterActorValue(Explosives,      Perception,     Luck,               DefaultSkillFormula);
    RegisterActorValue(Guns,            Agility,        Luck,               DefaultSkillFormula);
    RegisterActorValue(Lockpick,        Perception,     Luck,               DefaultSkillFormula);
    RegisterActorValue(Medicine,        Intelligence,   Luck,               DefaultSkillFormula);
    RegisterActorValue(MeleeWeapons,    Strength,       Luck,               DefaultSkillFormula);
    RegisterActorValue(Repair,          Intelligence,   Luck,               DefaultSkillFormula);
    RegisterActorValue(Science,         Intelligence,   Luck,               DefaultSkillFormula);
    RegisterActorValue(Sneak,           Agility,        Luck,               DefaultSkillFormula);
    RegisterActorValue(Speech,          Charisma,       Luck,               DefaultSkillFormula);
    RegisterActorValue(Survival,        Endurance,      Luck,               DefaultSkillFormula);
    RegisterActorValue(Unarmed,         Endurance,      Luck,               DefaultSkillFormula);

    // Register non-skills with new formulas.
    RegisterActorValue(ActionPoints,    Agility,        ActionPointsMult,   ActionPointsFormula);
    RegisterActorValue(RadHealthMax,    Rads,                               RadHealthMaxFormula);

    // Add AVs to Races
    tArray<TESRace*> RaceList = (*g_dataHandler)->arrRACE;
    for (int i = 0; i < RaceList.count; i++) {
        for (auto iter : m_SkillList)
            RaceList[i]->propertySheet.sheet->Push({ iter, 0 });

        for (auto iter : m_SkillModList)
            RaceList[i]->propertySheet.sheet->Push({ iter, 0 });

        RaceList[i]->propertySheet.sheet->Push({ InventoryWeight, 0 });
    }

    // Add Perks to NPCs
    PerkRankList defaultPerkRanks;
    for (auto iter : m_SkillPList)
        defaultPerkRanks.emplace_back(BGSPerkRankArray::Data{ iter, 1 });

    tArray<TESNPC*> NPCList = (*g_dataHandler)->arrNPC_;
    for (int i = 0; i < NPCList.count; i++) {

        TESNPC* thisNPC = NPCList[i];
        m_PerkMap.emplace_back(defaultPerkRanks);

        for (int j = thisNPC->perkRankArray.numPerkRanks - 1; j >= 0; j--)
            m_PerkMap.back().insert(m_PerkMap.back().begin(), BGSPerkRankArray::Data{
                thisNPC->perkRankArray.perkRanks[j].perk,
                thisNPC->perkRankArray.perkRanks[j].rank
                });

        thisNPC->perkRankArray.perkRanks    = m_PerkMap.back().data();
        thisNPC->perkRankArray.numPerkRanks = m_PerkMap.back().size();
    }

    b_RunThread = true;
    std::thread UpdateThread(ThreadedActorValueUpdate);
    UpdateThread.detach();
}

void ValueManager::Unload() {
    b_RunThread = false;
    while (!b_ThreadStopped) { }

    m_DependentMap.clear();
    m_PerkMap.clear();

    b_RunThread     = false;
    b_ThreadStopped = false;
}

void ValueManager::AddDependent(ActorValueInfo* Parent, ActorValueInfo* Child) {
    if (Parent->numDependentAVs < sizeof(Parent->dependentAVs) / 8) {
        Parent->dependentAVs[Parent->numDependentAVs++] = Child;
        m_DependentMap.emplace(Child, Parent);
        return;
    }

    _LOGWARNING("ValueManager: Failed to add %s as a dependent to %s.", Child->GetFullName(), Parent->GetFullName());
}

ActorValueInfo* ValueManager::GetDependent(ActorValueInfo* Child) {
    auto search = m_DependentMap.find(Child);
    if (search != m_DependentMap.end())
        return search->second;

    return nullptr;
}

// ------------------------------------------------------------------------------------------------
// Skill Functions
// ------------------------------------------------------------------------------------------------

float ValueManager::DefaultSkillFormula(ActorValueOwner* Owner, ActorValueInfo& AVIF) {
    if (!Owner || !&AVIF)
        return 0.0;
    return (2 + (2 * Owner->GetValue(GetDependent(&AVIF))) + ceilf(Owner->GetValue(Luck) / 2));;
}

float ValueManager::ActionPointsFormula(ActorValueOwner* Owner, ActorValueInfo& AVIF) {
    if (!Owner)
        return 0.0;
    float apBase = GetGameSetting("fAVDActionPointsBase")->data.f32;
    float apMult = GetGameSetting("fAVDActionPointsMult")->data.f32;
    float apCalc = apBase + (apMult * Owner->GetValue(Agility));
    return (apCalc * Owner->GetValue(ActionPointsMult));
}

float ValueManager::RadHealthMaxFormula(ActorValueOwner* Owner, ActorValueInfo& AVIF) {
    return 10000.0;
}

float ValueManager::DamageResistFormula(ActorValueInfo* AVIF, float Damage, float DamageResist) {
    if (!AVIF)
        return 0.0;
    float result = min(DamageResist, GetGameSetting("fPhysicalMaxDamageReduction")->data.f32);
    return max(0, ((100 - result) * 0.01));
}

void ValueManager::ActorValueUpdate(Actor* actor) {
    // Barter
    float BarterValue = actor->actorValueOwner.GetValue(Barter);
    actor->actorValueOwner.SetBase(Barter_BuyMod, (155 - (0.45 * BarterValue)));
    actor->actorValueOwner.SetBase(Barter_SellMod, (45 + (0.45 * BarterValue)));
    if (actor == (*g_player)) DialogueBarter->value = BarterValue;

    // Energy Weapons
    float EnergyWeaponsValue = actor->actorValueOwner.GetValue(EnergyWeapons);
    actor->actorValueOwner.SetBase(EnergyWeapons_Accuracy, WEAPON_ACC(EnergyWeaponsValue));
    actor->actorValueOwner.SetBase(EnergyWeapons_Damage, WEAPON_DAM(EnergyWeaponsValue));
    if (actor == (*g_player)) DialogueEnergyWeapons->value = EnergyWeaponsValue;

    // Explosives
    float ExplosivesValue = actor->actorValueOwner.GetValue(Explosives);
    actor->actorValueOwner.SetBase(Explosives_Accuracy, WEAPON_ACC(ExplosivesValue));
    actor->actorValueOwner.SetBase(Explosives_Damage, WEAPON_DAM(ExplosivesValue));
    if (actor == (*g_player)) DialogueExplosives->value = ExplosivesValue;

    // Guns
    float GunsValue = actor->actorValueOwner.GetValue(Guns);
    actor->actorValueOwner.SetBase(Guns_Accuracy, WEAPON_ACC(GunsValue));
    actor->actorValueOwner.SetBase(Guns_Damage, WEAPON_DAM(GunsValue));
    if (actor == (*g_player)) DialogueGuns->value = GunsValue;

    // Lockpick
    float LockpickValue = actor->actorValueOwner.GetValue(Lockpick);
    if (actor == (*g_player)) DialogueLockpick->value = LockpickValue;

    // Medicine
    float MedicineValue = actor->actorValueOwner.GetValue(Medicine);
    actor->actorValueOwner.SetBase(Medicine_UsageMod, USAGE_MOD(MedicineValue));
    if (actor == (*g_player)) DialogueMedicine->value = MedicineValue;

    // Melee Weapons
    float MeleeWeaponsValue = actor->actorValueOwner.GetValue(MeleeWeapons);
    actor->actorValueOwner.SetBase(MeleeWeapons_Secondary, WEAPON_SEC(MeleeWeaponsValue));
    actor->actorValueOwner.SetBase(MeleeWeapons_Damage, WEAPON_DAM(MeleeWeaponsValue));
    if (actor == (*g_player)) DialogueMeleeWeapons->value = MeleeWeaponsValue;

    // Repair
    float RepairValue = actor->actorValueOwner.GetValue(Repair);
    if (actor == (*g_player)) DialogueRepair->value = RepairValue;

    // Science
    float ScienceValue = actor->actorValueOwner.GetValue(Science);
    if (actor == (*g_player)) DialogueScience->value = ScienceValue;

    // Sneak
    float SneakValue = actor->actorValueOwner.GetValue(Sneak);
    actor->actorValueOwner.SetBase(Sneak_SkillMod,        (SneakValue / 1.8));
    actor->actorValueOwner.SetBase(Sneak_PickpocketMod,   (SneakValue * 0.8));
    actor->actorValueOwner.SetBase(Sneak_DetectionMod,    ((SneakValue - 50) / -150.0) + 0.5);
    if (actor == (*g_player)) DialogueSneak->value = SneakValue;

    // Speech
    float SpeechValue = actor->actorValueOwner.GetValue(Speech);
    if (actor == (*g_player)) DialogueSpeech->value = SpeechValue;

    // Survival
    float SurvivalValue = actor->actorValueOwner.GetValue(Survival);
    actor->actorValueOwner.SetBase(Survival_UsageMod, USAGE_MOD(SurvivalValue));
    if (actor == (*g_player)) DialogueSurvival->value = SurvivalValue;

    // Unarmed
    float UnarmedValue = actor->actorValueOwner.GetValue(Unarmed);
    actor->actorValueOwner.SetBase(Unarmed_Secondary, WEAPON_SEC(UnarmedValue));
    actor->actorValueOwner.SetBase(Unarmed_Damage, ceilf((UnarmedValue / 20.0) + 0.5));
    if (actor == (*g_player)) DialogueUnarmed->value = UnarmedValue;

    // Inventory Weight
    float WeightValue = CALL_MEMBER_FN(actor, GetInventoryWeight)();
    actor->actorValueOwner.SetBase(InventoryWeight, WeightValue);
}

void ValueManager::ThreadedActorValueUpdate() {
    while (b_RunThread) {
        ActorValueUpdate((*g_player));
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    b_ThreadStopped = true;
    return;
}

// ------------------------------------------------------------------------------------------------
// Private
// ------------------------------------------------------------------------------------------------

void ValueManager::AddDerivedFunction(ActorValueInfo* Child, DERIVED_FUNCTION_ARG) {
    Child->derived_func_vtable = reinterpret_cast<void*>(ActorValueDerivedVtbl.GetUIntPtr());
    Child->derived_func_ptr = &Child->derived_func_vtable;
    Child->derived_func = derivedFunction;
}

void ValueManager::AddCalculatedFunction(ActorValueInfo* Child, CALC_FUNCTION_ARG) {
    Child->func_vtable = reinterpret_cast<void*>(ActorValueCalcVtbl.GetUIntPtr());
    Child->func_ptr = &Child->func_vtable;
    Child->func = calcFunction;
}

void ValueManager::RegisterActorValue(ActorValueInfo* Child, ActorValueInfo* Parent, DERIVED_FUNCTION_ARG) {
    AddDerivedFunction(Child, derivedFunction);
    AddDependent(Parent, Child);
}

void ValueManager::RegisterActorValue(ActorValueInfo* Child, ActorValueInfo* Parent, ActorValueInfo* CoParent, DERIVED_FUNCTION_ARG) {
    AddDerivedFunction(Child, derivedFunction);
    AddDependent(Parent, Child);
    AddDependent(CoParent, Child);
}

// ------------------------------------------------------------------------------------------------
// Initial
// ------------------------------------------------------------------------------------------------

DependentMap                ValueManager::m_DependentMap;
std::vector<PerkRankList>   ValueManager::m_PerkMap;

bool                        ValueManager::b_RunThread        = false;
bool                        ValueManager::b_ThreadStopped    = false;