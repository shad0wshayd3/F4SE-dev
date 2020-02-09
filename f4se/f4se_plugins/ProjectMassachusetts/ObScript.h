#pragma once
#include "Forms.h"

static ObScriptParam kParams_ShowLevelUpMenu[2] = {
  {"menuID", kType_Integer, 0},
  {"points", kType_Integer, 1},
};

DEFINE_CMD_FULL(GetWeaponHealth, gwh, , 0, NULL);
DEFINE_CMD_FULL(SetWeaponHealth, swh, , 0, kParams_OneFloat);
DEFINE_CMD_FULL(ShowLevelUpMenu, slum, , 0, kParams_ShowLevelUpMenu);
DEFINE_CMD_EVAL_FULL(GetPermanentValue, , , 1, kParams_OneActorValue);

namespace ObScript {
    bool Init();
}