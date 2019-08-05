#pragma once
#include "DataManager.h"

#include <unordered_map>

typedef std::unordered_map<ActorValueInfo*, ActorValueInfo*>    DependentMap;
typedef std::vector<BGSPerkRankArray::Data>                     PerkRankList;

#define DERIVED_FUNCTION_ARG    float(*derivedFunction)(ActorValueOwner*, ActorValueInfo&)
#define CALC_FUNCTION_ARG       void(*calcFunction)(Actor*, ActorValueInfo&, float, float, Actor*)

#define WEAPON_DAM(value) ((0.5 * (value / 100.0)) + 0.5)
#define WEAPON_ACC(value) ((300 - value) / 250.0)
#define WEAPON_SEC(value) (value / 15.0)
#define USAGE_MOD(value)  ((value / 50.0) + 1.0)

class ValueManager : private DataManager {
public:
    static void                         Init();

    static void                         Unload();

    static ActorValueInfo*              GetDependent(ActorValueInfo* Child);

    static float                        DefaultSkillFormula(ActorValueOwner* Owner, ActorValueInfo& AVIF);
    static float                        ActionPointsFormula(ActorValueOwner* Owner, ActorValueInfo& AVIF);
    static float                        RadHealthMaxFormula(ActorValueOwner* Owner, ActorValueInfo& AVIF);
    static float                        DamageResistFormula(ActorValueInfo* AVIF, float Damage, float DamageResist);

    static void                         ActorValueUpdate(Actor* actor);
    static void                         ThreadedActorValueUpdate();

private:
    static void                         AddDependent(ActorValueInfo* Parent, ActorValueInfo* Child);

    static void                         AddDerivedFunction(ActorValueInfo* Child, DERIVED_FUNCTION_ARG);
    static void                         AddCalculatedFunction(ActorValueInfo* Child, CALC_FUNCTION_ARG);

    static void                         RegisterActorValue(ActorValueInfo* Child, ActorValueInfo* Parent,                           DERIVED_FUNCTION_ARG);
    static void                         RegisterActorValue(ActorValueInfo* Child, ActorValueInfo* Parent, ActorValueInfo* CoParent, DERIVED_FUNCTION_ARG);

    static DependentMap                 m_DependentMap;
    static std::vector<PerkRankList>    m_PerkMap;

    static bool                         b_RunThread;
    static bool                         b_ThreadStopped;
};