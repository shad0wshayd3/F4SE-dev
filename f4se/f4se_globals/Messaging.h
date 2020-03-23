#pragma once

#include "f4se/GameForms.h"

enum BakaMessages {
    kMessage_FrameworkInit,
    kMessage_RequestHardcoreNeeds,
    kMessage_RespondHardcoreNeeds,
};

struct HardcoreStage {
    SpellItem*  spell;
    int         threshold;
};

typedef std::vector<HardcoreStage>                          HCStageVector;
typedef std::pair<ActorValueInfo*, HCStageVector>           HCStagePair;
typedef std::vector<HCStagePair>                            HCStageData;

typedef std::vector<ActorValueInfo*>                        ActorValueList;