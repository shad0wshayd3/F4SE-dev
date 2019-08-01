#include "Papyrus.h"

#include "Data.h"
#include "f4se_globals/Globals.h"

#include "f4se/GameReferences.h"
#include "f4se/GameRTTI.h"
#include "f4se/PapyrusNativeFunctions.h"

namespace Papyrus {
    DECLARE_STRUCT(TaggedSkills, SCRIPT_NAME);
    TaggedSkills DefaultSkills;

    void AdvanceSkill(StaticFunctionTag* base, BSFixedString skillName, float value) {
        if (!skillName || !value)
            return;

        ActorValueInfo* skill = Skills::GetByName(skillName.c_str());
        if (skill != nullptr)
            (*g_player)->actorValueOwner.Mod(1, skill, value);
    }

    void IncrementSkill(StaticFunctionTag* base, ActorValueInfo* skill, UInt32 value) {
        if (!skill || !value)
            return;

        (*g_player)->actorValueOwner.Mod(1, skill, value);
    }

    void RemoveAllTraits(StaticFunctionTag* base) {
        for (auto trait : Perks::MasterTraitList) {
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

    bool RegisterFunctions(VirtualMachine* VM) {
        DefaultSkills.SetNone(true);

        VM->RegisterFunction(new NativeFunction1<StaticFunctionTag, void, UInt32>
            ("ShowSkillsMenu",        SCRIPT_NAME, Perks::ShowSkillsMenu, VM));

        VM->RegisterFunction(new NativeFunction1<StaticFunctionTag, void, UInt32>
            ("ShowPerksMenu",         SCRIPT_NAME, Perks::ShowPerksMenu, VM));

        VM->RegisterFunction(new NativeFunction1<StaticFunctionTag, void, UInt32>
            ("ShowTraitsMenu",        SCRIPT_NAME, Perks::ShowTraitsMenu, VM));

        VM->RegisterFunction(new NativeFunction3<StaticFunctionTag, bool, UInt32, bool, TaggedSkills>
            ("ShowTagMenu",           SCRIPT_NAME, Perks::ShowTagMenu, VM));

        VM->RegisterFunction(new NativeFunction3<StaticFunctionTag, bool, bool, bool, TaggedSkills>
            ("CharGenSkillSelection", SCRIPT_NAME, Perks::CharGenSkillSelect, VM));

        // Override existing Game.AdvanceSkill and Game.IncrementSkill functions
        VM->RegisterFunction(new NativeFunction2<StaticFunctionTag, void, BSFixedString, float>
            ("AdvanceSkill",    "Game", AdvanceSkill, VM));

        VM->RegisterFunction(new NativeFunction2<StaticFunctionTag, void, ActorValueInfo*, UInt32>
            ("IncrementSkill",  "Game", IncrementSkill, VM));

        VM->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMArray<UInt32>, BGSListForm*, TESForm*>
            ("FindNestedForm",  SCRIPT_NAME, FindNestedForm, VM));

        VM->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>
            ("RemoveAllTraits", SCRIPT_NAME, RemoveAllTraits, VM));

        return true;
    }
}