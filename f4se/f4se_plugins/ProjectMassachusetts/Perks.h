  #pragma once
#include "Forms.h"

int GetSkillPoints(int Level);
int GetPerkPoints();
int GetTraitPoints();
int GetTagPoints();
int GetTagCount();

bool IsFirstRank(BGSPerk* Perk);
bool IsFirstValidRank(BGSPerk* Perk);
PerkVector GetRankList(BGSPerk* Perk);
int GetPlayerRank(BGSPerk* Perk);

std::string GetPerkIconPath(BGSPerk* Perk);
std::string GetSkillIconPath(ActorValueInfo* avif);

class EntryList {
public:
    enum MenuID {
        kMenuID_Skills  = 0x10,
        kMenuID_Tags    = 0x11,
        kMenuID_Perks   = 0x20,
        kMenuID_Traits  = 0x21,
    };

                    EntryList();
                    EntryList(int ID, int Level, int PointsBase, int PointsUsed);
    GFxValue*       BuildEntryList(GFxMovieRoot* root, GFxValue* Result);

private:
    int             m_ID;
    int             m_Level;
    int             m_PointsBase;
    int             m_PointsUsed;
};