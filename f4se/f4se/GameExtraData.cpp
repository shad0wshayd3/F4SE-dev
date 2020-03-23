#include "f4se/GameExtraData.h"
#include "f4se/GameObjects.h"

//
RelocAddr <uintptr_t> s_BGSObjectInstanceExtra(0x02C4BE10);

// ??_7ExtraPowerLinks@@6B@
RelocAddr <uintptr_t> s_ExtraPowerLinksVtbl(0x02C52388);

// ??_7ExtraInstanceData@@6B@
RelocAddr <uintptr_t> s_ExtraInstanceDataVtbl(0x02C53C10);

// ??_7ExtraHealth@@6B@
RelocAddr <uintptr_t> s_ExtraHealthVtbl(0x02C521E8);

// ??_7ExtraMaterialSwap@@6B@
RelocAddr <uintptr_t> s_ExtraMaterialSwapVtbl(0x02C528E8);

bool BaseExtraList::HasType(UInt32 type) const
{
    UInt32 index = (type >> 3);
    UInt8 bitMask = 1 << (type % 8);
    return (flags[index] & bitMask) != 0;
}

bool ExtraDataList::HasType(UInt32 type)
{
    BSReadLocker locker(&m_lock);
    return (m_data.flags) ? m_data.HasType(type) : false;
}

void ExtraDataList::MarkType(UInt32 type, bool bCleared)
{
    if (!m_data.flags) {
        m_data.flags = (UInt8*)Heap_Allocate(sizeof(UInt8));
        memset(m_data.flags, 0, sizeof(UInt8));
    }

    UInt32 index = (type >> 3);
    UInt8 bitMask = 1 << (type % 8);
    UInt8& flag = m_data.flags[index];
    if (bCleared) {
        flag &= ~bitMask;
    } else {
        flag |= bitMask;
    }
}

bool ExtraDataList::Remove(UInt8 type, BSExtraData* toRemove)
{
    if (!toRemove) return false;

    BSReadAndWriteLocker locker(&m_lock);
    if (HasType(type)) {
        bool bRemoved = false;
        if (m_data.head == toRemove) {
            m_data.head = m_data.head->next;
            bRemoved = true;
        }

        for (BSExtraData* traverse = m_data.head; traverse; traverse = traverse->next) {
            if (traverse->next == toRemove) {
                traverse->next = toRemove->next;
                bRemoved = true;
                break;
            }
        }
        if (bRemoved) {
            MarkType(type, true);
        }
        return true;
    }

    return false;
}

bool ExtraDataList::Add(UInt8 type, BSExtraData* toAdd)
{
    if (!toAdd || HasType(type)) return false;

    BSReadAndWriteLocker locker(&m_lock);
    BSExtraData* next = m_data.head;
    m_data.head = toAdd;
    toAdd->next = next;
    MarkType(type, false);
    return true;
}

BSExtraData* ExtraDataList::GetByType(UInt32 type)
{
    if (!HasType(type))
        return NULL;

    BSReadLocker locker(&m_lock);
    for(BSExtraData * traverse = m_data.head; traverse; traverse = traverse->next) {
        if(traverse->type == type)
            return traverse;
    }
    return NULL;
}

#ifdef _DEBUG
#include "f4se_common\Utilities.h"

void ExtraDataList::Dump()
{
    for(BSExtraData * traverse = m_data; traverse; traverse = traverse->next)
    {
        gLog.Indent();
        _MESSAGE("%016I64X %s", traverse, GetObjectClassName(traverse));
        gLog.Outdent();
    }
}
#endif

BSExtraData* BSExtraData::Create(UInt32 size, UInt64 vtbl)
{
    void* memory = Heap_Allocate(size);
    memset(memory, 0, size);
    ((UInt64*)memory)[0] = vtbl;
    BSExtraData* xData = (BSExtraData*)memory;
    xData->unk10 = 0;
    xData->unk13 = 0;
    xData->unk14 = 0;
    xData->next = NULL;
    return xData;
}

BGSObjectInstanceExtra* BGSObjectInstanceExtra::Create()
{
    BGSObjectInstanceExtra* pObjectInstance = (BGSObjectInstanceExtra*)BSExtraData::Create(sizeof(BGSObjectInstanceExtra), s_BGSObjectInstanceExtra.GetUIntPtr());
    pObjectInstance->type = kExtraData_ObjectInstance;
    return pObjectInstance;
}

ExtraPowerLinks* ExtraPowerLinks::Create()
{
    ExtraPowerLinks* pPowerLinks = (ExtraPowerLinks*)BSExtraData::Create(sizeof(ExtraPowerLinks), s_ExtraPowerLinksVtbl.GetUIntPtr());
    pPowerLinks->type = kExtraData_PowerLinks;
    pPowerLinks->connections.entries = NULL;
    pPowerLinks->connections.count = 0;
    pPowerLinks->connections.capacity = 0;
    return pPowerLinks;
}

ExtraInstanceData* ExtraInstanceData::Create(TESForm * baseForm, TBO_InstanceData * instanceData)
{
    ExtraInstanceData* pInstanceData = (ExtraInstanceData*)BSExtraData::Create(sizeof(ExtraInstanceData), s_ExtraInstanceDataVtbl.GetUIntPtr());
    pInstanceData->type = kExtraData_InstanceData;
    pInstanceData->baseForm = baseForm;
    pInstanceData->instanceData = instanceData;
    InterlockedIncrement(&instanceData->m_refCount);
    return pInstanceData;
}

ExtraHealth* ExtraHealth::Create(float value)
{
    ExtraHealth* pHeath = (ExtraHealth*)BSExtraData::Create(sizeof(ExtraHealth), s_ExtraHealthVtbl.GetUIntPtr());
    pHeath->type = kExtraData_Health;
    pHeath->health = value;
    return pHeath;
}

ExtraMaterialSwap* ExtraMaterialSwap::Create(BGSMaterialSwap * matSwap)
{
    ExtraMaterialSwap* pMatSwap = (ExtraMaterialSwap*)BSExtraData::Create(sizeof(ExtraMaterialSwap), s_ExtraMaterialSwapVtbl.GetUIntPtr());
    pMatSwap->type = kExtraData_MaterialSwap;
    pMatSwap->materialSwap = matSwap;
    return pMatSwap;
}