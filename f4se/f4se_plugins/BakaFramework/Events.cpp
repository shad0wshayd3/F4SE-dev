#include "Events.h"

#include "f4se/GameReferences.h"

void WeightUpdate::Run() {
    SetBaseValue(m_refr, Forms::InventoryWeight, CALL_MEMBER_FN(m_refr, GetInventoryWeight)());
}

EventResult MenuOpenCloseEventHandler::ReceiveEvent(MenuOpenCloseEvent* evn, void* dispatcher) {
    if (!evn)
        return kEvent_Continue;

    if (evn->menuName == BSFixedString("HUDMenu")) {
        if (evn->isOpen) {
            IMenu* menu = (*g_ui)->GetMenu(evn->menuName);
            if (!menu)
                return kEvent_Continue;

            GameLoadedCallback();
        }
    }

    return kEvent_Continue;
}
MenuOpenCloseEventHandler g_MenuOpenCloseEventHandler;

EventResult RadiationSourceCountHandler::ReceiveEvent(RadiationSourceCount* evn, void* dispatcher) {
    if (evn) {
        SetBaseValue((*g_player), Forms::RadiationSourceCount, evn->count);
    }

    return kEvent_Continue;
}
RadiationSourceCountHandler g_RadiationSourceCountHandler;

EventResult PipboyLightEventHandler::ReceiveEvent(PipboyLightEvent* evn, void* dispatcher) {
    if (evn) {
        SetBaseValue((*g_player), Forms::PipboyLightActive, evn->enabled);
    }

    return kEvent_Continue;
}
PipboyLightEventHandler g_PipboyLightEventHandler;

EventResult TESContainerChangedEventHandler::ReceiveEvent(TESContainerChangedEvent* evn, void* dispatcher) {
    if (evn) {
        if (g_Tasks) {
            Actor* source = ToType(LookupFormByID(evn->sourceID), Actor);
            Actor* target = ToType(LookupFormByID(evn->targetID), Actor);

            if (source && source->formType == kFormType_ACHR)
                g_Tasks->AddTask(new WeightUpdate(source));

            if (target && target->formType == kFormType_ACHR)
                g_Tasks->AddTask(new WeightUpdate(target));
        }
    }

    return kEvent_Continue;
}
TESContainerChangedEventHandler g_TESContainerChangedEventHandler;
