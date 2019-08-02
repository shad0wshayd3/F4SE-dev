#include "Perks.h"

#include "f4se_globals/Globals.h"

#include "f4se/GameData.h"

#include <algorithm>
#include <thread>

namespace Perks {
    std::vector<BGSPerk*> EntirePerkList;
    std::vector<BGSPerk*> MasterPerkList;
    std::vector<BGSPerk*> MasterTraitList;
    std::vector<PerkData> CurrentList;
    std::vector<PerkData> DefaultList;
    
    std::vector<std::string> searchStrings;
    std::vector<ActorValueInfo*> SkillList;

    TaggedSkills PlayerTags;
    TaggedSkills DisplayTags;
    TaggedSkills DefaultTags;

    int PlayerLevel = 1;
    bool NeedLevelUp;

    GFxMovieRoot* CurrentRoot;

    int PerkPoints;
    int SkillPoints;
    int DisplayLevel;
    int MenuID; // 1: Skills, 2: Perks, 3: Traits, 4: Tag Skills

    void OpenLevelUpMenu(std::vector<PerkData> cList, int pPoints, int sPoints, int mID, int pLevel, TaggedSkills Tags) {
        CurrentList     = cList;
        PerkPoints      = pPoints;
        SkillPoints     = sPoints;
        MenuID          = mID;
        DisplayLevel    = pLevel;
        DisplayTags     = Tags;

        BSFixedString LevelUpMenu("LevelUpMenu");
        CALL_MEMBER_FN(*g_uiMessageManager, SendUIMessage)(LevelUpMenu, kMessage_Open);
    }

    bool isInMenuMode(bool AllowDialogueMenu = false) {
        bool dialogueMenuOpen = AllowDialogueMenu ? false : (*g_ui)->IsMenuOpen(BSFixedString("DialogueMenu"));

        return (
            ((*g_ui)->numPauseGame >= 1)
            || (*g_ui)->IsMenuOpen(BSFixedString("CookingMenu"))
            || (*g_ui)->IsMenuOpen(BSFixedString("FaderMenu"))
            || (*g_ui)->IsMenuOpen(BSFixedString("FavoritesMenu"))
            || (*g_ui)->IsMenuOpen(BSFixedString("PowerArmorModMenu"))
            || (*g_ui)->IsMenuOpen(BSFixedString("RobotModMenu"))
            || (*g_ui)->IsMenuOpen(BSFixedString("VATSMenu"))
            || (*g_ui)->IsMenuOpen(BSFixedString("WorkshopMenu"))
            || dialogueMenuOpen
            );
    }

    bool isSkillTag(TaggedSkills Tags, std::string EDID) {
        if (EDID == "Barter")
            return Tags.Barter;
        else if (EDID == "EnergyWeapons")
            return Tags.EnergyWeapons;
        else if (EDID == "Explosives")
            return Tags.Explosives;
        else if (EDID == "Guns")
            return Tags.Guns;
        else if (EDID == "Lockpick")
            return Tags.Lockpick;
        else if (EDID == "Medicine")
            return Tags.Medicine;
        else if (EDID == "MeleeWeapons")
            return Tags.MeleeWeapons;
        else if (EDID == "Repair")
            return Tags.Repair;
        else if (EDID == "Science")
            return Tags.Science;
        else if (EDID == "Sneak")
            return Tags.Sneak;
        else if (EDID == "Speech")
            return Tags.Speech;
        else if (EDID == "Survival")
            return Tags.Survival;
        else if (EDID == "Unarmed")
            return Tags.Unarmed;
        return false;
    }

    void BuildList() {
        EntirePerkList.clear();
        MasterPerkList.clear();
        MasterTraitList.clear();
        searchStrings.clear();
        SkillList.clear();

        tArray<BGSPerk*> Perks = (*g_dataHandler)->arrPERK;
        for (int i = 0; i < Perks.count; i++) {
            BGSPerk* thisPerk = Perks[i];

            if (thisPerk) {
                if (thisPerk->playable == 1) {
                    if (thisPerk->trait == 1) {
                        auto result = std::find(MasterTraitList.begin(), MasterTraitList.end(), thisPerk);
                        if (result == MasterTraitList.end()) {
                            MasterTraitList.emplace_back(thisPerk);
                            EntirePerkList.emplace_back(thisPerk);
                            continue;
                        }
                    }

                    auto result = std::find(MasterPerkList.begin(), MasterPerkList.end(), thisPerk);
                    if (result == MasterPerkList.end()) {
                        MasterPerkList.emplace_back(thisPerk);
                        EntirePerkList.emplace_back(thisPerk);
                    }
                }
            }
        }

        // Display sort order
        searchStrings.emplace_back("Level");
        searchStrings.emplace_back(g_Data.Strength->fullName.name.c_str());
        searchStrings.emplace_back(g_Data.Perception->fullName.name.c_str());
        searchStrings.emplace_back(g_Data.Endurance->fullName.name.c_str());
        searchStrings.emplace_back(g_Data.Charisma->fullName.name.c_str());
        searchStrings.emplace_back(g_Data.Intelligence->fullName.name.c_str());
        searchStrings.emplace_back(g_Data.Agility->fullName.name.c_str());
        searchStrings.emplace_back(g_Data.Luck->fullName.name.c_str());
        searchStrings.emplace_back(g_Data.Barter->fullName.name.c_str());
        searchStrings.emplace_back(g_Data.EnergyWeapons->fullName.name.c_str());
        searchStrings.emplace_back(g_Data.Explosives->fullName.name.c_str());
        searchStrings.emplace_back(g_Data.Guns->fullName.name.c_str());
        searchStrings.emplace_back(g_Data.Lockpick->fullName.name.c_str());
        searchStrings.emplace_back(g_Data.Medicine->fullName.name.c_str());
        searchStrings.emplace_back(g_Data.MeleeWeapons->fullName.name.c_str());
        searchStrings.emplace_back(g_Data.Science->fullName.name.c_str());
        searchStrings.emplace_back(g_Data.Sneak->fullName.name.c_str());
        searchStrings.emplace_back(g_Data.Speech->fullName.name.c_str());
        searchStrings.emplace_back(g_Data.Survival->fullName.name.c_str());
        searchStrings.emplace_back(g_Data.Unarmed->fullName.name.c_str());

        // Vector of Skills
        SkillList.emplace_back(g_Data.Barter);
        SkillList.emplace_back(g_Data.EnergyWeapons);
        SkillList.emplace_back(g_Data.Explosives);
        SkillList.emplace_back(g_Data.Guns);
        SkillList.emplace_back(g_Data.Lockpick);
        SkillList.emplace_back(g_Data.Medicine);
        SkillList.emplace_back(g_Data.MeleeWeapons);
        SkillList.emplace_back(g_Data.Repair);
        SkillList.emplace_back(g_Data.Science);
        SkillList.emplace_back(g_Data.Sneak);
        SkillList.emplace_back(g_Data.Speech);
        SkillList.emplace_back(g_Data.Survival);
        SkillList.emplace_back(g_Data.Unarmed);
    }

    std::string ReqSorter(std::vector<std::string> thisVector) {
        std::string result = "Req: ";
        bool bUseSkills = false;

    resetSearch:
        for (int i = 0; i < thisVector.size(); i++) {
            std::string thisString = thisVector.at(i);

            for (int j = 0; j < searchStrings.size(); j++) {
                std::string searchString = searchStrings.at(j);
                std::size_t place = thisString.find(searchString);
                if ((place == 0 || place == 22) && (bUseSkills || j <= 7)) {
                    thisVector.erase(thisVector.begin() + i);
                    result += thisString;
                    goto resetSearch;
                }
            }
        }

    resetCheck:
        if (thisVector.size() > 0) {
            if (bUseSkills) {
                result += thisVector.at(0);
                thisVector.erase(thisVector.begin());
                goto resetCheck;
            }

            bUseSkills = true;
            goto resetSearch;
        }

        if ((result.size() - 1) == result.find_last_of(", ")) {
            result.pop_back();
            result.pop_back();
        }

        else if ((result.size() - 1) == result.find_last_of(", </font>")) {
            for (int i = 0; i < 9; i++)
                result.pop_back();
            result += "</font>";
        }

        else if ((result.size() - 1) == result.find_last_of(" or </font>")) {
            for (int i = 0; i < 11; i++)
                result.pop_back();
            result += "</font>";
        }

        else if ((result.size() - 1) == result.find_last_of(" or ")) {
            for (int i = 0; i < 4; i++)
                result.pop_back();
        }

        if ((result == "Req: Level 1") || (result == "Req: Level 2"))
            result = "Req: --";

        return result;
    }

    bool PerkSorter(PerkData firstPerk, PerkData secondPerk) {
        if (firstPerk.Level != secondPerk.Level)
            return (firstPerk.Level < secondPerk.Level);

        const char* firstName = firstPerk.Name.c_str();
        const char* secondName = secondPerk.Name.c_str();
        int len = (strlen(firstName) > strlen(secondName)) ? strlen(firstName) : strlen(secondName);

        for (int i = 0; i < len; i++)
            if (firstName[i] != secondName[i])
                return (firstName[i] < secondName[i]);

        return (firstName < secondName);
    }

    bool RankSorter(BGSPerk* firstPerk, BGSPerk* secondPerk) {
        return (firstPerk->perkLevel < secondPerk->perkLevel);
    }

    std::vector<BGSPerk*> GetRankList(BGSPerk* perk) {
        BGSPerk* searchPerk = perk;
        bool reset = false;

        std::vector<BGSPerk*> result;
        for (int i = 0; ((i < perk->numRanks) && searchPerk->nextPerk); i++) {
            if ((searchPerk->sound != nullptr) || (strlen(searchPerk->swfPath) > 0) || reset) {
                if (!reset) { reset = true; i = 0; }
                result.emplace_back(searchPerk);
            }

            searchPerk = searchPerk->nextPerk;
        }

        if (perk->numRanks != result.size())
            _LOGWARNING("Perk Chain containing Form 0x%08X is broken!", searchPerk->formID);

        std::sort(result.begin(), result.end(), &RankSorter);
        return result;
    }

    bool isFirstAvailableRank(BGSPerk* perk) {
        std::vector<BGSPerk*> perkRanks = GetRankList(perk);

        for (auto thisRank : perkRanks)
            if (!HasPerk((*g_player), thisRank)) {
                if (perk == thisRank)
                    return true;

                return false;
            }

        return false;
    }

    std::vector<PerkData> BuildFormattedPerkList(Actor* thisActor, int thisLevel, bool trait) {
        int thisSex = CALL_MEMBER_FN(DYNAMIC_CAST(thisActor->baseForm, TESForm, TESNPC), GetSex)();

        std::vector<std::string> Requirements;
        std::vector<PerkData> Unavailable;
        std::vector<PerkData> Available;

        PerkData newPerkData;
        BSString description;

        TESDescription* perkDescription;
        Condition* PerkConditions;

        std::string Error = "<font color=\'#A9A9A9\'>";
        std::string CloseError = "</font>";

        std::string PerkName;
        std::string Requirement;
        std::string Required;
        std::size_t place;
        int PerkLevel;

        std::vector<BGSPerk*> MasterList;
        if (trait)
            MasterList = MasterTraitList;
        else MasterList = MasterPerkList;

        for (int i = 0; i < MasterList.size(); i++) {
            BGSPerk* thisPerk = MasterList.at(i);

            if (thisPerk->hidden == 1)
                goto skipPerk;

            if (thisPerk->numRanks == 0)
                goto skipPerk;

            if (HasPerk(thisActor, thisPerk))
                goto skipPerk;

            if (thisPerk->numRanks > 1)
                if (!isFirstAvailableRank(thisPerk))
                    goto skipPerk;

            Requirements.clear();

            PerkName = thisPerk->fullName.name.c_str();
            if (PerkName == "")
                goto skipPerk;

            perkDescription = DYNAMIC_CAST(thisPerk, BGSPerk, TESDescription);
            CALL_MEMBER_FN(perkDescription, Get)(&description, nullptr);
            PerkLevel = max(thisPerk->perkLevel, 1);

            PerkConditions = thisPerk->condition;
            while (PerkConditions) {
                int thisFunction = PerkConditions->functionId;
                int thisOperator = PerkConditions->comparisonType.op;
                int thisFlag = PerkConditions->comparisonType.flags;

                float thisOutput = PerkConditions->compareValue;

                TESForm* FormParam1 = PerkConditions->param1.form;
                int IntParam1 = PerkConditions->param1.u32;

                ActorValueInfo* ComparisonAV = nullptr;
                TESGlobal* ComparisonGlobal = nullptr;

                float ComparisonValue = 0.0;
                Requirement = "";

                switch (thisFunction) {
                case Condition::kFunction_GetPermanentValue:
                case Condition::kFunction_GetBaseValue:
                case Condition::kFunction_GetValue:
                    ComparisonAV = DYNAMIC_CAST(FormParam1, TESForm, ActorValueInfo);
                    Requirement += ComparisonAV->fullName.name.c_str();

                    if (thisFunction == Condition::kFunction_GetPermanentValue)
                        ComparisonValue = GetPermValue((*g_player), ComparisonAV);

                    if (thisFunction == Condition::kFunction_GetBaseValue)
                        ComparisonValue = (*g_player)->actorValueOwner.GetBase(ComparisonAV);

                    if (thisFunction == Condition::kFunction_GetValue)
                        ComparisonValue = (*g_player)->actorValueOwner.GetValue(ComparisonAV);

                    switch (thisOperator) {
                    case Condition::kCompareOp_Equal:
                        Requirement += " = ";
                        Requirement += std::to_string(int(thisOutput));

                        if (ComparisonValue != thisOutput)
                            Requirement = Error + Requirement + CloseError;

                        break;

                    case Condition::kCompareOp_NotEqual:
                        Requirement += " != ";
                        Requirement += std::to_string(int(thisOutput));

                        if (ComparisonValue == thisOutput)
                            Requirement = Error + Requirement + CloseError;

                        break;

                    case Condition::kCompareOp_Greater:
                        Requirement += " &gt; ";
                        Requirement += std::to_string(int(thisOutput));

                        if (ComparisonValue <= thisOutput)
                            Requirement = Error + Requirement + CloseError;

                        break;

                    case Condition::kCompareOp_GreaterEqual:
                        Requirement += " ";
                        Requirement += std::to_string(int(thisOutput));

                        if (ComparisonValue < thisOutput)
                            Requirement = Error + Requirement + CloseError;

                        break;

                    case Condition::kCompareOp_Less:
                        Requirement += " &lt; ";
                        Requirement += std::to_string(int(thisOutput));

                        if (ComparisonValue >= thisOutput)
                            Requirement = Error + Requirement + CloseError;

                        break;

                    case Condition::kCompareOp_LessEqual:
                        Requirement += " &lt;= ";
                        Requirement += std::to_string(int(thisOutput));

                        if (ComparisonValue > thisOutput)
                            Requirement = Error + Requirement + CloseError;

                        break;
                    }

                    if ((thisFlag == Condition::kCompareFlag_Or) && PerkConditions->next)
                        Requirements.emplace_back(Requirement + " or ");
                    else Requirements.emplace_back(Requirement + ", ");
                    break;

                case Condition::kFunction_GetGlobalValue:
                    ComparisonGlobal = DYNAMIC_CAST(FormParam1, TESForm, TESGlobal);

                    switch (thisOperator) {
                    case Condition::kCompareOp_Equal:
                        if (ComparisonGlobal->value != thisOutput)
                            goto skipPerk;

                        break;

                    case Condition::kCompareOp_NotEqual:
                        if (ComparisonGlobal->value == thisOutput)
                            goto skipPerk;

                        break;

                    case Condition::kCompareOp_Greater:
                        if (ComparisonGlobal->value <= thisOutput)
                            goto skipPerk;

                        break;

                    case Condition::kCompareOp_GreaterEqual:
                        if (ComparisonGlobal->value < thisOutput)
                            goto skipPerk;

                        break;

                    case Condition::kCompareOp_Less:
                        if (ComparisonGlobal->value >= thisOutput)
                            goto skipPerk;

                        break;

                    case Condition::kCompareOp_LessEqual:
                        if (ComparisonGlobal->value > thisOutput)
                            goto skipPerk;

                        break;
                    }
                    break;

                case Condition::kFunction_GetIsSex:
                    if (thisSex != (IntParam1 ^ int(thisOutput) ^ (thisOperator != Condition::kCompareOp_Equal)))
                        (void)0;
                    else goto skipPerk;
                    break;

                case Condition::kFunction_HasPerk:
                    BGSPerk* condPerk = DYNAMIC_CAST(FormParam1, TESForm, BGSPerk);
                    Requirement = "";

                    switch (thisOperator) {
                    case Condition::kCompareOp_Equal:
                        Requirement += condPerk->fullName.name.c_str();
                        Requirement += " perk, ";

                        if (!HasPerk(thisActor, condPerk))
                            Requirement = Error + Requirement + CloseError;

                        Requirements.emplace_back(Requirement);
                        break;

                    case Condition::kCompareOp_NotEqual:
                        if (HasPerk(thisActor, condPerk))
                            goto skipPerk;

                        break;
                    }
                    break;
                }

                PerkConditions = PerkConditions->next;
            }

            Requirement = "Level " + std::to_string(PerkLevel);
            if (thisLevel < PerkLevel)
                Requirement = Error + Requirement + CloseError;
            Requirements.emplace(Requirements.begin(), Requirement + ", ");

            Required = ReqSorter(Requirements);

            newPerkData.Perk = thisPerk;
            newPerkData.Name = PerkName;
            newPerkData.Level = PerkLevel;
            newPerkData.Description = description.Get();
            newPerkData.Requirements = Required;
            newPerkData.Eligible = true;

            place = Required.find("A9A9A9");
            if (place == std::string::npos)
                Available.emplace_back(newPerkData);
            else {
                newPerkData.Eligible = false;
                Unavailable.emplace_back(newPerkData);
            }

        skipPerk:
            (void)0;
        }

        std::sort(Unavailable.begin(), Unavailable.end(), &PerkSorter);
        std::sort(Available.begin(), Available.end(), &PerkSorter);

        for (int i = 0; i < Unavailable.size(); i++)
            Available.emplace_back(Unavailable[i]);

        return Available;
    }

    void PopulatePerkEntry(GFxValue* destination, GFxMovieRoot* root, PerkData thisPerk) {
        GFxValue Perk;

        root->CreateObject(&Perk);
        GFxHelperFunctions::RegisterString(&Perk, root, "text", thisPerk.Name.c_str());
        GFxHelperFunctions::RegisterString(&Perk, root, "Requirements", thisPerk.Requirements.c_str());
        GFxHelperFunctions::RegisterString(&Perk, root, "description", thisPerk.Description.c_str());
        GFxHelperFunctions::RegisterString(&Perk, root, "Path", thisPerk.Perk->swfPath.c_str());

        if (thisPerk.Perk->numRanks > 1) {
            BGSPerk* firstRank = GetRankList(thisPerk.Perk)[0];
            if (firstRank)
                Perk.SetMember("Path", &GFxValue(firstRank->swfPath.c_str()));
        }

        GFxHelperFunctions::RegisterBool(&Perk, "IsEligible", thisPerk.Eligible);

        GFxHelperFunctions::RegisterInt(&Perk, "Ranks", thisPerk.Perk->numRanks);
        GFxHelperFunctions::RegisterInt(&Perk, "FormID", thisPerk.Perk->formID);
        GFxHelperFunctions::RegisterInt(&Perk, "Level", thisPerk.Level);

        destination->PushBack(&Perk);
    }

    void ProcessPerkList(GFxMovieRoot* root, std::vector<PerkData> PerkList) {
        CurrentRoot = root;
        GFxValue akArgs[7];

        // Menu ID
        akArgs[4].SetNumber(MenuID);
        akArgs[5].SetNumber(DisplayLevel);

        // Perks and Perk Points
        root->CreateArray(&akArgs[0]);
        akArgs[1].SetNumber(PerkPoints);

        // Skills and Skill Points
        root->CreateArray(&akArgs[2]);
        akArgs[3].SetNumber(SkillPoints);
        akArgs[6].SetNumber(0);

        if (PerkList.size() != 0) {
            for (auto thisPerk : PerkList) {
                PopulatePerkEntry(&akArgs[0], root, thisPerk);
            }
        }
        else {
            int PreTagCount = 0;
            for (auto ActorValue : SkillList) {
                GFxValue Skill;
                root->CreateObject(&Skill);

                std::string EDID = ActorValue->GetEditorID();

                float value = GetPermValue((*g_player), ActorValue);
                float baseValue = value;

                if (isSkillTag(DisplayTags, EDID)) {
                    GFxHelperFunctions::RegisterBool(&Skill, "IsSelected", true);
                    GFxHelperFunctions::RegisterBool(&Skill, "IsLocked", DisplayTags.LockAll);
                    baseValue -= 15;
                    PreTagCount += 1;
                }
                else {
                    GFxHelperFunctions::RegisterBool(&Skill, "IsSelected", false);
                    GFxHelperFunctions::RegisterBool(&Skill, "IsLocked", false);
                }

                BSString description;
                TESDescription* avDesc = DYNAMIC_CAST(ActorValue, ActorValueInfo, TESDescription);
                CALL_MEMBER_FN(avDesc, Get)(&description, nullptr);

                GFxHelperFunctions::RegisterString(&Skill, root, "text", ActorValue->fullName.name);
                GFxHelperFunctions::RegisterString(&Skill, root, "EditorID", EDID.c_str());
                GFxHelperFunctions::RegisterString(&Skill, root, "description", description.Get());

                GFxHelperFunctions::RegisterInt(&Skill, "FormID", ActorValue->formID);
                GFxHelperFunctions::RegisterInt(&Skill, "basevalue", (int)baseValue);
                GFxHelperFunctions::RegisterInt(&Skill, "value", (int)value);

                akArgs[2].PushBack(&Skill);
            }

            akArgs[6].SetNumber(PreTagCount);
        }

        root->Invoke("root.Menu_mc.qqStart", nullptr, akArgs, 7);
    }

    bool CheckTagSelection(int Points, bool LockPreselect, Papyrus::TaggedSkills Tags) {
        if (!Tags.IsNone()) {
            bool value;

            DisplayTags = DefaultTags;
            DisplayTags.LockAll = LockPreselect;

            Tags.Get("Barter", &value);
            DisplayTags.Barter = value;
            if (value)
                Points -= 1;

            Tags.Get("EnergyWeapons", &value);
            DisplayTags.EnergyWeapons = value;
            if (value)
                Points -= 1;

            Tags.Get("Explosives", &value);
            DisplayTags.Explosives = value;
            if (value)
                Points -= 1;

            Tags.Get("Guns", &value);
            DisplayTags.Guns = value;
            if (value)
                Points -= 1;

            Tags.Get("Lockpick", &value);
            DisplayTags.Lockpick = value;
            if (value)
                Points -= 1;

            Tags.Get("Medicine", &value);
            DisplayTags.Medicine = value;
            if (value)
                Points -= 1;

            Tags.Get("MeleeWeapons", &value);
            DisplayTags.MeleeWeapons = value;
            if (value)
                Points -= 1;

            Tags.Get("Repair", &value);
            DisplayTags.Repair = value;
            if (value)
                Points -= 1;

            Tags.Get("Science", &value);
            DisplayTags.Science = value;
            if (value)
                Points -= 1;

            Tags.Get("Sneak", &value);
            DisplayTags.Sneak = value;
            if (value)
                Points -= 1;

            Tags.Get("Speech", &value);
            DisplayTags.Speech = value;
            if (value)
                Points -= 1;

            Tags.Get("Survival", &value);
            DisplayTags.Survival = value;
            if (value)
                Points -= 1;

            Tags.Get("Unarmed", &value);
            DisplayTags.Unarmed = value;
            if (value)
                Points -= 1;

            if (Points < 0)
                return false;
        }

        else
            DisplayTags = PlayerTags;

        return true;
    }

    void ShowSkillsMenu(StaticFunctionTag* base, UInt32 Points) {
        OpenLevelUpMenu(DefaultList, 0, Points, 1, GetLevel((*g_player)), DefaultTags);
    }

    void ShowPerksMenu(StaticFunctionTag* base, UInt32 Points) {
        int pLevel = GetLevel((*g_player));
        OpenLevelUpMenu(BuildFormattedPerkList((*g_player), pLevel, false), Points, 0, 2, pLevel, DefaultTags);
    }

    void ShowTraitsMenu(StaticFunctionTag* base, UInt32 Points) {
        int pLevel = GetLevel((*g_player));
        OpenLevelUpMenu(BuildFormattedPerkList((*g_player), pLevel, true), Points, 0, 3, pLevel, DefaultTags);
    }

    bool ShowTagMenu(StaticFunctionTag* base, UInt32 Points, bool LockPreselect, Papyrus::TaggedSkills Tags) {
        if (!CheckTagSelection(Points, LockPreselect, Tags))
            return false;

        OpenLevelUpMenu(DefaultList, 0, Points, 4, GetLevel((*g_player)), DisplayTags);
        return true;
    }

    void DoLevelUp() {
        while (((*g_player)->IsInCombat()) || isInMenuMode())
            std::this_thread::sleep_for(std::chrono::seconds(1));

        if (NeedLevelUp) {
            // Give the "Level Up!" text time to fuck off.
            std::this_thread::sleep_for(std::chrono::seconds(5));
            while (isInMenuMode())
                std::this_thread::sleep_for(std::chrono::seconds(1));

            int nextLevel = GetLevel((*g_player));
            while (PlayerLevel < nextLevel) {
                PlayerLevel += 1;
                std::vector<PerkData> pList = BuildFormattedPerkList((*g_player), PlayerLevel, false);

                int sPoints = Calculate::SkillPoints(PlayerLevel);
                int pPoints = Calculate::PerkPoints();

                OpenLevelUpMenu(DefaultList, 0, sPoints, 1, PlayerLevel, DefaultTags);
                std::this_thread::sleep_for(std::chrono::seconds(1));
                while (isInMenuMode())
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));

                OpenLevelUpMenu(pList, pPoints, 0, 2, PlayerLevel, DefaultTags);
                std::this_thread::sleep_for(std::chrono::seconds(1));
                while (isInMenuMode())
                    std::this_thread::sleep_for(std::chrono::seconds(1));
            }

            (*g_player)->actorValueOwner.SetBase(g_Data.CurrentLevel, PlayerLevel);
            NeedLevelUp = false;
        }
    }

    void DoLevelUpCharGen(bool allowDialogue) {
        // Mostly a copy of async_LevelUp, but for Traits + Tags
        while (((*g_player)->IsInCombat()) || isInMenuMode(allowDialogue))
            std::this_thread::sleep_for(std::chrono::seconds(1));

        if (NeedLevelUp) {
            int pLevel = GetLevel((*g_player));
            std::vector<PerkData> pList = BuildFormattedPerkList((*g_player), pLevel, true);

            int sPoints = Calculate::TagPoints();
            int pPoints = Calculate::TraitPoints();

            std::this_thread::sleep_for(std::chrono::seconds(1));
            while (isInMenuMode(allowDialogue))
                std::this_thread::sleep_for(std::chrono::seconds(1));

            OpenLevelUpMenu(DefaultList, 0, sPoints, 4, pLevel, DefaultTags);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            while (isInMenuMode(allowDialogue))
                std::this_thread::sleep_for(std::chrono::milliseconds(100));

            OpenLevelUpMenu(pList, pPoints, 0, 3, pLevel, DisplayTags);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            while (isInMenuMode(allowDialogue))
                std::this_thread::sleep_for(std::chrono::seconds(1));

            NeedLevelUp = false;
        }
    }

    void LUPrompt_Hook() {
        NeedLevelUp = true;
        std::thread LUMThread(DoLevelUp);
        LUMThread.detach();
    }

    bool CharGenSkillSelect(StaticFunctionTag* base, bool allowDialogue, bool LockPreselect, Papyrus::TaggedSkills Tags) {
        if (!CheckTagSelection(Calculate::TagPoints(), LockPreselect, Tags))
            return false;

        NeedLevelUp = true;
        std::thread LUMThread(DoLevelUpCharGen, allowDialogue);
        LUMThread.detach();
        return true;
    }
}