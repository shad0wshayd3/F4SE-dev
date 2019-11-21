#include "Perks.h"

#include "f4se/GameStreams.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

struct PerkEntry {
    std::string     text;
    std::string     Description;
    std::string     IconPath;
    bool            IsValidPerk;
    bool            IsSelected;
    bool            IsLocked;
    int             NumRanks;
    int             FormID;
    int             Level;
};

typedef std::vector<PerkEntry>					PerkEntryList;
typedef std::pair<std::string, std::string>		RequirementPair;
typedef std::vector<RequirementPair>			RequirementPairList;

#define AddErrorTags(str)\
    str = "<font color=\'#A9A9A9\'>" + str + "</font>"

#define AddRequirement(str, separator)\
    Reqs.emplace_back(RequirementPair{ str, separator });

// ------------------------------------------------------------------------------------------------
// Level Up Menu Points
// ------------------------------------------------------------------------------------------------

int GetSkillPoints(int Level) {
    float points = 10 + GetValue((*g_player), Forms::SkillPointBonus);
    points += (GetValue((*g_player), Forms::Intelligence) / 2.0);
    return (Level % 2 == 0) ? ceilf(points) : floorf(points);
}

int GetPerkPoints() {
    return ISettings::GetInteger("Stats:PerkPointsPerLevel", 1);
}

int GetTraitPoints() {
    return ISettings::GetInteger("Stats:NumTraits", 2);
}

int GetTagPoints() {
    return ISettings::GetInteger("Stats:NumTags", 3);
}

int GetTagCount() {
    int Result = 0;

    for (int i = 0; i < Forms::ListSkills.size(); i++) {
        if (Forms::PlayerTags & 1 << (i + 1)) {
            Result++;
        }
    }

    return Result;
}

// ------------------------------------------------------------------------
// Sorting Functions
// ------------------------------------------------------------------------

bool SortPerkEntries(PerkEntry Perk1, PerkEntry Perk2) {
    if (Perk1.Level != Perk2.Level)
        return (Perk1.Level < Perk2.Level);

    const char* Name1 = Perk1.text.c_str();
    const char* Name2 = Perk2.text.c_str();
    int Length = max(strlen(Name1), strlen(Name2));

    for (int i = 0; i < Length; i++)
        if (Perk1.text[i] != Perk2.text[i])
            return (Perk1.text[i] < Perk2.text[i]);

    return (Perk1.text < Perk2.text);
}

bool SortRanks(BGSPerk* Perk1, BGSPerk* Perk2) {
    return (Perk1->perkLevel < Perk2->perkLevel);
}

int SortRequirement(RequirementPairList::iterator Req, std::string Query, std::string& Result, int Size) {
	int Count = 0;
	if (Req->first.find(Query) != std::string::npos) {
		Result.append(Req->first);

		if (Size > 1) {
			Result.append(Req->second);

			if (Req->second.find("or") != std::string::npos) {
				Req++; Size--;
				Count += SortRequirement(Req, "", Result, Size);
			}
		}

		Count++;
	}

	return Count;
}

std::string SortRequirements(RequirementPairList Requirements) {
    std::string Result = "";
    bool AddedItem = false;

	for (auto iterSortList : Forms::ListSortOrder) {
		for (auto iterSort : iterSortList) {
			for (auto iterReq = Requirements.begin(); iterReq != Requirements.end();) {
				int Count = SortRequirement(iterReq, iterSort, Result, Requirements.size());
				if (Count > 0)
					for (Count; Count > 0; Count--)
						iterReq = Requirements.erase(iterReq);
				else iterReq++;
			}
		}
	}

    if ((Result == "Level 1") || (Result == "Level 2") || Result.empty())
        Result = "--";

    return Result;
}

// ------------------------------------------------------------------------
// Misc Functions
// ------------------------------------------------------------------------

std::string GetPerkIconPath(BGSPerk* Perk) {
    std::string Result = Perk->swfPath.c_str();
	std::string Path = "Interface/" + Result;
	
	BSResourceNiBinaryStream PerkPathStream(Path.c_str());
	if (PerkPathStream.IsValid())
		return Result;

    if (Perk->numRanks > 1) {
        BGSPerk* firstRank = GetRankList(Perk)[0];
		if (firstRank)
			Result = firstRank->swfPath.c_str();

		Path = "Interface/" + Result;
		BSResourceNiBinaryStream RankPathStream(Path.c_str());
		if (RankPathStream.IsValid())
			return Result;
    }

    std::stringstream ss;
    ss << "Components/VaultBoys/Perks/PerkClip_" << std::hex << std::setw(8) << std::setfill('0') << Perk->formID << ".swf";
    Result = ss.str();

	Path = "Interface/" + Result;
	BSResourceNiBinaryStream FormPathStream(Path.c_str());
	if (FormPathStream.IsValid())
		return Result;

    return "Components/VaultBoys/Perks/PerkClip_Default.swf";
}

std::string GetSkillIconPath(ActorValueInfo* avif) {
    std::stringstream ss;
    ss << "Components/VaultBoys/Skills/" << avif->GetEditorID() << ".swf";

	std::string Result = ss.str();
	std::string Path = "Interface/" + Result;
	BSResourceNiBinaryStream IconPathStream(Path.c_str());
	if (IconPathStream.IsValid())
		return Result;

	return "Components/VaultBoys/Perks/PerkClip_Default.swf";
}

PerkVector GetRankList(BGSPerk* Perk) {
    BGSPerk* iter = Perk;
    PerkVector Result;

    bool FoundFirstRank = false;
    for (int i = 0; ((i < Perk->numRanks) && iter->nextPerk); i++) {
        if (iter->sound || (strlen(iter->swfPath) > 0) || FoundFirstRank) {
            if (!FoundFirstRank) {
                FoundFirstRank  = true;
                i               = 0;
            }
            Result.emplace_back(iter);
        }
        iter = iter->nextPerk;
    }

    if (Perk->numRanks != Result.size())
        _LogWarning("Perk Chain containing Form %08X is broken!", Perk->formID);

    std::sort(Result.begin(), Result.end(), &SortRanks);
    return Result;
}

int GetPlayerRank(BGSPerk* Perk) {
	int Result = 0;

	auto RankList = GetRankList(Perk);
	for (auto iter : RankList) {
		if (HasPerk((*g_player), iter))
			Result++;
	}

	return Result;
}

bool IsFirstRank(BGSPerk* Perk) {
    PerkVector PerkRanks = GetRankList(Perk);
    return PerkRanks[0]->formID == Perk->formID;
}

bool IsFirstValidRank(BGSPerk* Perk) {
    PerkVector PerkRanks = GetRankList(Perk);

    for (auto iter : PerkRanks)
        if (!HasPerk((*g_player), iter)) {
            if (Perk->formID == iter->formID)
                return true;
            return false;
        }

    return false;
}

// ------------------------------------------------------------------------
// EntryList
// ------------------------------------------------------------------------

EntryList::EntryList() { 
    //
}

EntryList::EntryList(int ID, int Level, int PointsBase, int PointsUsed) {
    m_ID            = ID;
    m_Level         = Level;
    m_PointsBase    = PointsBase;
    m_PointsUsed    = PointsUsed;
}

GFxValue* EntryList::BuildEntryList(GFxMovieRoot* root, GFxValue* Result) {
    Result[0].SetInt(m_ID);
    Result[1].SetInt(m_Level);
    Result[2].SetInt(m_PointsBase);
    Result[3].SetInt(m_PointsUsed);
    root->CreateArray(&Result[4]);

    PlayerCharacter* Player = (*g_player);
    UInt16 FlagBase = 1;

    switch (m_ID) {
    case kMenuID_Skills:
    case kMenuID_Tags: {
        for (auto iter : Forms::ListSkills) {
            float ValueBase = GetPermValue(Player, iter);
            float Value     = ValueBase;

            bool IsSelected = false;
            bool IsLocked   = false;

            if (m_ID == kMenuID_Tags) {
                UInt16 Flag = 1 << FlagBase;
                FlagBase++;

                if (Forms::PlayerTags & Flag) {
                    m_PointsUsed    += 1;
                    ValueBase       -= 15;
                    IsSelected      = true;
                    //IsLocked        = true;
                }
            }

            bool ShowArrows = (m_ID == kMenuID_Skills);

            BSString DescriptionText;
            TESDescription* DescriptionForm = DYNAMIC_CAST(iter, ActorValueInfo, TESDescription);
            CALL_MEMBER_FN(DescriptionForm, Get)(&DescriptionText, nullptr);

            GFxValue Skill;
            root->CreateObject(&Skill);
            SetGFxValue(&Skill, root,   "text",         iter->GetFullName());
            SetGFxValue(&Skill, root,   "Description",  DescriptionText.Get());
            SetGFxValue(&Skill, root,   "IconPath",     GetSkillIconPath(iter));
            SetGFxValue(&Skill,         "ShowArrows",   ShowArrows);
            SetGFxValue(&Skill,         "IsSelected",   IsSelected);
            SetGFxValue(&Skill,         "IsLocked",     IsLocked);
            SetGFxValue(&Skill,         "FormID",       iter->formID);
            SetGFxValue(&Skill,         "ValueBase",    (int)ValueBase);
            SetGFxValue(&Skill,         "Value",        (int)Value);
            Result[4].PushBack(&Skill);
        }

        Result[3].SetInt(m_PointsUsed);
        break;
    }

    case kMenuID_Perks:
    case kMenuID_Traits: {
        PerkVector List = (m_ID == kMenuID_Traits) ? Forms::ListTraits : Forms::ListPerks;
        PerkEntryList PerksV, PerksI;

        for (auto iter : List) {
            if (HasPerk(Player, iter))
                continue;

            if (iter->numRanks > 1)
                if (!IsFirstValidRank(iter))
                    continue;

            std::string Name = iter->GetFullName();
            if (Name.empty())
                continue;

            std::string Requirement;
            RequirementPairList Reqs;

            BSString DescriptionText;
            TESDescription* DescriptionForm = DYNAMIC_CAST(iter, BGSPerk, TESDescription);
            CALL_MEMBER_FN(DescriptionForm, Get)(&DescriptionText, nullptr);

            int LevelRequirement = max(iter->perkLevel, 1);
            Requirement = "Level " + std::to_string(LevelRequirement);
            AddRequirement(Requirement, ", ");

            bool SkipPerk = false;

            Condition* Conditions = iter->condition;
            while (Conditions && !SkipPerk) {
                int kFunctionID = Conditions->functionId;
                int kCompareFlag = Conditions->comparisonType.flags;
                int kCompareOp = Conditions->comparisonType.op;

                float ComparisonValue = Conditions->compareValue;

                Requirement.clear();
                switch (kFunctionID) {
                case Condition::kFunction_GetValue:
                case Condition::kFunction_GetBaseValue:
                case Condition::kFunction_GetPermanentValue: {
                    ActorValueInfo* ActorValue = DYNAMIC_CAST(Conditions->param1.form, TESForm, ActorValueInfo);
                    Requirement += ActorValue->GetFullName();
                    float value = 0;

                    switch (kFunctionID) {
                    case Condition::kFunction_GetValue:
                        value = GetValue(Player, ActorValue);
                        break;

                    case Condition::kFunction_GetBaseValue:
                        value = GetBaseValue(Player, ActorValue);
                        break;

                    case Condition::kFunction_GetPermanentValue:
                        value = GetPermValue(Player, ActorValue);
                        break;
                    }

                    switch (kCompareOp) {
                    case Condition::kCompareOp_Equal:
                        Requirement += " = ";
                        Requirement += std::to_string(int(ComparisonValue));
                        break;

                    case Condition::kCompareOp_NotEqual:
                        Requirement += " != ";
                        Requirement += std::to_string(int(ComparisonValue));
                        break;

                    case Condition::kCompareOp_Greater:
                        Requirement += " &gt; ";
                        Requirement += std::to_string(int(ComparisonValue));
                        break;

                    case Condition::kCompareOp_GreaterEqual:
                        Requirement += " ";
                        Requirement += std::to_string(int(ComparisonValue));
                        break;

                    case Condition::kCompareOp_Less:
                        Requirement += " &lt; ";
                        Requirement += std::to_string(int(ComparisonValue));
                        break;

                    case Condition::kCompareOp_LessEqual:
                        Requirement += " &lt;= ";
                        Requirement += std::to_string(int(ComparisonValue));
                        break;
                    }

                    switch (kCompareFlag) {
                    case Condition::kCompareFlag_Or:
                        if (Conditions->next) {
                            AddRequirement(Requirement, " or ");
                            break;
                        }

                    default:
                        AddRequirement(Requirement, ", ");
                        break;
                    }

                    break;
                }

                case Condition::kFunction_GetIsSex: {
                    int PlayerSex = CALL_MEMBER_FN(DYNAMIC_CAST(Player->baseForm, TESForm, TESNPC), GetSex)();

                    switch (kCompareOp) {
                    case Condition::kCompareOp_Equal:
                        if (ComparisonValue > 0) {
                            if (!(PlayerSex == Conditions->param1.u32)) {
                                SkipPerk = true;
                                continue;
                            }
                        }
                        else {
                            if (PlayerSex == Conditions->param1.u32) {
                                SkipPerk = true;
                                continue;
                            }
                        }
                        break;

                    case Condition::kCompareOp_NotEqual:
                        if (ComparisonValue > 0) {
                            if (!(PlayerSex != Conditions->param1.u32)) {
                                SkipPerk = true;
                                continue;
                            }
                        }
                        else {
                            if (PlayerSex != Conditions->param1.u32) {
                                SkipPerk = true;
                                continue;
                            }
                        }
                        break;

                    default:
                        SkipPerk = true;
                        continue;
                    }

                    break;
                }

                case Condition::kFunction_GetGlobalValue: {
                    TESGlobal* GlobalValue = DYNAMIC_CAST(Conditions->param1.form, TESForm, TESGlobal);

                    switch (kCompareOp) {
                    case Condition::kCompareOp_Equal:
                        if (!(GlobalValue->value == ComparisonValue)) {
                            SkipPerk = true;
                            continue;
                        }
                        break;

                    case Condition::kCompareOp_NotEqual:
                        if (!(GlobalValue->value != ComparisonValue)) {
                            SkipPerk = true;
                            continue;
                        }
                        break;

                    case Condition::kCompareOp_Greater:
                        if (!(GlobalValue->value > ComparisonValue)) {
                            SkipPerk = true;
                            continue;
                        }
                        break;

                    case Condition::kCompareOp_GreaterEqual:
                        if (!(GlobalValue->value >= ComparisonValue)) {
                            SkipPerk = true;
                            continue;
                        }
                        break;

                    case Condition::kCompareOp_Less:
                        if (!(GlobalValue->value < ComparisonValue)) {
                            SkipPerk = true;
                            continue;
                        }
                        break;

                    case Condition::kCompareOp_LessEqual:
                        if (!(GlobalValue->value <= ComparisonValue)) {
                            SkipPerk = true;
                            continue;
                        }
                        break;
                    }

                    break;
                }

                case Condition::kFunction_HasPerk: {
                    BGSPerk* Perk = DYNAMIC_CAST(Conditions->param1.form, TESForm, BGSPerk);

                    switch (kCompareOp) {
                    case Condition::kCompareOp_Equal:
                        Requirement += (ComparisonValue) ? "" : " does not have ";
                        Requirement += Perk->GetFullName();
                        Requirement += " perk";
                        break;

                    case Condition::kCompareOp_NotEqual:
                        Requirement += (ComparisonValue) ? " does not have " : "";
                        Requirement += Perk->GetFullName();
                        Requirement += " perk";
                        break;
                    }

                    switch (kCompareFlag) {
                    case Condition::kCompareFlag_Or:
                        if (Conditions->next) {
                            AddRequirement(Requirement, " or ");
                            break;
                        }
                    default:
                        AddRequirement(Requirement, ", ");
                        break;
                    }

                    break;
                }

                default:
                    break;
                }

                Conditions = Conditions->next;
            }

            if (SkipPerk)
                continue;

			std::string Requirements = SortRequirements(Reqs);

			bool IsValidPerk = EvaluationConditions(&iter->condition, (*g_player), (*g_player)) && (m_Level >= LevelRequirement);
			if (!IsValidPerk)
				AddErrorTags(Requirements);

            std::stringstream Description;
            Description << "Req: " << Requirements << "<br>" << "$Ranks_cl " << std::to_string(iter->numRanks) << "<br><br>" << DescriptionText.Get();
            
            PerkEntry Entry;
            Entry.text          = Name;
            Entry.Description   = Description.str();
            Entry.IconPath      = GetPerkIconPath(iter);
            Entry.IsValidPerk   = IsValidPerk;
            Entry.IsSelected    = false;
            Entry.IsLocked      = false;
            Entry.NumRanks      = iter->numRanks;
            Entry.FormID        = iter->formID;
            Entry.Level         = LevelRequirement;

            if (IsValidPerk)
                PerksV.emplace_back(Entry);
            else PerksI.emplace_back(Entry);
        }

        std::sort(PerksV.begin(), PerksV.end(), &SortPerkEntries);
        std::sort(PerksI.begin(), PerksI.end(), &SortPerkEntries);
        PerksV.insert(PerksV.end(), PerksI.begin(), PerksI.end());
        
        for (auto iter : PerksV) {
            GFxValue Perk;
            root->CreateObject(&Perk);
            SetGFxValue(&Perk, root,    "text",         iter.text);
            SetGFxValue(&Perk, root,    "Description",  iter.Description);
            SetGFxValue(&Perk, root,    "IconPath",     iter.IconPath);
            SetGFxValue(&Perk,          "IsValidPerk",  iter.IsValidPerk);
            SetGFxValue(&Perk,          "IsSelected",   iter.IsSelected);
            SetGFxValue(&Perk,          "IsLocked",     iter.IsLocked);
            SetGFxValue(&Perk,          "NumRanks",     iter.NumRanks);
            SetGFxValue(&Perk,          "FormID",       iter.FormID);
            SetGFxValue(&Perk,          "Level",        iter.Level);
            Result[4].PushBack(&Perk);
        }

        break;
    }
    }

    return Result;
}