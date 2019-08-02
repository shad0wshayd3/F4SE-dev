#pragma once

#include "f4se/GameReferences.h"
#include "f4se/ObScript.h"

#include <vector>

typedef std::vector<ObScriptCommand*> ObScriptCommandList;

#define EXTRACT_ARGS    paramInfo, scriptData, opcodeOffsetPtr, thisObj, containingObj, scriptObj, locals
#define EXECUTE_ARGS    void* paramInfo, void* scriptData, TESObjectREFR* thisObj, void* containingObj, void* scriptObj, void* locals, double* result, void* opcodeOffsetPtr
#define EVAL_ARGS       TESObjectREFR* thisObj, void* arg1, void* arg2, double* result

bool ObScript_Default_Parse(UInt32 numParams, void* paramInfo, void* lineBuf, void* scriptBuf);

#define DEFINE_CMD_FULL(longName, shortName, description, refRequired, paramInfo)\
    extern bool Cmd_ ## longName ## _Execute(EXECUTE_ARGS);\
    static ObScriptCommand (kCommand_ ## longName) = {\
        #longName,\
        #shortName,\
        0,\
        0,\
        #description,\
        refRequired,\
        0,\
        (sizeof(paramInfo) / sizeof(ObScriptParam)),\
        0,\
        paramInfo,\
        Cmd_ ## longName ## _Execute,\
        ObScript_Default_Parse,\
        NULL,\
        0,\
        0\
    }

#define DEFINE_CMD_EVAL_FULL(longName, shortName, description, refRequired, paramInfo) \
    extern bool Cmd_ ## longName ## _Execute(EXECUTE_ARGS);\
    extern bool Cmd_ ## longName ## _Eval(EVAL_ARGS);\
    static ObScriptCommand (kCommand_ ## longName) = {\
        #longName,\
        #shortName,\
        0,\
        0,\
        #description,\
        refRequired,\
        0,\
        (sizeof(paramInfo) / sizeof(ObScriptParam)),\
        0,\
        paramInfo,\
        Cmd_ ## longName ## _Execute,\
        ObScript_Default_Parse,\
        Cmd_ ## longName ## _Eval,\
        1,\
        0\
    }

class IObScript {
public:

    void Init();

    ObScriptCommand*        GetCommand(const char* name);
    ObScriptCommandList     GetMatchingCommands(bool(*filterFunction)(ObScriptCommand* command));

    bool                    OverloadCommand(ObScriptCommand* command, ObScriptCommand overload);
    bool                    OverloadCommand(const char* name, ObScriptCommand overload);

private:
    ObScriptCommandList     m_commands;

};

extern IObScript g_ObScript;

// ------------------------------------------------------------------------------------------------
// Params
// ------------------------------------------------------------------------------------------------

enum ObScriptParamType {
    kType_String,
    kType_Integer,
    kType_Float,
    kType_ObjectID,             // EquipItem
    kType_ObjectRef,            // Activate
    kType_ActorValue,           // ModValue
    kType_Actor,                // StartCombat
    kType_SpellItem,            // AddSpell
    kType_Axis,                 // SetPos
    kType_Cell,                 // GetInCell
    kType_AnimationGroup,       // IsAnimPlaying
    kType_MagicItem,            // Cast
    kType_Sound,                // PlaySound
    kType_Topic,                // Say
    kType_Quest,                // StartQuest
    kType_Race,                 // GetIsRace
    kType_Class,                // GetIsClass
    kType_Faction,              // GetInFaction
    kType_Sex,                  // GetIsSex
    kType_Global,               // GetGlobalValue
    kType_Furniture,            // IsCurrentFurnitureObj
    kType_TESObject,            // PlaceAtMe
    kType_Unknown016,           // unused
    kType_QuestStage,           // SetStage
    kType_MapMarker,            // AddToMap
    kType_ActorBase,            // SetEssential
    kType_Container,            // RemoveAllItems
    kType_Worldspace,           // GetInWorldspace
    kType_CrimeType,            // GetCrimeKnown
    kType_Package,              // GetIsCurrentPackage
    kType_CombatStyle,          // SetCombatStyle
    kType_MagicEffect,          // HasMagicEffect
    kType_FormType,             // GetIsObjectType
    kType_WeatherID,            // GetIsCurrentWeather
    kType_Unk22,                // unused
    kType_Owner,                // IsOwner
    kType_EffectShader,         // PlayMagicShaderVisuals
    kType_FormList,             // IsInList
    kType_Unk26,                // unused
    kType_Perk,                 // AddPerk
    kType_Note,                 // AddNote
    kType_MiscellaneousStat,    // GetPCMiscStat
    kType_ImagespaceModifier,   // ApplyImageSpaceModifier
    kType_Imagespace,           // SetImagespace
    kType_Unk2C,                // unused
    kType_Unk2D,                // unused
    kType_EventFunction,        // GetEventData
    kType_EventMember,          // GetEventData
    kType_EventData,            // GetEventData
    kType_VoiceType,            // GetIsVoiceType
    kType_EncounterZone,        // GetInZone
    kType_IdleForm,             // IsLastIdlePlayed
    kType_Message,              // ShowMessage
    kType_InvObjOrFormList,     // AddItem
    kType_Alignment,            // GetIsAlignment
    kType_EquipType,            // GetIsUsedItemEquipType
    kType_NonFormList,          // GetIsUsedItem
    kType_Music,                // AddMusic
    kType_CriticalStage,        // SetCriticalStage
    kType_Keyword,              // HasKeyword
    kType_RefType,              // HasRefType
    kType_Location,             // GetInCurrentLocation
    kType_Unknown03E,           // unused
    kType_QuestAlias,           // GetIsAliasRef
    kType_Unk40,                // unused
    kType_Unk41,                // unused
    kType_Unk42,                // unused
    kType_Scene,                // StartScene
    kType_CastingSource,        // HasEquippedSpell
    kType_AssociationType,      // HasAssociationType
    kType_Unk46,                // unused
    kType_Unk47,                // unused
    kType_Unk48,                // unused
    kType_Unk49,                // unused
    kType_Unk4A,                // unused
    kType_Unk4B,                // unused
    kType_PackageData,          // GetWithinPackageLocation
    kType_Unk4D,                // unused
    kType_KnowableForm,         // PlayerKnows
    kType_Region,               // IsPlayerInRegion
    kType_Unk50,                // unused
    kType_Unk51,                // unused
    kType_Unk52,                // unused
    kType_Unk53,                // unused
    kType_Unk54,                // unused
    kType_SceneAction,          // GetSceneActionPercent
    kType_Unk56,                // unused
    kType_FurnitureEntryType    // IsFurnitureEntryType
};

static ObScriptParam kParams_OneInt[1] = {
    {   "int",      kType_Integer,  0 },
};

static ObScriptParam kParams_TwoInts[2] = {
    {   "int",      kType_Integer,  0 },
    {   "int",      kType_Integer,  0 }
};

static ObScriptParam kParams_OneString[1] = {
    {   "string",   kType_String,   0 },
};

static ObScriptParam kParams_TwoStrings[2] = {
    {   "string",   kType_String,   0 },
    {   "string",   kType_String,   0 }
};

static ObScriptParam kParams_OneFloat[1] = {
    {   "float",    kType_Float,    0 }
};

static ObScriptParam kParams_TwoFloats[2] = {
    {   "float",    kType_Float,    0 },
    {   "float",    kType_Float,    0 }
};

static ObScriptParam kParams_OneActorValue[1] = {
    {   "Actor Value",  kType_ActorValue,   0 }
};
