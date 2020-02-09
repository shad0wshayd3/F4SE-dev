#include "Config.h"

#include "ObScript.h"
#include "f4se_globals/Globals.h"

#include "f4se/PluginAPI.h"

PluginHandle g_PluginHandle = kPluginHandle_Invalid;

extern "C" {
bool F4SEPlugin_Query(const F4SEInterface* F4SE, PluginInfo* Info) {
    InitializePlugin(PLUGIN_NAME, CONFIG_FILE_NAME);
    ITimeKeeper startupClock = ITimeKeeper();

    _LogMessage("%s log opened (PC-64)", PLUGIN_NAME);
    _LogMessage("This is a plugin log only and does not contain information on any other part of the game, including crashes.");

    Info->infoVersion = PluginInfo::kInfoVersion;
    Info->name = PLUGIN_NAME;
    Info->version = PLUGIN_VERSION;

    g_PluginHandle = F4SE->GetPluginHandle();

    if (F4SE->runtimeVersion != SUPPORTED_RUNTIME_VERSION) {
        _LogError("Unsupported runtime version v%d.%d.%d.%d. This DLL is built for v%d.%d.%d.%d only. Plugin will be disabled.",
                  GET_EXE_VERSION_MAJOR(F4SE->runtimeVersion),
                  GET_EXE_VERSION_MINOR(F4SE->runtimeVersion),
                  GET_EXE_VERSION_BUILD(F4SE->runtimeVersion),
                  GET_EXE_VERSION_SUB(F4SE->runtimeVersion),
                  GET_EXE_VERSION_MAJOR(SUPPORTED_RUNTIME_VERSION),
                  GET_EXE_VERSION_MINOR(SUPPORTED_RUNTIME_VERSION),
                  GET_EXE_VERSION_BUILD(SUPPORTED_RUNTIME_VERSION),
                  GET_EXE_VERSION_SUB(SUPPORTED_RUNTIME_VERSION));

        return false;
    }

    if (!IObScript::Init()) {
        _LogError("Failed to initialize ObScript interface. Plugin will be disabled.");
        return false;
    }

    _LogMessage("F4SEPlugin_Query Time: %fms", startupClock.Format(ITimeKeeper::kDuration_Milli));
    return true;
}

bool F4SEPlugin_Load(const F4SEInterface* F4SE) {
    if (!ObScript::Hook_Commit()) {
        _LogWarning("Failed to commit ObScript functions.");
        return false;
    }

    if (!ObScript::InitializeBetaComment()) {
        _LogWarning("Failed to initialize BetaComment.");
        return false;
    }

    _LogMessage("Plugin loaded successfully.");
    return true;
}
}
