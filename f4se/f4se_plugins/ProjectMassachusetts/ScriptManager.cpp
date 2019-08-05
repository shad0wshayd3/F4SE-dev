#include "ScriptManager.h"
#include "PerkManager.h"

#include "f4se/PapyrusNativeFunctions.h"

namespace ObScript {
    bool Cmd_GetPermanentValue_Execute(EXECUTE_ARGS) {
        *result = 0.0;

        ActorValueInfo* avif;
        if (!ExtractArgs(EXTRACT_ARGS, &avif))
            return true;

        if (!thisObj || !avif)
            return true;

        *(float*)result = GetPermValue(thisObj, avif);
        Console_Print("GetPermanentValue: %s >> %0.2f", avif->avName, result);
        return true;
    }

    bool Cmd_GetPermanentValue_Eval(EVAL_ARGS) {
        *result = 0.0;
        ActorValueInfo* avif = (ActorValueInfo*)arg1;

        if (!thisObj || !avif)
            return true;

        if (!thisObj->formID)
            thisObj = (*g_player);

        *(float*)result = GetPermValue(thisObj, avif);
        return true;
    }

    bool Cmd_ShowLevelUpMenu_Execute(EXECUTE_ARGS) {
        UInt32 menuID = 0;
        UInt32 points = 0;

        if (!ExtractArgs(EXTRACT_ARGS, &menuID, &points))
            return true;

        if (!menuID)
            return true;

        if (!points)
            points = 0;

        BSFixedString LevelUpMenu("LevelUpMenu");

        switch (menuID) {
        case 1:
            PerkManager::ShowSkillsMenu(nullptr, (points != 0) ? points : 15);
            break;

        case 2:
            PerkManager::ShowPerksMenu(nullptr, (points != 0) ? points : 1);
            break;

        case 3:
            PerkManager::ShowTraitsMenu(nullptr, (points != 0) ? points : 2);
            break;

        case 4:
            PerkManager::ShowTagMenu(nullptr, (points != 0) ? points : 3, Papyrus::DefaultSkills, false);
            break;

        case 5:
            if ((*g_ui)->IsMenuOpen(LevelUpMenu)) {
                CALL_MEMBER_FN(*g_uiMessageManager, SendUIMessage)(LevelUpMenu, kMessage_Close);
                RemoveImagespaceModifier(DataManager::PipboyMenuIMOD);
            }
            else {
                Console_Print("The Level Up Menu isn't open, baka!");
            }
            break;

        default:
            Console_Print("Invalid menu ID!");
            break;
        }

        return true;
    }
}

namespace Papyrus {
    DECLARE_STRUCT(TaggedSkills, SCRIPT_NAME);
    TaggedSkills DefaultSkills;

    void AdvanceSkill(StaticFunctionTag* base, BSFixedString skillName, float value) {
        if (!skillName || !value)
            return;

        ActorValueInfo* skill = DataManager::GetSkillByName(skillName.c_str());
        if (skill != nullptr)
            (*g_player)->actorValueOwner.Mod(1, skill, value);
    }

    void IncrementSkill(StaticFunctionTag* base, ActorValueInfo* skill, UInt32 value) {
        if (!skill || !value)
            return;

        (*g_player)->actorValueOwner.Mod(1, skill, value);
    }

    void RemoveAllTraits(StaticFunctionTag* base) {
        for (auto trait : DataManager::m_TraitList) {
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
}

bool ScriptManager::Init(VirtualMachine* VM) {
// --------------------------------------------------------------------------------------------
// ObScript
// --------------------------------------------------------------------------------------------
    ObScript::kCommand_ShowLevelUpMenu.helpText = "ids: 1-skills, 2-perks, 3-traits, 4-tags, 5-close";
    if (!g_ObScript.OverloadCommand("DualCast", ObScript::kCommand_ShowLevelUpMenu))
        return false;

    ObScript::kCommand_GetPermanentValue.helpText = "Get the value of a property ignoring temporary modifiers [player.getpermanentvalue energyweapons]";
    if (!g_ObScript.OverloadCommand("GetPermanentValue", ObScript::kCommand_GetPermanentValue))
        return false;

    return true;

// --------------------------------------------------------------------------------------------
// Papyrus
// --------------------------------------------------------------------------------------------
    Papyrus::DefaultSkills.SetNone(true);

    VM->RegisterFunction(new NativeFunction1<StaticFunctionTag, void, UInt32>
        ("ShowSkillsMenu",        SCRIPT_NAME, PerkManager::ShowSkillsMenu, VM));

    VM->RegisterFunction(new NativeFunction1<StaticFunctionTag, void, UInt32>
        ("ShowPerksMenu",         SCRIPT_NAME, PerkManager::ShowPerksMenu, VM));

    VM->RegisterFunction(new NativeFunction1<StaticFunctionTag, void, UInt32>
        ("ShowTraitsMenu",        SCRIPT_NAME, PerkManager::ShowTraitsMenu, VM));

    VM->RegisterFunction(new NativeFunction3<StaticFunctionTag, bool, UInt32, Papyrus::TaggedSkills, bool>
        ("ShowTagMenu",           SCRIPT_NAME, PerkManager::ShowTagMenu, VM));

    VM->RegisterFunction(new NativeFunction3<StaticFunctionTag, bool, UInt32, Papyrus::TaggedSkills, bool>
        ("CharGenSkillSelection", SCRIPT_NAME, PerkManager::CharGenSkillSelect, VM));

    // Override existing Game.AdvanceSkill and Game.IncrementSkill functions
    VM->RegisterFunction(new NativeFunction2<StaticFunctionTag, void, BSFixedString, float>
        ("AdvanceSkill",    "Game", Papyrus::AdvanceSkill, VM));

    VM->RegisterFunction(new NativeFunction2<StaticFunctionTag, void, ActorValueInfo*, UInt32>
        ("IncrementSkill",  "Game", Papyrus::IncrementSkill, VM));

    VM->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMArray<UInt32>, BGSListForm*, TESForm*>
        ("FindNestedForm",  SCRIPT_NAME, Papyrus::FindNestedForm, VM));

    VM->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>
        ("RemoveAllTraits", SCRIPT_NAME, Papyrus::RemoveAllTraits, VM));

    return true;
}