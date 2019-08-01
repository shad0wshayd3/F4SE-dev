#include "ObScript.h"

#include "Data.h"

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
            Perks::ShowSkillsMenu(nullptr, (points != 0) ? points : 15);
            break;

        case 2:
            Perks::ShowPerksMenu(nullptr, (points != 0) ? points : 1);
            break;

        case 3:
            Perks::ShowTraitsMenu(nullptr, (points != 0) ? points : 2);
            break;

        case 4:
            Perks::ShowTagMenu(nullptr, (points != 0) ? points : 3, false, Papyrus::DefaultSkills);
            break;

        case 5:
            if ((*g_ui)->IsMenuOpen(LevelUpMenu)) {
                CALL_MEMBER_FN(*g_uiMessageManager, SendUIMessage)(LevelUpMenu, kMessage_Close);
                RemoveImagespaceModifier(g_Data.PipboyMenuImod);
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

    bool Commit() {
        kCommand_ShowLevelUpMenu.helpText = "ids: 1-skills, 2-perks, 3-traits, 4-tags, 5-close";
        if (!g_ObScript.OverloadCommand("DualCast", kCommand_ShowLevelUpMenu))
            return false;

        kCommand_GetPermanentValue.helpText = "Get the value of a property ignoring temporary modifiers [player.getpermanentvalue energyweapons]";
        if (!g_ObScript.OverloadCommand("GetPermanentValue", kCommand_GetPermanentValue))
            return false;

        return true;
    }
}