#include "Papyrus.h"

void AdvanceSkill(StaticFunctionTag* base, BSFixedString name, float value) {
    if (!name || !value)
        return;

    ActorValueInfo* Skill = Forms::GetSkillByName(name.c_str());
    if (Skill)
        ModPermValue((*g_player), Skill, value);
}

void IncrementSkill(StaticFunctionTag* base, ActorValueInfo* skill, UInt32 value) {
    if (!skill || !value)
        return;

    ModPermValue((*g_player), skill, value);
}

void RemoveAllTraits(StaticFunctionTag* base) {
    for (auto trait : Forms::ListTraits) {
        if (HasPerk((*g_player), trait)) {
            RemovePerk((*g_player), trait);
        }
    }
}

std::vector<UInt32> SearchFormList(BGSListForm* FormList, TESForm* Query, std::vector<UInt32> Result) {
    for (int i = 0; i < FormList->forms.count; i++) {
        TESForm* Form = FormList->forms[i];
        if (Form->formID == Query->formID) {
            Result.emplace_back(i); // Result.insert(Result.begin(), i);
            return Result;
        }

        if (Form->formType == FormType::kFormType_FLST) {
            std::vector<UInt32> nResult = SearchFormList(DYNAMIC_CAST(Form, TESForm, BGSListForm), Query, Result);
            if (Result.size() != nResult.size()) {
                nResult.emplace_back(i);
                return nResult;
            }
        }
    }

    return Result;
}

VMArray<UInt32> FindNestedForm(StaticFunctionTag* base, BGSListForm* RootList, TESForm* Query) {
    VMArray<UInt32> Result;
    Result.SetNone(true);

    if (!RootList || !Query)
        return Result;

    Result.SetNone(false);

    std::vector<UInt32> result_t;
    result_t = SearchFormList(RootList, Query, result_t);

    for (UInt32 ret : result_t)
        Result.Push(&ret);

    return Result;
}

bool Papyrus::Init(VirtualMachine* VM) {
    // VM->RegisterFunction(new NativeFunction1<StaticFunctionTag, void, UInt32>
    //     ("ShowSkillsMenu",          PAPYRUS_SCRIPT_NAME,    PerkManager::ShowSkillsMenu,        VM));

    // VM->RegisterFunction(new NativeFunction1<StaticFunctionTag, void, UInt32>
    //     ("ShowPerksMenu",           PAPYRUS_SCRIPT_NAME,    PerkManager::ShowPerksMenu,         VM));

    // VM->RegisterFunction(new NativeFunction1<StaticFunctionTag, void, UInt32>
    //     ("ShowTraitsMenu",          PAPYRUS_SCRIPT_NAME,    PerkManager::ShowTraitsMenu,        VM));

    // VM->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, UInt32>
    //     ("ShowTagMenu",             PAPYRUS_SCRIPT_NAME,    PerkManager::ShowTagMenu,           VM));

    // VM->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, UInt32>
    //     ("CharGenSkillSelection",   PAPYRUS_SCRIPT_NAME,    PerkManager::CharGenSkillSelect,    VM));

    VM->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMArray<UInt32>, BGSListForm*, TESForm*>
        ("FindNestedForm",          PAPYRUS_SCRIPT_NAME,    FindNestedForm,                     VM));

    VM->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>
        ("RemoveAllTraits",         PAPYRUS_SCRIPT_NAME,    RemoveAllTraits,                    VM));

    // Override existing Game.AdvanceSkill and Game.IncrementSkill functions
    VM->RegisterFunction(new NativeFunction2<StaticFunctionTag, void, BSFixedString, float>
        ("AdvanceSkill",            "Game",                 AdvanceSkill,                       VM));

    VM->RegisterFunction(new NativeFunction2<StaticFunctionTag, void, ActorValueInfo*, UInt32>
        ("IncrementSkill",          "Game",                 IncrementSkill,                     VM));

    return true;
}