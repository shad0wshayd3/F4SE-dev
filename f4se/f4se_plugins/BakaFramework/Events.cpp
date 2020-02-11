#include "Events.h"

#include <thread>

#include "f4se/GameReferences.h"
#include "f4se_globals/Utilities.h"

void UpdateWeight() {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));    // Delay for dropped items
    SetValue((*g_player), Forms::InventoryWeight, CALL_MEMBER_FN((*g_player), GetInventoryWeight)());
}

EventResult CurrentRadiationSourceCountHandler::ReceiveEvent(CurrentRadiationSourceCount* evn, void* dispatcher) {
    if (evn) {
        SetValue((*g_player), Forms::RadiationSourceCount, evn->count);
    }

    return kEvent_Continue;
}
CurrentRadiationSourceCountHandler g_CurrentRadiationSourceCountHandler;

EventResult PipboyLightEventHandler::ReceiveEvent(PipboyLightEvent* evn, void* dispatcher) {
    if (evn) {
        SetValue((*g_player), Forms::PipboyLightActive, evn->enabled);
    }

    return kEvent_Continue;
}
PipboyLightEventHandler g_PipboyLightEventHandler;

EventResult TESContainerChangedEventHandler::ReceiveEvent(TESContainerChangedEvent* evn, void* dispatcher) {
    if (evn) {
        if ((evn->sourceID == 0x14) || (evn->targetID == 0x14)) {
            std::thread UpdateInventoryWeightThread(UpdateWeight);
            UpdateInventoryWeightThread.detach();
        }
    }

    return kEvent_Continue;
}
TESContainerChangedEventHandler g_TESContainerChangedEventHandler;

EventResult MenuOpenCloseEventHandler::ReceiveEvent(MenuOpenCloseEvent* evn, void* dispatcher) {
    if (!evn)
        return kEvent_Continue;

    if (evn->menuName == BSFixedString("HUDMenu")) {
        if (evn->isOpen) {
            GetGlobalEventDispatcher<CurrentRadiationSourceCount>().AddEventSink(&g_CurrentRadiationSourceCountHandler);
            GetGlobalEventDispatcher<PipboyLightEvent>().AddEventSink(&g_PipboyLightEventHandler);
        }
    }

    return kEvent_Continue;
}
MenuOpenCloseEventHandler g_MenuOpenCloseEventHandler;