#pragma once

#include "Config.h"

#include "f4se_globals/Globals.h"

#include "f4se/GameData.h"
#include "f4se/GameForms.h"
#include "f4se/GameObjects.h"
#include "f4se/GameRTTI.h"
#include "f4se/PluginAPI.h"
#include "f4se/PapyrusStruct.h"

extern PluginHandle                    g_PluginHandle;

extern F4SEPapyrusInterface*           g_Papyrus;
extern F4SEMessagingInterface*         g_Messaging;
extern F4SEScaleformInterface*         g_Scaleform;
extern F4SESerializationInterface*     g_Serialization;

struct PerkData {
    BGSPerk*        Perk;
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

typedef std::vector<ActorValueInfo*>    SkillList;
typedef std::vector<BGSPerk*>           PerkList;
typedef std::vector<PerkData>           DataList;

extern float GetPermValue(TESObjectREFR* Obj, ActorValueInfo* AVIF);

namespace Papyrus {
    DECLARE_EXTERN_STRUCT(TaggedSkills);
}

#define LookupTypeByID(FormID, Type)\
    (##Type*) Runtime_DynamicCast((void*)(LookupFormByID(FormID)), RTTI_TESForm, RTTI_##Type)

#define LookupTypeFromPlugin(FormID, SettingName, Type)\
    (##Type*) Runtime_DynamicCast((void*)(DataManager::LookupFormFromPlugin(g_Settings.GetInteger(##SettingName##, ##FormID##))), RTTI_TESForm, RTTI_##Type)

#define CheckForm(Form, FormType, Name)\
    if (##Form##) {\
        _LOGMESSAGENT("Variable: %s", #Form); g_Log.Indent();\
        _LOGMESSAGENT("%s is FormType: %i. Should be %i.", #Form, ##Form##->formType, FormType);\
        if (##Form##->formType == ##FormType##) {\
            switch (FormType) {\
            case kFormType_FLST:\
                _LOGMESSAGENT("%s reporting FormList count as: %i", #Form, ##Form##->##Name##);\
                break;\
            default:\
                _LOGMESSAGENT("%s reporting name as: %s", #Form, ##Form##->##Name##);\
                break;\
            }\
        }\
        g_Log.Outdent();\
        _LOGMESSAGENT("");\
    }\
    else {\
        return false;\
    }\

class DataManager {
public:
    static bool                     Load();
    static void                     Unload();

    static TESForm*                 LookupFormFromPlugin(UInt32 FormID);
    static ActorValueInfo*          GetSkillByName(std::string skillName);

    static PerkList                 m_MasterPerkList;
    static PerkList                 m_PerkList;
    static PerkList                 m_TraitList;
    static PerkList                 m_SkillPList;

    static SkillList                m_SkillList;
    static SkillList                m_SkillModList;
    static SkillList                m_SpecialList;

    static ActorValueInfo*          Strength;
    static ActorValueInfo*          Perception;
    static ActorValueInfo*          Endurance;
    static ActorValueInfo*          Charisma;
    static ActorValueInfo*          Intelligence;
    static ActorValueInfo*          Agility;
    static ActorValueInfo*          Luck;
    static ActorValueInfo*          Experience;
    static ActorValueInfo*          Health;
    static ActorValueInfo*          ActionPoints;
    static ActorValueInfo*          Rads;
    static ActorValueInfo*          PoisonResistance;
    static ActorValueInfo*          RadResistExposure;
    static ActorValueInfo*          RadResistIngestion;
    static ActorValueInfo*          RadHealthMax;
    static ActorValueInfo*          HeadCondition;
    static ActorValueInfo*          TorsoCondition;
    static ActorValueInfo*          LeftArmCondition;
    static ActorValueInfo*          RightArmCondition;
    static ActorValueInfo*          LeftLegCondition;
    static ActorValueInfo*          RightLegCondition;
    static TESImageSpaceModifier*   PipboyMenuIMOD;
    static BGSKeyword*              ObjectTypeStimpak;
    static AlchemyItem*             RadX;
    static ActorValueInfo*          Barter;
    static ActorValueInfo*          EnergyWeapons;
    static ActorValueInfo*          Explosives;
    static ActorValueInfo*          Guns;
    static ActorValueInfo*          Lockpick;
    static ActorValueInfo*          Medicine;
    static ActorValueInfo*          MeleeWeapons;
    static ActorValueInfo*          Repair;
    static ActorValueInfo*          Science;
    static ActorValueInfo*          Sneak;
    static ActorValueInfo*          Speech;
    static ActorValueInfo*          Survival;
    static ActorValueInfo*          Unarmed;
    static ActorValueInfo*          Barter_BuyMod;
    static ActorValueInfo*          Barter_SellMod;
    static ActorValueInfo*          EnergyWeapons_Damage;
    static ActorValueInfo*          EnergyWeapons_Accuracy;
    static ActorValueInfo*          Explosives_Damage;
    static ActorValueInfo*          Explosives_Accuracy;
    static ActorValueInfo*          Guns_Damage;
    static ActorValueInfo*          Guns_Accuracy;
    static ActorValueInfo*          Medicine_UsageMod;
    static ActorValueInfo*          MeleeWeapons_Damage;
    static ActorValueInfo*          MeleeWeapons_Secondary;
    static ActorValueInfo*          Sneak_SkillMod;
    static ActorValueInfo*          Sneak_DetectionMod;
    static ActorValueInfo*          Sneak_PickpocketMod;
    static ActorValueInfo*          Survival_UsageMod;
    static ActorValueInfo*          Unarmed_Damage;
    static ActorValueInfo*          Unarmed_Secondary;
    static ActorValueInfo*          ActionPointsMult;
    static ActorValueInfo*          SkillOffset;
    static ActorValueInfo*          InventoryWeight;
    static ActorValueInfo*          Dehydration;
    static ActorValueInfo*          Starvation;
    static ActorValueInfo*          SleepDeprivation;
    static TESGlobal*               DialogueBarter;
    static TESGlobal*               DialogueEnergyWeapons;
    static TESGlobal*               DialogueExplosives;
    static TESGlobal*               DialogueGuns;
    static TESGlobal*               DialogueLockpick;
    static TESGlobal*               DialogueMedicine;
    static TESGlobal*               DialogueMeleeWeapons;
    static TESGlobal*               DialogueRepair;
    static TESGlobal*               DialogueScience;
    static TESGlobal*               DialogueSneak;
    static TESGlobal*               DialogueSpeech;
    static TESGlobal*               DialogueSurvival;
    static TESGlobal*               DialogueUnarmed;
    static TESGlobal*               LimbTarget;
    static BGSListForm*             StimpakOrder;
    static AlchemyItem*             DoctorsBag;
    static TESQuest*                PerksQuest;

private:
    static bool                     CheckGamePlugin();
    static bool                     CheckLoadedData();

    static std::string              m_PluginName;
    static const ModInfo*           m_ModInfo;
};