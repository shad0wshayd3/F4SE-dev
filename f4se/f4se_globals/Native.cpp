#include "Native.h"

RelocAddr <_LevelUpPrompt> LevelUpPrompt(0xB3A5A0);
RelocAddr <_AddPerk> AddPerk(0xDA60E0);
RelocAddr <_RemovePerk> RemovePerk(0xDA61F0);
RelocAddr <_HasPerk> HasPerk(0xDA64E0);
RelocAddr <_GetItemCount> GetItemCount(0x13FB5E0);
RelocAddr <_ApplyImagespaceModifier> ApplyImagespaceModifier(0x396720);
RelocAddr <_RemoveImagespaceModifier> RemoveImagespaceModifier(0x396B30);
RelocAddr <_CalculateDamageResist> CalculateDamageResist(0x65FAC0);
RelocAddr <_GetXPForLevel> GetXPForLevel(0x664A10);
RelocAddr <_ExtractArgs> ExtractArgs(0x4E36B0);
RelocAddr <_DoTokenReplacement_Internal> DoTokenReplacement_Internal(0xC0C70);

RelocAddr <uintptr_t> ActorValueDerivedVtbl(0x2CEDBC8);
// ActorValue AVOwner*, AVInfo&

RelocAddr <uintptr_t> ActorValueCalcVtbl(0x2CEDC08);
// ActorValue Actor*, AVInfo&, float, float, Actor*

RelocPtr <ItemMenuDataManager*> g_itemMenuDataMgr(0x58D4980);
// aSelectedItemHP (1), first cs:qword 1 loc_ up (base sup). 249 ref.

RelocPtr <PipboyDataManager*> g_pipboyDataMgr(0x58D0AF0);
// aSpecialList (1), first cs:qword in the loc_ top. 110 ref.