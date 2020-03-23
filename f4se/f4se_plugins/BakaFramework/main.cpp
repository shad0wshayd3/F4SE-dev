#include "Forms.h"

#include "Events.h"
#include "ObScript.h"
#include "Papyrus.h"

#include "common/ICriticalSection.h"
#include "f4se_common/BranchTrampoline.h"

#include <sstream>

PluginHandle                    g_PluginHandle      = kPluginHandle_Invalid;
F4SEMessagingInterface*         g_Messaging         = nullptr;
F4SEPapyrusInterface*           g_Papyrus           = nullptr;
F4SETaskInterface*              g_Tasks             = nullptr;
ICriticalSection                s_DataLock;

void GameLoadedCallback() {
    GetGlobalEventDispatcher<RadiationSourceCount>().AddEventSink(&g_RadiationSourceCountHandler);
    GetGlobalEventDispatcher<PipboyLightEvent>().AddEventSink(&g_PipboyLightEventHandler);

    g_Messaging->Dispatch(g_PluginHandle, BakaMessages::kMessage_FrameworkInit, nullptr, sizeof(nullptr), nullptr);
}

void F4SEMessageHandler(F4SEMessagingInterface::Message* msg) {
    switch (msg->type) {
    case F4SEMessagingInterface::kMessage_PostLoad:
        break;

    case F4SEMessagingInterface::kMessage_PostPostLoad:
        break;

    case F4SEMessagingInterface::kMessage_NewGame:
        break;

    case F4SEMessagingInterface::kMessage_GameLoaded:
        break;

    case F4SEMessagingInterface::kMessage_GameDataReady:
        s_DataLock.Enter();

        if (static_cast<bool>(msg->data)) {
            if (!Forms::Load()) {
                s_DataLock.Leave();
                HALT("Game Plugin error.");
            }

            if ((*g_ui))
                (*g_ui)->menuOpenCloseEventSource.AddEventSink(&g_MenuOpenCloseEventHandler);
            GetEventDispatcher<TESContainerChangedEvent>()->AddEventSink(&g_TESContainerChangedEventHandler);

        } else {
            Forms::Unload();
        }

        s_DataLock.Leave();
        break;

    default:
        break;
    }
}

extern "C" {
    bool F4SEPlugin_Query(const F4SEInterface* F4SE, PluginInfo* Info) {
        InitializePlugin(PLUGIN_NAME, CONFIG_FILE_NAME);
        ITimeKeeper PluginQueryTimer = ITimeKeeper();

        _LogMessage("%s log opened (PC-64)", PLUGIN_NAME);
        _LogMessage("This is a plugin log only and does not contain information on any other part of the game, including crashes.");

        Info->infoVersion   = PluginInfo::kInfoVersion;
        Info->name          = PLUGIN_NAME;
        Info->version       = PLUGIN_VERSION;

        g_PluginHandle = F4SE->GetPluginHandle();

        if (F4SE->runtimeVersion != SUPPORTED_RUNTIME_VERSION) {
            _LogError("Unsupported runtime version v%d.%d.%d.%d. This DLL is built for v%d.%d.%d.%d only. Plugin will be disabled.",
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

        g_Messaging = (F4SEMessagingInterface*)F4SE->QueryInterface(kInterface_Messaging);
        if (!g_Messaging) {
            _LogError("Failed to get Messaging interface. Plugin will be disabled.");
            return false;
        }

        g_Papyrus = (F4SEPapyrusInterface*)F4SE->QueryInterface(kInterface_Papyrus);
        if (!g_Papyrus) {
            _LogError("Failed to get Papyrus interface. Plugin will be disabled.");
            return false;
        }

        g_Tasks = (F4SETaskInterface*)F4SE->QueryInterface(kInterface_Task);
        if (!g_Tasks) {
            _LogError("Failed to get Tasks interface. Plugin will be disabled.");
            return false;
        }

        if (!g_branchTrampoline.Create(1024 * 64)) {
            _LogError("Failed to create Branch Trampoline. Plugin will be disabled.");
            return false;
        }

        if (!g_localTrampoline.Create(1024 * 64, nullptr)) {
            _LogError("Failed to create CodeGen Buffer. Plugin will be disabled.");
            return false;
        }

        if (!IObScript::Init()) {
            _LogError("Failed to initialize ObScript interface. Plugin will be disabled.");
            return false;
        }

        _LogMessage("F4SEPlugin_Query Time: %fms", PluginQueryTimer.Format(ITimeKeeper::kDuration_Milli));
        return true;
    }

    bool F4SEPlugin_Load(const F4SEInterface* F4SE) {
        if (!g_Messaging->RegisterListener(g_PluginHandle, "F4SE", F4SEMessageHandler)) {
            _LogWarning("Failed to register for Messaging listener.");
            return false;
        }

        if (!g_Papyrus->Register(Papyrus::Init)) {
            _LogWarning("Failed to register Papyrus script functions.");
            return false;
        }

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
