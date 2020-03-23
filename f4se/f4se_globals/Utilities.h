#pragma once

#include "Definitions.h"
#include "Native.h"

#include "ILog.h"

#include "f4se/GameReferences.h"
#include "f4se/GameRTTI.h"

class CurrentTime {
public:
    CurrentTime(float currentTime);
    CurrentTime(UInt16 Day, UInt16 Hour, UInt16 Minute): Day(Day), Hour(Hour), Minute(Minute) { }

    void operator=(CurrentTime const& newTime) {
        Day     = newTime.Day;
        Hour    = newTime.Hour;
        Minute  = newTime.Minute;
    }

    bool operator>(CurrentTime const& compTime) {
        if (Day == compTime.Day) {
            if (Hour == compTime.Hour) {
                return (Minute > compTime.Minute);
            }
            else if (Hour > compTime.Hour) {
                return true;
            }
        }
        else if (Day > compTime.Day) {
            return true;
        }

        return false;
    }

    bool operator<(CurrentTime const& compTime) {
        if (Day == compTime.Day) {
            if (Hour == compTime.Hour) {
                return (Minute < compTime.Minute);
            }
            else if (Hour < compTime.Hour) {
                return true;
            }
        }
        else if (Day < compTime.Day) {
            return true;
        }

        return false;
    }

    bool operator==(CurrentTime const& compTime) {
        return ((Day == compTime.Day) && (Hour == compTime.Hour) && (Minute == compTime.Minute));
    }

    bool operator!=(CurrentTime const& compTime) {
        return ((Day != compTime.Day) || (Hour != compTime.Hour) || (Minute != compTime.Minute));
    }

    int GetDiff(CurrentTime const compTime) {
        return (((Day - compTime.Day) * 1440) + ((Hour - compTime.Hour) * 60) + (Minute - compTime.Minute));
    }

    UInt16 Day;
    UInt16 Hour;
    UInt16 Minute;
};

class GFxLogMembers: public GFxValue::ObjectInterface::ObjVisitor {
public:
    GFxLogMembers(std::string prefix):
        m_prefix(prefix) { }

    GFxLogMembers(std::string prefix, int maxDepth):
        m_prefix(prefix), m_maxDepth(maxDepth) { }

    GFxLogMembers(std::string prefix, int maxDepth, int depth):
        m_prefix(prefix), m_maxDepth(maxDepth), m_depth(depth) { }

    void SetShowDisplayMembers(bool bValue) {
        m_showDM = bValue;
    }

    virtual bool ShowDisplayMembers(void) override {
        return m_showDM;
    }

    virtual void Visit(const char* member, GFxValue* value) override;

private:
    std::string     m_prefix    = "";
    int             m_maxDepth  = 5;
    int             m_depth     = 0;
    bool            m_showDM    = false;
};

class GFxLogElements: public GFxValue::ObjectInterface::ArrayVisitor {
public:
    GFxLogElements(GFxValue* value): m_value(value) { }
    virtual void Visit(UInt32 idx, GFxValue* val);
    GFxValue* m_value;
};

void                        VisitGFxMembers(GFxValue value, std::string prefix, int maxDepth, bool displayMembers);

void                        Notification(const char* Message, ...);
void                        NotificationSound(const char* Message, const char* Sound, ...);
bool                        strifind(const std::string& str, const std::string& search);

ModInfo*                    GetFile(TESForm* form);
void                        GetFileModifiedTime(ModInfo* file, _SYSTEMTIME& systemFileTime);

UInt16                      GetLevel(Actor* actor);
void                        PlayIdle(Actor* actor, TESIdleForm* idle);
void                        EquipItem(Actor* actor, TESForm* Form, bool PreventUnequip);
void                        UnequipItem(Actor* actor, TESForm* Form, bool PreventReequip);

UInt32                      GetHandleIDByIndex(UInt32 index);
UInt32                      GetStackIDByIndex(UInt32 index);
TESForm*                    GetInventoryFormByHandleID(UInt32 HandleID);
TESForm*                    GetInventoryFormByIndex(UInt32 index);
BGSInventoryItem*           GetInventoryItemByHandleID(UInt32 HandleID);
BGSInventoryItem*           GetInventoryItemByIndex(UInt32 index);
BGSInventoryItem::Stack*    GetStackByStackID(BGSInventoryItem* Item, int StackID);
ExtraDataList*              GetExtraDataListByStackID(BGSInventoryItem* Item, int StackID);
ExtraDataList*              GetExtraDataListByIndex(UInt32 index);

// ------------------------------------------------------------------------------------------------
//  Value Functions
// ------------------------------------------------------------------------------------------------

inline float GetValue(TESForm* form, ActorValueInfo* avif) {
    ActorValueOwner* owner = DYNAMIC_CAST(form, TESForm, ActorValueOwner);
    return owner->GetValue(avif);
}

inline float GetBaseValue(TESForm* form, ActorValueInfo* avif) {
    ActorValueOwner* owner = DYNAMIC_CAST(form, TESForm, ActorValueOwner);
    return owner->GetBase(avif);
}

inline float GetPermValue(TESForm* form, ActorValueInfo* avif) {
    ActorValueOwner* owner = DYNAMIC_CAST(form, TESForm, ActorValueOwner);
    return owner->GetBase(avif) + owner->GetMod(1, avif);
}

inline float GetTempValue(TESForm* form, ActorValueInfo* avif) {
    ActorValueOwner* owner = DYNAMIC_CAST(form, TESForm, ActorValueOwner);
    return owner->GetMod(0, avif);
}

inline float GetMaxValue(TESForm* form, ActorValueInfo* avif) {
    ActorValueOwner* owner = DYNAMIC_CAST(form, TESForm, ActorValueOwner);
    return owner->GetMaximum(avif);
}

inline void SetBaseValue(TESForm* form, ActorValueInfo* avif, float value) {
    ActorValueOwner* owner = DYNAMIC_CAST(form, TESForm, ActorValueOwner);
    owner->SetBase(avif, value);
}

inline void ModBaseValue(TESForm* form, ActorValueInfo* avif, float value) {
    ActorValueOwner* owner = DYNAMIC_CAST(form, TESForm, ActorValueOwner);
    owner->ModBase(avif, value);
}

inline void ModPermValue(TESForm* form, ActorValueInfo* avif, float value) {
    ActorValueOwner* owner = DYNAMIC_CAST(form, TESForm, ActorValueOwner);
    owner->Mod(0, avif, value);
}

inline void ModTempValue(TESForm* form, ActorValueInfo* avif, float value) {
    ActorValueOwner* owner = DYNAMIC_CAST(form, TESForm, ActorValueOwner);
    owner->Mod(1, avif, value);
}

inline void RestoreValue(TESForm* form, ActorValueInfo* avif, float value) {
    ActorValueOwner* owner = DYNAMIC_CAST(form, TESForm, ActorValueOwner);
    value = fabsf(value);
    if (avif->avFlags & ActorValueInfo::kFlag_DamageIsPositive)
        value = -value;
    owner->Mod(2, avif, value);
}

inline void DamageValue(TESForm* form, ActorValueInfo* avif, float value) {
    ActorValueOwner* owner = DYNAMIC_CAST(form, TESForm, ActorValueOwner);
    value = fabsf(value);
    if (!(avif->avFlags & ActorValueInfo::kFlag_DamageIsPositive))
        value = -value;
    owner->Mod(2, avif, value);
}

// ------------------------------------------------------------------------------------------------
//  Defines
// ------------------------------------------------------------------------------------------------

#define GetTableItem(Index, Name)\
    ((*g_PipboyDataManager)->inventoryData.inventoryObjects[Index]->table.Find(&BSFixedString(Name)))

#define GetTableValue(Type, Index, Name)\
    ((PipboyPrimitiveValue<Type>*)(GetTableItem(Index, Name)->value))->value

#define GetTableArray(Index, Name)\
    ((PipboyArray*)GetTableItem(Index, Name)->value)

#define GetTableArrayValue(Type, Index, Array)\
    ((PipboyPrimitiveValue<Type>*)Array->value[Index])->value

#define LookupTypeByID(FormID, Type)\
    (##Type*) Runtime_DynamicCast((void*)(LookupFormByID(FormID)), RTTI_TESForm, RTTI_##Type)

#define SortGeneric(Entry1, Entry2, Member)\
    if (Entry1.##Member## != Entry2.##Member##)\
        return (Entry1.##Member## < Entry2.##Member##)

#define SortStrings(Entry1, Entry2, Member)\
    const char* Name1 = Entry1.##Member##.c_str();\
    const char* Name2 = Entry2.##Member##.c_str();\
    int Length = max(strlen(Name1), strlen(Name2));\
    for (int z = 0; z < Length; z++)\
        if (Name1[z] != Name2[z])\
            return (Name1[z] < Name2[z]);\
    if (Name1 != Name2)\
        return (Name1 < Name2)

#define StringToUpper(Str)\
    for (auto& chr : Str)\
        chr = toupper(chr)