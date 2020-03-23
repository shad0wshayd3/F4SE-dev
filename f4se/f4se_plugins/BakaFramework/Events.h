#pragma once

#include "Forms.h"
#include "f4se_globals/EventDefinitions.h"

class WeightUpdate: public ITaskDelegate {
public:
    WeightUpdate(TESObjectREFR* refr) {
        m_refr = refr;
    }

    virtual ~WeightUpdate() {
        m_refr = nullptr;
    }

    virtual void Run() override;

protected:
    TESObjectREFR* m_refr;
};

DECLARE_EVENT_CLASS(MenuOpenCloseEvent);
DECLARE_EVENT_CLASS(RadiationSourceCount);
DECLARE_EVENT_CLASS(PipboyLightEvent);
DECLARE_EVENT_CLASS(TESContainerChangedEvent);
