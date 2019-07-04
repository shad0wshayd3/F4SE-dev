#pragma once
#include "f4se_common/f4se_version.h"

//-----------------------
// Plugin Information
//-----------------------
#define PLUGIN_VERSION              1
#define PLUGIN_VERSION_STRING       "1.0.0"
#define PLUGIN_NAME_SHORT           "$projectname$"
#define PLUGIN_NAME_LONG            "$projectname$"
#define SUPPORTED_RUNTIME_VERSION   CURRENT_RELEASE_RUNTIME
#define MINIMUM_RUNTIME_VERSION     RUNTIME_VERSION_1_10_138
#define COMPATIBLE(runtimeVersion)  (runtimeVersion == SUPPORTED_RUNTIME_VERSION)