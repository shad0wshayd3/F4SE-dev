#pragma once

#include "f4se_globals/Globals.h"

static ObScriptParam kParams_ShowLevelUpMenu[2] = {
    {   "menuID", kType_Integer, 0 },
    {   "points", kType_Integer, 1 },
};

namespace ObScript {
    bool Commit();

    DEFINE_CMD_FULL(ShowLevelUpMenu, slum, , 0, kParams_ShowLevelUpMenu);
    DEFINE_CMD_EVAL_FULL(GetPermanentValue, , , 1, kParams_OneActorValue);
}