#include "Virtual.h"

bool HasKeyword(TESForm* form, BGSKeyword* keyword) {
    IKeywordFormBase* keywordFormBase = DYNAMIC_CAST(form, TESForm, IKeywordFormBase);
    if (keywordFormBase) {
        auto HasKeyword_Internal = GetVirtualFunction<_IKeywordFormBase_HasKeyword>(keywordFormBase, 1);
        if (HasKeyword_Internal(keywordFormBase, keyword, 0)) { // unk3=0 in condition HasKeyword
            return true;
        }
    }

    return false;
}