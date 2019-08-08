#include "Utilities.h"

#include "Native.h"

#include "f4se/GameRTTI.h"

UInt16 GetLevel(Actor* actor) {
    TESActorBaseData* base = DYNAMIC_CAST(actor->baseForm, TESForm, TESActorBaseData);
    return CALL_MEMBER_FN(base, GetLevel)();
}

void PlayIdle(Actor* actor, TESIdleForm* idle) {
    PlayIdle_Internal(actor->middleProcess, actor, 0x35, idle, 0x01, 0x00);
}