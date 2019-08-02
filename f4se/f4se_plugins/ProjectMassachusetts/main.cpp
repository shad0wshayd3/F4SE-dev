#include "Data.h"
#include "Config.h"
#include "ObScript.h"
#include "Papyrus.h"
#include "Perks.h"
#include "Scaleform.h"
#include "Serialization.h"
#include "Skills.h"

#include "f4se_globals/Globals.h"

#include "common/ICriticalSection.h"
#include "f4se_common/BranchTrampoline.h"

ILog                            g_Log           = ILog(PLUGIN_NAME_SHORT);
ISettings                       g_Settings      = ISettings(INI_FILE_NAME);
PluginHandle                    g_PluginHandle  = kPluginHandle_Invalid;

DataManager                     g_Data;
GameSettings                    g_GameSettings;
IObScript                       g_ObScript;
ICriticalSection                s_LoadLock;

F4SEPapyrusInterface*           g_Papyrus       = NULL;
F4SEMessagingInterface*         g_Messaging     = NULL;
F4SEScaleformInterface*         g_Scaleform     = NULL;
F4SESerializationInterface*     g_Serialization = NULL;

extern "C" {
    void F4SEMessageHandler(F4SEMessagingInterface::Message* Message) {
        switch (Message->type) {
        case F4SEMessagingInterface::kMessage_GameDataReady:
            s_LoadLock.Enter();

            if (reinterpret_cast<uintptr_t>(Message->data)) {
                ITimeKeeper DataLoader = ITimeKeeper();
                DataLoader.Start();

                if (!g_Data.Init()) {
                    s_LoadLock.Leave();
                    return;
                }

                if ((*g_player)->actorValueOwner.GetValue(g_Data.CurrentLevel) == 0)
                    (*g_player)->actorValueOwner.SetBase(g_Data.CurrentLevel, 1);

                g_GameSettings.Init();
                Skills::RegisterSkills();
                Perks::BuildList();

                _LOGMESSAGE("F4SEMessageHandler() Time: %fms", DataLoader.Format(ITimeKeeper::Milli));
            }
            else {
                _LOGMESSAGE("Data should be unloaded.");
                g_Data = DataManager(); // Okay?
            }

            s_LoadLock.Leave();
        }
    }

    bool F4SEPlugin_Query(const F4SEInterface* F4SE, PluginInfo* Info) {
        ITimeKeeper startupClock = ITimeKeeper();
        startupClock.Start();

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

        g_ObScript.Init();

        if (!g_branchTrampoline.Create(1024 * 64)) {
            _LOGERROR("Failed to create Branch Trampoline. Plugin will be disabled.");
            return false;
        }

        if (!g_localTrampoline.Create(1024 * 64, nullptr)) {
            _LOGERROR("Failed to create CodeGen Buffer. Plugin will be disabled.");
            return false;
        }

        _LOGMESSAGE("F4SEPlugin_Query Time: %fms", startupClock.Format(ITimeKeeper::Milli));
        return true;
    }

    bool F4SEPlugin_Load(const F4SEInterface* F4SE) {
        if (!g_Papyrus->Register(Papyrus::RegisterFunctions)) {
            _LOGWARNING("Failed to register Papyrus functions");
            return false;
        }

        if (!g_Messaging->RegisterListener(g_PluginHandle, "F4SE", F4SEMessageHandler)) {
            _LOGWARNING("Failed to register for Messaging listener");
            return false;
        }

        if (!g_Scaleform->Register("RTNG", Scaleform::RegisterFunctions)) {
            _LOGWARNING("Failed to register for Scaleform interface");
            return false;
        }

        if (!Serialization::Hook_Commit(g_PluginHandle)) {
            _LOGWARNING("Failed to commit Serialization interface.");
            return false;
        }

        if (!ObScript::Commit()) {
            _LOGWARNING("Failed to commit ObScript functions.");
            return false;
        }

        // Hook "Hold [Pipboy] to open Perks menu." to detect player level ups.
        g_branchTrampoline.Write6Branch(LevelUpPrompt.GetUIntPtr(), (uintptr_t)Perks::LUPrompt_Hook);

        // Hook DR Calc to replace it with something sane
        g_branchTrampoline.Write6Branch(CalculateDamageResist.GetUIntPtr(), (uintptr_t)Calculate::DamageResist);

        _LOGMESSAGE("Plugin loaded successfully.");
        return true;
    }
}
