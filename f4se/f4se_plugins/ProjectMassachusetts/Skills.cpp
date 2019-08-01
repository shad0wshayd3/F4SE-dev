#include "Skills.h"

#include "Data.h"
#include "f4se_globals/Globals.h"

#include "f4se/GameSettings.h"

#include <algorithm>
#include <unordered_map>

namespace Calculate {
    std::unordered_multimap<ActorValueInfo*, ActorValueInfo*> HardcodedMap;
    std::unordered_multimap<ActorValueInfo*, TESGlobal*> UpdateMap;
    std::unordered_map<std::string, ActorValueInfo*> SkillMap;

    ActorValueInfo* GetDependent(ActorValueInfo* Hardcoded) {
        auto search = HardcodedMap.find(Hardcoded);
        if (search != HardcodedMap.end())
            return search->second;

        return nullptr;
    }

    bool AddDependentAV(ActorValueInfo* Hardcoded, ActorValueInfo* Dependent) {
        if (Dependent->numDependentAVs < sizeof(Dependent->dependentAVs) / 8) {
            Dependent->dependentAVs[Dependent->numDependentAVs++] = Hardcoded;
            return true;
        }

        return false;
    }

    void RegisterDerived(ActorValueInfo* Hardcoded, float(*f)(ActorValueOwner*, ActorValueInfo&)) {
        Hardcoded->derived_func_vtable = reinterpret_cast<void*>(ActorValueDerivedVtbl.GetUIntPtr());
        Hardcoded->derived_func_ptr = &Hardcoded->derived_func_vtable;
        Hardcoded->derived_func = f;
    }

    void RegisterCalc(ActorValueInfo* Hardcoded, void(*f)(Actor*, ActorValueInfo&, float, float, Actor*)) {
        Hardcoded->func_vtable = reinterpret_cast<void*>(ActorValueCalcVtbl.GetUIntPtr());
        Hardcoded->func_ptr = &Hardcoded->func_vtable;
        Hardcoded->func = f;
    }

    void RegisterHardcodedAV(ActorValueInfo* Hardcoded, float(*CalcFunction)(ActorValueOwner*, ActorValueInfo&), ActorValueInfo* Dependent) {
        RegisterDerived(Hardcoded, CalcFunction);
        AddDependentAV(Hardcoded, Dependent);
        HardcodedMap.emplace(Hardcoded, Dependent);
    }

    void RegisterHardcodedAV(ActorValueInfo* Hardcoded, float(*CalcFunction)(ActorValueOwner*, ActorValueInfo&), ActorValueInfo* Dependent, ActorValueInfo* UpdateWith) {
        RegisterDerived(Hardcoded, CalcFunction);
        AddDependentAV(Hardcoded, Dependent);
        AddDependentAV(Hardcoded, UpdateWith);
        HardcodedMap.emplace(Hardcoded, Dependent);
    }

    void RegisterHardcodedAV(ActorValueInfo* Hardcoded, float(*CalcFunction)(ActorValueOwner*, ActorValueInfo&), ActorValueInfo* Dependent, ActorValueInfo* UpdateW01, ActorValueInfo* UpdateW02) {
        RegisterDerived(Hardcoded, CalcFunction);
        AddDependentAV(Hardcoded, Dependent);
        AddDependentAV(Hardcoded, UpdateW01);
        AddDependentAV(Hardcoded, UpdateW02);
        HardcodedMap.emplace(Hardcoded, Dependent);
    }

    void RegisterHardcodedAV(ActorValueInfo* Hardcoded, float(*CalcFunction)(ActorValueOwner*, ActorValueInfo&), ActorValueInfo* Dependent, ActorValueInfo* UpdateW01, void(*rCalcFunction)(Actor*, ActorValueInfo&, float, float, Actor*), TESGlobal* UpdateGlobal) {
        RegisterDerived(Hardcoded, CalcFunction);
        RegisterCalc(Hardcoded, rCalcFunction);
        AddDependentAV(Hardcoded, Dependent);
        AddDependentAV(Hardcoded, UpdateW01);
        HardcodedMap.emplace(Hardcoded, Dependent);
        UpdateMap.emplace(Hardcoded, UpdateGlobal);
    }

    float Skill(ActorValueOwner* owner, ActorValueInfo& avif) {
        if (!owner || !&avif)
            return 0.0;
        return (2 + (2 * owner->GetValue(GetDependent(&avif))) + ceilf(owner->GetValue(g_Data.Luck) / 2));
    }

    float BarterBuyMod(ActorValueOwner* owner, ActorValueInfo& avif) {
        if (!owner || !&avif)
            return 0.0;
        return (155 - (0.45 * owner->GetValue(GetDependent(&avif))));
    }

    float BarterSellMod(ActorValueOwner* owner, ActorValueInfo& avif) {
        if (!owner || !&avif)
            return 0.0;
        return (45 + (0.45 * owner->GetValue(GetDependent(&avif))));
    }

    float WeaponDamage(ActorValueOwner* owner, ActorValueInfo& avif) {
        if (!owner || !&avif)
            return 0.0;
        return ((0.5 * (owner->GetValue(GetDependent(&avif)) / 100)) + 0.5);
    }

    float WeaponAccuracy(ActorValueOwner* owner, ActorValueInfo& avif) {
        if (!owner || !&avif)
            return 0.0;
        return ((300 - owner->GetValue(GetDependent(&avif))) / 250);
    }

    float WeaponSecondary(ActorValueOwner* owner, ActorValueInfo& avif) {
        if (!owner || !&avif)
            return 0.0;
        return (owner->GetValue(GetDependent(&avif)) / 15);
    }

    float UnarmedDamage(ActorValueOwner* owner, ActorValueInfo& avif) {
        if (!owner || !&avif)
            return 0.0;
        return ceilf((owner->GetValue(GetDependent(&avif)) / 20.0) + 0.5);
    }

    float UsageMod(ActorValueOwner* owner, ActorValueInfo& avif) {
        if (!owner || !&avif)
            return 0.0;
        return ((owner->GetValue(GetDependent(&avif)) / 50) + 1.0);
    }

    float SneakSkill(ActorValueOwner* owner, ActorValueInfo& avif) {
        if (!owner || !&avif)
            return 0.0;
        return owner->GetValue(GetDependent(&avif)) / 1.8;
    }

    float SneakDetection(ActorValueOwner* owner, ActorValueInfo& avif) {
        if (!owner || !&avif)
            return 0.0;
        return ((owner->GetValue(GetDependent(&avif)) - 50) / -150) + 0.5;
    }

    float SneakPickpocket(ActorValueOwner* owner, ActorValueInfo& avif) {
        if (!owner || !&avif)
            return 0.0;
        return owner->GetValue(GetDependent(&avif)) * 0.8;
    }

    float RadHealthMax(ActorValueOwner* owner, ActorValueInfo& avif) {
        return 10000.0;
    }

    float ActionPoints(ActorValueOwner* owner, ActorValueInfo& avif) {
        if (!owner)
            return 0.0;
        float apBase = GetGameSetting("fAVDActionPointsBase")->data.f32;
        float apMult = GetGameSetting("fAVDActionPointsMult")->data.f32;
        float apCalc = apBase + (apMult * owner->GetValue(g_Data.Agility));
        return (apCalc * owner->GetValue(g_Data.ActionPointsMult));
    }

    float DamageResist(ActorValueInfo* avif, float Damage, float DamageResist) {
        if (!avif)
            return 0.0;
        float result = min(DamageResist, GetGameSetting("fPhysicalMaxDamageReduction")->data.f32);
        return max(0, ((100 - result) * 0.01));
    }

    int SkillPoints(int PlayerLevel) {
        float intMod = 10 + ((*g_player)->actorValueOwner.GetBase(g_Data.Intelligence) / 2.0);
        intMod += (*g_player)->actorValueOwner.GetValue(g_Data.SkillOffset);

        return (PlayerLevel % 2 == 0) ? ceilf(intMod) : floorf(intMod);
    }

    int PerkPoints() {
        return 1;
    }

    int TraitPoints() {
        return 2;
    }

    int TagPoints() {
        return 3;
    }
}

namespace Skills {
    using namespace Calculate;

    ActorValueInfo* GetByName(std::string skillName) {
        std::transform(skillName.begin(), skillName.end(), skillName.begin(), ::tolower);

        auto search = SkillMap.find(skillName);
        if (search != SkillMap.end())
            return search->second;

        return nullptr;
    }

    TESGlobal* GetGlobal(ActorValueInfo* Hardcoded) {
        auto search = UpdateMap.find(Hardcoded);
        if (search != UpdateMap.end())
            return search->second;

        return nullptr;
    }

    void SetRelatedGlobal(Actor* actor, ActorValueInfo &avif, float value, float mod, Actor* unk00) {
        if (!actor || !&avif)
            return;

        if (actor != (*g_player))
            return;

        TESGlobal* updateGlobal = GetGlobal(&avif);
        if (updateGlobal != nullptr)
            updateGlobal->value = actor->actorValueOwner.GetValue(&avif);
    }

    void RegisterSkills() {
        HardcodedMap.clear();
        UpdateMap.clear();
        SkillMap.clear();

        // Register the skills to be updated when their corresponding SPECIAL values are updated
        RegisterHardcodedAV(g_Data.Barter,        Skill, g_Data.Charisma,     g_Data.Luck, SetRelatedGlobal, g_Data.DialogueBarter);
        RegisterHardcodedAV(g_Data.EnergyWeapons, Skill, g_Data.Perception,   g_Data.Luck, SetRelatedGlobal, g_Data.DialogueEnergyWeapons);
        RegisterHardcodedAV(g_Data.Explosives,    Skill, g_Data.Perception,   g_Data.Luck, SetRelatedGlobal, g_Data.DialogueExplosives);
        RegisterHardcodedAV(g_Data.Guns,          Skill, g_Data.Agility,      g_Data.Luck, SetRelatedGlobal, g_Data.DialogueGuns);
        RegisterHardcodedAV(g_Data.Lockpick,      Skill, g_Data.Perception,   g_Data.Luck, SetRelatedGlobal, g_Data.DialogueLockpick);
        RegisterHardcodedAV(g_Data.Medicine,      Skill, g_Data.Intelligence, g_Data.Luck, SetRelatedGlobal, g_Data.DialogueMedicine);
        RegisterHardcodedAV(g_Data.MeleeWeapons,  Skill, g_Data.Strength,     g_Data.Luck, SetRelatedGlobal, g_Data.DialogueMeleeWeapons);
        RegisterHardcodedAV(g_Data.Repair,        Skill, g_Data.Intelligence, g_Data.Luck, SetRelatedGlobal, g_Data.DialogueRepair);
        RegisterHardcodedAV(g_Data.Science,       Skill, g_Data.Intelligence, g_Data.Luck, SetRelatedGlobal, g_Data.DialogueScience);
        RegisterHardcodedAV(g_Data.Sneak,         Skill, g_Data.Agility,      g_Data.Luck, SetRelatedGlobal, g_Data.DialogueSneak);
        RegisterHardcodedAV(g_Data.Speech,        Skill, g_Data.Charisma,     g_Data.Luck, SetRelatedGlobal, g_Data.DialogueSpeech);
        RegisterHardcodedAV(g_Data.Survival,      Skill, g_Data.Endurance,    g_Data.Luck, SetRelatedGlobal, g_Data.DialogueSurvival);
        RegisterHardcodedAV(g_Data.Unarmed,       Skill, g_Data.Endurance,    g_Data.Luck, SetRelatedGlobal, g_Data.DialogueUnarmed);

        SkillMap.emplace("barter",        g_Data.Barter);
        SkillMap.emplace("energyweapons", g_Data.EnergyWeapons);
        SkillMap.emplace("explosives",    g_Data.Explosives);
        SkillMap.emplace("guns",          g_Data.Guns);
        SkillMap.emplace("lockpick",      g_Data.Lockpick);
        SkillMap.emplace("medicine",      g_Data.Medicine);
        SkillMap.emplace("meleeweapons",  g_Data.MeleeWeapons);
        SkillMap.emplace("repair",        g_Data.Repair);
        SkillMap.emplace("science",       g_Data.Science);
        SkillMap.emplace("sneak",         g_Data.Sneak);
        SkillMap.emplace("speech",        g_Data.Speech);
        SkillMap.emplace("survival",      g_Data.Survival);
        SkillMap.emplace("unarmed",       g_Data.Unarmed);

        RegisterHardcodedAV(g_Data.ActionPoints, ActionPoints, g_Data.Agility, g_Data.ActionPointsMult);
        RegisterHardcodedAV(g_Data.RadHealthMax, RadHealthMax, g_Data.Rads);

        // Also register the skill mods for SPECIAL updates and skill updates since dependency updates don't recurse
        RegisterHardcodedAV(g_Data.Barter_BuyMod,          BarterBuyMod,    g_Data.Barter,         g_Data.Charisma,       g_Data.Luck);
        RegisterHardcodedAV(g_Data.Barter_SellMod,         BarterSellMod,   g_Data.Barter,         g_Data.Charisma,       g_Data.Luck);
        RegisterHardcodedAV(g_Data.EnergyWeapons_Damage,   WeaponDamage,    g_Data.EnergyWeapons,  g_Data.Perception,     g_Data.Luck);
        RegisterHardcodedAV(g_Data.EnergyWeapons_Accuracy, WeaponAccuracy,  g_Data.EnergyWeapons,  g_Data.Perception,     g_Data.Luck);
        RegisterHardcodedAV(g_Data.Explosives_Damage,      WeaponDamage,    g_Data.Explosives,     g_Data.Perception,     g_Data.Luck);
        RegisterHardcodedAV(g_Data.Explosives_Accuracy,    WeaponAccuracy,  g_Data.Explosives,     g_Data.Perception,     g_Data.Luck);
        RegisterHardcodedAV(g_Data.Guns_Damage,            WeaponDamage,    g_Data.Guns,           g_Data.Agility,        g_Data.Luck);
        RegisterHardcodedAV(g_Data.Guns_Accuracy,          WeaponAccuracy,  g_Data.Guns,           g_Data.Agility,        g_Data.Luck);
        RegisterHardcodedAV(g_Data.Medicine_UsageMod,      UsageMod,        g_Data.Medicine,       g_Data.Intelligence,   g_Data.Luck);
        RegisterHardcodedAV(g_Data.MeleeWeapons_Damage,    WeaponDamage,    g_Data.MeleeWeapons,   g_Data.Strength,       g_Data.Luck);
        RegisterHardcodedAV(g_Data.MeleeWeapons_Secondary, WeaponSecondary, g_Data.MeleeWeapons,   g_Data.Strength,       g_Data.Luck);
        RegisterHardcodedAV(g_Data.Sneak_SkillMod,         SneakSkill,      g_Data.Sneak,          g_Data.Agility,        g_Data.Luck);
        RegisterHardcodedAV(g_Data.Sneak_DetectionMod,     SneakDetection,  g_Data.Sneak,          g_Data.Agility,        g_Data.Luck);
        RegisterHardcodedAV(g_Data.Sneak_PickpocketMod,    SneakPickpocket, g_Data.Sneak,          g_Data.Agility,        g_Data.Luck);
        RegisterHardcodedAV(g_Data.Survival_UsageMod,      UsageMod,        g_Data.Survival,       g_Data.Endurance,      g_Data.Luck);
        RegisterHardcodedAV(g_Data.Unarmed_Damage,         UnarmedDamage,   g_Data.Unarmed,        g_Data.Endurance,      g_Data.Luck);
        RegisterHardcodedAV(g_Data.Unarmed_Secondary,      WeaponSecondary, g_Data.Unarmed,        g_Data.Endurance,      g_Data.Luck);
    }
}