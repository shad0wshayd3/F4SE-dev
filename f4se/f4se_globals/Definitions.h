#pragma once

#include "MenuDefinitions.h"
#include "f4se/GameExtraData.h"
#include "f4se/GameObjects.h"

class ExtraCharge : public BSExtraData {
public:
    float charge;
    UInt32 pad1C;

    static ExtraCharge* Create(float value);
};
STATIC_ASSERT(sizeof(ExtraCharge) == 0x20);

class ExtraObjectHealth : public BSExtraData {
public:
    float health;    // 18
    UInt32 pad1C;    // 1C

    static ExtraObjectHealth* Create(float value);
};
STATIC_ASSERT(sizeof(ExtraObjectHealth) == 0x20);

class BGSNote : public TESBoundObject {
public:
    enum { kTypeID = kFormType_NOTE };

    TESWeightForm               weight;
    TESValueForm                value;
    TESModel                    model;
    TESFullName                 fullName;
    TESIcon                     icon;
    BGSPickupPutdownSounds      pickupSounds;
};

class TESIdleForm : public TESForm {
public:
    enum { kTypeID = kFormType_IDLE };
};

class TESKey : public TESObjectMISC {
public:
    enum { kTypeID = kFormType_KEYM };
};

class TESObjectBOOK : public TESBoundObject {
public:
    enum { kTypeID = kFormType_BOOK };

    TESFullName                 fullName;
    BGSModelMaterialSwap        materialSwap;
    TESIcon                     icon;
    TESValueForm                value;
    TESWeightForm               weight;
    TESDescription              description;
    BGSDestructibleObjectForm   destructible;
    BGSMessageIcon              messageIcon;
    BGSPickupPutdownSounds      pickupSounds;
    BGSKeywordForm              keywordForm;
    BGSFeaturedItemMessage      featured;
};