#pragma once

#include "f4se_globals/Globals.h"

#define TIMESTAMP "%m/%d/%Y (%I:%M)"
#define INFO_SIZE 0x7FFF

namespace ObScript {
    bool Hook_Commit();
    bool InitializeBetaComment();

    DEFINE_CMD_FULL(BetaComment, bc, Log information about a reference to file, 0, kParams_OneString);
}