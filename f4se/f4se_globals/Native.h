#pragma once

#include "Definitions.h"

#include "f4se/GameExtraData.h"
#include "f4se/GameMenus.h"
#include "f4se/GameReferences.h"

// ------------------------------------------------------------------------------------------------
// typedefs
// ------------------------------------------------------------------------------------------------

typedef bool                (*_BGSInventoryItemCompareData) (BGSInventoryItemEx::CheckStackIDFunctor* thisFunctor, BGSInventoryItem::Stack* stack);
typedef void                (*_BGSInventoryItemWriteImpl)   (BGSInventoryItemEx::StackDataWriteFunctor* thisFunctor, TESBoundObject* baseObject, BGSInventoryItem::Stack* stack);
typedef void                (*_SetInventoryItemStackData)   (TESObjectREFR* refr, TESBoundObject* baseObject, BGSInventoryItemEx::StackDataCompareFunctor* compareFunctor, BGSInventoryItemEx::StackDataWriteFunctor* writeFunctor, bool(*objCompareFunctor)(TESBoundObject*, TESBoundObject*), bool abAlwaysContinue);

typedef ActorValueInfo**    (*_GetHardcodedAV)              (void);
typedef void                (*_DoTokenReplacement)          (ExtraTextDisplayData* extraText, BSString& text);
typedef BGSInventoryItem*   (*_GetItemByHandleID)           (InventoryInterface* invInterface, UInt32* handleID);
typedef TESForm*            (*_GetFormByHandleID)           (InventoryInterface* invInterface, UInt32* handleID);
typedef void                (*_BGSObjectInstanceCtor)       (BGSObjectInstance* instance, TESForm* form, TBO_InstanceData* instanceData);
typedef bool                (*_ExtractArgs)                 (void* paramInfo, void* scriptData, void* opcodeOffset, TESObjectREFR* thisObj, void* containingObj, void* script, void* eventList, ...);
typedef float               (*_CalculateDamageResist)       (ActorValueInfo* avif, float Damage, float DamageResist);
typedef void                (*_Notification)                (const char* Message, const char* Sound, UInt64 unk00, UInt64 unk01, UInt64 unk02);
typedef float               (*_GetXPForLevel)               (UInt16 level);
typedef void                (*_PopulateItemCard)            (GFxValue* InfoObj, BGSInventoryItem* Item, UInt16 StackID, InvItemStackList CompareList);
typedef void                (*_ContainerMenuInvoke)         (ContainerMenuBase* menu, GFxFunctionHandler::Args* args);
typedef void                (*_ExamineMenuInvoke)           (ExamineMenu* menu, GFxFunctionHandler::Args* args);
typedef void                (*_LevelUpPrompt);
typedef void                (*_PipboyMenuInvoke)            (PipboyMenu* menu, GFxFunctionHandler::Args* args);
typedef void                (*_CalcSpellProperties)         (ActiveEffect* effect, Actor* caster, MagicTarget* target);
typedef bool                (*_IsInAir)                     (Actor* actor);
typedef float               (*_CalcWeight)                  (Actor* actor, TESForm* item, BGSInventoryItem::Stack* stack, float baseWeight, bool* unk00);
typedef void                (*_AddPerk)                     (Actor* actor, BGSPerk* perk, bool notify);
typedef void                (*_RemovePerk)                  (Actor* actor, BGSPerk* perk);
typedef UInt8               (*_HasPerk)                     (Actor* actor, BGSPerk* perk);
typedef bool                (*_AddSpell)                    (Actor* actor, SpellItem* spell);
typedef bool                (*_RemoveSpell)                 (Actor* actor, SpellItem* spell);
typedef bool                (*_HasSpell)                    (Actor* actor, SpellItem* spell);
typedef void                (*_UpdateActor)                 (Actor* actor, float timeDelta);
typedef void                (*_EquipItem)                   (void* EquipManager, Actor* actor, void* equipStruct, UInt64 unk01, UInt64 unk02, void* unk03, UInt64 unk04, bool PreventRemoval, UInt64 unk05, UInt64 unk06, void* unk07);
typedef void                (*_UnequipItem)                 (void* EquipManager, Actor* actor, void* equipStruct, UInt64 unk01, void* unk02, UInt64 unk03, UInt64 unk04, bool PreventRemoval, UInt64 unk05, UInt64 unk06, void* unk07);
typedef void                (*_PlayIdle)                    (void* unk00, Actor* actor, UInt32 unk02, TESIdleForm* idle, UInt32 unk04, void* unk05);
typedef bool                (*_ShowWaitMenu)                (Actor* actor, void* unk00, bool unk01, bool unk02);
typedef UInt32              (*_GetItemCount)                (TESObjectREFR* refr, TESForm* form);
typedef float               (*_GetCurrentGameTime)          (void);

// ------------------------------------------------------------------------------------------------
// externs
// ------------------------------------------------------------------------------------------------

extern RelocAddr    <_BGSInventoryItemCompareData>  CheckStackIDFunctorCompare;
extern RelocAddr    <_BGSInventoryItemWriteImpl>    ModifyModDataFunctorWriteImpl;
extern RelocAddr    <_BGSInventoryItemWriteImpl>    SetHealthFunctorWriteImpl;
extern RelocAddr    <_BGSInventoryItemWriteImpl>    WriteTextExtraWriteImpl;
extern RelocAddr    <_SetInventoryItemStackData>    SetInventoryItemStackData;

extern RelocAddr    <_GetHardcodedAV>               GetHardcodedAV;
extern RelocAddr    <_DoTokenReplacement>           DoTokenReplacement;
extern RelocAddr    <_GetItemByHandleID>            GetItemByHandleID_Internal;
extern RelocAddr    <_GetFormByHandleID>            GetFormByHandleID_Internal;
extern RelocAddr    <_BGSObjectInstanceCtor>        BGSObjectInstanceCtor;
extern RelocAddr    <_ExtractArgs>                  ExtractArgs;
extern RelocAddr    <_GetXPForLevel>                GetXPForLevel;
extern RelocAddr    <_Notification>                 Notification_internal;
extern RelocAddr    <_LevelUpPrompt>                LevelUpPrompt;
extern RelocAddr    <_IsInAir>                      IsInAir;
extern RelocAddr    <_AddPerk>                      AddPerk;
extern RelocAddr    <_RemovePerk>                   RemovePerk;
extern RelocAddr    <_HasPerk>                      HasPerk;
extern RelocAddr    <_AddSpell>                     AddSpell;
extern RelocAddr    <_RemoveSpell>                  RemoveSpell;
extern RelocAddr    <_HasSpell>                     HasSpell;
extern RelocAddr    <_EquipItem>                    EquipItem_Internal;
extern RelocAddr    <_UnequipItem>                  UnequipItem_Internal;
extern RelocAddr    <_PlayIdle>                     PlayIdle_Internal;
extern RelocAddr    <_ShowWaitMenu>                 ShowWaitMenu;
extern RelocAddr    <_GetItemCount>                 GetItemCount;
extern RelocAddr    <_GetCurrentGameTime>           GetCurrentGameTime;
extern RelocAddr    <uintptr_t>                     ExtraChargeVtbl;
extern RelocAddr    <uintptr_t>                     ExtraObjectHealthVtbl;
extern RelocAddr    <uintptr_t>                     ActorValueDerivedVtbl;
extern RelocAddr    <uintptr_t>                     ActorValueCalcVtbl;
extern RelocPtr     <PipboyDataManager*>            g_PipboyDataManager;
extern RelocPtr     <InventoryInterface*>           g_InventoryInterface;
extern RelocPtr     <void*>                         g_EquipManager;

// ------------------------------------------------------------------------------------------------
// originals
// ------------------------------------------------------------------------------------------------

extern _ContainerMenuInvoke ContainerMenuInvoke_Original;
void HookContainerMenuInvoke(void (*hookFunc)(ContainerMenuBase*, GFxFunctionHandler::Args*));

extern _ExamineMenuInvoke ExamineMenuInvoke_Original;
void HookExamineMenuInvoke(void (*hookFunc)(ExamineMenu*, GFxFunctionHandler::Args*));

extern _PipboyMenuInvoke PipboyMenuInvoke_Original;
void HookPipboyMenuInvoke(void (*hookFunc)(PipboyMenu*, GFxFunctionHandler::Args*));

extern _PopulateItemCard PopulateItemCard_Original;
void HookPopulateItemCard(void (*hookFunc)(GFxValue*, BGSInventoryItem*, UInt16, InvItemStackList));

extern _UpdateActor UpdateActor_Original;
void HookUpdateActor(void (*hookFunc)(Actor*, float));

// ------------------------------------------------------------------------------------------------
// Native Events
// ------------------------------------------------------------------------------------------------

// Alphabetically sorted, except TESHitEvent
DECLARE_EVENT_DISPATCHER(TESActivateEvent,                                  0x00441C90);    // 'Unable to initialize script virtual machine' | sub_1 past qword, sub_1, sub_3
DECLARE_EVENT_DISPATCHER(TESBookReadEvent,                                  0x00441E70);    // down 3
DECLARE_EVENT_DISPATCHER(TESCellAttachDetachEvent,                          0x00441F10);    // down 1
DECLARE_EVENT_DISPATCHER(TESCellFullyLoadedEvent,                           0x00441FB0);    // down 1
DECLARE_EVENT_DISPATCHER(TESContainerChangedEvent,                          0x004424B0);    // down 8
DECLARE_EVENT_DISPATCHER(TESEnterSneakingEvent,                             0x004427D0);    // down 5
DECLARE_EVENT_DISPATCHER(TESEquipEvent,                                     0x00442870);    // down 1
DECLARE_EVENT_DISPATCHER(TESGrabReleaseEvent,                               0x00442CD0);    // down 7
DECLARE_EVENT_DISPATCHER(TESLimbCrippleEvent,                               0x00442E10);    // down 2
DECLARE_EVENT_DISPATCHER(TESLockChangedEvent,                               0x00442FF0);    // down 3
DECLARE_EVENT_DISPATCHER(TESMagicEffectApplyEvent,                          0x00443090);    // down 1
DECLARE_EVENT_DISPATCHER(TESSleepStartEvent,                                0x00443D10);    // down 20
DECLARE_EVENT_DISPATCHER(TESSleepStopEvent,                                 0x00443DB0);    // down 1
DECLARE_EVENT_DISPATCHER(TESWaitStartEvent,                                 0x004443F0);    // down 10
DECLARE_EVENT_DISPATCHER(TESWaitStopEvent,                                  0x00444490);    // down 1
DECLARE_EVENT_DISPATCHER(TESHitEvent,                                       0x00444670);    // down 3 (last)

// const BSTGlobalEvent::EventSource<T>::`vftable' (Requires ClassInformer)
DECLARE_GLOBAL_EVENT_DISPATCHER(RadiationSourceCount,                       0x05A66008);
DECLARE_GLOBAL_EVENT_DISPATCHER(PipboyLightEvent,                           0x05A66340);
DECLARE_GLOBAL_EVENT_DISPATCHER(PlayerAmmoCountEvent,                       0x05A60560);
DECLARE_GLOBAL_EVENT_DISPATCHER(PlayerWeaponReloadEvent,                    0x05A60570);

DECLARE_SINGLETON_EVENT_DISPATCHER(WeaponFiredEvent,                        0x058E3E90);    // 'Weapon Equip Slot' | rax, qword ptr cs:unk_14*