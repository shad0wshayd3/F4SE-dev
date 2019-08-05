#include "PerkManager.h"

#include <algorithm>
#include <thread>

void PerkManager::Init() {
    tArray<BGSPerk*> Perks = (*g_dataHandler)->arrPERK;
    for (int i = 0; i < Perks.count; i++) {
        if ((Perks[i]->playable) && (!Perks[i]->hidden) && (Perks[i]->numRanks > 0)) {
            if (Perks[i]->trait) {
                auto result = std::find(m_TraitList.begin(), m_TraitList.end(), Perks[i]);
                if (result == m_TraitList.end()) {
                    m_TraitList.emplace_back(Perks[i]);
                    m_MasterPerkList.emplace_back(Perks[i]);
                }
            }
            else {
                auto result = std::find(m_PerkList.begin(), m_PerkList.end(), Perks[i]);
                if (result == m_PerkList.end()) {
                    m_PerkList.emplace_back(Perks[i]);
                    m_MasterPerkList.emplace_back(Perks[i]);
                }
            }
        }
    }

    // Sort Level above everything
    m_SortStrings.emplace_back(std::vector<std::string>{});
    m_SortStrings[0].emplace_back("Level");

    // Any SPECIAL reqs
    m_SortStrings.emplace_back(std::vector<std::string>{});
    for (auto iter : m_SpecialList)
        m_SortStrings[1].emplace_back(iter->GetFullName());

    // Any Skill reqs
    m_SortStrings.emplace_back(std::vector<std::string>{});
    for (auto iter : m_SkillList)
        m_SortStrings[2].emplace_back(iter->GetFullName());

    // Any perks
    m_SortStrings.emplace_back(std::vector<std::string>{});
    m_SortStrings[3].emplace_back("perk");

    // And anything else
    m_SortStrings.emplace_back(std::vector<std::string>{});

    // Set Base CurrentLevel to 1, for new saves
    m_CurrentLevel = 1;
}

void PerkManager::Unload() {
    m_SortStrings.clear();
    m_ActiveList.clear();
    m_DefaultList.clear();
}

void PerkManager::ShowSkillsMenu(StaticFunctionTag* base, UInt32 Points) {
    OpenLevelUpMenu(Points, GetLevel((*g_player)));
}

void PerkManager::ShowPerksMenu(StaticFunctionTag* base, UInt32 Points) {
    int PlayerLevel = GetLevel((*g_player));
    OpenLevelUpMenu(BuildPerkList(PlayerLevel), Points, PlayerLevel, kMenuID_Perks);
}

void PerkManager::ShowTraitsMenu(StaticFunctionTag* base, UInt32 Points) {
    int PlayerLevel = GetLevel((*g_player));
    OpenLevelUpMenu(BuildPerkList(PlayerLevel, true), Points, PlayerLevel, kMenuID_Traits);
}

bool PerkManager::ShowTagMenu(StaticFunctionTag* base, UInt32 Points, Papyrus::TaggedSkills Tags, bool LockPreselected) {
    if (!ValidateTags(Tags, GetTagPoints(), LockPreselected))
        return false;

    OpenLevelUpMenu(m_ActiveTags, Points, GetLevel((*g_player)));
    return true;
}

void PerkManager::LevelUp_Hook() {
    m_LevelUp = true;
    std::thread LUMThread(LevelUpThread);
    LUMThread.detach();
}

bool PerkManager::CharGenSkillSelect(StaticFunctionTag* base, UInt32 Points, Papyrus::TaggedSkills Tags, bool LockPreselected) {
    if (!ValidateTags(Tags, GetTagPoints(), LockPreselected))
        return false;

    m_LevelUp = true;
    std::thread LUMThread(CharGenThread, true);
    LUMThread.detach();
    return true;
}

PerkList PerkManager::GetRankList(BGSPerk* Perk) {
    BGSPerk* iterPerk = Perk;
    PerkList Result;

    bool FoundFirst = false;
    for (int i = 0; ((i < Perk->numRanks) && iterPerk->nextPerk); i++) {
        if ((iterPerk->sound != nullptr) || (strlen(iterPerk->swfPath) > 0) || FoundFirst) {
            if (!FoundFirst) {
                FoundFirst = true;
                i = 0;
            }

            Result.emplace_back(iterPerk);
        }

        iterPerk = iterPerk->nextPerk;
    }

    if (Perk->numRanks != Result.size())
        _LOGWARNING("PerkManager: Perk Chain containing Form 0x%08X is broken!", Perk->formID);

    std::sort(Result.begin(), Result.end(), &RankSorter);
    return Result;
}

void PerkManager::ProcessPerkList(GFxMovieRoot* Root) {
    m_CurrentRoot = Root;
    GFxValue akArgs[7];

    // Menu ID
    akArgs[4].SetNumber(m_ActiveMenuID);

    // Level to Show
    akArgs[5].SetNumber(m_ActiveLevel);

    // Perks and Perk Points
    m_CurrentRoot->CreateArray(&akArgs[0]);
    akArgs[1].SetNumber(m_PerkPoints);

    // Skills and Skill Points
    m_CurrentRoot->CreateArray(&akArgs[2]);
    akArgs[3].SetNumber(m_SkillPoints);
    akArgs[6].SetNumber(0);

    if (m_ActiveList.size() > 0) {
        for (auto iter : m_ActiveList) {
            GFxValue Perk;
            m_CurrentRoot->CreateObject(&Perk);

            GFxHelperFunctions::RegisterString(&Perk, m_CurrentRoot, "text",            iter.Name.c_str());
            GFxHelperFunctions::RegisterString(&Perk, m_CurrentRoot, "Requirements",    iter.Requirements.c_str());
            GFxHelperFunctions::RegisterString(&Perk, m_CurrentRoot, "description",     iter.Description.c_str());
            GFxHelperFunctions::RegisterString(&Perk, m_CurrentRoot, "Path",            iter.Perk->swfPath.c_str());

            if (iter.Perk->numRanks > 1) {
                BGSPerk* firstRank = GetRankList(iter.Perk)[0];
                if (firstRank)
                    Perk.SetMember("Path", &GFxValue(firstRank->swfPath.c_str()));
            }

            GFxHelperFunctions::RegisterBool(&Perk, "IsEligible",   iter.Eligible);
            GFxHelperFunctions::RegisterInt(&Perk,  "Ranks",        iter.Perk->numRanks);
            GFxHelperFunctions::RegisterInt(&Perk,  "FormID",       iter.Perk->formID);
            GFxHelperFunctions::RegisterInt(&Perk,  "Level",        iter.Level);

            akArgs[0].PushBack(&Perk);
        }
    }
    else {
        int PreTagCount = 0;
        for (auto iter : m_SkillList) {
            GFxValue Skill;
            m_CurrentRoot->CreateObject(&Skill);

            float value = GetPermValue((*g_player), iter);
            float baseValue = value;

            if (IsTagEnabled(m_ActiveTags, iter)) {
                GFxHelperFunctions::RegisterBool(&Skill, "IsSelected",  true);
                GFxHelperFunctions::RegisterBool(&Skill, "IsLocked",    m_ActiveTags.LockAll);
                baseValue -= 15;
                PreTagCount += 1;
            }
            else {
                GFxHelperFunctions::RegisterBool(&Skill, "IsSelected", false);
                GFxHelperFunctions::RegisterBool(&Skill, "IsLocked", false);
            }

            BSString DescText;
            TESDescription* DescriptionForm = DYNAMIC_CAST(iter, ActorValueInfo, TESDescription);
            CALL_MEMBER_FN(DescriptionForm, Get)(&DescText, nullptr);

            GFxHelperFunctions::RegisterString(&Skill, m_CurrentRoot, "text",           iter->GetFullName());
            GFxHelperFunctions::RegisterString(&Skill, m_CurrentRoot, "EditorID",       iter->GetEditorID());
            GFxHelperFunctions::RegisterString(&Skill, m_CurrentRoot, "description",    DescText.Get());

            GFxHelperFunctions::RegisterInt(&Skill, "FormID",       iter->formID);
            GFxHelperFunctions::RegisterInt(&Skill, "basevalue",    (int)baseValue);
            GFxHelperFunctions::RegisterInt(&Skill, "value",        (int)value);

            akArgs[2].PushBack(&Skill);
        }

        akArgs[6].SetNumber(PreTagCount);
    }

    m_CurrentRoot->Invoke("root.Menu_mc.qqStart", nullptr, akArgs, 7);
}

int PerkManager::GetSkillPoints(int PlayerLevel) {
    float intMod = 10 + ((*g_player)->actorValueOwner.GetBase(Intelligence) / 2.0);
    intMod += (*g_player)->actorValueOwner.GetValue(SkillOffset);

    return (PlayerLevel % 2 == 0) ? ceilf(intMod) : floorf(intMod);
}

int PerkManager::GetPerkPoints() {
    return 1;
}

int PerkManager::GetTraitPoints() {
    return 2;
}

int PerkManager::GetTagPoints() {
    return 3;
}

// ------------------------------------------------------------------------------------------------
// Threads
// ------------------------------------------------------------------------------------------------

void PerkManager::LevelUpThread() {
    while (((*g_player)->IsInCombat()) || IsBlockingMenuOpen())
        std::this_thread::sleep_for(std::chrono::seconds(1));

    if (m_LevelUp) {
        // Give the "Level Up!" text time to fuck off.
        std::this_thread::sleep_for(std::chrono::seconds(5));
        while (IsBlockingMenuOpen())
            std::this_thread::sleep_for(std::chrono::seconds(1));

        int nextLevel = GetLevel((*g_player));
        while (m_CurrentLevel < nextLevel) {
            m_CurrentLevel += 1;
            DataList PerkList01 = BuildPerkList(m_CurrentLevel);

            int SkillPoints = GetSkillPoints(m_CurrentLevel);
            int PerkPoints = GetPerkPoints();

            // Open Skill Menu
            OpenLevelUpMenu(SkillPoints, m_CurrentLevel);
            std::this_thread::sleep_for(std::chrono::seconds(1));

            while (IsBlockingMenuOpen())
                std::this_thread::sleep_for(std::chrono::milliseconds(100));

            // Open Perk Menu
            OpenLevelUpMenu(PerkList01, PerkPoints, m_CurrentLevel, kMenuID_Perks);
            std::this_thread::sleep_for(std::chrono::seconds(1));

            while (IsBlockingMenuOpen())
                std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        m_LevelUp = false;
    }
}

void PerkManager::CharGenThread(bool AllowDialogueMenu) {
    // Mostly a copy of async_LevelUp, but for SPECIAL + Traits + Tags
    while (((*g_player)->IsInCombat()) || IsBlockingMenuOpen(AllowDialogueMenu))
        std::this_thread::sleep_for(std::chrono::seconds(1));

    if (m_LevelUp) {
        m_CurrentLevel = GetLevel((*g_player));
        DataList TraitList = BuildPerkList(m_CurrentLevel, true);

        int TagPoints = GetTagPoints();
        int TraitPoints = GetTraitPoints();

        BSFixedString LevelUpMenu("SPECIALMenu");
        CALL_MEMBER_FN(*g_uiMessageManager, SendUIMessage)(LevelUpMenu, kMessage_Open);

        // Wait for SPECIAL Menu
        std::this_thread::sleep_for(std::chrono::seconds(1));
        while (IsBlockingMenuOpen(AllowDialogueMenu))
            std::this_thread::sleep_for(std::chrono::seconds(1));

        // Open Tags Menu
        OpenLevelUpMenu(m_DefaultTags, TagPoints, m_CurrentLevel);
        std::this_thread::sleep_for(std::chrono::seconds(1));

        while (IsBlockingMenuOpen(AllowDialogueMenu))
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Open Traits Menu
        OpenLevelUpMenu(TraitList, TraitPoints, m_CurrentLevel, kMenuID_Traits);
        std::this_thread::sleep_for(std::chrono::seconds(1));

        while (IsBlockingMenuOpen(AllowDialogueMenu))
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

        m_LevelUp = false;
    }
}

// ------------------------------------------------------------------------------------------------
// Private
// ------------------------------------------------------------------------------------------------

void PerkManager::OpenLevelUpMenu(DataList ActiveList, int PerkPoints, int PlayerLevel, MenuID kMenuID) {
    m_ActiveList    = ActiveList;
    m_PerkPoints    = PerkPoints;
    m_ActiveLevel   = PlayerLevel;
    m_ActiveMenuID  = kMenuID;

    m_ActiveTags    = m_DefaultTags;
    m_SkillPoints   = 0;

    BSFixedString LevelUpMenu("LevelUpMenu");
    CALL_MEMBER_FN(*g_uiMessageManager, SendUIMessage)(LevelUpMenu, kMessage_Open);
}

void PerkManager::OpenLevelUpMenu(TaggedSkills ActiveTags, int SkillPoints, int PlayerLevel) {
    m_ActiveTags    = ActiveTags;
    m_SkillPoints   = SkillPoints;
    m_ActiveLevel   = PlayerLevel;
    m_ActiveMenuID  = kMenuID_Tags;

    m_ActiveList    = m_DefaultList;
    m_PerkPoints    = 0;

    BSFixedString LevelUpMenu("LevelUpMenu");
    CALL_MEMBER_FN(*g_uiMessageManager, SendUIMessage)(LevelUpMenu, kMessage_Open);
}

void PerkManager::OpenLevelUpMenu(int SkillPoints, int PlayerLevel) {
    m_SkillPoints   = SkillPoints;
    m_ActiveLevel   = PlayerLevel;
    m_ActiveMenuID  = kMenuID_Skills;

    m_ActiveList    = m_DefaultList;
    m_ActiveTags    = m_DefaultTags;
    m_PerkPoints    = 0;

    BSFixedString LevelUpMenu("LevelUpMenu");
    CALL_MEMBER_FN(*g_uiMessageManager, SendUIMessage)(LevelUpMenu, kMessage_Open);
}

bool PerkManager::PerkSorter(PerkData FirstPerk, PerkData SecondPerk) {
    if (FirstPerk.Level != SecondPerk.Level)
        return (FirstPerk.Level < SecondPerk.Level);

    const char* FirstName = FirstPerk.Name.c_str();
    const char* SecondName = SecondPerk.Name.c_str();
    int len = (strlen(FirstName) > strlen(SecondName)) ? strlen(FirstName) : strlen(SecondName);

    for (int i = 0; i < len; i++)
        if (FirstName[i] != SecondName[i])
            return (FirstName[i] < SecondName[i]);

    return (FirstName < SecondName);
}

bool PerkManager::RankSorter(BGSPerk* FirstPerk, BGSPerk* SecondPerk) {
    return (FirstPerk->perkLevel < SecondPerk->perkLevel);
}

std::string PerkManager::RequirementSorter(std::vector<RequirementData> RequirementsList) {
    std::string Result = "Req: ";
    bool AddedItem = false;

    for (auto iterSortList : m_SortStrings) {
        for (auto iterReqList = RequirementsList.begin(); iterReqList != RequirementsList.end(); ) {
            for (auto iterSort : iterSortList) {
                std::size_t index = iterReqList->Default.find(iterSort);
                if (index != std::string::npos) {
                    Result.append((RequirementsList.size() > 1) ? iterReqList->Default.c_str() : iterReqList->End.c_str());
                    iterReqList = RequirementsList.erase(iterReqList);
                    AddedItem   = true;
                    break;
                }
            }

            // We still have Requirements, but no sorting rules for them, add
            if (iterSortList.size() == 0) {
                Result.append((RequirementsList.size() > 1) ? iterReqList->Default.c_str() : iterReqList->End.c_str());
                iterReqList = RequirementsList.erase(iterReqList);
                AddedItem = true;
            }

            if (!AddedItem)
                iterReqList++;

            AddedItem = false;
        }
    }

    if ((Result == "Req: Level 1") || (Result == "Req: Level 2"))
        Result = "Req: --";

    return Result;
}

bool PerkManager::IsFirstValidRank(BGSPerk* Perk) {
    PerkList PerkRanks = GetRankList(Perk);

    for (auto iter : PerkRanks)
        if (!HasPerk((*g_player), iter)) {
            if (Perk == iter)
                return true;
            return false;
        }

    return false;
}

bool PerkManager::IsBlockingMenuOpen(bool AllowDialogueMenu) {
    bool IsMenuOpenDialogue = AllowDialogueMenu ? false :
        (*g_ui)->IsMenuOpen(BSFixedString("DialogueMenu"));

    return (
        ((*g_ui)->numPauseGame >= 1)
        || (*g_ui)->IsMenuOpen(BSFixedString("CookingMenu"))
        || (*g_ui)->IsMenuOpen(BSFixedString("FaderMenu"))
        || (*g_ui)->IsMenuOpen(BSFixedString("FavoritesMenu"))
        || (*g_ui)->IsMenuOpen(BSFixedString("PowerArmorModMenu"))
        || (*g_ui)->IsMenuOpen(BSFixedString("RobotModMenu"))
        || (*g_ui)->IsMenuOpen(BSFixedString("VATSMenu"))
        || (*g_ui)->IsMenuOpen(BSFixedString("WorkshopMenu"))
        || IsMenuOpenDialogue
        );
}

#define CompareTag(CmpSkill) if (CmpSkill->formID == Skill->formID) return Tags.CmpSkill
bool PerkManager::IsTagEnabled(TaggedSkills Tags, ActorValueInfo * Skill) {
    CompareTag(Barter);
    CompareTag(EnergyWeapons);
    CompareTag(Explosives);
    CompareTag(Guns);
    CompareTag(Lockpick);
    CompareTag(Medicine);
    CompareTag(MeleeWeapons);
    CompareTag(Repair);
    CompareTag(Science);
    CompareTag(Sneak);
    CompareTag(Speech);
    CompareTag(Survival);
    CompareTag(Unarmed);
    return false;
}

#define ErrorTags(string) string = "<font color=\'#A9A9A9\'>" + string + "</font>"
DataList PerkManager::BuildPerkList(int ForLevel, bool Trait) {
    PerkList    MasterList;
    DataList    PerksInvalid;
    DataList    PerksValid;

    Actor*      actor = (*g_player);
    bool        Skipped;

    if (Trait)
        MasterList = m_TraitList;
    else MasterList = m_PerkList;

    for (auto iter : MasterList) {
        if (HasPerk(actor, iter))
            continue;

        if (iter->numRanks > 1)
            if (!IsFirstValidRank(iter))
                continue;

        std::string PerkName = iter->GetFullName();
        if (PerkName.empty())
            continue;

        BSString DescriptionText;
        TESDescription* DescForm = DYNAMIC_CAST(iter, BGSPerk, TESDescription);
        CALL_MEMBER_FN(DescForm, Get)(&DescriptionText, nullptr);

        std::string                     Requirement;
        std::vector<RequirementData>    Requirements;

        int PerkLevel = max(iter->perkLevel, 1);

        Requirement = "Level " + std::to_string(PerkLevel);
        if (ForLevel < PerkLevel)
            ErrorTags(Requirement);
        Requirements.emplace_back(RequirementData{ Requirement, Requirement + ", " });

        Skipped = false;

        Condition* PerkConditions = iter->condition;
        while (PerkConditions && !Skipped) {
            int             kFunction       = PerkConditions->functionId;
            int             kCompareOp      = PerkConditions->comparisonType.op;
            int             kCompareFlag    = PerkConditions->comparisonType.flags;

            float           CompareValue    = PerkConditions->compareValue;
            float           PlayerValue     = 0.0;

            ActorValueInfo* ActorValue      = nullptr;
            TESGlobal*      GlobalValue     = nullptr;

            int             PlayerSex       = 0;

            Requirement.clear();

            switch (kFunction) {
            case Condition::kFunction_GetValue:
            case Condition::kFunction_GetBaseValue:
            case Condition::kFunction_GetPermanentValue:
                ActorValue = DYNAMIC_CAST(PerkConditions->param1.form, TESForm, ActorValueInfo);
                Requirement += ActorValue->GetFullName();

                switch (kFunction) {
                case Condition::kFunction_GetValue:
                    PlayerValue = actor->actorValueOwner.GetValue(ActorValue);
                    break;

                case Condition::kFunction_GetBaseValue:
                    PlayerValue = actor->actorValueOwner.GetBase(ActorValue);
                    break;

                case Condition::kFunction_GetPermanentValue:
                    PlayerValue = GetPermValue(actor, ActorValue);
                    break;
                }

                switch (kCompareOp) {
                case Condition::kCompareOp_Equal:
                    Requirement += " = ";
                    Requirement += std::to_string(int(CompareValue));

                    if (!(PlayerValue == CompareValue))
                        ErrorTags(Requirement);
                    break;

                case Condition::kCompareOp_NotEqual:
                    Requirement += " != ";
                    Requirement += std::to_string(int(CompareValue));

                    if (!(PlayerValue != CompareValue))
                        ErrorTags(Requirement);
                    break;

                case Condition::kCompareOp_Greater:
                    Requirement += " &gt; ";
                    Requirement += std::to_string(int(CompareValue));

                    if (!(PlayerValue > CompareValue))
                        ErrorTags(Requirement);
                    break;

                case Condition::kCompareOp_GreaterEqual:
                    Requirement += " ";
                    Requirement += std::to_string(int(CompareValue));

                    if (!(PlayerValue >= CompareValue))
                        ErrorTags(Requirement);
                    break;

                case Condition::kCompareOp_Less:
                    Requirement += " &lt; ";
                    Requirement += std::to_string(int(CompareValue));

                    if (!(PlayerValue < CompareValue))
                        ErrorTags(Requirement);
                    break;

                case Condition::kCompareOp_LessEqual:
                    Requirement += " &lt;= ";
                    Requirement += std::to_string(int(CompareValue));

                    if (!(PlayerValue <= CompareValue))
                        ErrorTags(Requirement);
                    break;
                }

                switch (kCompareFlag) {
                case Condition::kCompareFlag_Or:
                    if (PerkConditions->next) {
                        Requirements.emplace_back(RequirementData{ Requirement, Requirement + " or " });
                        break;
                    }

                default:
                    Requirements.emplace_back(RequirementData{ Requirement, Requirement + ", " });
                    break;
                }

                break;

            case Condition::kFunction_GetIsSex:
                PlayerSex = CALL_MEMBER_FN(DYNAMIC_CAST(actor->baseForm, TESForm, TESNPC), GetSex)();

                switch (kCompareOp) {
                case Condition::kCompareOp_Equal:
                    if (CompareValue > 0) {
                        if (!(PlayerSex == PerkConditions->param1.u32)) {
                            Skipped = true;
                            continue;
                        }
                    }
                    else
                        if (PlayerSex == PerkConditions->param1.u32) {
                            Skipped = true;
                            continue;
                        }
                    break;

                case Condition::kCompareOp_NotEqual:
                    if (CompareValue > 0) {
                        if (!(PlayerSex != PerkConditions->param1.u32)) {
                            Skipped = true;
                            continue;
                        }
                    }
                    else
                        if (PlayerSex != PerkConditions->param1.u32) {
                            Skipped = true;
                            continue;
                        }
                    break;

                default:
                    break;
                }

                break;

            case Condition::kFunction_GetGlobalValue:
                GlobalValue = DYNAMIC_CAST(PerkConditions->param1.form, TESForm, TESGlobal);

                switch (kCompareOp) {
                case Condition::kCompareOp_Equal:
                    if (!(GlobalValue->value == CompareValue)) {
                        Skipped = true;
                        continue;
                    }
                    break;

                case Condition::kCompareOp_NotEqual:
                    if (!(GlobalValue->value != CompareValue)) {
                        Skipped = true;
                        continue;
                    }
                    break;

                case Condition::kCompareOp_Greater:
                    if (!(GlobalValue->value > CompareValue)) {
                        Skipped = true;
                        continue;
                    }
                    break;

                case Condition::kCompareOp_GreaterEqual:
                    if (!(GlobalValue->value >= CompareValue)) {
                        Skipped = true;
                        continue;
                    }
                    break;

                case Condition::kCompareOp_Less:
                    if (!(GlobalValue->value < CompareValue)) {
                        Skipped = true;
                        continue;
                    }
                    break;

                case Condition::kCompareOp_LessEqual:
                    if (!(GlobalValue->value <= CompareValue)) {
                        Skipped = true;
                        continue;
                    }
                    break;
                }

                break;

            case Condition::kFunction_HasPerk:
                BGSPerk* CondPerk = DYNAMIC_CAST(PerkConditions->param1.form, TESForm, BGSPerk);

                switch (kCompareOp) {
                case Condition::kCompareOp_Equal:
                    Requirement += (CompareValue) ? "" : " does not have ";
                    Requirement += CondPerk->GetFullName();
                    Requirement += " perk";

                    if (!(HasPerk(actor, CondPerk) == (CompareValue > 0)))
                        ErrorTags(Requirement);
                    break;

                case Condition::kCompareOp_NotEqual:
                    Requirement += (CompareValue) ? " does not have " : "";
                    Requirement += CondPerk->GetFullName();
                    Requirement += " perk";

                    if (!(HasPerk(actor, CondPerk) != (CompareValue > 0)))
                        ErrorTags(Requirement);
                    break;
                }

                switch (kCompareFlag) {
                case Condition::kCompareFlag_Or:
                    if (PerkConditions->next) {
                        Requirements.emplace_back(RequirementData{ Requirement, Requirement + " or " });
                        break;
                    }
                default:
                    Requirements.emplace_back(RequirementData{ Requirement, Requirement + ", " });
                    break;
                }

                break;
            }

            PerkConditions = PerkConditions->next;
        }

        if (Skipped)
            continue;

        Requirement = RequirementSorter(Requirements);
        std::size_t index = Requirement.find("A9A9A9");
        if (index != std::string::npos)
            PerksInvalid.emplace_back(PerkData{ iter, PerkLevel, PerkName, DescriptionText.Get(), Requirement, false });
        else
            PerksValid.emplace_back(PerkData{ iter, PerkLevel, PerkName, DescriptionText.Get(), Requirement, true });
    }

    std::sort(PerksInvalid.begin(), PerksInvalid.end(), &PerkSorter);
    std::sort(PerksValid.begin(), PerksValid.end(), &PerkSorter);

    PerksValid.insert(PerksValid.end(), PerksInvalid.begin(), PerksInvalid.end());
    return PerksValid;
}

bool PerkManager::ValidateTags(Papyrus::TaggedSkills Tags, int Points, bool LockPreselected) {
    if (!Tags.IsNone()) {
        bool value;

        m_ActiveTags = m_DefaultTags;
        m_ActiveTags.LockAll = LockPreselected;

        Tags.Get("Barter", &value);
        m_ActiveTags.Barter = value;
        if (value)
            Points -= 1;

        Tags.Get("EnergyWeapons", &value);
        m_ActiveTags.EnergyWeapons = value;
        if (value)
            Points -= 1;

        Tags.Get("Explosives", &value);
        m_ActiveTags.Explosives = value;
        if (value)
            Points -= 1;

        Tags.Get("Guns", &value);
        m_ActiveTags.Guns = value;
        if (value)
            Points -= 1;

        Tags.Get("Lockpick", &value);
        m_ActiveTags.Lockpick = value;
        if (value)
            Points -= 1;

        Tags.Get("Medicine", &value);
        m_ActiveTags.Medicine = value;
        if (value)
            Points -= 1;

        Tags.Get("MeleeWeapons", &value);
        m_ActiveTags.MeleeWeapons = value;
        if (value)
            Points -= 1;

        Tags.Get("Repair", &value);
        m_ActiveTags.Repair = value;
        if (value)
            Points -= 1;

        Tags.Get("Science", &value);
        m_ActiveTags.Science = value;
        if (value)
            Points -= 1;

        Tags.Get("Sneak", &value);
        m_ActiveTags.Sneak = value;
        if (value)
            Points -= 1;

        Tags.Get("Speech", &value);
        m_ActiveTags.Speech = value;
        if (value)
            Points -= 1;

        Tags.Get("Survival", &value);
        m_ActiveTags.Survival = value;
        if (value)
            Points -= 1;

        Tags.Get("Unarmed", &value);
        m_ActiveTags.Unarmed = value;
        if (value)
            Points -= 1;

        if (Points < 0)
            return false;
    }

    else
        m_ActiveTags = m_PlayerTags;

    return true;
}

// ------------------------------------------------------------------------------------------------
// Initial
// ------------------------------------------------------------------------------------------------

std::vector<std::vector<std::string>>   PerkManager::m_SortStrings;
DataList                                PerkManager::m_ActiveList;
DataList                                PerkManager::m_DefaultList;

TaggedSkills                            PerkManager::m_PlayerTags;
TaggedSkills                            PerkManager::m_ActiveTags;
TaggedSkills                            PerkManager::m_DefaultTags;

GFxMovieRoot*                           PerkManager::m_CurrentRoot;

int                                     PerkManager::m_CurrentLevel;
bool                                    PerkManager::m_LevelUp;

int                                     PerkManager::m_PerkPoints;
int                                     PerkManager::m_SkillPoints;
int                                     PerkManager::m_ActiveLevel;
int                                     PerkManager::m_ActiveMenuID;