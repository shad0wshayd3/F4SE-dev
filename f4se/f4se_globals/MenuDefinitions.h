#pragma once

#include "EventDefinitions.h"
#include "f4se/GameMenus.h"

template<typename T>
struct SimpleCollector {
    UInt64              unk00;
    tArray<T>           itemsArray;
};

struct InvItemStack {
public:
    BGSInventoryItem*   Item;
    UInt16              StackID;
};

class InventoryInterface {
public:
    struct CountChangedEvent {
        //
    };

    struct FavoriteChangedEvent {
        //
    };

    void*                                       unk00;
    BSTEventDispatcher<CountChangedEvent>       countChangedEventDispatcher;
    BSTEventDispatcher<FavoriteChangedEvent>    favoriteChangedEventDispatcher;

    struct Entry {
    public:
        UInt32                                  handleID;
        UInt32                                  ownerHandle;
        UInt16                                  itemPosition;
        UInt16                                  unk0A;
    };
    tArray<Entry>                               inventoryItems;
};

struct ObjectInstanceData {
    ObjectInstanceData() : Form(nullptr), InstanceData(nullptr) { }
    ObjectInstanceData(TESForm* Form) : Form(Form), InstanceData(nullptr) { }

    TESForm*            Form;
    TBO_InstanceData*   InstanceData;
};

// ------------------------------------------------------------------------------------------------
// Base Menu Classes
// ------------------------------------------------------------------------------------------------

class ContainerMenuBase : public GameMenuBase {
public:
    enum BGSCodeObj {
        kFunction_PlaySound                         = 0x00,
        kFunction_TransferItem                      = 0x01,
        kFunction_OnIntroAnimComplete               = 0x02,
        kFunction_Show3D                            = 0x03,
        kFunction_ExitMenu                          = 0x04,
        kFunction_TakeAllItems                      = 0x05,
        kFunction_GetItemValue                      = 0x06,
        kFunction_UpdateItemPickpocketInfo          = 0x07,
        kFunction_GetSelectedItemEquippable         = 0x08,
        kFunction_GetSelectedItemEquipped           = 0x09,
        kFunction_ToggleSelectedItemEquipped        = 0x0A,
        kFunction_ConfirmInvest                     = 0x0B,
        kFunction_SendXButton                       = 0x0C,
        kFunction_SendYButton                       = 0x0D,
        kFunction_UpdateSortButtonLabel             = 0x0F,
        kFunction_SortItems                         = 0x10,
        kFunction_InspectItem                       = 0x11,
    };

    struct UnkInventoryStruct {
        UInt32                                  HandleID;
        UInt32                                  pad0c;
        UInt32                                  unk08;                          // if unk08 & 0x80000000 then unk10 = stackID else *stackID
        UInt32                                  unk0c;
        UInt16                                  stackid;                        // ?
        UInt16                                  pad12[3];
        UInt32                                  unk18;
        UInt32                                  unk1c;
    };

    BSTEventSink<InvInterfaceStateChangeEvent>  eventSink1;                     // E0
    BSTEventSink<MenuOpenCloseEvent>            eventSink2;                     // E8
    UInt64                                      unkF0[(0x200 - 0xF0) / 8];      // F0
    tArray<UnkInventoryStruct>                  playerItems;                    // 200
    UInt64                                      unk218[(0x280 - 0x218) / 8];    // 218
    tArray<UnkInventoryStruct>                  contItems;                      // 280
    UInt64                                      unk298[(0x428 - 0x298) / 8];    // 298
    UInt32                                      targetHandle;                   // 428
    UInt32                                      unk42c;                         // 42c
};

class ExamineMenu : public GameMenuBase {
public:
    enum BGSCodeObj {
        kFunction_RegisterComponents                = 0x00,
        kFunction_PlaySound                         = 0x01,
        kFunction_StartAnimation                    = 0x02,
        kFunction_ConfirmBuild                      = 0x03, 
        kFunction_StartBuildConfirm                 = 0x04,
        kFunction_CancelConfirm                     = 0x05,
        kFunction_HideMenu                          = 0x06,
        kFunction_SendTutorialEvent                 = 0x07,
        kFunction_OnAlternateButton                 = 0x08,
        kFunction_RegisterRequirementList           = 0x09,
        kFunction_UpdateItemSelectList              = 0x0A,
        kFunction_ShowItem                          = 0x0C,
        kFunction_FillModPartArray                  = 0x0D,
        kFunction_SwitchMod                         = 0x0E,
        kFunction_ItemSelect                        = 0x0F,
        kFunction_RequestItemSelectListData         = 0x10,
        kFunction_StartItemSelection                = 0x11,
        kFunction_SetName                           = 0x12,
        kFunction_RevertChanges                     = 0x13,
        kFunction_CheckRequirements                 = 0x14,
        kFunction_HasNullMod                        = 0x15,
        kFunction_UpdateRequirements                = 0x16,
        kFunction_SwitchBaseItem                    = 0x17,
        kFunction_ToggleFavoriteMod                 = 0x18,
        kFunction_ShouldShowTagForSearchButton      = 0x19,
        kFunction_ToggleItemEquipped                = 0x1A,
        kFunction_RepairSelectedItem                = 0x1B,
        kFunction_CanRepairSelectedItem             = 0x1C,
        kFunction_IsSelectedItemEquipped            = 0x1D,
        kFunction_ScrapItem                         = 0x1E,
        kFunction_SetItemSelectValuesForComponents  = 0x1F,
        kFunction_OnBuildFailed                     = 0x20,
        kFunction_StartRotate3DItem                 = 0x21,
        kFunction_EndRotate3DItem                   = 0x22,
        kFunction_RemoveHighlight                   = 0x23,
        kFunction_ZoomIn                            = 0x24,
        kFunction_ZoomOut                           = 0x25,
    };
};

class PipboyMenu : public GameMenuBase {
public:
    enum BGSCodeObj {
        kFunction_PlaySound                         = 0x00,
        kFunction_PlaySmallTransition               = 0x01,
        kFunction_PopulatePipboyInfoObj             = 0x02,
        kFunction_OnNewPage                         = 0x03,
        kFunction_OnNewTab                          = 0x04,
        kFunction_UseStimpak                        = 0x05,
        kFunction_UseRadaway                        = 0x06,
        kFunction_ShowPerksMenu                     = 0x07,
        kFunction_PlayPerkSound                     = 0x08,
        kFunction_StopPerkSound                     = 0x09,
        kFunction_OnPerksTabOpen                    = 0x0A,
        kFunction_OnPerksTabClose                   = 0x0B,
        kFunction_SelectItem                        = 0x0C,
        kFunction_OnInvItemSelection                = 0x0D,
        kFunction_UpdateItem3D                      = 0x0E,
        kFunction_SetQuickkey                       = 0x0F,
        kFunction_ItemDrop                          = 0x10,
        kFunction_SortItemList                      = 0x11,
        kFunction_ExamineItem                       = 0x12,
        kFunction_OnComponentViewToggle             = 0x13,
        kFunction_ToggleComponentFavorite           = 0x14,
        kFunction_SetQuestActive                    = 0x15,
        kFunction_OnQuestSelection                  = 0x16,
        kFunction_ShowQuestOnMap                    = 0x17,
        kFunction_ShowWorkshopOnMap                 = 0x18,
        kFunction_RegisterMap                       = 0x19,
        kFunction_UnregisterMap                     = 0x1A,
        kFunction_FastTravel                        = 0x1D,
        kFunction_CheckHardcoreModeFastTravel       = 0x1E,
        kFunction_RefreshMapMarkers                 = 0x1F,
        kFunction_HasSetPlayerMarkerRequest         = 0x20,
        kFunction_SetPlayerMarker                   = 0x21,
        kFunction_ClearPlayerMarker                 = 0x22,
        kFunction_OnSwitchBetweenWorldLocalMap      = 0x23,
        kFunction_CenterMarkerRequest               = 0x24,
        kFunction_OnModalOpen                       = 0x25,
        kFunction_ToggleRadioStationActiveStatus    = 0x26,
        kFunction_OnShowHotKeys                     = 0x27,
        kFunction_ToggleMovementToDirectional       = 0x28,
    };

	enum Pages {
		kPage_Stats			= 0,
		kPage_Inventory		= 1,
		kPage_Data			= 2,
		kPage_Map			= 3,
		kPage_Radio			= 4,
	};

	enum Tabs {
		kTab_Stats_Status	= 0,
		kTab_Stats_SPECIAL	= 1,
		kTab_Stats_Skills	= 2,
		kTab_Stats_Perks	= 3,

		kTab_Inv_Weapons	= 0,
		kTab_Inv_Apparel	= 1,
		kTab_Inv_Aid		= 2,
		kTab_Inv_Misc		= 3,
		kTab_Inv_Junk		= 4,
		kTab_Inv_Mods		= 5,
		kTab_Inv_Ammo		= 6,
		kTab_Inv_Keys		= 7,

		kTab_Data_Quests	= 0,
		kTab_Data_Notes		= 1,
		kTab_Data_Workshops	= 2,
		kTab_Data_Stats		= 3,
	};

    enum Filters {
        kFilter_Weapons		= 0x00000002,	// kFormType_WEAP
        kFilter_Apparel		= 0x00000004,   // kFormType_ARMO
        kFilter_Aid			= 0x00000008,   // kFormType_ALCH || kFormType_INGR
        kFilter_Misc		= 0x00000200,   // kFormType_MISC && components->count = 0 || kFormType_KEYM
        kFilter_BOOK		= 0x00000280,   // kFormType_BOOK
        kFilter_Junk		= 0x00000400,   // kFormType_MISC && components->count > 0
        kFilter_Mods		= 0x00000800,   // kFormType_MISC && Referenced by OMOD?
        kFilter_Ammo		= 0x00001000,   // kFormType_AMMO
        kFilter_NOTE		= 0x00002200,   // kFormType_NOTE

		kFilter_Keys		= 0x00008000,	// PM Keys Tab
		kFilter_Notes		= 0x00016000,	// PM Notes Tab
    };
};

// ------------------------------------------------------------------------------------------------
// Pipboy Scaleform Classes
// ------------------------------------------------------------------------------------------------

class PipboyArray : public PipboyValue {
public:
    tArray<PipboyValue*>                                        value;                  // 18
    void*                                                       unk30;
    void*                                                       unk38;
    void*                                                       unk40;
    void*                                                       unk48;
    void*                                                       unk50;
    void*                                                       unk58;
    tArray<void*>                                               unk60;
    UInt8                                                       unk78;                  // init'd as 1
};

class PipboyDataGroup {
public:
    virtual                                                     ~PipboyDataGroup();

    virtual void                                                Unk_01();
    virtual void                                                Unk_02();
    virtual void                                                Unk_03();

    void*                                                       unk08;
    tArray<void*>                                               arr1;
    tArray<void*>                                               arr2;
    tArray<void*>                                               arr3;
    void*                                                       unk58;
    CRITICAL_SECTION                                            lpcs;                   // 60
    void*                                                       unk88;
    PipboyObject*                                               object;                 // 0x90
};

class PipboyStatsData : public PipboyDataGroup {
public:
    void*                                                       unk98[20];
};

class PipboySpecialData : public PipboyDataGroup {
public:
    void*                                                       unk98[2];
};

class PipboyPerksData : public PipboyDataGroup {
public:
    void*                                                       unk98[8];
};

class PipboyInventoryData : public PipboyDataGroup {
public:
    BSTEventSink<BGSInventoryListEvent::Event>                  es1;                    // 98
    BSTEventSink<ActorEquipManagerEvent::Event>                 es2;                    // A0
    BSTEventSink<ActorValueEvents::ActorValueChangedEvent>      es3;                    // A8
    BSTEventSink<PerkValueEvents::PerkEntryUpdatedEvent>        es4;                    // B0
    BSTEventSink<PerkValueEvents::PerkValueChangedEvent>        es5;                    // B8
    BSTEventSink<InventoryInterface::FavoriteChangedEvent>      es6;                    // C0
    BSTEventSink<HolotapeStateChanged::Event>                   es7;                    // C8
    BSTEventSink<BGSInventoryItemEvent::Event>                  es8;                    // D0
    BSTEventSink<FavoriteMgr_Events::ComponentFavoriteEvent>    es9;                    // D8
    BSTEventSink<PlayerDifficultySettingChanged::Event>         es10;                   // E0
    PipboyObject*                                               inventoryObject;        // E8

    // 10
    struct HSF0Entry {
        UInt32                                                  formid;
        void**                                                  unk08;
    };
    tHashSet<HSF0Entry, UInt32>                                 hsF0;                   // something related to sorting?

    // 10
    struct HS120Entry {
        BGSComponent*                                           component;
        PipboyObject*                                           object;

        operator BGSComponent* () const { return component; }
    };
    tHashSet<HS120Entry, BGSComponent*>                         hs120;

    tArray<PipboyObject*>                                       inventoryObjects;       // 150
    tHashSet<UInt32>                                            hs168;                  // contain FormTypes for update?
};

class PipboyQuestData : public PipboyDataGroup {
public:
    void*                                                       unk98[10];
};

class PipboyWorkshopData : public PipboyDataGroup {
public:
    BSTEventSink<ActorValueEvents::ActorValueChangedEvent>      es1;
    BSTEventSink<Workshop::WorkshopModeEvent>                   es2;
    PipboyArray*                                                unkPA_A8;
    tHashSet<UInt32, UInt32>                                    unkHS_B0;
    tHashSet<UInt32, UInt32>                                    unkHS_E0;
    tHashSet<UInt32, UInt32>                                    unkHS_110;
};

class PipboyLogData : public PipboyDataGroup {
public:
    void*                                                       unk98[8];
};

class PipboyMapData : public PipboyDataGroup {
public:
    BSTEventSink<TravelMarkerStateChange::Event>                es1;
    BSTEventSink<PlayerUpdateEvent>                             es2;
    BSTEventSink<BGSActorCellEvent>                             es3;
    BSTEventSink<TESQuestEvent::Event>                          es4;
    BSTEventSink<PlayerCharacterQuestEvent::Event>              es5;
    BSTEventSink<CustomMarkerUpdate::Event>                     es6;
    BSTEventSink<LocationMarkerArrayUpdate::Event>              es7;
    BSTEventSink<LocalMapCameraUpdate::Event>                   es8;
    BSTEventSink<TESLocationClearedEvent>                       es9;
    BSTEventSink<ActorValueEvents::ActorValueChangedEvent>      es10;
    PipboyObject*                                               mapDataObject;          // always same as 0x90
    /*
    object with fields:
    CurrCell (string)
    CurrWorldspace (string)
    World (object)
        Player (object)
            Y
            Rotation
            X
        Custom (object)
            Height
            Y (float)
            Visible (bool)
            X (float)
        PowerArmor (object)
            Height
            Y (float)
            Visible (bool)
            X (float)
        Quests (array of objects)
            Height
            Y
            OnDoor (bool)
            Name
            Shared
            QuestId (array of uints)
            X
        Locations (array of objects)
            Y (float)
            Discovered (bool)
            LocationMarkerFormId (uint)
            X (float)
            ClearedStatus (bool)
            Name (string)
            LocationFormId (uint)
            Visible (bool)
            type (uint)
        Extents (object)
            SWX (float)
            NEY (float)
            NWX (float)
            NEX (float)
            NWY (float)
            SWY (float)
        WorldMapTexture (string)
        Local (object)
            Player (object)
                Y
                Rotation
                X
            Custom (object)
                Height
                Y (float)
                Visible (bool)
                X (float)
            PowerArmor (object)
                Height
                Y (float)
                Visible (bool)
                X (float)
            Doors (array of objects)
                Y (float)
                Name
                Visible (bool)
                X (float)
            Quests (array of objects)
                Height
                Y
                OnDoor (bool)
                Name
                Shared
                QuestId (array of uints)
                X
            Extents (object)
                SWX (float)
                NEY (float)
                NWX (float)
                NEX (float)
                NWY (float)
                SWY (float)
    */

    struct MarkerInfo {
        TESFullName*                                            name;                   // 00
        PipboyObject*                                           obj;                    // 08
        /*
        object with fields:

        Discovered (bool)
        type (uint)
        X (float)
        Y (float)
        Name (string)
        LocationFormId (uint)
        LocationMarkerFormId (uint)
        Visible (bool)
        ClearedStatus (bool)

        WorkshopOwned (bool) optional
        WorkshopPopulation (uint) optional
        WorkshopHappinessPct (float) optional
        */

        bool operator==(const BSFixedString a_key) const { return name->name.data == a_key.data; }
    };
    tHashSet<MarkerInfo, TESFullName*>                          markersInfo;

    struct Marker {
        UInt32                                                  arrayIndex;             // 00
        UInt32                                                  handle;                 // 04

        bool operator==(const UInt32 a_key) const { return arrayIndex == a_key; }
    };
    tHashSet<Marker, UInt32>                                    markers;

    tArray<PipboyObject*>                                       unkarr1;
    tArray<UInt32>                                              unkarr2;                // array of handles?
    
    struct unkstr2 {
        UInt32                                                  handle;                 // 00
        UInt32                                                  pad04;                  // 04
        PipboyObject*                                           obj;                    // 08
    };
    tHashSet<unkstr2, UInt32>                                   unkhs1;
    tHashSet<unkstr2, UInt32>                                   unkhs2;                 // looks like just copy of unkhs1

    NiPoint3                                                    playerPos[2];
    PipboyObject*                                               unkPos1;                // object with fields X (float) Y (float) and Rotation (float)
    PipboyObject*                                               unkPos2;                // object with fields X (float) Y (float) and Rotation (float)
    PipboyObject*                                               unkMarker1;             // probably custom marker. object with fields X (float) Y (float) Visible (bool) and Height (uchar)
    PipboyObject*                                               unkMarker2;             // probably custom marker. object with fields X (float) Y (float) Visible (bool) and Height (uchar)
    PipboyObject*                                               unkMarker3;             // probably PA marker. object with fields X (float) Y (float) Visible (bool) and Height (uchar)
    PipboyObject*                                               unkMarker4;             // probably PA marker. object with fields X (float) Y (float) Visible (bool) and Height (uchar)
    PipboyObject*                                               unkExtents1;            // object with fields SWX(float) NEY(float) NWX(float) NEX(float) NWY(float) SWY(float)
    PipboyObject*                                               unkExtents2;            // object with fields SWX(float) NEY(float) NWX(float) NEX(float) NWY(float) SWY(float)
};

class PipboyRadioData : public PipboyDataGroup {
public:
    BSTEventSink<RadioManager::PipboyFrequencyDetectionEvent>   es1;
    BSTEventSink<RadioManager::PipboyRadioTuningEvent>          es2;
    PipboyArray*                                                radioDataObject;        // always same as 0x90
    /*
    array of radiostations objects with fields:
    active (bool)
    frequency (float)
    text (string)
    inRange (bool)
    */
};

class PipboyPlayerInfoData : public PipboyDataGroup {
public:
    BSTEventSink<ActorValueEvents::ActorValueChangedEvent>      es1;
    BSTEventSink<BGSInventoryListEvent::Event>                  es2;
    BSTEventSink<ActorEquipManagerEvent::Event>                 es3;
    BSTEventSink<LevelIncrease::Event>                          es4;
    BSTEventSink<PerkPointIncreaseEvent>                        es5;
    BSTEventSink<PerkValueEvents::PerkEntryUpdatedEvent>        es6;
    BSTEventSink<HourPassed::Event>                             es7;
    BSTEventSink<SPECIALMenuEvent::NameChangedEvent>            es8;
    BSTEventSink<PlayerUpdateEvent>                             es9;
    BSTEventSink<BGSInventoryItemEvent::Event>                  es10;
    BSTEventSink<PlayerActiveEffectChanged::Event>              es11;
    BSTEventSink<PlayerCharacterQuestEvent::Event>              es12;
    BSTEventSink<PlayerDifficultySettingChanged::Event>         es13;
    PipboyObject*                                               playerInfoDataObject;   // always same as 0x90
    /*
    object with fields:
    CurrWeight (float)
    DateYear (uint)
    PerkPoints (uint)
    Caps (sint)
    MaxWeight (float)
    CurrHP
    MaxAP
    CurrAP
    MaxHP
    SlotResists (array of arrays of objects)
            Value (float)
            type (uint)
    CurrentHPGain (float)
    TotalDamages (array of objects)
        Value (float)
        type (uint)
    TotalResists (array of objects)
        Value (float)
        type (uint)
    TimeHour (float)
    DateDay
    DateMonth (uint)
    XPLevel (sint)
    PlayerName (string)
    XPProgressPct (float)
    */
};

class PipboyStatusData : public PipboyDataGroup {
public:
    BSTEventSink<PlayerLifeStateChanged::Event>                 es1;
    BSTEventSink<PlayerInDialogueChanged::Event>                es2;
    BSTEventSink<MenuOpenCloseEvent>                            es3;
    BSTEventSink<BGSInventoryListEvent::Event>                  es4;
    BSTEventSink<LoadingStatusChanged::Event>                   es5;
    BSTEventSink<VATSEvents::ModeChange>                        es6;
    BSTEventSink<UserEventEnabledEvent>                         es7;
    PipboyObject*                                               statusDataObject;       // always same as 0x90
    /*
    object with fields:
    IsInAnimation (bool)
    IsPipboyNotEquipped (bool)
    IsInVats (bool)
    IsPlayerPipboyLocked (bool)
    MinigameFormIds (array of uints)
    IsInAutoVanity (bool)
    IsPlayerMovementLocked (bool)
    EffectColor (array of float RGB)
    IsDataUnavailable (bool)
    IsMenuOpen (bool)
    IsPlayerDead (bool)
    IsInVatsPlayback (bool)
    IsPlayerInDialogue (bool)
    IsLoading (bool)
    */
    PipboyArray*                                                unkD8;                  // probably effectColor
};

struct PipboyDataManager {
    UInt64                                                      unk00[0xA0 / 8];        // 00
    PipboyStatsData                                             statsData;              // A0
    PipboySpecialData                                           specialData;            // 1D8
    PipboyPerksData                                             perksData;              // 280
    PipboyInventoryData                                         inventoryData;          // 358
    PipboyQuestData                                             questData;              // 4F0
    PipboyWorkshopData                                          workshopData;           // 5D8
    PipboyLogData                                               logData;                // 718
    PipboyMapData                                               mapData;                // 7F0
    PipboyRadioData                                             radioData;              // A28
    PipboyPlayerInfoData                                        playerInfoData;         // AD8
    PipboyStatusData                                            statusData;             // BE0
};

//
//
//

#define DECLARE_CUSTOM_MENU(MenuName)\
class MenuName## : public GameMenuBase {\
public:\
    MenuName##();\
    virtual ~##MenuName##();\
    static const char* sMenuName;\
    static void OpenMenu() {\
        BSFixedString MenuName(sMenuName);\
        CALL_MEMBER_FN((*g_uiMessageManager), SendUIMessage)(MenuName, kMessage_Open);\
    }\
    static void CloseMenu() {\
        BSFixedString MenuName(sMenuName);\
        CALL_MEMBER_FN((*g_uiMessageManager), SendUIMessage)(MenuName, kMessage_Close);\
    }\
    virtual void DrawNextFrame(float unk0, void* unk1) override final;\
    virtual void RegisterFunctions() override final;\
    DEFINE_STATIC_HEAP(ScaleformHeap_Allocate, ScaleformHeap_Free)\
}