#include "Forms.h"

#include "Events.h"
#include "ObScript.h"
#include "Papyrus.h"
#include "Scaleform.h"
#include "Seralize.h"
#include "Values.h"

#include "f4se_common/BranchTrampoline.h"
#include "common/ICriticalSection.h"

PluginHandle g_PluginHandle = kPluginHandle_Invalid;
F4SEPapyrusInterface* g_Papyrus = nullptr;
F4SEMessagingInterface* g_Messaging = nullptr;
F4SEScaleformInterface* g_Scaleform = nullptr;
F4SESerializationInterface* g_Serialization = nullptr;
ICriticalSection s_DataLock;

static IMenu* CreateBlurMenu() {
    return new Menus::BlurMenu();
}

static IMenu* CreateRepairMenu() {
    return new Menus::RepairMenu();
}

extern "C" {
void F4SEMessageHandler(F4SEMessagingInterface::Message* msg) {
    switch (msg->type) {
    case F4SEMessagingInterface::kMessage_GameDataReady:
        s_DataLock.Enter();

        if (reinterpret_cast<bool>(msg->data)) {
            if (!Forms::Load()) {
                s_DataLock.Leave();
                HALT("Game Plugin error.");
            }

            // Register New Menus
            _DebugMessage("Registering Menus...");
            if ((*g_ui) != nullptr) {
                (*g_ui)->menuOpenCloseEventSource.AddEventSink(&g_MenuOpenCloseEventHandler);

                if (!(*g_ui)->menuTable.Find(&BSFixedString("BlurMenu")))
                    (*g_ui)->Register("BlurMenu", CreateBlurMenu);

                if (!(*g_ui)->menuTable.Find(&BSFixedString("RepairMenu")))
                    (*g_ui)->Register("RepairMenu", CreateRepairMenu);
            }

            // Register Events
            _DebugMessage("Registering Events...");
            GetEventDispatcher<TESContainerChangedEvent>()->AddEventSink(&g_TESContainerChangedEventHandler);
            GetEventDispatcher<TESInitScriptEvent>()->AddEventSink(&g_TESInitScriptEventHandler);
            GetEventDispatcher<TESSleepStartEvent>()->AddEventSink(&g_TESSleepStartEventHandler);
            GetEventDispatcher<TESSleepStopEvent>()->AddEventSink(&g_TESSleepStopEventHandler);
            GetEventDispatcher<TESWaitStartEvent>()->AddEventSink(&g_TESWaitStartEventHandler);
            GetEventDispatcher<TESWaitStopEvent>()->AddEventSink(&g_TESWaitStopEventHandler);

            //GetGlobalEventDispatcher<CurrentRadiationSourceCount>   ().AddEventSink(&g_CurrentRadiationSourceCountHandler);
            //GetGlobalEventDispatcher<PipboyLightEvent>              ().AddEventSink(&g_PipboyLightEventHandler);

            GetSingletonEventDispatcher(WeaponFiredEvent).AddEventSink(&g_WeaponFiredEventHandler);

            // Register Values
            _DebugMessage("Registering Values...");
            Values::RegisterValues();

            _DebugMessage("Finished Registration.");
        }

        else {
            Forms::Unload();
        }

        s_DataLock.Leave();
        break;

    default:
        break;
    }
}

bool F4SEPlugin_Query(const F4SEInterface* F4SE, PluginInfo* Info) {
    InitializePlugin(PLUGIN_NAME, CONFIG_FILE_NAME);
    _LogLevel(ISettings::GetInteger("General:LogLevel", 0));

    _LogMessage("%s log opened (PC-64)", PLUGIN_NAME);
    _LogMessage("This is a plugin log only and does not contain information on any other part of the game, including crashes.");
    _DebugMessage("Debug output enabled.");

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

    g_Messaging = (F4SEMessagingInterface*)F4SE->QueryInterface(kInterface_Messaging);
    if (!g_Messaging) {
        _LogError("Failed to get Messaging interface. Plugin will be disabled.");
        return false;
    }

    g_Scaleform = (F4SEScaleformInterface*)F4SE->QueryInterface(kInterface_Scaleform);
    if (!g_Scaleform) {
        _LogError("Failed to get Scaleform interface. Plugin will be disabled.");
        return false;
    }

    g_Papyrus = (F4SEPapyrusInterface*)F4SE->QueryInterface(kInterface_Papyrus);
    if (!g_Papyrus) {
        _LogError("Failed to get Papyrus interface. Plugin will be disabled.");
        return false;
    }

    g_Serialization = (F4SESerializationInterface*)F4SE->QueryInterface(kInterface_Serialization);
    if (!g_Serialization) {
        _LogError("Failed to get Serialization interface. Plugin will be disabled.");
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

    if (!Serialize::Commit()) {
        _LogWarning("Failed to commit Serialization interface.");
        return false;
    }

    if (!ObScript::Init()) {
        _LogWarning("Failed to register ObScript script functions.");
        return false;
    }

    _DebugMessage("Hooking Functions...");
    HookContainerMenuInvoke(ContainerMenuInvoke_Hook);
    HookExamineMenuInvoke(ExamineMenuInvoke_Hook);
    HookPipboyMenuInvoke(PipboyMenuInvoke_Hook);
    HookPopulateItemCard(PopulateItemCard_Hook);

    g_branchTrampoline.Write6Branch(CalculateDamageResist.GetUIntPtr(), (uintptr_t)DamageResistFormula);
    g_branchTrampoline.Write6Branch(LevelUpPrompt.GetUIntPtr(), (uintptr_t)LevelUpMenuPrompt_Hook);

    _LogMessage("Plugin loaded successfully.");
    return true;
}
}