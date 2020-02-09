#pragma once

#include "f4se/bhkWorld.h"
#include "f4se/GameObjects.h"

struct ActorEquipManagerEvent {
    struct Event {
        //
    };
};

struct ActorValueEvents {
    struct ActorValueChangedEvent {
        //
    };
};

struct BGSActorCellEvent {
    //
};

struct BGSInventoryItemEvent {
    struct Event {
        UInt32 ownerRefHandle;
        UInt32 itemHandleID;
    };
};

struct CurrentRadiationSourceCount {
    UInt32 count;
};

struct CustomMarkerUpdate {
    struct Event {
        //
    };
};

struct FavoriteMgr_Events {
    struct ComponentFavoriteEvent {
        BGSComponent* component;
        UInt8 taggedForSearch;
    };
};

struct HolotapeStateChanged {
    struct Event {
        //
    };
};

struct HourPassed {
    struct Event {
        //
    };
};

struct InvInterfaceStateChangeEvent {
    //
};

struct LevelIncrease {
    struct Event {
        UInt32 gainedLevel;
        UInt32 pad04;
        Actor* actor;
        UInt32 unk10;
        UInt32 unk14;
        float gainedExp;
        UInt32 unk1C[3];
        Actor* actor1;
        void* unk30;
        Actor* actor2;
        UInt32 fromLevel;
        UInt32 pad44;
    };
};

struct LoadingStatusChanged {
    struct Event {
        //
    };
};

struct LocalMapCameraUpdate {
    struct Event {
        //
    };
};

struct LocationMarkerArrayUpdate {
    struct Event {
        //
    };
};

struct PipboyLightEvent {
    UInt8 enabled;
};

struct PerkPointIncreaseEvent {
    //
};

struct PerkValueEvents {
    struct PerkEntryUpdatedEvent {
        //
    };

    struct PerkValueChangedEvent {
        //
    };
};

struct PlayerActiveEffectChanged {
    struct Event {
        //
    };
};

struct PlayerAmmoCountEvent {
    UInt32 ammoCount;
    UInt32 ammoReserves;
    UInt64 unk08;
    TESObjectWEAP* weapon;
    TESObjectWEAP::InstanceData* weaponData;
};

struct PlayerCharacterQuestEvent {
    struct Event {
        //
    };
};

struct PlayerDifficultySettingChanged {
    struct Event {
        UInt32 From;
        UInt32 To;
    };
};

struct PlayerInDialogueChanged {
    struct Event {
        //
    };
};

struct PlayerLifeStateChanged {
    struct Event {
        //
    };
};

struct PlayerWeaponReloadEvent {
    //
};

struct PlayerUpdateEvent {
    //
};

struct RadioManager {
    struct PipboyFrequencyDetectionEvent {
        //
    };

    struct PipboyRadioTuningEvent {
        //
    };
};

struct SPECIALMenuEvent {
    struct NameChangedEvent {
        //
    };
};

struct TESActivateEvent {
    //
};

struct TESBookReadEvent {
    //
};

struct TESCellAttachDetachEvent {
    //
};

struct TESCellFullyLoadedEvent {
    //
};

struct TESContainerChangedEvent {
    UInt32 sourceID;
    UInt32 targetID;
    UInt32 formID;
    UInt32 count;
    UInt32 refID;
    void* unk14;
};

struct TESEnterSneakingEvent {
    //
};

struct TESEquipEvent {
    //
};

struct TESGrabReleaseEvent {
    //
};

// struct TESHitEvent {};
// Defined in f4se/GameEvents.h

struct TESLimbCrippleEvent {
    //
};

struct TESLocationClearedEvent {
    //
};

struct TESLockChangedEvent {
    //
};

struct TESMagicEffectApplyEvent {
    //
};

struct TESQuestEvent {
    struct Event {
        //
    };
};

struct TESSleepStartEvent {
    float startTime;
    float desiredStopTime;
    TESObjectREFR* furnitureRef;
};

struct TESSleepStopEvent {
    bool interrupted;
};

struct TESWaitStartEvent {
    float startTime;
    float desiredStopTime;
};

struct TESWaitStopEvent {
    bool interrupted;
};

struct TravelMarkerStateChange {
    struct Event {
        //
    };
};

struct VATSEvents {
    struct ModeChange {
        //
    };
};

struct WeaponFiredEvent {
    void* unk00;
    TESObjectREFR* refr;
    void* unk10;
};

struct Workshop {
    struct WorkshopModeEvent {
        TESObjectREFR* WorkshopRef;
        bool start;
    };

    struct PowerOnEvent {
        TESObjectREFR* ref;
    };

    struct PowerOffEvent {
        TESObjectREFR* ref;
    };

    struct ItemGrabbedEvent {
        TESObjectREFR* workshopRef;
        TESObjectREFR* objectRef;
    };

    struct ItemRepairedEvent {
        TESObjectREFR* workshopRef;
        TESObjectREFR* objectRef;
    };

    struct ItemPlacedEvent {
        TESObjectREFR* workshopRef;
        TESObjectREFR* objectRef;
        NiTransform transform;
    };

    struct ItemMovedEvent {
        TESObjectREFR* workshopRef;
        TESObjectREFR* objectRef;
        UInt32 unk10[(0x50 - 0x10) / 4];
        NiPoint3 position;
    };

    struct ItemDestroyedEvent {
        TESObjectREFR* workshopRef;
        TESObjectREFR* objectRef;
    };

    struct BuildableAreaEvent {
        UInt8 leaveArea;
    };

    struct ItemScrappedEvent {
        void* unk00;

        struct Component {
            TESObjectMISC* component;
            UInt32 count;
            UInt32 pad0C;
        };
        tArray<Component>* components;
    };

    struct PlacementStatusEvent {
        UInt32 flags;
        float unk04;
        bhkWorldM* _bhkWorldM;
        float unk10[8];
        TESObjectREFR* objectRef;
        UInt64 unk40;
        TESObjectREFR* workshopRef;
        float unk50[10];
        NiTransform transform;
    };
};

template<typename EventT>
BSTEventDispatcher<EventT> GetGlobalEventDispatcher(){};

template<typename EventT>
BSTEventDispatcher<EventT>* GetSingletonEventDispatcher_Internal(){};

#define DECLARE_GLOBAL_EVENT_DISPATCHER(Event, address)                                                                                                                                                \
    template<>                                                                                                                                                                                         \
    inline BSTEventDispatcher<Event> GetGlobalEventDispatcher() {                                                                                                                                      \
        typedef BSTGlobalEvent::EventSource<Event>*(_GetGlobalEventDispatcher);                                                                                                                        \
        RelocPtr<_GetGlobalEventDispatcher> GetDispatcher(address);                                                                                                                                    \
        return (*GetDispatcher)->eventDispatcher;                                                                                                                                                      \
    }

#define DECLARE_SINGLETON_EVENT_DISPATCHER(Event, address)                                                                                                                                             \
    template<>                                                                                                                                                                                         \
    inline BSTEventDispatcher<Event>* GetSingletonEventDispatcher_Internal() {                                                                                                                         \
        typedef BSTEventDispatcher<Event>*(_GetSingletonEventDispatcher_Internal);                                                                                                                     \
        RelocAddr<_GetSingletonEventDispatcher_Internal> GetDispatcher(address);                                                                                                                       \
        return GetDispatcher;                                                                                                                                                                          \
    }

#define DECLARE_EVENT_CLASS(Event)                                                                                                                                                                     \
    class Event##Handler: public BSTEventSink<Event> {                                                                                                                                                 \
    public:                                                                                                                                                                                            \
        virtual ~Event##Handler(){};                                                                                                                                                                   \
        virtual EventResult ReceiveEvent(Event* evn, void* dispatcher) override;                                                                                                                       \
    };                                                                                                                                                                                                 \
    extern Event##Handler g_##Event##Handler

#define GetSingletonEventDispatcher(Event) (*GetSingletonEventDispatcher_Internal<Event>())