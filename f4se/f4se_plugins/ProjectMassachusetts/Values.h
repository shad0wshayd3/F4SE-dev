#pragma once
#include "Forms.h"

#include <atomic>
#include <thread>

#define DERIVED_FUNCTION_ARG float (*derivedFunction)(ActorValueOwner*, ActorValueInfo&)
#define CALC_FUNCTION_ARG    void (*calcFunction)(Actor*, ActorValueInfo&, float, float, Actor*)

float DamageResistFormula(ActorValueInfo* AVIF, float Damage, float DamageResist);

void ActorValueUpdateThread(Actor* actor);
void ActorValueUpdateThread(TESObjectREFR* refr);

void AddFunction(ActorValueInfo* Child, DERIVED_FUNCTION_ARG);
void AddFunction(ActorValueInfo* Child, CALC_FUNCTION_ARG);

void RegisterActorValue(ActorValueInfo* Child, ActorValueInfo* Parent, DERIVED_FUNCTION_ARG);
void RegisterActorValue(ActorValueInfo* Child, ActorValueInfo* Parent, ActorValueInfo* CoParent, DERIVED_FUNCTION_ARG);

namespace Values {
    extern std::atomic<bool> RunPlayerThread;
    extern std::atomic<bool> PlayerThreadDidExit;
    void RegisterValues();
}    // namespace Values