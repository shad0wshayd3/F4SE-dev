#pragma once
#include "f4se_common/f4se_version.h"

//-----------------------
// Plugin Information
//-----------------------
#define PLUGIN_VERSION                  1
#define PLUGIN_VERSION_STRING           "1.0.0"
#define PLUGIN_NAME                     "BakaFramework"
#define SUPPORTED_RUNTIME_VERSION       CURRENT_RELEASE_RUNTIME
#define MINIMUM_RUNTIME_VERSION         RUNTIME_VERSION_1_10_163
#define COMPATIBLE(runtimeVersion)      (runtimeVersion == SUPPORTED_RUNTIME_VERSION)

#define CONFIG_FILE_NAME                "Data\\F4SE\\plugins\\BakaFramework.ini"
#define DEFAULT_PLUGIN_NAME             "BakaFramework.esm"
#define PAPYRUS_SCRIPT_NAME             "BakaUtils"