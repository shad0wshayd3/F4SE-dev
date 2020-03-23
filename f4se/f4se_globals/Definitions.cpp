#include "Definitions.h"
#include "Native.h"

BGSInventoryItemEx::CheckStackIDFunctor::CheckStackIDFunctor(UInt32 index) {
    this->index = index;
}

bool BGSInventoryItemEx::CheckStackIDFunctor::CompareData(BGSInventoryItem::Stack* stack) {
    return CheckStackIDFunctorCompare(this, stack);
}

BGSInventoryItemEx::ModifyModDataFunctor::ModifyModDataFunctor(BGSMod::Attachment::Mod* mod, UInt8 index, bool doAttach, bool* modified) {
    this->bShouldSplitStacks    = true;
    this->mod                   = mod;
    this->baseObject            = nullptr;
    this->index                 = index;
    this->doAttach              = doAttach;
    this->locked                = false;
    this->modified              = modified;
}

void BGSInventoryItemEx::ModifyModDataFunctor::WriteDataImpl(TESBoundObject* baseObject, BGSInventoryItem::Stack* stack) {
    ModifyModDataFunctorWriteImpl(this, baseObject, stack);
}

BGSInventoryItemEx::SetHealthFunctor::SetHealthFunctor(float health) {
    this->bShouldSplitStacks    = true;
    this->health                = health;
}

void BGSInventoryItemEx::SetHealthFunctor::WriteDataImpl(TESBoundObject* baseObject, BGSInventoryItem::Stack* stack) {
    SetHealthFunctorWriteImpl(this, baseObject, stack);
}

BGSInventoryItemEx::WriteTextExtra::WriteTextExtra(const char* name) {
    this->bShouldSplitStacks    = true;
    this->name                  = name;
}

void BGSInventoryItemEx::WriteTextExtra::WriteDataImpl(TESBoundObject* baseObject, BGSInventoryItem::Stack* stack) {
    WriteTextExtraWriteImpl(this, baseObject, stack);
}

BGSObjectInstance::BGSObjectInstance(TESForm* form, TBO_InstanceData* instance) {
    BGSObjectInstanceCtor(this, form, instance);
};

template <typename T>
BGSObjectInstanceT<T>::BGSObjectInstanceT(T form, TBO_InstanceData* instance) {
    BGSObjectInstanceCtor(this, form, instance);
};

ExtraCharge* ExtraCharge::Create(float value) {
    ExtraCharge* pCharge = (ExtraCharge*)BSExtraData::Create(sizeof(ExtraCharge), ExtraChargeVtbl.GetUIntPtr());
    pCharge->type = kExtraData_Charge;
    pCharge->charge = value;
    return pCharge;
}

ExtraObjectHealth* ExtraObjectHealth::Create(float value) {
    ExtraObjectHealth* pObjectHealth = (ExtraObjectHealth*)BSExtraData::Create(sizeof(ExtraObjectHealth), ExtraObjectHealthVtbl.GetUIntPtr());
    pObjectHealth->type = kExtraData_ObjectHealth;
    pObjectHealth->health = value;
    return pObjectHealth;
}

UInt32 TESObjectBOOK::Data::GetSanitizedType() {
    if (flags & kType_Perk)
        return kType_Perk;
    if (flags & kType_Spell)
        return kType_Spell;
    if (flags & kType_Skill)
        return kType_Skill;
    return kType_None;
}