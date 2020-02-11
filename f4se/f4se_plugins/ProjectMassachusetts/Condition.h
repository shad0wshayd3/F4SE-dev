#pragma once
#include "Forms.h"

struct WeaponConditionData {
    WeaponConditionData();
    WeaponConditionData(TESForm* form, ExtraDataList* extradata);
    WeaponConditionData(TESObjectREFR* refr);
    WeaponConditionData(Actor* actor);

    TESForm*                        Form;
    ExtraDataList*                  extraData;
    TESObjectWEAP::InstanceData*    instance;
};

float GetWeaponConditionMaximum(WeaponConditionData Data);
float GetWeaponConditionPercent(WeaponConditionData Data);
float GetWeaponConditionCurrent(WeaponConditionData Data);
float GetWeaponConditionMaximum(TESObjectREFR* refr);
float GetWeaponConditionPercent(TESObjectREFR* refr);
float GetWeaponConditionCurrent(TESObjectREFR* refr);
void SetWeaponConditionMaximum(WeaponConditionData Data, float Value);
void SetWeaponConditionPercent(WeaponConditionData Data, float Value);
void SetWeaponConditionCurrent(WeaponConditionData Data, float Value);
void SetWeaponConditionMaximum(TESObjectREFR* refr, float Value);
void SetWeaponConditionPercent(TESObjectREFR* refr, float Value);
void SetWeaponConditionCurrent(TESObjectREFR* refr, float Value);
void ModWeaponCondition(WeaponConditionData Data, float Value);
void ModWeaponCondition(TESObjectREFR* refr, float Value);
void ModWeaponCondition(Actor* actor);

TESForm* GetEquippedWeaponForm(Actor* actor);
TESObjectWEAP::InstanceData* GetEquippedWeaponInstanceData(Actor* actor);
ExtraDataList* GetEquippedWeaponExtraData(Actor* actor);
void UnequipWeapon(Actor* actor);

void InitializeWeaponCondition(WeaponConditionData Data);
void InitializeWeaponCondition(TESObjectREFR* refr);
void InitializeInventoryItemCondition(TESObjectREFR* refr, TESForm* Form);
void InitializeInventoryCondition(TESObjectREFR* refr);