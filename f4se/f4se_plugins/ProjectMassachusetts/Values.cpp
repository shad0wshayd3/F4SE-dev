#include "Values.h"
#include "f4se/GameSettings.h"

#include <chrono>

#define WEAPON_DAM(value) ((0.5 * (value / 100.0)) + 0.5)
#define WEAPON_ACC(value) ((300 - value) / 250.0)
#define WEAPON_SEC(value) (value / 15.0)
#define USAGE_MOD(value)  ((value / 50.0) + 1.0)

std::atomic<bool>   Values::RunPlayerThread     = false;
std::atomic<bool>   Values::PlayerThreadDidExit = false;

float DefaultSkillFormula(ActorValueOwner* Owner, ActorValueInfo& AVIF) {
    if (!Owner || !&AVIF)
        return 0.0;
    return (2 + (2 * Owner->GetValue(Forms::GetDependent(&AVIF))) + ceilf(Owner->GetValue(Forms::Luck) / 2));
}

float ActionPointsFormula(ActorValueOwner* Owner, ActorValueInfo& AVIF) {
    if (!Owner)
        return 0.0;
    float apBase = GetGameSetting("fAVDActionPointsBase")->data.f32;
    float apMult = GetGameSetting("fAVDActionPointsMult")->data.f32;
    float apCalc = apBase + (apMult * Owner->GetValue(Forms::Agility));
    return (apCalc * Owner->GetValue(Forms::ActionPointsMult));
}

float RadHealthMaxFormula(ActorValueOwner* Owner, ActorValueInfo& AVIF) {
    return 10000.0;
}

float DamageResistFormula(ActorValueInfo* AVIF, float Damage, float DamageResist) {
    if (!AVIF)
        return 0.0;
    float result = min(DamageResist, GetGameSetting("fPhysicalMaxDamageReduction")->data.f32);
    return max(0, ((100 - result) * 0.01));
}

//
//
//

void ActorValueUpdateThread(Actor* actor) {
    float BarterValue           = GetValue(actor, Forms::Barter);
    SetValue(actor, Forms::Barter_BuyMod,           (155 - (0.45 * BarterValue)));
    SetValue(actor, Forms::Barter_SellMod,          (45 + (0.45 * BarterValue)));

    float EnergyWeaponsValue    = GetValue(actor, Forms::EnergyWeapons);
    SetValue(actor, Forms::EnergyWeapons_Accuracy,  WEAPON_ACC(EnergyWeaponsValue));
    SetValue(actor, Forms::EnergyWeapons_Damage,    WEAPON_DAM(EnergyWeaponsValue));

    float ExplosivesValue       = GetValue(actor, Forms::Explosives);
    SetValue(actor, Forms::Explosives_Accuracy,     WEAPON_ACC(ExplosivesValue));
    SetValue(actor, Forms::Explosives_Damage,       WEAPON_DAM(ExplosivesValue));

    float GunsValue             = GetValue(actor, Forms::Guns);
    SetValue(actor, Forms::Guns_Accuracy,           WEAPON_ACC(GunsValue));
    SetValue(actor, Forms::Guns_Damage,             WEAPON_DAM(GunsValue));

    float MedicineValue         = GetValue(actor, Forms::Medicine);
    SetValue(actor, Forms::Medicine_UsageMod,       USAGE_MOD(MedicineValue));

    float MeleeWeaponsValue     = GetValue(actor, Forms::MeleeWeapons);
    SetValue(actor, Forms::MeleeWeapons_Secondary,  WEAPON_SEC(MeleeWeaponsValue));
    SetValue(actor, Forms::MeleeWeapons_Damage,     WEAPON_DAM(MeleeWeaponsValue));

    float SneakValue            = GetValue(actor, Forms::Sneak);
    SetValue(actor, Forms::Sneak_SkillMod,          (SneakValue / 1.8));
    SetValue(actor, Forms::Sneak_PickpocketMod,     (SneakValue * 0.8));
    SetValue(actor, Forms::Sneak_DetectionMod,      ((SneakValue - 50) / -150.0) + 0.5);

    float SurvivalValue         = GetValue(actor, Forms::Survival);
    SetValue(actor, Forms::Survival_UsageMod,       USAGE_MOD(SurvivalValue));

    float UnarmedValue          = GetValue(actor, Forms::Unarmed);
    SetValue(actor, Forms::Unarmed_Secondary,       WEAPON_SEC(UnarmedValue));
    SetValue(actor, Forms::Unarmed_Damage,          ceilf((UnarmedValue / 20.0) + 0.5));

    float WeightValue           = CALL_MEMBER_FN(actor, GetInventoryWeight)();
    SetValue(actor, Forms::InventoryWeight,         WeightValue);
}

void ActorValueUpdateThread(TESObjectREFR* refr) {
    if (refr->actorValueOwner.GetValue(Forms::Barter_BuyMod) == 0.0) {
        Actor* actor = DYNAMIC_CAST(refr, TESObjectREFR, Actor);
        ActorValueUpdateThread(actor);
    }
}

void PlayerUpdateThread() {
    Values::RunPlayerThread = true;

    while (Values::RunPlayerThread) {
        ActorValueUpdateThread((*g_player));
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    Values::PlayerThreadDidExit = true;
}

//
//
//

void AddFunction(ActorValueInfo* Child, DERIVED_FUNCTION_ARG) {
    Child->derived_func_vtable = reinterpret_cast<void*>(ActorValueDerivedVtbl.GetUIntPtr());
    Child->derived_func_ptr = &Child->derived_func_vtable;
    Child->derived_func = derivedFunction;
}

void AddFunction(ActorValueInfo* Child, CALC_FUNCTION_ARG) {
    Child->func_vtable = reinterpret_cast<void*>(ActorValueCalcVtbl.GetUIntPtr());
    Child->func_ptr = &Child->func_vtable;
    Child->func = calcFunction;
}

void RegisterActorValue(ActorValueInfo* Child, ActorValueInfo* Parent, DERIVED_FUNCTION_ARG) {
    AddFunction(Child, derivedFunction);
    Forms::AddDependent(Parent, Child);
}

void RegisterActorValue(ActorValueInfo* Child, ActorValueInfo* Parent, ActorValueInfo* CoParent, DERIVED_FUNCTION_ARG) {
    AddFunction(Child, derivedFunction);
    Forms::AddDependent(Parent, Child);
    Forms::AddDependent(CoParent, Child);
}

void Values::RegisterValues() {
    RegisterActorValue(Forms::Barter,           Forms::Charisma,        Forms::Luck,                DefaultSkillFormula);
    RegisterActorValue(Forms::EnergyWeapons,    Forms::Perception,      Forms::Luck,                DefaultSkillFormula);
    RegisterActorValue(Forms::Explosives,       Forms::Perception,      Forms::Luck,                DefaultSkillFormula);
    RegisterActorValue(Forms::Guns,             Forms::Agility,         Forms::Luck,                DefaultSkillFormula);
    RegisterActorValue(Forms::Lockpick,         Forms::Perception,      Forms::Luck,                DefaultSkillFormula);
    RegisterActorValue(Forms::Medicine,         Forms::Intelligence,    Forms::Luck,                DefaultSkillFormula);
    RegisterActorValue(Forms::MeleeWeapons,     Forms::Strength,        Forms::Luck,                DefaultSkillFormula);
    RegisterActorValue(Forms::Repair,           Forms::Intelligence,    Forms::Luck,                DefaultSkillFormula);
    RegisterActorValue(Forms::Science,          Forms::Intelligence,    Forms::Luck,                DefaultSkillFormula);
    RegisterActorValue(Forms::Sneak,            Forms::Agility,         Forms::Luck,                DefaultSkillFormula);
    RegisterActorValue(Forms::Speech,           Forms::Charisma,        Forms::Luck,                DefaultSkillFormula);
    RegisterActorValue(Forms::Survival,         Forms::Endurance,       Forms::Luck,                DefaultSkillFormula);
    RegisterActorValue(Forms::Unarmed,          Forms::Endurance,       Forms::Luck,                DefaultSkillFormula);

    // Register non-skills with new formulas.
    RegisterActorValue(Forms::ActionPoints,     Forms::Agility,         Forms::ActionPointsMult,    ActionPointsFormula);
    RegisterActorValue(Forms::RadHealthMax,     Forms::Rads,                                        RadHealthMaxFormula);

    // Add AVs to Races
    tArray<TESRace*> RaceList = (*g_dataHandler)->arrRACE;
    for (int i = 0; i < RaceList.count; i++) {
        for (auto iter : Forms::ListSkills)
            RaceList[i]->propertySheet.sheet->Push({ iter, 0 });

        for (auto iter : Forms::ListSkillMods)
            RaceList[i]->propertySheet.sheet->Push({ iter, 0 });

        RaceList[i]->propertySheet.sheet->Push({ Forms::InventoryWeight, 0 });
    }

    // Add Perks to NPCs
    PerkRankVector defaultPerkRanks;
    for (auto iter : Forms::ListSkillPerks)
        defaultPerkRanks.emplace_back(BGSPerkRankArray::Data{ iter, 1 });

    tArray<TESNPC*> NPCList = (*g_dataHandler)->arrNPC_;
    for (int i = 0; i < NPCList.count; i++) {

        TESNPC* thisNPC = NPCList[i];
        Forms::RegisteredActors.emplace_back(defaultPerkRanks);

        for (int j = thisNPC->perkRankArray.numPerkRanks - 1; j >= 0; j--)
            Forms::RegisteredActors.back().insert(Forms::RegisteredActors.back().begin(),
                BGSPerkRankArray::Data{
                    thisNPC->perkRankArray.perkRanks[j].perk,
                    thisNPC->perkRankArray.perkRanks[j].rank
                });

        thisNPC->perkRankArray.perkRanks    = Forms::RegisteredActors.back().data();
        thisNPC->perkRankArray.numPerkRanks = Forms::RegisteredActors.back().size();
    }

    RunPlayerThread     = false;
    PlayerThreadDidExit = false;
    std::thread PlayerThread(PlayerUpdateThread);
    PlayerThread.detach();
}