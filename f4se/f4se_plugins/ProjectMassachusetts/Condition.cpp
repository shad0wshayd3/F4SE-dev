#include "Condition.h"
#include "f4se/GameSettings.h"

//#include <cmath>

#define HasExtraData(ExtraData, Type)\
    ExtraData->HasType(kExtraData_##Type##)

#define SetExtraData(ExtraData, Type, name, value)\
    (((Extra##Type##*)ExtraData->GetByType(kExtraData_##Type##))->##name##) = value;

#define CreateHealthData(ExtraData, Type, value)\
    ExtraData->Add(kExtraData_##Type##, Extra##Type##::Create(value))

// ------------------------------------------------------------------------------------------------
// WeaponCondition
// ------------------------------------------------------------------------------------------------

WeaponConditionData::WeaponConditionData() : Form(nullptr), extraData(nullptr), instance(nullptr) { }

WeaponConditionData::WeaponConditionData(TESForm* form, ExtraDataList* extradata) {
    Form        = form;
    extraData   = extradata;
    instance    = nullptr;

    if (extraData)
        if (HasExtraData(extraData, InstanceData))
            instance = GetItemInstanceData(extraData, TESObjectWEAP);
}

WeaponConditionData::WeaponConditionData(TESObjectREFR* refr) {
    if (refr) {
        Form        = refr->baseForm;
        extraData   = refr->extraDataList;
        instance    = nullptr;

        if (extraData)
            if (HasExtraData(extraData, InstanceData))
                instance = GetItemInstanceData(extraData, TESObjectWEAP);
    }
}

WeaponConditionData::WeaponConditionData(Actor* actor) {
    if (actor) {
        Form        = GetEquippedWeaponForm(actor);
        extraData   = GetEquippedWeaponExtraData(actor);
        instance    = GetEquippedWeaponInstanceData(actor);
    }
}

float GetWeaponConditionMaximum(WeaponConditionData Data) {
    if (!Data.extraData)
        return -1.0;
    if (!HasExtraData(Data.extraData, Charge))
        return -1.0;
    return GetExtraDataValue(Data.extraData, Charge, charge);
}

float GetWeaponConditionPercent(WeaponConditionData Data) {
    if (!Data.extraData)
        return -1.0;
    if (!HasExtraData(Data.extraData, Health))
        return -1.0;
    return GetExtraDataValue(Data.extraData, Health, health);
}

float GetWeaponConditionCurrent(WeaponConditionData Data) {
    float Percent = GetWeaponConditionPercent(Data);
    float Maximum = GetWeaponConditionMaximum(Data);
    if ((0 > Percent) || (0 > Maximum))
        return -1.0;
    return (Percent * Maximum);
}

float GetWeaponConditionMaximum(TESObjectREFR* refr) {
    if (!refr)
        return -1.0;
    return GetWeaponConditionMaximum(WeaponConditionData(refr));
}

float GetWeaponConditionPercent(TESObjectREFR* refr) {
    if (!refr)
        return -1.0;
    return GetWeaponConditionPercent(WeaponConditionData(refr));
}

float GetWeaponConditionCurrent(TESObjectREFR* refr) {
    if (!refr)
        return -1.0;
    return GetWeaponConditionPercent(WeaponConditionData(refr));
}

void SetWeaponConditionMaximum(WeaponConditionData Data, float Value) {
    if (!Data.extraData)
        return;
    if (!HasExtraData(Data.extraData, ObjectHealth))
        return;
    Value = max(1, Value);
    SetExtraData(Data.extraData, Charge, charge, Value);
}

void SetWeaponConditionPercent(WeaponConditionData Data, float Value) {
    if (!Data.extraData)
        return;
    if (!HasExtraData(Data.extraData, Health))
        return;
    Value = max(0, Value);
    SetExtraData(Data.extraData, Health, health, Value);
}

void SetWeaponConditionCurrent(WeaponConditionData Data, float Value) {
    float Current = GetWeaponConditionCurrent(Data);
    float Maximum = GetWeaponConditionMaximum(Data);
    if ((0 > Current) || (0 > Maximum))
        return;

    if (!Data.Form)
        return;

    //if (!Data.instance)
    //    return;
    //
    //if (Current > 0) {
    //    Data.instance->baseDamage   /= Current;
    //    Data.instance->value        /= Current;
    //}

    Value = max(0, Value);
    SetWeaponConditionPercent(Data, (Value / Maximum));

    //Data.instance->baseDamage   *= Current;
    //Data.instance->value        *= Current;
    //
    //ObjectInstanceData ObjectData;
    //UpdateInstanceData(ObjectData, Data.Form, Data.instance);
}

void SetWeaponConditionMaximum(TESObjectREFR* refr, float Value) {
    if (!refr)
        return;
    SetWeaponConditionMaximum(WeaponConditionData(refr), Value);
}

void SetWeaponConditionPercent(TESObjectREFR* refr, float Value) {
    if (!refr)
        return;
    SetWeaponConditionPercent(WeaponConditionData(refr), Value);
}

void SetWeaponConditionCurrent(TESObjectREFR* refr, float Value) {
    if (!refr)
        return;
    SetWeaponConditionCurrent(WeaponConditionData(refr), Value);
}

void ModWeaponCondition(WeaponConditionData Data, float Value) {
    float Current = GetWeaponConditionCurrent(Data);
    if (0 > Current)
        return;
    Value = max(0, (Current + Value));
    SetWeaponConditionCurrent(Data, Value);
}

void ModWeaponCondition(TESObjectREFR* refr, float Value) {
    if (!refr)
        return;
    ModWeaponCondition(WeaponConditionData(refr), Value);
}

void ModWeaponCondition(Actor* actor) {
    if (!actor)
        return;

    WeaponConditionData Data(actor);

    float Value = 0.2; //GetValue(actor, Forms::ConditionRate);
    if (0 > Value)
        Value = 0.2;
    ModWeaponCondition(Data, (Value * -1.0));

    float Current = GetWeaponConditionCurrent(Data);
    if (Current == 0.0)
        UnequipWeapon(actor);
}

// ------------------------------------------------------------------------------------------------
// Utils
// ------------------------------------------------------------------------------------------------

TESForm* GetEquippedWeaponForm(Actor* actor) {
    for (auto iter : { 0x20, 0x21, 0x25, 0x29 }) {
        if (actor->equipData->slots[iter].item != nullptr)
            return actor->equipData->slots[iter].item;
    }

    return nullptr;;
}

TESObjectWEAP::InstanceData* GetEquippedWeaponInstanceData(Actor* actor) {
    for (auto iter : { 0x20, 0x21, 0x25, 0x29 }) {
        if (actor->equipData->slots[iter].item != nullptr)
            return CastInstanceData(actor->equipData->slots[iter].instanceData, TESObjectWEAP);
    }

    return nullptr;
}

ExtraDataList* GetEquippedWeaponExtraData(Actor* actor) {
    ExtraDataList* Result = nullptr;
    for (auto iter : { 0x20, 0x21, 0x25, 0x29 }) {
        if (actor->equipData->slots[iter].item != nullptr) {
            actor->GetEquippedExtraData(iter, &Result);
            break;
        }
    }

    return Result;
}

void UnequipWeapon(Actor* actor) {
    if (!actor)
        return;

    TESForm* Weapon = GetEquippedWeaponForm(actor);
    if (!Weapon)
        return;

    NotificationSound(GetGameSetting("sWeaponBreak")->data.s, "WPNJunkJetFirePlayer");
    UnequipItem(actor, Weapon, false);
}

// ------------------------------------------------------------------------------------------------
// Initializers
// ------------------------------------------------------------------------------------------------

void InitializeWeaponCondition(WeaponConditionData Data) {
    float MaxHealth = 1000;
    float MinHealth = 500;
    float StartCond = 0;

    if (!Data.extraData)
        return;

    // Create Condition Values
    if (!HasExtraData(Data.extraData, Health)) {
        if (HasExtraData(Data.extraData, InstanceData)) {
            if (Data.instance->modifiers) {
                for (int i = 0; i < Data.instance->modifiers->count; i++) {
                    auto iter = Data.instance->modifiers->entries[i];
                    MaxHealth   = (iter.avInfo == Forms::ItemCondMaxHealth) ? iter.unk08 : MaxHealth;
                    MinHealth   = (iter.avInfo == Forms::ItemCondMinHealth) ? iter.unk08 : MinHealth;
                    StartCond   = (iter.avInfo == Forms::ItemCondStartCond) ? iter.unk08 : StartCond;
                }
            }
        }

        float InitialHealth = (StartCond > 0) ? StartCond :
            ((rand() % int(MaxHealth - MinHealth) + MinHealth) / MaxHealth);

        CreateHealthData(Data.extraData, Health, 0);
        CreateHealthData(Data.extraData, Charge, MaxHealth);
        SetWeaponConditionPercent(Data, InitialHealth);
    }

    // Update Condition Values with new Max
    else {
        if (!HasExtraData(Data.extraData, Health) || !HasExtraData(Data.extraData, ObjectHealth))
            return;

        float curMax = GetWeaponConditionMaximum(Data);
        if (HasExtraData(Data.extraData, InstanceData)) {
            if (Data.instance->modifiers) {
                for (int i = 0; i < Data.instance->modifiers->count; i++) {
                    auto iter = Data.instance->modifiers->entries[i];
                    if (iter.avInfo == Forms::ItemCondMaxHealth) {
                        if (curMax != iter.unk08) {
                            SetWeaponConditionMaximum(Data, iter.unk08);
                        }

                        break;
                    }
                }
            }
        }
    }
}

void InitializeWeaponCondition(TESObjectREFR* refr) {
    if (!refr)
        return;
    InitializeWeaponCondition(WeaponConditionData(refr));
}

void InitializeInventoryItemCondition(TESObjectREFR* refr, TESForm* Form) {
    auto inventory = refr->inventoryList;
    if (inventory) {
        BSReadAndWriteLocker(&inventory->inventoryLock);

        for (int i = 0; i < inventory->items.count; i++) {
            BGSInventoryItem iter;
            inventory->items.GetNthItem(i, iter);
            if (iter.form->formID == Form->formID) {
                if (iter.stack) {
                    int iterCount = 0;
                    for (BGSInventoryItem::Stack* traverse = iter.stack; traverse; traverse->next) {
                        if (!traverse)
                            break;

                        if (!traverse->extraData)
                            break;

                        if (!HasExtraData(traverse->extraData, Health)) {
                            switch (Form->formType) {
                            case kFormType_WEAP:
                                InitializeWeaponCondition(WeaponConditionData(iter.form, traverse->extraData));
                                break;

                            default:
                                break;
                            }
                            
                        }

                        iterCount++;
                        if (iterCount > traverse->count)
                            break;
                    }
                }

                break;
            }
        }
    }
}

void InitializeInventoryCondition(TESObjectREFR* refr) {
    auto inventory = refr->inventoryList;
    if (inventory) {
        BSReadAndWriteLocker(&inventory->inventoryLock);

        for (int i = 0; i < inventory->items.count; i++) {
            BGSInventoryItem iter;
            inventory->items.GetNthItem(i, iter);

            switch (iter.form->formType) {
            case kFormType_WEAP: {
                if (iter.stack) {
                    int iterCount = 0;
                    for (BGSInventoryItem::Stack* traverse = iter.stack; traverse; traverse->next) {
                        if (!traverse)
                            break;

                        if (!traverse->extraData)
                            break;

                        if (!HasExtraData(traverse->extraData, Health))
                            InitializeWeaponCondition(WeaponConditionData(iter.form, traverse->extraData));

                        iterCount++;
                        if (iterCount > traverse->count)
                            break;
                    }
                }

                break;
            }

            default:
                break;
            }
        }
    }
}