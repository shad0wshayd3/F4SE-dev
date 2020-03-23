#pragma once

#include "f4se/GameForms.h"
#include "f4se/GameRTTI.h"

// Thanks, reg
template <typename T>
T GetVirtualFunction(void* baseObject, int vtblIndex) {
    uintptr_t* vtbl = reinterpret_cast<uintptr_t**>(baseObject)[0];
    return reinterpret_cast<T>(vtbl[vtblIndex]);
}

typedef bool(*_IKeywordFormBase_HasKeyword)(IKeywordFormBase* keywordFormBase, BGSKeyword* keyword, UInt32 unk3);
extern bool HasKeyword(TESForm* form, BGSKeyword* keyword);