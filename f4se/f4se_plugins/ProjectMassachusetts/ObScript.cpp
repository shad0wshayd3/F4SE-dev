#include "ObScript.h"

#include "Condition.h"
#include "Perks.h"
#include "Scaleform.h"

bool Cmd_GetPermanentValue_Execute(EXECUTE_ARGS) {
    *result = 0.0;

    ActorValueInfo* avif;
    if (!ExtractArgs(PASS_EXTRACT_ARGS, &avif))
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

bool Cmd_GetWeaponHealth_Execute(EXECUTE_ARGS) {
    if (!thisObj)
        thisObj = (*g_player);

    WeaponConditionData Data;
    BSFixedString objName = CALL_MEMBER_FN(thisObj, GetReferenceName)();
    std::string name = objName.c_str();

    switch (thisObj->baseForm->formType) {
    case kFormType_ACHR:
    case kFormType_NPC_:
    case kFormType_REFR: {
        Actor* actor = DYNAMIC_CAST(thisObj, TESObjectREFR, Actor);

        if (actor) {
            if (name.empty())
                name = (thisObj->formID == 0x14) ? "Player" : "Actor";

            Data = WeaponConditionData(actor);
            if (!Data.extraData) {
                Console_Print("%s does not have a valid weapon equipped.", name.c_str());
                return true;
            }

            name.append("'s Equipped Weapon");
        }
        break;
    }

    case kFormType_WEAP:
        if (name.empty())
            name = "Weapon";

        Data = WeaponConditionData(thisObj);
        break;

    default:
        Console_Print("%s is not a weapon.", name.c_str());
        return true;
    }

    float Current = GetWeaponConditionCurrent(Data);
    float Maximum = GetWeaponConditionMaximum(Data);

    if (Current >= 0) {
        float Percent = (Current / Maximum) * 100;
        Console_Print("%s Condition >> %0.2f / %0.2f (%0.2f%%)", name.c_str(), Current, Maximum, Percent);
    } else {
        Console_Print("%s does not have Condition. Recieved %0.2f", name.c_str(), Current);
    }

    return true;
}

bool Cmd_SetWeaponHealth_Execute(EXECUTE_ARGS) {
    if (!thisObj)
        thisObj = (*g_player);

    float value = 0.0;
    if (!ExtractArgs(PASS_EXTRACT_ARGS, &value))
        return true;

    if (!value)
        value = 0.0;

    WeaponConditionData Data;
    BSFixedString objName = CALL_MEMBER_FN(thisObj, GetReferenceName)();
    std::string name = objName.c_str();

    switch (thisObj->baseForm->formType) {
    case kFormType_ACHR:
    case kFormType_NPC_:
    case kFormType_REFR: {
        Actor* actor = DYNAMIC_CAST(thisObj, TESObjectREFR, Actor);

        if (actor) {
            if (name.empty())
                name = (thisObj->formID == 0x14) ? "Player" : "Actor";

            Data = WeaponConditionData(actor);
            if (!Data.extraData) {
                Console_Print("%s does not have a valid weapon equipped.", name.c_str());
                return true;
            }

            name.append("'s Equipped Weapon");
        }
        break;
    }

    case kFormType_WEAP:
        if (name.empty())
            name = "Weapon";

        Data = WeaponConditionData(thisObj);
        break;

    default:
        Console_Print("%s is not a weapon.", name.c_str());
        return true;
    }

    SetWeaponConditionCurrent(Data, value);
    float Current = GetWeaponConditionCurrent(Data);

    if (Current >= 0) {
        Console_Print("Set %s Condition to %0.2f", name.c_str(), Current);
    } else {
        Console_Print("%s does not have Condition to set.", name.c_str());
    }

    return true;
}

bool Cmd_ShowLevelUpMenu_Execute(EXECUTE_ARGS) {
    UInt32 menuID = 0;
    UInt32 points = 0;

    if (!ExtractArgs(PASS_EXTRACT_ARGS, &menuID, &points))
        return true;

    if (!menuID)
        return true;

    if (!points)
        points = 0;

    switch (menuID) {
    case 1:
        OpenLevelUpMenu(EntryList::kMenuID_Skills, GetLevel((*g_player)), points, 0);
        break;
    case 2:
        OpenLevelUpMenu(EntryList::kMenuID_Perks, GetLevel((*g_player)), points, 0);
        break;
    case 3:
        OpenLevelUpMenu(EntryList::kMenuID_Traits, GetLevel((*g_player)), points, 0);
        break;
    case 4:
        OpenLevelUpMenu(EntryList::kMenuID_Tags, GetLevel((*g_player)), points, 0);
        break;

    case 5: {
        BSFixedString LevelUpMenu("LevelUpMenu");
        if ((*g_ui)->IsMenuOpen(LevelUpMenu))
            CALL_MEMBER_FN(*g_uiMessageManager, SendUIMessage)(LevelUpMenu, kMessage_Close);
        else
            Console_Print("The Level Up Menu isn't open, baka!");
        break;
    }

    default:
        Console_Print("Invalid menu ID!");
        break;
    }

    return true;
}

bool ObScript::Init() {
    kObScriptCommand_GetWeaponHealth.helpText = "Get the health of a weapon or an actor's equipped weapon [player.getweaponhealth]";
    if (!IObScript::OverloadCommand("GetLegalDocs", kObScriptCommand_GetWeaponHealth))
        return false;

    kObScriptCommand_SetWeaponHealth.helpText = "Set the health of a weapon, or an actor's equipped weapon [player.setweaponhealth 100]";
    if (!IObScript::OverloadCommand("UnlockWord", kObScriptCommand_SetWeaponHealth))
        return false;

    kObScriptCommand_ShowLevelUpMenu.helpText = "ids: 1-skills, 2-perks, 3-traits, 4-tags, 5-close menu";
    if (!IObScript::OverloadCommand("DualCast", kObScriptCommand_ShowLevelUpMenu))
        return false;

    kObScriptCommand_GetPermanentValue.helpText = "Get the value of a property ignoring temporary modifiers [player.getpermanentvalue barter]";
    if (!IObScript::OverloadCommand("GetPermanentValue", kObScriptCommand_GetPermanentValue))
        return false;

    return true;
}