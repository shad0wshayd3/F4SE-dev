#pragma once
#include "DataManager.h"

struct RequirementData {
    std::string End;
    std::string Default;
};

class PerkManager : private DataManager {
public:
    static void                                     Init();
    static void                                     Unload();

    static void                                     ShowSkillsMenu(StaticFunctionTag* base, UInt32 Points);
    static void                                     ShowPerksMenu(StaticFunctionTag* base, UInt32 Points);
    static void                                     ShowTraitsMenu(StaticFunctionTag* base, UInt32 Points);
    static bool                                     ShowTagMenu(StaticFunctionTag* base, UInt32 Points, Papyrus::TaggedSkills Tags, bool LockPreselected);

    static void                                     LevelUp_Hook();
    static bool                                     CharGenSkillSelect(StaticFunctionTag* base, UInt32 Points, Papyrus::TaggedSkills Tags, bool LockPreselected);

    static void                                     LevelUpThread();
    static void                                     CharGenThread(bool AllowDialogueMenu);

    static PerkList                                 GetRankList(BGSPerk* Perk);
    static void                                     ProcessPerkList(GFxMovieRoot* Root);

    static int                                      GetSkillPoints(int PlayerLevel);
    static int                                      GetPerkPoints();
    static int                                      GetTraitPoints();
    static int                                      GetTagPoints();

    static TaggedSkills                             m_PlayerTags;
    static TaggedSkills                             m_DefaultTags;

    static int                                      m_CurrentLevel;
    static bool                                     m_LevelUp;

private:
    enum MenuID {
        kMenuID_Skills  = 1,
        kMenuID_Perks   = 2,
        kMenuID_Traits  = 3,
        kMenuID_Tags    = 4,
    };

    static void                                     OpenLevelUpMenu(DataList ActiveList, int PerkPoints, int PlayerLevel, MenuID kMenuID);
    static void                                     OpenLevelUpMenu(TaggedSkills ActiveTags, int SkillPoints, int PlayerLevel);
    static void                                     OpenLevelUpMenu(int SkillPoints, int PlayerLevel);

    static bool                                     PerkSorter(PerkData FirstPerk, PerkData SecondPerk);
    static bool                                     RankSorter(BGSPerk* FirstPerk, BGSPerk* SecondPerk);
    static std::string                              RequirementSorter(std::vector<RequirementData> RequirementsList);

    static bool                                     IsFirstValidRank(BGSPerk* Perk);
    static bool                                     IsBlockingMenuOpen(bool AllowDialogueMenu = false);
    static bool                                     IsTagEnabled(TaggedSkills Tags, ActorValueInfo* Skill);

    static DataList                                 BuildPerkList(int ForLevel, bool Trait = false);

    static bool                                     ValidateTags(Papyrus::TaggedSkills Tags, int Points, bool LockPreselect);

    static std::vector<std::vector<std::string>>    m_SortStrings;

    static DataList                                 m_ActiveList;
    static DataList                                 m_DefaultList;

    static TaggedSkills                             m_ActiveTags;

    static GFxMovieRoot*                            m_CurrentRoot;

    static int                                      m_PerkPoints;
    static int                                      m_SkillPoints;
    static int                                      m_ActiveLevel;
    static int                                      m_ActiveMenuID;
};

