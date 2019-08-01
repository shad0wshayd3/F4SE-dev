#pragma once

#include "f4se_common/f4se_version.h"

//-----------------------
// Plugin Information
//-----------------------
#define PLUGIN_VERSION              6
#define PLUGIN_VERSION_STRING       "6.0.0"
#define PLUGIN_NAME_SHORT           "BetaCommentEnabler"
#define PLUGIN_NAME_LONG            "BetaCommentEnabler"
#define SUPPORTED_RUNTIME_VERSION   CURRENT_RELEASE_RUNTIME
#define MINIMUM_RUNTIME_VERSION     RUNTIME_VERSION_1_10_138
#define COMPATIBLE(runtimeVersion)  (runtimeVersion == SUPPORTED_RUNTIME_VERSION)

#define INI_FILE_NAME               "Data\\F4SE\\plugins\\BetaCommentEnabler.ini"
