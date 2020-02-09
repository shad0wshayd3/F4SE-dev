#pragma once

#include "Definitions.h"
#include "Native.h"

#include "ILog.h"

#include "f4se/GameReferences.h"
#include "f4se/GameRTTI.h"

#define GetTableItem(Index, Name) ((*g_PipboyDataManager)->inventoryData.inventoryObjects[Index]->table.Find(&BSFixedString(Name)))

#define GetTableValue(Type, Index, Name) ((PipboyPrimitiveValue<Type>*)(GetTableItem(Index, Name)->value))->value

#define GetTableArray(Index, Name) ((PipboyArray*)GetTableItem(Index, Name)->value)

#define GetTableArrayValue(Type, Index, Array) ((PipboyPrimitiveValue<Type>*)Array->value[Index])->value

#define CastInstanceData(instanceData, Type) (##Type## ::InstanceData*)Runtime_DynamicCast(instanceData, RTTI_TBO_InstanceData, RTTI_##Type##__InstanceData)

#define GetItemInstanceData(ExtraData, Type)                                                                                                                                                           \
    (##Type## ::InstanceData*)Runtime_DynamicCast(((ExtraInstanceData*)ExtraData->GetByType(kExtraData_InstanceData))->instanceData, RTTI_TBO_InstanceData, RTTI_##Type##__InstanceData)

#define LookupTypeByID(FormID, Type) (##Type*)Runtime_DynamicCast((void*)(LookupFormByID(FormID)), RTTI_TESForm, RTTI_##Type)

#define GetExtraDataByType(ExtraData, Type) ((Extra##Type##*)ExtraData->GetByType(kExtraData_##Type##))

#define GetExtraDataValue(ExtraData, Type, name) (GetExtraDataByType(ExtraData, Type)->##name##)

#define SortGeneric(Entry1, Entry2, Member)                                                                                                                                                            \
    if (Entry1.##Member## != Entry2.##Member##)                                                                                                                                                        \
    return (Entry1.##Member## < Entry2.##Member##)

#define SortStrings(Entry1, Entry2, Member)                                                                                                                                                            \
    const char* Name1 = Entry1.##Member##.c_str();                                                                                                                                                     \
    const char* Name2 = Entry2.##Member##.c_str();                                                                                                                                                     \
    int Length = max(strlen(Name1), strlen(Name2));                                                                                                                                                    \
    for (int z = 0; z < Length; z++)                                                                                                                                                                   \
        if (Name1[z] != Name2[z])                                                                                                                                                                      \
            return (Name1[z] < Name2[z]);                                                                                                                                                              \
    if (Name1 != Name2)                                                                                                                                                                                \
    return (Name1 < Name2)

#define StringToUpper(Str)                                                                                                                                                                             \
    for (auto& chr: Str)                                                                                                                                                                               \
    chr = toupper(chr)

void Notification(const char* Message, ...);
void NotificationSound(const char* Message, const char* Sound, ...);
bool strifind(const std::string& str, const std::string& search);

UInt16 GetLevel(Actor* actor);
void PlayIdle(Actor* actor, TESIdleForm* idle);
void EquipItem(Actor* actor, TESForm* Form, bool PreventUnequip);
void UnequipItem(Actor* actor, TESForm* Form, bool PreventReequip);

float GetValue(Actor* actor, ActorValueInfo* avif);
float GetBaseValue(Actor* actor, ActorValueInfo* avif);
float GetPermValue(Actor* actor, ActorValueInfo* avif);
float GetTempValue(Actor* actor, ActorValueInfo* avif);
float GetTempMod(Actor* actor, ActorValueInfo* avif);
float GetValue(TESObjectREFR* actor, ActorValueInfo* avif);
float GetBaseValue(TESObjectREFR* actor, ActorValueInfo* avif);
float GetPermValue(TESObjectREFR* actor, ActorValueInfo* avif);
float GetTempValue(TESObjectREFR* actor, ActorValueInfo* avif);
float GetTempMod(TESObjectREFR* actor, ActorValueInfo* avif);
int GetValueInt(Actor* actor, ActorValueInfo* avif);
int GetBaseValueInt(Actor* actor, ActorValueInfo* avif);
int GetPermValueInt(Actor* actor, ActorValueInfo* avif);
void ModValue(Actor* actor, ActorValueInfo* avif, float value, UInt32 type = 0);
void ModPermValue(Actor* actor, ActorValueInfo* avif, float value);
void SetValue(Actor* actor, ActorValueInfo* avif, float value);

TESForm* LookupFormFromMod(const ModInfo* modInfo, UInt32 FormID);

UInt32 GetHandleIDByIndex(UInt32 index);
UInt32 GetStackIDByIndex(UInt32 index);
TESForm* GetInventoryFormByHandleID(UInt32 HandleID);
TESForm* GetInventoryFormByIndex(UInt32 index);
BGSInventoryItem* GetInventoryItemByHandleID(UInt32 HandleID);
BGSInventoryItem* GetInventoryItemByIndex(UInt32 index);
BGSInventoryItem::Stack* GetStackByStackID(BGSInventoryItem* Item, int StackID);
ExtraDataList* GetExtraDataListByStackID(BGSInventoryItem* Item, int StackID);
ExtraDataList* GetExtraDataListByIndex(UInt32 index);

class GFxLogMembers: public GFxValue::ObjectInterface::ObjVisitor {
public:
    GFxLogMembers(GFxValue* value): m_value(value) {}
    virtual bool ShowDisplayMembers(void) override { return true; }
    virtual void Visit(const char* member, GFxValue* value) override;
    GFxValue* m_value;
};

class GFxLogElements: public GFxValue::ObjectInterface::ArrayVisitor {
public:
    GFxLogElements(GFxValue* value): m_value(value) {}
    virtual void Visit(UInt32 idx, GFxValue* val);
    GFxValue* m_value;
};