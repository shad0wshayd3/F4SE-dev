#pragma once

#include "f4se/GameObjects.h"

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

class TESIdleForm : public TESForm {
public:
    enum { kTypeID = kFormType_IDLE };
};