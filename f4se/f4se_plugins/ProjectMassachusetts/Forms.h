#pragma once

#include "Config.h"
#include "f4se/GameData.h"
#include "f4se/PluginAPI.h"
#include "f4se_globals/Globals.h"

#include <unordered_map>
#include <vector>

extern PluginHandle                 g_PluginHandle;
extern F4SEPapyrusInterface*        g_Papyrus;
extern F4SEMessagingInterface*      g_Messaging;
extern F4SEScaleformInterface*      g_Scaleform;
extern F4SESerializationInterface*  g_Serialization;

typedef std::vector<ActorValueInfo*>                AVVector;
typedef std::vector<BGSPerk*>                       PerkVector;
typedef std::vector<UInt32>                         UInt32Vector;
typedef std::vector<UInt32Vector>                   UInt32VectorVector;
typedef std::vector<std::string>                    StringVector;
typedef std::vector<StringVector>                   StringVectorVector;
typedef std::vector<GFxValue>                       EntryVector;
typedef std::vector<BGSPerkRankArray::Data>         PerkRankVector;
typedef std::vector<PerkRankVector>                 PerkRankMap;
typedef std::vector<std::pair<BGSKeyword*, UInt32>>	KeywordMap;
typedef std::map<ActorValueInfo*, ActorValueInfo*>  ActorValueMap;

class Forms {
public:
    static bool                     Load();
    static void                     Unload();

    static void                     AddDependent(ActorValueInfo* Parent, ActorValueInfo* Child);
    static ActorValueInfo*          GetDependent(ActorValueInfo* Child);
    static ActorValueInfo*          GetSkillByName(const char* name);

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
    static ActorValueInfo*          SkillPointBonus;

    static ActorValueInfo*          Dehydration;
    static ActorValueInfo*          Starvation;
    static ActorValueInfo*          SleepDeprivation;

    static ActorValueInfo*          ItemCondMaxHealth;
    static ActorValueInfo*          ItemCondMinHealth;
    static ActorValueInfo*          ItemCondStartCond;

    static BGSListForm*             StimpakOrder;
    static AlchemyItem*             DoctorsBag;

    static BGSPerk*                 Perk_Barter;
    static BGSPerk*                 Perk_EnergyWeapons;
    static BGSPerk*                 Perk_Explosives;
    static BGSPerk*                 Perk_Guns;
    static BGSPerk*                 Perk_Lockpick;
    static BGSPerk*                 Perk_Medicine;
    static BGSPerk*                 Perk_MeleeWeapons;
    static BGSPerk*                 Perk_Repair;
    static BGSPerk*                 Perk_Science;
    static BGSPerk*                 Perk_Sneak;
    static BGSPerk*                 Perk_Speech;
    static BGSPerk*                 Perk_Survival;
    static BGSPerk*                 Perk_Unarmed;
    static BGSPerk*                 Perk_Hardcore;

    static AVVector                 ListSpecial;
    static AVVector                 ListSkills;
    static AVVector                 ListSkillMods;

    static PerkVector               ListPerks;
    static PerkVector               ListTraits;
    static PerkVector               ListSkillPerks;
    static PerkVector               ListMaster;

    static StringVectorVector       ListSortOrder;
	static UInt32VectorVector       EffectSortOrder;

	static KeywordMap               ObjectTypes;

    static ActorValueMap            RegisteredDependents;
    static PerkRankMap              RegisteredActors;

    enum kTagFlags : UInt16 {
        kTag_Barter             = 1 << 0x01,
        kTag_EnergyWeapons      = 1 << 0x02,
        kTag_Explosives         = 1 << 0x03,
        kTag_Guns               = 1 << 0x04,
        kTag_Lockpick           = 1 << 0x05,
        kTag_Medicine           = 1 << 0x06,
        kTag_MeleeWeapons       = 1 << 0x07,
        kTag_Repair             = 1 << 0x08,
        kTag_Science            = 1 << 0x09,
        kTag_Sneak              = 1 << 0x0A,
        kTag_Speech             = 1 << 0x0B,
        kTag_Survival           = 1 << 0x0C,
        kTag_Unarmed            = 1 << 0x0D,
    };
    static UInt16                   PlayerTags;

    static bool                     PlayerLevelUp;
    static UInt16                   PlayerLevel;

private:
    static bool                     CheckLoadedForms();
    static void                     ClearMaps();
};