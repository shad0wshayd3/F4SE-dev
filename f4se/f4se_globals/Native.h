#pragma once

#include "f4se/GameExtraData.h"
#include "f4se/GameMenus.h"
#include "f4se/GameReferences.h"

typedef void(*_LevelUpPrompt)();
extern RelocAddr <_LevelUpPrompt> LevelUpPrompt;

typedef void(*_AddPerk)(Actor* actor, BGSPerk* perk, bool notify);
extern RelocAddr <_AddPerk> AddPerk;

typedef void(*_RemovePerk)(Actor* actor, BGSPerk* perk);
extern RelocAddr <_RemovePerk> RemovePerk;

typedef bool(*_HasPerk)(Actor* actor, BGSPerk* perk);
extern RelocAddr <_HasPerk> HasPerk;

typedef UInt32(*_GetItemCount)(TESObjectREFR* refr, TESForm* form);
extern RelocAddr <_GetItemCount> GetItemCount;

typedef UInt32(*_ApplyImagespaceModifier)(TESImageSpaceModifier* imod, float intensity, UInt32 unk00);
extern RelocAddr <_ApplyImagespaceModifier> ApplyImagespaceModifier;

typedef void(*_RemoveImagespaceModifier)(TESImageSpaceModifier* imod);
extern RelocAddr <_RemoveImagespaceModifier> RemoveImagespaceModifier;

typedef float(*_CalculateDamageResist)(ActorValueInfo* avif, float Damage, float DamageResist);
extern RelocAddr <_CalculateDamageResist> CalculateDamageResist;

typedef float(*_GetXPForLevel)(UInt16 level);
extern RelocAddr <_GetXPForLevel> GetXPForLevel;

typedef bool(*_ExtractArgs)(void* paramInfo, void* scriptData, void* opcodeOffset, TESObjectREFR* thisObj, void* containingObj, void* script, void* eventList, ...);
extern RelocAddr <_ExtractArgs> ExtractArgs;

typedef bool (*_ObScript_Parse)(UInt32 numParams, void* paramInfo, void* lineBuf, void* scriptBuf);
extern RelocAddr <_ObScript_Parse> ObScript_Parse;

typedef void(*_DoTokenReplacement_Internal)(ExtraTextDisplayData* extraText, BSString& text);
extern RelocAddr <_DoTokenReplacement_Internal> DoTokenReplacement_Internal;

extern RelocAddr <uintptr_t> ActorValueDerivedVtbl;

extern RelocAddr <uintptr_t> ActorValueCalcVtbl;

class ItemMenuDataManager {
public:
    DEFINE_MEMBER_FN_1(GetSelectedItem, BGSInventoryItem*, 0x001A3650, UInt32& handleID);
    // ModdingSuccess (1), first CALL in loc_ top. 163 ref.

    DEFINE_MEMBER_FN_1(GetSelectedForm, TESForm*, 0x001A3740, UInt32& handleID);
    // CheckStackIDFunctor (1), called twice use 1st, 3 loc_ up, 2nd CALL. 30 ref.
};

class PipboyDataManager {
public:
    UInt64                  unk00[0x4A8 >> 3];
    tArray<PipboyObject*>   itemData;
};
STATIC_ASSERT(sizeof(PipboyDataManager) == 0x4C0);
