#include "DataManager.h"
#include "PerkManager.h"
#include "SaveManager.h"
#include "ScaleformManager.h"
#include "ScriptManager.h"
#include "ValueManager.h"

#include "common/ICriticalSection.h"
#include "f4se_common/BranchTrampoline.h"

ICriticalSection                s_LoadLock;

PluginHandle                    g_PluginHandle      = kPluginHandle_Invalid;

F4SEPapyrusInterface*           g_Papyrus           = NULL;
F4SEMessagingInterface*         g_Messaging         = NULL;
F4SEScaleformInterface*         g_Scaleform         = NULL;
F4SESerializationInterface*     g_Serialization     = NULL;

extern "C" {
    void F4SEMessageHandler(F4SEMessagingInterface::Message* Message) {
        switch (Message->type) {
        case F4SEMessagingInterface::kMessage_GameDataReady:
            ITimeKeeper MessageHandlerTimer = ITimeKeeper();
            MessageHandlerTimer.Start();

            s_LoadLock.Enter();

            if (reinterpret_cast<uintptr_t>(Message->data)) {
                if (!DataManager::Load()) {
                    s_LoadLock.Leave();
                    HALT("Game Plugin error.");
                }

                g_GlobalSettings.Init("PM_");

                ValueManager::Init();
                PerkManager::Init();

                _LOGMESSAGE("Data should now be loaded.");
                _LOGMESSAGE("F4SEMessageHandler() Time: %fms", MessageHandlerTimer.Format(ITimeKeeper::Milli));
            }
            else {
                // Unload DataManager
                DataManager::Unload();

                // Unload everything else
                ValueManager::Unload();
                PerkManager::Unload();

                _LOGMESSAGE("Data should now be unloaded.");
                _LOGMESSAGE("F4SEMessageHandler() Time: %fms", MessageHandlerTimer.Format(ITimeKeeper::Milli));
            }

            s_LoadLock.Leave();
            break;
        }
    }

    bool F4SEPlugin_Query(const F4SEInterface* F4SE, PluginInfo* Info) {
        ITimeKeeper PluginQueryTimer = ITimeKeeper();
        PluginQueryTimer.Start();

        _LOGMESSAGE("%s log opened (PC-64)", PLUGIN_NAME_LONG);
        _LOGMESSAGE("This is a plugin log only and does not contain information on any other part of the game, including crashes.");

        Info->infoVersion   = PluginInfo::kInfoVersion;
        Info->name          = PLUGIN_NAME_LONG;
        Info->version       = PLUGIN_VERSION;

        g_PluginHandle      = F4SE->GetPluginHandle();

        if (F4SE->runtimeVersion != SUPPORTED_RUNTIME_VERSION) {
            _LOGERROR("Unsupported runtime version v%d.%d.%d.%d. This DLL is built for v%d.%d.%d.%d only. Plugin will be disabled.",
                GET_EXE_VERSION_MAJOR (F4SE->runtimeVersion),
                GET_EXE_VERSION_MINOR (F4SE->runtimeVersion),
                GET_EXE_VERSION_BUILD (F4SE->runtimeVersion),
                GET_EXE_VERSION_SUB   (F4SE->runtimeVersion),
                GET_EXE_VERSION_MAJOR (SUPPORTED_RUNTIME_VERSION),
                GET_EXE_VERSION_MINOR (SUPPORTED_RUNTIME_VERSION),
                GET_EXE_VERSION_BUILD (SUPPORTED_RUNTIME_VERSION),
                GET_EXE_VERSION_SUB   (SUPPORTED_RUNTIME_VERSION));

            return false;
        }

        g_Papyrus = (F4SEPapyrusInterface*)F4SE->QueryInterface(kInterface_Papyrus);
        if (!g_Papyrus) {
            _LOGERROR("Failed to get Papyrus interface. Plugin will be disabled.");
            return false;
        }

        if (!g_ObScript.Init()) {
            _LOGERROR("Failed to get ObScript interface. Plugin will be disabled.");
            return false;
        }

        g_Messaging = (F4SEMessagingInterface*)F4SE->QueryInterface(kInterface_Messaging);
        if (!g_Messaging) {
            _LOGERROR("Failed to get Messaging interface. Plugin will be disabled.");
            return false;
        }

        g_Scaleform = (F4SEScaleformInterface*)F4SE->QueryInterface(kInterface_Scaleform);
        if (!g_Scaleform) {
            _LOGERROR("Failed to get Scaleform interface. Plugin will be disabled.");
            return false;
        }

        g_Serialization = (F4SESerializationInterface*)F4SE->QueryInterface(kInterface_Serialization);
        if (!g_Serialization) {
            _LOGERROR("Failed to get Serialization interface. Plugin will be disabled.");
            return false;
        }

        if (!g_branchTrampoline.Create(1024 * 64)) {
            _LOGERROR("Failed to create Branch Trampoline. Plugin will be disabled.");
            return false;
        }

        if (!g_localTrampoline.Create(1024 * 64, nullptr)) {
            _LOGERROR("Failed to create CodeGen Buffer. Plugin will be disabled.");
            return false;
        }

        _LOGMESSAGE("F4SEPlugin_Query() Time: %fms", PluginQueryTimer.Format(ITimeKeeper::Milli));
        return true;
    }

    bool F4SEPlugin_Load(const F4SEInterface* F4SE) {
        if (!g_Papyrus->Register(ScriptManager::Init)) {
            _LOGWARNING("Failed to register Script functions");
            return false;
        }

        if (!g_Messaging->RegisterListener(g_PluginHandle, "F4SE", F4SEMessageHandler)) {
            _LOGWARNING("Failed to register for Messaging listener");
            return false;
        }

        if (!g_Scaleform->Register("RTNG", ScaleformManager::Init)) {
            _LOGWARNING("Failed to register for Scaleform interface");
            return false;
        }

        if (!SaveManager::Hook_Commit(g_PluginHandle)) {
            _LOGWARNING("Failed to commit Serialization interface.");
            return false;
        }

        // Hook "Hold [Pipboy] to open Perks menu." to detect player level ups.
        g_branchTrampoline.Write6Branch(LevelUpPrompt.GetUIntPtr(), (uintptr_t)PerkManager::LevelUp_Hook);

        // Hook DR Calc to replace it with something sane
        g_branchTrampoline.Write6Branch(CalculateDamageResist.GetUIntPtr(), (uintptr_t)ValueManager::DamageResistFormula);

        _LOGMESSAGE("Plugin loaded successfully.");
        return true;
    }
}
