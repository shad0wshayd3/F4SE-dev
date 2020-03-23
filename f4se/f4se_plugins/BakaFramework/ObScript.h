#pragma once

#include "f4se_globals/Globals.h"

namespace ObScript {
    bool Hook_Commit();
    bool InitializeBetaComment();

    DEFINE_CMD_FULL(BetaComment, bc, Log comments about a reference to file, 0, kParams_OneString);
    DEFINE_CMD_EVAL_FULL(GetPermanentValue, , , 1, kParams_OneActorValue);
}