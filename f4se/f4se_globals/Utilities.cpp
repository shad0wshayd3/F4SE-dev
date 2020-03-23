#include "Utilities.h"

#include "f4se/GameData.h"

#include <algorithm>

// ------------------------------------------------------------------------------------------------
//  GameTime
// ------------------------------------------------------------------------------------------------

CurrentTime::CurrentTime(float currentTime) {
    Day     = truncf(currentTime);
    Hour    = truncf((currentTime - Day) * 24.0);
    Minute  = truncf((((currentTime - Day) * 24.0) - Hour) * 60.0);
}

// ------------------------------------------------------------------------------------------------
//  Scaleform Logging
// ------------------------------------------------------------------------------------------------

void GFxLogMembers::Visit(const char* memberName, GFxValue* value) {
    std::string displayName(m_prefix);
    displayName.append(".").append(memberName);
    const char* displayChar = displayName.c_str();

    switch (value->GetType()) {
    case GFxValue::kType_Undefined:
    case GFxValue::kType_Null:
        _LogMessageNT("%s (Null)", displayChar);
        break;

    case GFxValue::kType_Bool:
        _LogMessageNT("%s (Bool):", displayChar);
        _LogMessageNT("    %s", value->GetBool() ? "True" : "False");
        break;

    case GFxValue::kType_Int:
    case GFxValue::kType_UInt:
        _LogMessageNT("%s (%s):", displayChar,
            (value->GetType() == GFxValue::kType_Int) ? "Int" : "UInt");
        _LogMessageNT((!_stricmp(memberName, "FormID")) ? "    %08X" : "    %i",
            (value->GetType() == GFxValue::kType_Int) ? value->GetInt() : value->GetUInt());
        break;

    case GFxValue::kType_Number:
        _LogMessageNT("%s (Number):", displayChar);
        _LogMessageNT("    %f", value->GetNumber());
        break;

    case GFxValue::kType_String:
        _LogMessageNT("%s (String):", displayChar);
        _LogMessageNT("    %s", value->GetString());
        break;

    case GFxValue::kType_Unknown7:
        _LogMessageNT("%s (Unknown7)", displayChar);
        break;

    case GFxValue::kType_Object:
    case GFxValue::kType_DisplayObject: {
        _LogMessageNT("%s (%s):", displayChar,
            (value->GetType() == GFxValue::kType_Object) ? "Object" : "DisplayObject");

        _LogIndent(); m_depth++;

        if (m_depth >= m_maxDepth) {
            _LogMessageNT("We're in too deep!");
        }
        else if (std::string(memberName).find("instance") != std::string::npos) {
            _LogMessageNT("This isn't useful.");
        }
        // todo: toggle these with flags or something
        else if ((!_stricmp(memberName, "content")) || (!_stricmp(memberName, "parent")) || (!_stricmp(memberName, "root")) || (!_stricmp(memberName, "stage"))) {
            _LogMessageNT("This isn't useful.");
        }
        else {
            GFxLogMembers LM(displayChar, m_maxDepth, m_depth);
            LM.SetShowDisplayMembers(m_showDM);
            value->VisitMembers(&LM);
        }

        _LogOutdent(); m_depth--;
        break;
    }

    case GFxValue::kType_Array: {
        int arraySize = value->GetArraySize();
        _LogMessageNT("%s (Array):", displayChar);
        _LogMessageNT((arraySize > 0) ? "    %i Elements:" : "    %i Elements", arraySize);
        _LogIndent();

        // GFxLogElements LE(&Member);
        // Member.VisitElements(&LE, 0, arraySize);
        _LogOutdent();
        break;
    }

    case GFxValue::kType_Function:
        _LogMessageNT("%s (Function)", displayChar);
        break;

    default:
        _LogMessageNT("%s (Unknown Type: %i)", displayChar, value->GetType());
        break;
    }
}

void GFxLogElements::Visit(UInt32 idx, GFxValue* value) {
    _LogMessageNT("Index: %i", idx);
    _LogIndent();

    GFxLogMembers LM("", 10);
    value->VisitMembers(&LM);

    _LogOutdent();
}

void VisitGFxMembers(GFxValue value, std::string prefix, int maxDepth, bool displayMembers) {
    GFxLogMembers LM(prefix, maxDepth);
    LM.SetShowDisplayMembers(displayMembers);
    value.VisitMembers(&LM);
}

// ------------------------------------------------------------------------------------------------
//  Internal Function Handlers
// ------------------------------------------------------------------------------------------------

void NotificationInternal(const char* Message, va_list args, const char* Sound) {
    char messageBuf[8192];
    vsprintf_s(messageBuf, sizeof(messageBuf), Message, args);
    Notification_internal(messageBuf, Sound, 0, 1, 1);
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

ModInfo* GetFile(TESForm* form) {
    if (!form->unk08)
        return nullptr;

    if (form->unk08->size <= 0)
        return nullptr;

    int index = form->unk08->size - 1;
    return form->unk08->entries[index];
}

void GetFileModifiedTime(ModInfo* file, _SYSTEMTIME& systemFileTime) {
    _FILETIME localFileTime;
    FileTimeToLocalFileTime(&file->fileTime.modified, &localFileTime);
    FileTimeToSystemTime(&localFileTime, &systemFileTime);
}

UInt16 GetLevel(Actor* actor) {
    TESActorBaseData* base = DYNAMIC_CAST(actor->baseForm, TESForm, TESActorBaseData);
    return CALL_MEMBER_FN(base, GetLevel)();
}

void PlayIdle(Actor* actor, TESIdleForm* idle) {
    PlayIdle_Internal(actor->middleProcess, actor, 0x35, idle, 0x01, 0x00);
}

void EquipItem(Actor* actor, TESForm* Form, bool PreventUnequip) {
    BGSObjectInstance Data(Form, nullptr);
    EquipItem_Internal((*g_EquipManager), actor, &Data, 0, 1, nullptr, 1, PreventUnequip, 1, 0, nullptr);
}

void UnequipItem(Actor* actor, TESForm* Form, bool PreventReequip) {
    BGSObjectInstance Data(Form, nullptr);
    UnequipItem_Internal((*g_EquipManager), actor, &Data, 1, nullptr, -1, 1, PreventReequip, 1, 0, nullptr);
}

// ------------------------------------------------------------------------------------------------
//  Lookup
// ------------------------------------------------------------------------------------------------

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