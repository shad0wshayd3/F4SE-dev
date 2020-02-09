#pragma once

#include "f4se_common/f4se_version.h"

//-----------------------
// Plugin Information
//-----------------------
#define PLUGIN_VERSION             10
#define PLUGIN_VERSION_STRING      "1.3.0"
#define PLUGIN_NAME                "BetaCommentEnabler"
#define SUPPORTED_RUNTIME_VERSION  CURRENT_RELEASE_RUNTIME
#define MINIMUM_RUNTIME_VERSION    RUNTIME_VERSION_1_10_163
#define COMPATIBLE(runtimeVersion) (runtimeVersion == SUPPORTED_RUNTIME_VERSION)

#define CONFIG_FILE_NAME "Data\\F4SE\\plugins\\BetaCommentEnabler.ini"