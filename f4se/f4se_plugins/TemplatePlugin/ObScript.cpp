#include "ObScript.h"

namespace ObScript {
    bool Hook_Commit() {
        // if (!IObScript::OverloadCommand("Example", kObScriptCommand_Example)) {
        //     return false;
        // }

        return true;
    }
}