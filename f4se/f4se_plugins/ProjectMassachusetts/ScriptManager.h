#pragma once
#include "DataManager.h"

#include "f4se/PapyrusVM.h"

#define SCRIPT_NAME "Massachusetts:RTNG"

static ObScriptParam kParams_ShowLevelUpMenu[2] = {
    {   "menuID", kType_Integer, 0 },
    {   "points", kType_Integer, 1 },
};

namespace ObScript {
    DEFINE_CMD_FULL(ShowLevelUpMenu, slum, , 0, kParams_ShowLevelUpMenu);
    DEFINE_CMD_EVAL_FULL(GetPermanentValue, , , 1, kParams_OneActorValue);
}

class ScriptManager : private DataManager {
public:
    static bool    Init(VirtualMachine* VM);
};