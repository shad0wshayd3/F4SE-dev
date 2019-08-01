#pragma once

#include "f4se/GameForms.h"
#include "f4se/GameMenus.h"
#include "f4se/GameRTTI.h"
#include "f4se/PluginAPI.h"
#include "f4se/PapyrusStruct.h"

#include <string>
#include <unordered_map>
#include <vector>

struct PerkData {
    BGSPerk* Perk;
    int             Level;
    std::string     Name;
    std::string     Description;
    std::string     Requirements;
    bool            Eligible;
};

struct TaggedSkills {
    bool Barter;
    bool EnergyWeapons;
    bool Explosives;
    bool Guns;
    bool Lockpick;
    bool Medicine;
    bool MeleeWeapons;
    bool Repair;
    bool Science;
    bool Sneak;
    bool Speech;
    bool Survival;
    bool Unarmed;
    bool LockAll;
};

namespace Calculate {
    extern float DamageResist(ActorValueInfo* avif, float Damage, float DamageResist);
    extern ActorValueInfo* GetDependent(ActorValueInfo* Hardcoded);

    extern int SkillPoints(int PlayerLevel);
    extern int PerkPoints();
    extern int TraitPoints();
    extern int TagPoints();
}

namespace GFxHelperFunctions {
    extern void RegisterString(GFxValue* dst, GFxMovieRoot* root, const char* name, const char* str);
    extern void RegisterNumber(GFxValue* dst, const char* name, double value);
    extern void RegisterInt(GFxValue* dst, const char* name, int value);
    extern void RegisterBool(GFxValue* dst, const char* name, bool value);
}

namespace Papyrus {
    DECLARE_EXTERN_STRUCT(TaggedSkills);
    extern Papyrus::TaggedSkills DefaultSkills;
}

namespace Perks {
    extern std::vector<BGSPerk*> EntirePerkList;
    extern std::vector<BGSPerk*> MasterPerkList;
    extern std::vector<BGSPerk*> MasterTraitList;
    extern std::vector<PerkData> CurrentList;
    extern std::vector<PerkData> DefaultList;
    extern std::vector<ActorValueInfo*> SkillList;

    extern TaggedSkills PlayerTags;
    extern TaggedSkills DisplayTags;
    extern TaggedSkills DefaultTags;

    extern bool NeedLevelUp;
    extern int PlayerLevel;

    extern std::vector<BGSPerk*> GetRankList(BGSPerk* perk);
    extern void ProcessPerkList(GFxMovieRoot* root, std::vector<PerkData> perks);

    extern void ShowSkillsMenu(StaticFunctionTag* base, UInt32 Points);
    extern void ShowPerksMenu(StaticFunctionTag* base, UInt32 Points);
    extern void ShowTraitsMenu(StaticFunctionTag* base, UInt32 Points);
    extern bool ShowTagMenu(StaticFunctionTag* base, UInt32 Points, bool LockPreselect, Papyrus::TaggedSkills Tags);
    extern bool CharGenSkillSelect(StaticFunctionTag* base, bool AllowDialogueMenu, bool LockPreselect, Papyrus::TaggedSkills Tags);

    extern void LUPrompt_Hook();
}

namespace Skills {
    extern ActorValueInfo* GetByName(std::string skillName);
}

extern F4SEPapyrusInterface*            g_Papyrus;
extern F4SEMessagingInterface*          g_Messaging;
extern F4SEScaleformInterface*          g_Scaleform;
extern F4SESerializationInterface*      g_Serialization;

#define LookupTypeByID(id, to) ( ## to *) Runtime_DynamicCast((void*)(LookupFormByID(id)), RTTI_TESForm, RTTI_ ## to)
#define LookupTypeFromPlugin(id, to) ( ## to *) Runtime_DynamicCast((void*)(LookupFormFromPlugin(id)), RTTI_TESForm, RTTI_ ## to)
#define GetFormID(id, name) g_Settings.GetInteger(## name ##, ## id ##)

#define CheckForm(Form, FormTypeID, NameVar)\
    if (##Form##) {\
        g_Log.LogMessageNT("%s", #Form);\
        g_Log.Indent();\
        g_Log.LogMessageNT("%s has a value, it is not null.", #Form);\
        g_Log.LogMessageNT("%s is FormType: %i. Should be %i.", #Form, ##Form##->formType, FormTypeID);\
        if (##Form##->formType == ##FormTypeID##) {\
            if (FormTypeID == 94)\
                g_Log.LogMessageNT("%s reporting list count as: %i", #Form, ##Form##->##NameVar##);\
            else g_Log.LogMessageNT("%s reporting name as: %s", #Form, ##Form##->##NameVar##);\
        }\
        g_Log.LogMessageNT("");\
        g_Log.Outdent();\
    }

float GetPermValue(TESObjectREFR* thisObj, ActorValueInfo* avif);

class DataManager {
public:
    bool                    Init();

    /* Vanilla Actor Values */
    ActorValueInfo*         Strength;
    ActorValueInfo*         Perception;
    ActorValueInfo*         Endurance;
    ActorValueInfo*         Charisma;
    ActorValueInfo*         Intelligence;
    ActorValueInfo*         Agility;
    ActorValueInfo*         Luck;
    ActorValueInfo*         Experience;
    ActorValueInfo*         Health;
    ActorValueInfo*         ActionPoints;
    ActorValueInfo*         Rads;
    ActorValueInfo*         RadResistExposure;
    ActorValueInfo*         RadResistIngestion;
    ActorValueInfo*         RadHealthMax;
    ActorValueInfo*         HeadCondition;
    ActorValueInfo*         TorsoCondition;
    ActorValueInfo*         LeftArmCondition;
    ActorValueInfo*         RightArmCondition;
    ActorValueInfo*         LeftLegCondition;
    ActorValueInfo*         RightLegCondition;

    /* Vanilla Misc Types */
    TESImageSpaceModifier*  PipboyMenuImod;
    TESForm*                ObjectTypeStimpak;
    TESForm*                RadX;

    /* Mod Actor Values */
    ActorValueInfo*         Barter;
    ActorValueInfo*         EnergyWeapons;
    ActorValueInfo*         Explosives;
    ActorValueInfo*         Guns;
    ActorValueInfo*         Lockpick;
    ActorValueInfo*         Medicine;
    ActorValueInfo*         MeleeWeapons;
    ActorValueInfo*         Repair;
    ActorValueInfo*         Science;
    ActorValueInfo*         Sneak;
    ActorValueInfo*         Speech;
    ActorValueInfo*         Survival;
    ActorValueInfo*         Unarmed;
    ActorValueInfo*         Barter_BuyMod;
    ActorValueInfo*         Barter_SellMod;
    ActorValueInfo*         EnergyWeapons_Damage;
    ActorValueInfo*         EnergyWeapons_Accuracy;
    ActorValueInfo*         Explosives_Damage;
    ActorValueInfo*         Explosives_Accuracy;
    ActorValueInfo*         Guns_Damage;
    ActorValueInfo*         Guns_Accuracy;
    ActorValueInfo*         Medicine_UsageMod;
    ActorValueInfo*         MeleeWeapons_Damage;
    ActorValueInfo*         MeleeWeapons_Secondary;
    ActorValueInfo*         Sneak_SkillMod;
    ActorValueInfo*         Sneak_DetectionMod;
    ActorValueInfo*         Sneak_PickpocketMod;
    ActorValueInfo*         Survival_UsageMod;
    ActorValueInfo*         Unarmed_Damage;
    ActorValueInfo*         Unarmed_Secondary;
    ActorValueInfo*         ActionPointsMult;
    ActorValueInfo*         SkillOffset;
    ActorValueInfo*         CurrentLevel;
    ActorValueInfo*         Dehydration;
    ActorValueInfo*         Starvation;
    ActorValueInfo*         SleepDeprivation;

    /* Mod Globals */
    TESGlobal*              DialogueBarter;
    TESGlobal*              DialogueEnergyWeapons;
    TESGlobal*              DialogueExplosives;
    TESGlobal*              DialogueGuns;
    TESGlobal*              DialogueLockpick;
    TESGlobal*              DialogueMedicine;
    TESGlobal*              DialogueMeleeWeapons;
    TESGlobal*              DialogueRepair;
    TESGlobal*              DialogueScience;
    TESGlobal*              DialogueSneak;
    TESGlobal*              DialogueSpeech;
    TESGlobal*              DialogueSurvival;
    TESGlobal*              DialogueUnarmed;
    TESGlobal*              LimbTarget;

    /* Mod Misc Types */
    BGSListForm*            StimpakOrder;
    TESQuest*               PerksQuest;
    TESForm*                DoctorsBag;

private:
    bool                    GetGamePlugin();
    TESForm*                LookupFormFromPlugin(UInt32 FormID);

    void                    DoSelfCheck();

    bool                    m_Wait;
    const ModInfo*          m_ModInfo;
    std::string             m_pluginName;
};

class GameSettings {
public:
    void    Init();

    bool    GetBool(std::string settingName);
    float   GetFloat(std::string settingName);

private:
    std::unordered_map<std::string, UInt32> SettingsMap;
};

extern DataManager g_Data;
extern GameSettings g_GameSettings;