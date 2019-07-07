#include "Utilities.h"

#include "f4se/GameReferences.h"
#include "f4se/GameRTTI.h"

UInt16 GetLevel(Actor* actor) {
    TESActorBaseData* base = DYNAMIC_CAST(actor->baseForm, TESForm, TESActorBaseData);
    return CALL_MEMBER_FN(base, GetLevel)();
}
