#include "Utilities.h"

#include "f4se/GameData.h"

#include <algorithm>

void NotificationInternal(const char* Message, va_list args, const char* Sound) {
    char formatBuf[8192];
    vsprintf_s(formatBuf, sizeof(formatBuf), Message, args);
    Notification_internal(Message, Sound, 0, 1, 1);
}

void NotificationSound(const char* Message, const char* Sound, ...) {
    va_list args; va_start(args, Message);
    NotificationInternal(Message, args, Sound);
    va_end(args);
}

void Notification(const char* Message, ...) {
    va_list args; va_start(args, Message);
    NotificationInternal(Message, args, "");
    va_end(args);
}

bool strifind(const std::string& str, const std::string& search) {
    if (search.empty())
        return true;

    auto it = std::search(str.begin(), str.end(), search.begin(), search.end(), [](char c1, char c2) {
        return std::toupper(c1) == std::toupper(c2);
        });

    return (it != str.end());
}

UInt16 GetLevel(Actor* actor) {
    TESActorBaseData* base = DYNAMIC_CAST(actor->baseForm, TESForm, TESActorBaseData);
    return CALL_MEMBER_FN(base, GetLevel)();
}

void PlayIdle(Actor* actor, TESIdleForm* idle) {
    PlayIdle_Internal(actor->middleProcess, actor, 0x35, idle, 0x01, 0x00);
}

void EquipItem(Actor* actor, TESForm* Form, bool PreventUnequip) {
    ObjectInstanceData Data(Form);
    EquipItem_Internal((*g_EquipManager), actor, &Data, 0, 1, nullptr, 1, PreventUnequip, 1, 0, nullptr);
}

void UnequipItem(Actor* actor, TESForm* Form, bool PreventReequip) {
    ObjectInstanceData Data(Form);
    UnequipItem_Internal((*g_EquipManager), actor, &Data, 1, nullptr, -1, 1, PreventReequip, 1, 0, nullptr);
}

// ------------------------------------------------------------------------------------------------
// Value Functions
// ------------------------------------------------------------------------------------------------

float GetValue(Actor* actor, ActorValueInfo* avif) {
    return actor->actorValueOwner.GetValue(avif);
}

float GetBaseValue(Actor* actor, ActorValueInfo* avif) {
    return actor->actorValueOwner.GetBase(avif);
}

float GetPermValue(Actor* actor, ActorValueInfo* avif) {
    return actor->actorValueOwner.GetBase(avif) + actor->actorValueOwner.GetMod(1, avif);
}

float GetTempValue(Actor* actor, ActorValueInfo* avif) {
    return actor->actorValueOwner.GetBase(avif) + actor->actorValueOwner.GetMod(0, avif);
}

float GetValue(TESObjectREFR* refr, ActorValueInfo* avif) {
    return refr->actorValueOwner.GetValue(avif);
}

float GetBaseValue(TESObjectREFR* refr, ActorValueInfo* avif) {
    return refr->actorValueOwner.GetBase(avif);
}

float GetPermValue(TESObjectREFR* refr, ActorValueInfo* avif) {
    return refr->actorValueOwner.GetBase(avif) + refr->actorValueOwner.GetMod(1, avif);
}

float GetTempValue(TESObjectREFR* refr, ActorValueInfo* avif) {
    return refr->actorValueOwner.GetBase(avif) + refr->actorValueOwner.GetMod(0, avif);
}

int GetValueInt(Actor* actor, ActorValueInfo* avif) {
    return (int)GetValue(actor, avif);
}

int GetBaseValueInt(Actor* actor, ActorValueInfo* avif) {
    return (int)GetBaseValue(actor, avif);
}

int GetPermValueInt(Actor* actor, ActorValueInfo* avif) {
    return (int)GetPermValue(actor, avif);
}

void ModValue(Actor* actor, ActorValueInfo* avif, float value, UInt32 type) {
    actor->actorValueOwner.Mod(type, avif, value);
}

void ModPermValue(Actor* actor, ActorValueInfo* avif, float value) {
    ModValue(actor, avif, value, 1);
}

void SetValue(Actor* actor, ActorValueInfo* avif, float value) {
    actor->actorValueOwner.SetBase(avif, value);
}

// ------------------------------------------------------------------------------------------------
//  Lookup
// ------------------------------------------------------------------------------------------------

TESForm* LookupFormFromMod(const ModInfo* modInfo, UInt32 FormID) {
    FormID |= (modInfo->modIndex) << 24;
    return LookupFormByID(FormID);
}

UInt32 GetHandleIDByIndex(UInt32 index) {
    return GetTableValue(UInt32, index, "HandleID");
}

UInt32 GetStackIDByIndex(UInt32 index) {
    PipboyArray* StackIDs = GetTableArray(index, "StackID");
    UInt32 Result = 0;

    for (int i = 0; i < StackIDs->value.count; i++) {
        Result = GetTableArrayValue(UInt32, i, StackIDs);
    }

    return Result;
}

TESForm* GetInventoryFormByHandleID(UInt32 HandleID) {
    return GetFormByHandleID_Internal(*g_InventoryInterface, &HandleID);
}

TESForm* GetInventoryFormByIndex(UInt32 index) {
    UInt32 HandleID = GetTableValue(UInt32, index, "HandleID");
    return GetInventoryFormByHandleID(HandleID);
}

BGSInventoryItem* GetInventoryItemByHandleID(UInt32 HandleID) {
    return GetItemByHandleID_Internal(*g_InventoryInterface, &HandleID);
}

BGSInventoryItem* GetInventoryItemByIndex(UInt32 index) {
    UInt32 HandleID = GetTableValue(UInt32, index, "HandleID");
    return GetInventoryItemByHandleID(HandleID);
}

BGSInventoryItem::Stack* GetStackByStackID(BGSInventoryItem* Item, int StackID) {
    BGSInventoryItem::Stack* traverse = Item->stack;
    if (!traverse)
        return nullptr;

    while (StackID != 0) {
        traverse = traverse->next;
        if (!traverse)
            return nullptr;
        StackID--;
    }

    return traverse;
}

ExtraDataList* GetExtraDataListByStackID(BGSInventoryItem* Item, int StackID) {
    if (!Item)
        return nullptr;

    BGSInventoryItem::Stack* stack = GetStackByStackID(Item, StackID);
    return (stack) ? stack->extraData : nullptr;
}

ExtraDataList* GetExtraDataListByIndex(UInt32 index) {
    BGSInventoryItem*   Item    = GetInventoryItemByIndex(index);
    UInt32              StackID = GetStackIDByIndex(index);

    if (!Item)
        return nullptr;

    BGSInventoryItem::Stack* stack = GetStackByStackID(Item, StackID);
    return (stack) ? stack->extraData : nullptr;
}

void GFxLogMembers::Visit(const char* member, GFxValue* value) {
    GFxValue Member;
    m_value->GetMember(member, &Member);

    switch (Member.GetType()) {
    case GFxValue::kType_Bool:
        _LogMessageNT("%s: %s", member, Member.GetBool() ? "True" : "False");
        break;

    case GFxValue::kType_Int:
    case GFxValue::kType_UInt:
        if (!_stricmp(member, "FormID")) {
            _LogMessageNT("%s: %08X: %i", member, Member.GetInt(), LookupFormByID(Member.GetInt())->formType);
        }
        else {
            _LogMessageNT("%s: %i", member, Member.GetInt());
        }
        break;

    case GFxValue::kType_Number:
        _LogMessageNT("%s: %f", member, Member.GetNumber());
        break;

    case GFxValue::kType_String:
        _LogMessageNT("%s: %s", member, Member.GetString());
        break;

    default:
        _LogMessageNT("%s: Type %i", member, Member.GetType());
        break;
    }
}

void GFxLogMembersBasic::Visit(const char* member, GFxValue* value) {
    _LogMessageNT(member);
}