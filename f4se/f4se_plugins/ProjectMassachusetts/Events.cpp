#include "Events.h"
#include "Condition.h"
#include "Values.h"

EventResult TESContainerChangedEventHandler::ReceiveEvent(TESContainerChangedEvent* evn, void* dispatcher) {
    if (evn) {
        if (evn->targetID == 0x00000000 || evn->refID != 0x00000000)
            return kEvent_Continue;

        TESObjectREFR* refr = LookupTypeByID(evn->targetID, TESObjectREFR);
        if (!refr)
            return kEvent_Continue;

        switch (refr->baseForm->formType) {
        case kFormType_ACHR:
        case kFormType_CONT:
        case kFormType_NPC_:
        case kFormType_REFR: {
            TESForm* Form = LookupFormByID(evn->formID);
            switch (Form->formType) {
            case kFormType_ARMO: {
                break;
            }

            case kFormType_WEAP:
                InitializeInventoryItemCondition(refr, Form);
                break;

            default:
                break;
            }

            break;
        }

        default:
            break;
        }
    }

    return kEvent_Continue;
}
TESContainerChangedEventHandler g_TESContainerChangedEventHandler;

EventResult TESInitScriptEventHandler::ReceiveEvent(TESInitScriptEvent* evn, void* dispatcher) {
    if (evn) {
        if (evn->reference) {
            switch (evn->reference->baseForm->formType) {
            case kFormType_ACHR:
            case kFormType_NPC_:
            case kFormType_REFR:
                ActorValueUpdateThread(evn->reference);

            case kFormType_CONT:
                InitializeInventoryCondition(evn->reference);
                break;

            case kFormType_AMMO:
                break;

            case kFormType_ARMO:
                break;

            case kFormType_WEAP:
                InitializeWeaponCondition(evn->reference);
                break;

            case kFormType_ACTI:
            case kFormType_ALCH:
            case kFormType_ASPC:
            case kFormType_BOOK:
            case kFormType_BNDS:
            case kFormType_DOOR:
            case kFormType_EXPL:
            case kFormType_FLOR:
            case kFormType_FURN:
            case kFormType_HAZD:
            case kFormType_IDLM:
            case kFormType_LIGH:
            case kFormType_KEYM:
            case kFormType_MISC:
            case kFormType_MSTT:
            case kFormType_NOTE:
            case kFormType_PGRE:
            case kFormType_SCOL:
            case kFormType_SOUN:
            case kFormType_STAT:
            case kFormType_TACT:
            case kFormType_TERM:
                break;

            default:
                _LogWarningNT("Unhandled FormType: %i", evn->reference->baseForm->formType);
                break;
            }
        }
    }

    return kEvent_Continue;
}
TESInitScriptEventHandler g_TESInitScriptEventHandler;

EventResult TESSleepStartEventHandler::ReceiveEvent(TESSleepStartEvent* evn, void* dispatcher) {
    if (evn) {
        // Console_Print("TESSleepStartEvent: %0.2f, %0.2f, 0x%08X", evn->startTime, evn->desiredStopTime, evn->furnitureRef->formID);
    }

    return kEvent_Continue;
}
TESSleepStartEventHandler g_TESSleepStartEventHandler;

EventResult TESSleepStopEventHandler::ReceiveEvent(TESSleepStopEvent* evn, void* dispatcher) {
    if (evn) {
        // Console_Print("TESSleepStopEvent");
    }

    return kEvent_Continue;
}
TESSleepStopEventHandler g_TESSleepStopEventHandler;

EventResult TESWaitStartEventHandler::ReceiveEvent(TESWaitStartEvent* evn, void* dispatcher) {
    if (evn) {
        // Console_Print("TESWaitStartEvent: %0.2f, %0.2f", evn->startTime, evn->desiredStopTime);
    }

    return kEvent_Continue;
}
TESWaitStartEventHandler g_TESWaitStartEventHandler;

EventResult TESWaitStopEventHandler::ReceiveEvent(TESWaitStopEvent* evn, void* dispatcher) {
    if (evn) {
        // Console_Print("TESWaitStopEvent");
    }

    return kEvent_Continue;
}
TESWaitStopEventHandler g_TESWaitStopEventHandler;

EventResult CurrentRadiationSourceCountHandler::ReceiveEvent(CurrentRadiationSourceCount* evn, void* dispatcher) {
    if (evn) {
        SetValue((*g_player), Forms::RadiationSourceCount, evn->count);
    }

    return kEvent_Continue;
}
CurrentRadiationSourceCountHandler g_CurrentRadiationSourceCountHandler;

EventResult PipboyLightEventHandler::ReceiveEvent(PipboyLightEvent* evn, void* dispatcher) {
    if (evn) {
        SetValue((*g_player), Forms::PipboyLightEnabled, evn->enabled);
    }

    return kEvent_Continue;
}
PipboyLightEventHandler g_PipboyLightEventHandler;

EventResult WeaponFiredEventHandler::ReceiveEvent(WeaponFiredEvent* evn, void* dispatcher) {
    if (evn) {
        if (evn->refr) {
            Actor* actor = DYNAMIC_CAST(evn->refr, TESObjectREFR, Actor);
            ModWeaponCondition(actor);
        }
    }

    return kEvent_Continue;
}
WeaponFiredEventHandler g_WeaponFiredEventHandler;