#include "Scaleform.h"

#include "Condition.h"
#include "InputHandler.h"
#include "Perks.h"

#include "f4se/ScaleformLoader.h"
#include "f4se/ScaleformValue.h"

#include <chrono>
#include <iomanip>
#include <regex>
#include <sstream>
#include <thread>

#define PipboyInventoryObjects (*g_PipboyDataManager)->inventoryData.inventoryObjects

EntryList LevelUpObj;

void OpenLevelUpMenu(int ID, int Level, int PointsBase, int PointsUsed) {
    LevelUpObj = EntryList(ID, Level, PointsBase, PointsUsed);

    BSFixedString LevelUpMenu("LevelUpMenu");
    CALL_MEMBER_FN(*g_uiMessageManager, SendUIMessage)(LevelUpMenu, kMessage_Open);
}

namespace BSRegex {
    using namespace std::regex_constants;

    std::regex Face("(face='\\$[A-Za-z0-9_]+')", ECMAScript | icase);
    std::regex Size("(size='(([0-1]?[0-9])|(2[0-3]))')", ECMAScript | icase);
    std::regex Color("(color='#[A-Fa-f0-9]+')", ECMAScript | icase);
    std::regex Alias("(<(Alias|BaseName|Global|Relationship|Token)+[A-Z0-9=_. ]*>)", ECMAScript | icase);
    std::regex Pagebreak("([<\\[](br|pagebreak)+[\\]>])", ECMAScript | icase);

    std::regex SingleLine("(\r?\n)");
    std::regex DoubleLine("(\r?\n)(\r?\n)+");
}    // namespace BSRegex

namespace GFX {
    class ActiveEffects: public GFxFunctionHandler {
    private:
        UInt32 GetType(TESForm* Form) {
            switch (Form->formType) {
            case kFormType_ARMO:
                return 43;

            case kFormType_ALCH:
            case kFormType_ENCH:
            case kFormType_INGR:
            case kFormType_SCRL:
            case kFormType_SPEL: {
                MagicItem* SourceItem = DYNAMIC_CAST(Form, TESForm, MagicItem);
                for (auto iter: Forms::ObjectTypes) {
                    for (int i = 0; i < SourceItem->keywordForm.numKeywords; i++) {
                        if (iter.first->formID == SourceItem->keywordForm.keywords[i]->formID)
                            return iter.second;
                    }
                }

                return 50;
            }

            default:
                _LogWarning("GFx::ActiveEffects - Unhandled Form Type: %i", Form->formType);
                break;
            }

            return 59;
        }

        bool ShouldShowMaximum(ActorValueInfo* ActorValue) {
            bool IsHealth = (ActorValue->formID == 0x2D4);
            bool IsAction = (ActorValue->formID == 0x2D5);
            bool IsCarryW = (ActorValue->formID == 0x2DC);
            return (IsHealth || IsAction || IsCarryW);
        }

        bool ShouldShowPercent(ActorValueInfo* ActorValue) {
            bool IsHealthRate = (ActorValue->formID == 0x2D7);
            bool IsActionRate = (ActorValue->formID == 0x28D);
            bool IsResist = ((ActorValue->formID >= 0x2E3) && (0x2EB >= ActorValue->formID));
            return (IsHealthRate || IsActionRate || IsResist);
        }

        bool ShouldMultiply(ActorValueInfo* ActorValue) {
            bool IsHealthRate = (ActorValue->formID == 0x2D7);
            return (IsHealthRate);
        }

        void AddValueEffect(std::stringstream& Result, EffectEntry Effect, ActorValueInfo* ActorValue, std::string Name) {
            bool Recover = (Effect.Setting->flags & EffectSetting::kFlag_Recover);

            bool Max = Recover && ShouldShowMaximum(ActorValue);
            bool Percent = Recover && ShouldShowPercent(ActorValue);
            bool Multiply = Percent && ShouldMultiply(ActorValue);

            float Weight = (Effect.Setting->primaryActorValue->formID == ActorValue->formID) ? 1.0 : Effect.Setting->secondaryAVWeight;

            int Magnitude = (int)roundf(Effect.Magnitude * Weight * ((Multiply) ? 100.0 : 1.0));

            StringToUpper(Name);
            Result << ((Max) ? "MAX " : "") << Name << ((Recover) ? " " : "/s ") << Magnitude << ((Percent) ? "%" : "");
        }

        bool SortEntry(EffectEntry Effect, UInt32 FormID, std::vector<EffectEntry>& Result) {
            switch (Effect.Setting->archetype) {
            case EffectSetting::kArch_ValueModifier: {
                if ((FormID == Effect.Setting->primaryActorValue->formID) || (FormID == 0)) {
                    for (int i = 0; i < Result.size(); i++) {
                        if (Result[i].Setting->archetype == Effect.Setting->archetype) {
                            if (Result[i].Setting->primaryActorValue->formID == Effect.Setting->primaryActorValue->formID) {
                                Result[i].Magnitude += Effect.Magnitude;
                                return true;
                            }
                        }
                    }

                    Result.emplace_back(Effect);
                    return true;
                }
                break;
            }

            case EffectSetting::kArch_DualValueModifier: {
                // Maybe do more based on the secondary value?
                if ((FormID == Effect.Setting->primaryActorValue->formID) || (FormID == 0)) {
                    for (int i = 0; i < Result.size(); i++) {
                        if (Result[i].Setting->archetype == Effect.Setting->archetype) {
                            if (Result[i].Setting->primaryActorValue->formID == Effect.Setting->primaryActorValue->formID) {
                                Result[i].Magnitude += Effect.Magnitude;
                                return true;
                            }
                        }
                    }

                    Result.emplace_back(Effect);
                    return true;
                }

                break;
            }

            case EffectSetting::kArch_PeakValueModifier: {
                if ((FormID == Effect.Setting->primaryActorValue->formID) || (FormID == 0)) {
                    for (int i = 0; i < Result.size(); i++) {
                        if (Result[i].Setting->archetype == Effect.Setting->archetype) {
                            if (Result[i].Setting->primaryActorValue->formID == Effect.Setting->primaryActorValue->formID) {
                                if (Effect.Magnitude > Result[i].Magnitude) {
                                    Result[i].Magnitude = Effect.Magnitude;
                                    Result[i].Duration = Effect.Duration;
                                }

                                return true;
                            }
                        }
                    }

                    Result.emplace_back(Effect);
                    return true;
                }
                break;
            }

            case EffectSetting::kArch_Script:
            case EffectSetting::kArch_Cloak: {
                if (FormID == 0) {
                    for (int i = 0; i < Result.size(); i++) {
                        if (Result[i].Setting->archetype == Effect.Setting->archetype) {
                            if (!_stricmp(Result[i].Setting->description.c_str(), Effect.Setting->description.c_str())) {
                                return true;
                            }
                        }
                    }

                    Result.emplace_back(Effect);
                    return true;
                }

                break;
            }

            case EffectSetting::kArch_Chameleon: {
                if (FormID == 0) {
                    for (int i = 0; i < Result.size(); i++) {
                        if (Result[i].Setting->archetype == Effect.Setting->archetype) {
                            BGSPerk* StealthPerk1 = Result[i].Setting->perkToApply;
                            BGSPerk* StealthPerk2 = Effect.Setting->perkToApply;
                            if (!StealthPerk1 || !StealthPerk2)
                                return true;

                            if (StealthPerk1->formID == StealthPerk2->formID)
                                return true;

                            BSString DescriptionText1, DescriptionText2;
                            TESDescription* DescriptionForm1 = DYNAMIC_CAST(StealthPerk1, BGSPerk, TESDescription);
                            TESDescription* DescriptionForm2 = DYNAMIC_CAST(StealthPerk2, BGSPerk, TESDescription);
                            CALL_MEMBER_FN(DescriptionForm1, Get)(&DescriptionText1, nullptr);
                            CALL_MEMBER_FN(DescriptionForm2, Get)(&DescriptionText2, nullptr);

                            if (!_stricmp(DescriptionText1.Get(), DescriptionText2.Get()))
                                return true;
                        }
                    }

                    Result.emplace_back(Effect);
                    return true;
                }

                break;
            }

            default:
                _LogWarning("TestEffectEntry: Unhandled Effect Archetype: %i", Effect.Setting->archetype);
                break;
            }

            if (FormID == 0) {
                Result.emplace_back(Effect);
                return true;
            }

            return false;
        }

        void SortEntries(std::vector<EffectEntry>& Effects) {
            std::vector<EffectEntry> Result;

            for (auto iterSortList: Forms::EffectSortOrder) {
                for (auto iterSort: iterSortList) {
                    for (auto iterEffect = Effects.begin(); iterEffect != Effects.end();) {
                        if (SortEntry((*iterEffect), iterSort, Result))
                            iterEffect = Effects.erase(iterEffect);
                        else
                            iterEffect++;
                    }
                }
            }

            Effects.clear();
            Effects.insert(Effects.end(), Result.begin(), Result.end());
        }

        std::string GetDescription(std::vector<EffectEntry> Effects) {
            std::stringstream Result;
            std::string Seperator = ", ";

            bool SkipSeparator = false;
            for (auto iter = Effects.begin(); iter != Effects.end();) {
                switch (iter->Setting->archetype) {
                case EffectSetting::kArch_ValueModifier:
                case EffectSetting::kArch_DualValueModifier:
                case EffectSetting::kArch_PeakValueModifier: {
                    ActorValueInfo* ActorValue1 = iter->Setting->primaryActorValue;
                    ActorValueInfo* ActorValue2 = iter->Setting->secondaryActorValue;

                    std::string Name1 = ActorValue1->avAbbreviation.c_str();
                    if (Name1.empty())
                        Name1 = ActorValue1->avName;

                    AddValueEffect(Result, (*iter), ActorValue1, Name1);

                    if (ActorValue2) {
                        std::string Name2 = ActorValue2->avAbbreviation.c_str();
                        if (Name2.empty())
                            Name2 = ActorValue2->avName;

                        if (_stricmp(Name1.c_str(), Name2.c_str()) != 0) {
                            Result << Seperator;
                            AddValueEffect(Result, (*iter), ActorValue2, Name2);
                        }
                    }

                    break;
                }

                case EffectSetting::kArch_Script:
                case EffectSetting::kArch_Cloak: {
                    std::string Effect = iter->Setting->description.c_str();
                    while (ispunct(Effect.back()))
                        Effect.pop_back();

                    if (Effect.empty()) {
                        SkipSeparator = true;
                        break;
                    }

                    StringToUpper(Effect);
                    Result << Effect;
                    break;
                }

                case EffectSetting::kArch_Chameleon: {
                    BGSPerk* StealthPerk = iter->Setting->perkToApply;
                    if (!StealthPerk) {
                        SkipSeparator = true;
                        break;
                    }

                    BSString DescriptionText;
                    TESDescription* DescriptionForm = DYNAMIC_CAST(StealthPerk, BGSPerk, TESDescription);
                    CALL_MEMBER_FN(DescriptionForm, Get)(&DescriptionText, nullptr);

                    std::string Effect = DescriptionText.Get();

                    while (ispunct(Effect.back()))
                        Effect.pop_back();

                    if (Effect.empty()) {
                        SkipSeparator = true;
                        break;
                    }

                    StringToUpper(Effect);
                    Result << Effect;
                }

                default:
                    break;
                }

                iter++;
                if ((iter != Effects.end()) && !SkipSeparator)
                    Result << Seperator;
                SkipSeparator = false;
            }

            return Result.str();
        }

    public:
        virtual void Invoke(Args* args) {
            GFxMovieRoot* root = args->movie->movieRoot;
            root->CreateArray(args->result);

            auto ActiveEffectsT = (*g_player)->magicTarget.GetActiveEffects();
            std::vector<ActiveEffectEntry> ActiveEffectsV;

            for (int i = 0; i < ActiveEffectsT->count; i++) {
                ActiveEffect* Effect = ActiveEffectsT->entries[i];
                EffectSetting* Setting = Effect->data->setting;

                if (!Effect || !Setting)
                    continue;

                if ((Effect->flags & ActiveEffect::kFlag_Inactive) || (Setting->flags & EffectSetting::kFlag_HideInUI))
                    continue;

                TESForm* Source = (Effect->sourceItem) ? Effect->sourceItem : Effect->sourceSpell;
                if (!Source)
                    return;

                // Perks are almost all abilities, and shouldn't show up, so we filter out abilities
                SpellItem* SpellSource = DYNAMIC_CAST(Source, TESForm, SpellItem);
                if (SpellSource)
                    if (SpellSource->data.type == SpellItem::kAbility)
                        continue;

                EffectEntry newEffect;
                newEffect.Setting = Setting;
                newEffect.FormID = Setting->formID;
                newEffect.Magnitude = Effect->magnitude;
                newEffect.Duration = Effect->duration;

                bool SkipEffect = false;
                for (int i = 0; i < ActiveEffectsV.size(); i++) {
                    if (ActiveEffectsV[i].FormID == Source->formID) {
                        (Setting->flags & EffectSetting::kFlag_Detrimental) ? ActiveEffectsV[i].EffectsD.emplace_back(newEffect) : ActiveEffectsV[i].EffectsB.emplace_back(newEffect);
                        SkipEffect = true;
                    }
                }

                if (!SkipEffect) {
                    ActiveEffectEntry newActiveEffect;
                    newActiveEffect.Name = Source->GetFullName();
                    newActiveEffect.FormID = Source->formID;
                    (Setting->flags & EffectSetting::kFlag_Detrimental) ? newActiveEffect.EffectsD.emplace_back(newEffect) : newActiveEffect.EffectsB.emplace_back(newEffect);
                    newActiveEffect.Type = GetType(Source);
                    ActiveEffectsV.emplace_back(newActiveEffect);
                }
            }

            std::sort(ActiveEffectsV.begin(), ActiveEffectsV.end(), [](ActiveEffectEntry const& E1, ActiveEffectEntry const& E2) {
                SortGeneric(E1, E2, Type);
                SortStrings(E1, E2, Name);
                return false;
            });

            for (auto iter: ActiveEffectsV) {
                SortEntries(iter.EffectsB);
                SortEntries(iter.EffectsD);
                iter.EffectsB.insert(iter.EffectsB.end(), iter.EffectsD.begin(), iter.EffectsD.end());

                bool HasDuration = false;
                for (auto iterEffect: iter.EffectsB) {
                    if (iterEffect.Duration > 0) {
                        HasDuration = true;
                        break;
                    }
                }

                GFxValue Entry;
                root->CreateObject(&Entry);
                SetGFxValue(&Entry, root, "text", iter.Name.c_str());
                SetGFxValue(&Entry, root, "Description", GetDescription(iter.EffectsB).c_str());
                SetGFxValue(&Entry, "HasDuration", HasDuration);
                SetGFxValue(&Entry, "Type", iter.Type);
                args->result->PushBack(&Entry);
            }
        }
    };

    class AddPerks: public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            GFxValue Element, FormID;

            for (int i = 0; i < args->args[0].GetArraySize(); i++) {
                args->args[0].GetElement(i, &Element);
                Element.GetMember("FormID", &FormID);

                BGSPerk* Perk = LookupTypeByID(FormID.GetUInt(), BGSPerk);
                AddPerk((*g_player), Perk, false);
            }
        }
    };

    class AllowTextInput: public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) { (*g_inputMgr)->AllowTextInput(args->args[0].GetBool()); }
    };

    class CloseMenu: public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            BSFixedString MenuName(args->args[0].GetString());
            CALL_MEMBER_FN((*g_uiMessageManager), SendUIMessage)(MenuName, kMessage_Close);
        }
    };

    class DoctorBagCount: public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) { args->result->SetUInt(GetItemCount((*g_player), Forms::DoctorsBag)); }
    };

    class FilterSearch: public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            GFxValue Result[2];
            GFxValue filterFlag;
            filterFlag.SetInt(0x10);

            GFxMovieRoot* root = args->movie->movieRoot;
            root->CreateArray(&Result[0]);
            Result[1] = filterFlag;

            std::string q = args->args[0].GetString();
            int ArraySize = args->args[1].GetArraySize();

            int matchCount = 0;
            for (int i = 0; i < ArraySize; i++) {
                GFxValue Item, Text;
                args->args[1].GetElement(i, &Item);
                Item.GetMember("text", &Text);

                if (Item.HasMember("IsDefault"))
                    continue;

                std::string name = Text.GetString();
                if (strifind(name, q)) {
                    Item.SetMember("filterFlag", &filterFlag);
                    matchCount++;
                } else
                    (Item.SetMember("filterFlag", &GFxValue(0)));

                Result[0].PushBack(&Item);
            }

            if (matchCount == 0) {
                GFxValue Default;
                root->CreateObject(&Default);
                SetGFxValue(&Default, root, "text", "$NONE_MATCHING");
                SetGFxValue(&Default, "filterFlag", filterFlag);
                SetGFxValue(&Default, "IsDefault", true);
                Result[0].PushBack(&Default);
            }

            root->Invoke("root.Menu_mc.FilterSearchCallback", nullptr, Result, 2);
        }
    };

    class ForceUnlock: public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            //
        }
    };

    class GetDescription: public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            GFxMovieRoot* root = args->movie->movieRoot;

            GFxValue FormID;
            args->args[0].GetMember("formID", &FormID);

            std::string Description;
            bool HasFeatured = false;

            TESForm* Form = LookupFormByID(FormID.GetUInt());
            if (Form) {
                if (Form->formType == kFormType_BOOK) {
                    TESObjectBOOK* Book = DYNAMIC_CAST(Form, TESForm, TESObjectBOOK);

                    BSString DescriptionText;
                    TESDescription* DescriptionForm = DYNAMIC_CAST(Book, TESObjectBOOK, TESDescription);
                    CALL_MEMBER_FN(DescriptionForm, Get)(&DescriptionText, nullptr);

                    Description = DescriptionText.Get();
                    if (Description.empty()) {
                        switch (Book->data.GetSanitizedType()) {
                        case TESObjectBOOK::Data::kType_Perk: {
                            if (Book->data.teaches.Perk) {
                                DescriptionForm = DYNAMIC_CAST(Book->data.teaches.Perk, BGSPerk, TESDescription);
                                CALL_MEMBER_FN(DescriptionForm, Get)(&DescriptionText, nullptr);
                                HasFeatured = true;
                            }

                            break;
                        }

                        case TESObjectBOOK::Data::kType_Spell: {
                            if (Book->data.teaches.Spell) {
                                DescriptionForm = DYNAMIC_CAST(Book->data.teaches.Spell, SpellItem, TESDescription);
                                CALL_MEMBER_FN(DescriptionForm, Get)(&DescriptionText, nullptr);
                                HasFeatured = true;
                            }

                            break;
                        }

                        default:
                            break;
                        }

                        Description = DescriptionText.Get();
                        if (Description.empty()) {
                            if (Book->featured.message) {
                                DescriptionForm = DYNAMIC_CAST(Book->featured.message, BGSMessage, TESDescription);
                                CALL_MEMBER_FN(DescriptionForm, Get)(&DescriptionText, nullptr);
                                HasFeatured = true;
                            }

                            Description = DescriptionText.Get();
                        }
                    }

                    if (!HasFeatured) {
                        if (std::regex_search(Description, BSRegex::Alias)) {
                            ExtraDataList* ExtraData = GetExtraDataListByIndex(args->args[1].GetUInt());
                            if (ExtraData) {
                                ExtraTextDisplayData* TextDisplayData = GetExtraDataByType(ExtraData, TextDisplayData);
                                if (TextDisplayData) {
                                    DoTokenReplacement(TextDisplayData, DescriptionText);
                                    Description = DescriptionText.Get();
                                }
                            }
                        }

                        Description = std::regex_replace(Description, BSRegex::Pagebreak, "<br>");
                        Description = std::regex_replace(Description, BSRegex::SingleLine, "<br>");
                        Description = std::regex_replace(Description, BSRegex::DoubleLine, "<br>");
                        Description = std::regex_replace(Description, BSRegex::Face, "face='$MAIN_Font'");
                        Description = std::regex_replace(Description, BSRegex::Size, "size='24')");
                        Description = std::regex_replace(Description, BSRegex::Color, "color='#FFFFFF'");
                    }

                    Book->bounds1;
                    Book->bounds2;
                }
            }

            bool HasDescription = (HasFeatured || !Description.empty());
            SetGFxValue(&args->args[0], root, "Description", Description);
            SetGFxValue(&args->args[0], "HasDescription", HasDescription);
        }
    };

    class HardcoreValues: public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            args->movie->movieRoot->CreateObject(args->result);
            SetGFxValue(args->result, "H2O", GetValueInt((*g_player), Forms::Dehydration));
            SetGFxValue(args->result, "FOD", GetValueInt((*g_player), Forms::Starvation));
            SetGFxValue(args->result, "SLP", GetValueInt((*g_player), Forms::SleepDeprivation));
            SetGFxValue(args->result, "IsEnabled", false);
        }
    };

    class InvFilter: public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            GFxMovieRoot* root = args->movie->movieRoot;
            root->CreateArray(args->result);

            GFxValue DataObj, CurrentPage, CurrentTab;
            root->GetVariable(&DataObj, "root.Menu_mc.DataObj");
            DataObj.GetMember("CurrentPage", &CurrentPage);
            DataObj.GetMember("CurrentTab", &CurrentTab);

            switch (CurrentPage.GetUInt()) {
            case PipboyMenu::kPage_Inventory:
                switch (CurrentTab.GetUInt()) {
                case PipboyMenu::kTab_Inv_Weapons:
                    args->result->SetUInt(PipboyMenu::kFilter_Weapons);
                    break;
                case PipboyMenu::kTab_Inv_Apparel:
                    args->result->SetUInt(PipboyMenu::kFilter_Apparel);
                    break;
                case PipboyMenu::kTab_Inv_Aid:
                    args->result->SetUInt(PipboyMenu::kFilter_Aid);
                    break;
                case PipboyMenu::kTab_Inv_Misc:
                    args->result->SetUInt(PipboyMenu::kFilter_Misc);
                    break;
                case PipboyMenu::kTab_Inv_Junk:
                    args->result->SetUInt(PipboyMenu::kFilter_Junk);
                    break;
                case PipboyMenu::kTab_Inv_Mods:
                    args->result->SetUInt(PipboyMenu::kFilter_Mods);
                    break;
                case PipboyMenu::kTab_Inv_Ammo:
                    args->result->SetUInt(PipboyMenu::kFilter_Ammo);
                    break;
                case PipboyMenu::kTab_Inv_Keys:
                    args->result->SetUInt(PipboyMenu::kFilter_Keys);
                    break;
                default:
                    _LogMessage("InvFilter: Unhandled Inventory Tab: %i", CurrentTab.GetUInt());
                    break;
                }
                break;

            case PipboyMenu::kPage_Data:
                args->result->SetUInt(PipboyMenu::kFilter_Notes);
                break;

            default:
                _LogMessage("InvFilter: Unhandled Pipboy Page: %i", CurrentPage.GetUInt());
                break;
            }
        }
    };

    class InvItems: public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            GFxMovieRoot* root = args->movie->movieRoot;
            root->CreateArray(args->result);

            GFxValue DataObj, InvItems;
            root->GetVariable(&DataObj, "root.Menu_mc.DataObj");
            DataObj.GetMember("InvItems", &InvItems);

            for (int i = 0; i < InvItems.GetArraySize(); i++) {
                GFxValue Entry, FilterFlag, FormID;
                InvItems.GetElement(i, &Entry);
                Entry.GetMember("filterFlag", &FilterFlag);

                switch (FilterFlag.GetUInt()) {
                case PipboyMenu::kFilter_NOTE:
                    SetGFxValue(&Entry, "isHolotape", true);

                case PipboyMenu::kFilter_BOOK:
                    SetGFxValue(&Entry, "filterFlag", PipboyMenu::kFilter_Notes);
                    break;

                case PipboyMenu::kFilter_Misc:
                    Entry.GetMember("formID", &FormID);
                    if (LookupFormByID(FormID.GetUInt())->formType == FormType::kFormType_KEYM)
                        SetGFxValue(&Entry, "filterFlag", PipboyMenu::kFilter_Keys);
                    break;

                default:
                    break;
                }

                args->result->PushBack(&Entry);
            }
        }
    };

    class LockpickLevel: public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) { args->result->SetInt(GetValueInt((*g_player), Forms::Lockpick)); }
    };

    class ModSkills: public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            GFxValue Element, FormID, Value;

            for (int i = 0; i < args->args[0].GetArraySize(); i++) {
                args->args[0].GetElement(i, &Element);
                Element.GetMember("FormID", &FormID);
                Element.GetMember("Value", &Value);

                ActorValueInfo* ActorValue = LookupTypeByID(FormID.GetUInt(), ActorValueInfo);
                ModPermValue((*g_player), ActorValue, Value.GetNumber());
            }
        }
    };

    class PerksList: public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            GFxMovieRoot* root = args->movie->movieRoot;
            root->CreateArray(args->result);

            for (auto iter: Forms::ListMaster) {
                if (!HasPerk((*g_player), iter))
                    continue;

                std::string Name = iter->GetFullName();
                if (Name.empty())
                    continue;

                PerkVector Ranks;
                if (iter->numRanks > 1) {
                    Ranks = GetRankList(iter);
                    if (Ranks[0]->formID != iter->formID)
                        continue;
                } else
                    Ranks.emplace_back(iter);

                GFxValue Descriptions;
                root->CreateArray(&Descriptions);

                int NumRanks = 0;
                for (auto iterRank: Ranks) {
                    if (HasPerk((*g_player), iterRank))
                        NumRanks++;

                    BSString DescriptionText;
                    TESDescription* DescriptionForm = DYNAMIC_CAST(iter, BGSPerk, TESDescription);
                    CALL_MEMBER_FN(DescriptionForm, Get)(&DescriptionText, nullptr);
                    Descriptions.PushBack(&GFxValue(DescriptionText.Get()));
                }

                GFxValue Perk;
                root->CreateObject(&Perk);
                SetGFxValue(&Perk, root, "text", Name);
                SetGFxValue(&Perk, root, "IconPath", GetPerkIconPath(iter));
                SetGFxValue(&Perk, "FormID", iter->formID);
                SetGFxValue(&Perk, "MaxRank", iter->numRanks);
                SetGFxValue(&Perk, "Rank", NumRanks);

                Perk.SetMember("Descriptions", &Descriptions);
                args->result->PushBack(&Perk);
            }
        }
    };

    class PlaySoundUI: public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) { PlayUISound(args->args[0].GetString()); }
    };

    class PopulateRepairItemCard: public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            GFxMovieRoot* root = args->movie->movieRoot;

            UInt32 HandleID = args->args[0].GetUInt();
            UInt32 StackID = args->args[1].GetUInt();

            BGSInventoryItem* Item = GetInventoryItemByHandleID(HandleID);
            if (!Item)
                return;

            if (!Item->form)
                return;

            BGSInventoryItem::Stack* Stack = GetStackByStackID(Item, StackID);
            if (!Stack)
                return;

            GFxValue CurrentCard, PreviewCard;
            args->args[2].GetMember("Current_mc", &CurrentCard);
            args->args[2].GetMember("Preview_mc", &PreviewCard);

            GFxValue CurrentInfo, PreviewInfo;
            CurrentCard.GetMember("InfoObj", &CurrentInfo);
            PreviewCard.GetMember("InfoObj", &PreviewInfo);

            if (!CurrentInfo.IsArray() || !PreviewInfo.IsArray())
                return;

            SimpleCollector<InvItemStack> Test{0, {}};
            PopulateItemCard_Hook(&CurrentInfo, Item, StackID, &Test);
            PopulateItemCard_Hook(&PreviewInfo, Item, StackID, &Test);

            for (int i = 0; i < CurrentInfo.GetArraySize(); i++) {
                GFxValue element;
                CurrentInfo.GetElement(i, &element);
                SetGFxValue(&element, "diffRating", 0);
            }

            for (int i = 0; i < PreviewInfo.GetArraySize(); i++) {
                GFxValue element;
                PreviewInfo.GetElement(i, &element);
                SetGFxValue(&element, "diffRating", 0);
            }
        }
    };

    class RadXCount: public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) { args->result->SetUInt(GetItemCount((*g_player), Forms::RadX)); }
    };

    class RadiationValues: public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            int Current = GetValueInt((*g_player), Forms::Rads);
            int Resists = GetValueInt((*g_player), Forms::RadResistExposure);
            int Maximum = ISettings::GetInteger("Radiation::Threshold05", 1000);

            std::map<int, const char*> RadiationThreshold;
            RadiationThreshold.emplace(200, "Radiation::Threshold01");
            RadiationThreshold.emplace(400, "Radiation::Threshold02");
            RadiationThreshold.emplace(600, "Radiation::Threshold03");
            RadiationThreshold.emplace(800, "Radiation::Threshold04");
            RadiationThreshold.emplace(1000, "Radiation::Threshold05");

            int Next = 0;
            for (auto iter: RadiationThreshold) {
                Next = ISettings::GetInteger(iter.second, iter.first);
                if (Next > Current)
                    break;
            }

            args->movie->movieRoot->CreateObject(args->result);
            SetGFxValue(args->result, "Current", Current);
            SetGFxValue(args->result, "Resists", Resists);
            SetGFxValue(args->result, "Maximum", Maximum);
            SetGFxValue(args->result, "Next", Next);
        }
    };

    class RepairList: public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            GFxMovieRoot* root = args->movie->movieRoot;
            root->CreateArray(args->result);

            std::vector<RepairMenuEntry> RepairV, RepairI;
            for (int i = 0; i < PipboyInventoryObjects.count; i++) {
                TESForm* Form = GetInventoryFormByIndex(i);
                if (!Form)
                    continue;

                ExtraDataList* ExtraData = GetExtraDataListByIndex(i);
                if (ExtraData->HasType(kExtraData_Health)) {
                    switch (Form->formType) {
                    case kFormType_WEAP: {
                        float Percent = GetWeaponConditionPercent(WeaponConditionData(Form, ExtraData));
                        if (Percent == -1.0)
                            break;

                        RepairMenuEntry Entry;
                        Entry.text = GetTableValue(BSFixedString, i, "text");
                        Entry.HandleID = GetHandleIDByIndex(i);
                        Entry.StackID = GetStackIDByIndex(i);
                        Entry.Condition = Percent;

                        if (Percent == 1.0)
                            RepairI.emplace_back(Entry);
                        else
                            RepairV.emplace_back(Entry);
                        break;
                    }

                    default:
                        break;
                    }
                }
            }

            std::sort(RepairV.begin(), RepairV.end(), [](RepairMenuEntry const& E1, RepairMenuEntry const& E2) {
                SortStrings(E1, E2, text);
                SortGeneric(E1, E2, Condition);
                return false;
            });

            std::sort(RepairI.begin(), RepairI.end(), [](RepairMenuEntry const& E1, RepairMenuEntry const& E2) {
                SortStrings(E1, E2, text);
                SortGeneric(E1, E2, Condition);
                return false;
            });

            RepairV.insert(RepairV.end(), RepairI.begin(), RepairI.end());

            for (auto iter: RepairV) {
                GFxValue Entry;
                root->CreateObject(&Entry);
                SetGFxValue(&Entry, root, "text", iter.text);
                SetGFxValue(&Entry, "HandleID", iter.HandleID);
                SetGFxValue(&Entry, "StackID", iter.StackID);
                SetGFxValue(&Entry, "Condition", iter.Condition);
                args->result->PushBack(&Entry);
            }
        }
    };

    class RepairMenuInit: public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            GFxMovieRoot* root = args->movie->movieRoot;

            int Skill = GetValueInt((*g_player), Forms::Repair);
            int Count = GetItemCount((*g_player), LookupFormByID(0xF));

            GFxValue Result[3];
            Result[0].SetInt(0x10);
            Result[1].SetInt(Skill);
            Result[2].SetInt(Count);
            root->Invoke("root.Menu_mc.Init", nullptr, Result, 3);
        }
    };

    class SkillsList: public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            GFxMovieRoot* root = args->movie->movieRoot;
            root->CreateArray(args->result);

            for (auto iter: Forms::ListSkills) {
                BSString DescriptionText;
                TESDescription* DescriptionForm = DYNAMIC_CAST(iter, ActorValueInfo, TESDescription);
                CALL_MEMBER_FN(DescriptionForm, Get)(&DescriptionText, nullptr);

                GFxValue Skill;
                root->CreateObject(&Skill);
                SetGFxValue(&Skill, root, "text", iter->GetFullName());
                SetGFxValue(&Skill, root, "Description", DescriptionText.Get());
                SetGFxValue(&Skill, root, "IconPath", GetSkillIconPath(iter));
                SetGFxValue(&Skill, "Value", GetValueInt((*g_player), iter));
                SetGFxValue(&Skill, "Modifier", GetTempMod((*g_player), iter));
                args->result->PushBack(&Skill);
            }
        }
    };

    class StimpakCount: public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) { args->result->SetUInt(GetItemCount((*g_player), Forms::StimpakOrder)); }
    };

    class TagSkills: public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            GFxValue Element, FormID;

            for (int i = 0; i < Forms::ListSkills.size(); i++)
                if (Forms::PlayerTags & 1 << (i + 1))
                    ModPermValue((*g_player), Forms::ListSkills[i], -15);
            Forms::PlayerTags = 0;

            for (int i = 0; i < args->args[0].GetArraySize(); i++) {
                args->args[0].GetElement(i, &Element);
                Element.GetMember("FormID", &FormID);

                for (int j = 0; j < Forms::ListSkills.size(); j++) {
                    if (Forms::ListSkills[j]->formID == FormID.GetUInt()) {
                        ModPermValue((*g_player), Forms::ListSkills[j], 15);
                        Forms::PlayerTags |= 1 << (j + 1);
                    }
                }
            }
        }
    };

    class UseDoctorBag: public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) { EquipItem((*g_player), Forms::DoctorsBag, false); }
    };

    class UseRadX: public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) { EquipItem((*g_player), Forms::RadX, false); }
    };

    class UseStimpak: public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            for (int i = 0; i < Forms::StimpakOrder->forms.count; i++) {
                TESForm* Stimpak = Forms::StimpakOrder->forms[i];
                if (GetItemCount((*g_player), Stimpak) > 0) {
                    EquipItem((*g_player), Stimpak, false);
                    break;
                }
            }
        }
    };
}    // namespace GFX

namespace ItemCard {
    int GetMax(GFxValue* Entry) {
        GFxValue Value;
        Entry->GetMember("value", &Value);
        if (!Value.IsString())
            return -1;

        std::string ValueStr = Value.GetString();
        if (ValueStr.empty())
            return -1;

        auto delimiter = ValueStr.find_first_of("/");
        std::string maximum = ValueStr.substr(0, delimiter);

        return std::stoi(maximum);
    }

    void DivToMeter(GFxValue* Entry) {
        GFxValue Value;
        Entry->GetMember("value", &Value);
        if (!Value.IsString())
            return;

        std::string ValueStr = Value.GetString();
        if (ValueStr.empty())
            return;

        auto delimiter = ValueStr.find_first_of("/");
        std::string current = ValueStr.substr(delimiter + 1);
        std::string maximum = ValueStr.substr(0, delimiter);

        SetGFxValue(Entry, "value", std::stoi(current));
        SetGFxValue(Entry, "max", std::stoi(maximum));
        SetGFxValue(Entry, "IsMeter", true);
    }

    void ReformatCondition(GFxValue* InfoObj, GFxMovieRoot* root, const char* title, bool ShowMax) {
        for (int i = 0; i < InfoObj->GetArraySize(); i++) {
            GFxValue element, text;
            InfoObj->GetElement(i, &element);

            element.GetMember("text", &text);
            if (!_stricmp(text.GetString(), title)) {
                DivToMeter(&element);

                if (ShowMax) {
                    int maximum = GetMax(&element);
                    if (maximum != -1) {
                        GFxValue MaxCondEntry;
                        SetGFxValue(&MaxCondEntry, root, "text", "$MAX CND");
                        SetGFxValue(&MaxCondEntry, "value", maximum);
                        SetGFxValue(&MaxCondEntry, "IsMeterMax", true);
                        InfoObj->PushBack(&MaxCondEntry);
                    }
                }
            }
        }
    }
}    // namespace ItemCard

namespace Menus {
    const char* BlurMenu::sMenuName = "BlurMenu";
    const char* RepairMenu::sMenuName = "RepairMenu";

    BlurMenu::BlurMenu(): GameMenuBase() {
        flags = kFlag_BlurBackground | kFlag_DisableInteractive;

        depth = 0x08;
    }

    BlurMenu::~BlurMenu() {
        //
    }

    void BlurMenu::RegisterFunctions() {
        //
    }

    void BlurMenu::DrawNextFrame(float unk0, void* unk1) { __super::DrawNextFrame(unk0, unk1); }

    RepairMenu::RepairMenu(): GameMenuBase() {
        flags = kFlag_PauseGame | kFlag_ShowCursor | kFlag_PreventGameLoad | kFlag_HideOther | kFlag_UpdateCursorOnPlatformChange |
                //kFlag_ApplyDropDownFilter           |
                kFlag_Unk10000 | kFlag_BlurBackground;

        depth = 0x09;

        if (CALL_MEMBER_FN((*g_scaleformManager), LoadMovie)(this, movie, "RepairMenu", "root", 2)) {
            GFxMovieRoot* root = movie->movieRoot;
            RegisterRepairMenuInput(true);

            CreateBaseShaderTarget(shaderTarget, stage);
            shaderTarget->SetFilterColor(false);

            //InitializeButtonBar(this, shaderTarget, "root.Menu_mc.ButtonHintBar_mc", 2);

            GFxValue BGSCodeObj;
            root->GetVariable(&BGSCodeObj, "root.Menu_mc.BGSCodeObj");
            RegisterFunction<GFX::CloseMenu>(&BGSCodeObj, root, "CloseMenu");
            RegisterFunction<GFX::PlaySoundUI>(&BGSCodeObj, root, "PlaySound");
            RegisterFunction<GFX::PopulateRepairItemCard>(&BGSCodeObj, root, "PopulateRepairItemCard");
            RegisterFunction<GFX::RepairList>(&BGSCodeObj, root, "RepairList");
            RegisterFunction<GFX::RepairMenuInit>(&BGSCodeObj, root, "RepairMenuInit");
            root->Invoke("root.Menu_mc.onCodeObjCreate", nullptr, nullptr, 0);
        }
    }

    RepairMenu::~RepairMenu() {
        //
    }

    void RepairMenu::RegisterFunctions() {
        //
    }

    void RepairMenu::DrawNextFrame(float unk0, void* unk1) { __super::DrawNextFrame(unk0, unk1); }
}    // namespace Menus

namespace Threads {
    bool IsBlockingMenuOpen(bool AllowDialogueMenu) {
        bool IsMenuOpenDialogue = AllowDialogueMenu ? false : (*g_ui)->IsMenuOpen(BSFixedString("DialogueMenu"));

        return (((*g_ui)->numPauseGame >= 1) || (*g_ui)->IsMenuOpen(BSFixedString("CookingMenu")) || (*g_ui)->IsMenuOpen(BSFixedString("FaderMenu")) ||
                (*g_ui)->IsMenuOpen(BSFixedString("FavoritesMenu")) || (*g_ui)->IsMenuOpen(BSFixedString("PowerArmorModMenu")) || (*g_ui)->IsMenuOpen(BSFixedString("RobotModMenu")) ||
                (*g_ui)->IsMenuOpen(BSFixedString("VATSMenu")) || (*g_ui)->IsMenuOpen(BSFixedString("WorkshopMenu")) || IsMenuOpenDialogue);
    }

    void LevelUpMenu() {
        while (((*g_player)->IsInCombat()) || IsBlockingMenuOpen(false))
            std::this_thread::sleep_for(std::chrono::seconds(1));

        if (Forms::PlayerLevelUp) {
            // Give the "Level Up!" text time to kindly fuck off.
            std::this_thread::sleep_for(std::chrono::seconds(5));
            while (IsBlockingMenuOpen(false))
                std::this_thread::sleep_for(std::chrono::seconds(1));

            int nextLevel = GetLevel((*g_player));
            while (Forms::PlayerLevel < nextLevel) {
                Forms::PlayerLevel++;

                // Open Skill Menu
                OpenLevelUpMenu(EntryList::kMenuID_Skills, Forms::PlayerLevel, GetSkillPoints(Forms::PlayerLevel), 0);
                std::this_thread::sleep_for(std::chrono::seconds(1));

                while (IsBlockingMenuOpen(false))
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));

                // Open Perk Menu
                OpenLevelUpMenu(EntryList::kMenuID_Perks, Forms::PlayerLevel, GetPerkPoints(), 0);
                std::this_thread::sleep_for(std::chrono::seconds(1));

                while (IsBlockingMenuOpen(false))
                    std::this_thread::sleep_for(std::chrono::seconds(1));
            }

            Forms::PlayerLevelUp = false;
        }
    }

    void CharGenMenu() {
        while (((*g_player)->IsInCombat()) || IsBlockingMenuOpen(true))
            std::this_thread::sleep_for(std::chrono::seconds(1));

        if (Forms::PlayerLevelUp) {
            Forms::PlayerLevel = GetLevel((*g_player));

            BSFixedString LevelUpMenu("SPECIALMenu");
            CALL_MEMBER_FN(*g_uiMessageManager, SendUIMessage)(LevelUpMenu, kMessage_Open);

            // Wait for SPECIAL Menu
            std::this_thread::sleep_for(std::chrono::seconds(1));
            while (IsBlockingMenuOpen(true))
                std::this_thread::sleep_for(std::chrono::seconds(1));

            // Open Tags Menu
            OpenLevelUpMenu(EntryList::kMenuID_Tags, Forms::PlayerLevel, GetTagPoints(), 0);
            std::this_thread::sleep_for(std::chrono::seconds(1));

            while (IsBlockingMenuOpen(true))
                std::this_thread::sleep_for(std::chrono::milliseconds(100));

            // Open Traits Menu
            OpenLevelUpMenu(EntryList::kMenuID_Traits, Forms::PlayerLevel, GetTraitPoints(), 0);
            std::this_thread::sleep_for(std::chrono::seconds(1));

            while (IsBlockingMenuOpen(true))
                std::this_thread::sleep_for(std::chrono::milliseconds(100));

            Forms::PlayerLevelUp = false;
        }
    }
}    // namespace Threads

void ContainerMenuInvoke_Hook(ContainerMenuBase* menu, GFxFunctionHandler::Args* args) {
    ContainerMenuInvoke_Original(menu, args);
}

void ExamineMenuInvoke_Hook(ExamineMenu* menu, GFxFunctionHandler::Args* args) {
    ExamineMenuInvoke_Original(menu, args);
}

void LevelUpMenuPrompt_Hook() {
    Forms::PlayerLevelUp = true;
    std::thread LevelUpMenuPromptThread(Threads::LevelUpMenu);
    LevelUpMenuPromptThread.detach();
}

bool CharGenMenuPrompt() {
    if (GetTagCount() > GetTagPoints())
        return false;

    Forms::PlayerLevelUp = true;
    std::thread CharGenMenuPromptThread(Threads::CharGenMenu);
    CharGenMenuPromptThread.detach();

    return true;
}

void PipboyMenuInvoke_Hook(PipboyMenu* menu, GFxFunctionHandler::Args* args) {
    PipboyMenuInvoke_Original(menu, args);

    GFxMovieRoot* root = menu->movie->movieRoot;

    switch (args->optionID) {
    case PipboyMenu::kFunction_OnInvItemSelection: {
        int SelectedIndex = args->args[0].GetInt();
        if (SelectedIndex > -1) {
            BGSInventoryItem* Item = GetInventoryItemByIndex(SelectedIndex);
            UInt32 StackID = GetStackIDByIndex(SelectedIndex);

            SimpleCollector<InvItemStack> Blank{0, {}};
            PopulateItemCard_Custom(&args->args[1], Item, StackID, &Blank);
        }

        break;
    }

    default:
        break;
    }
}

void PopulateItemCard_Custom(GFxValue* InfoObj, BGSInventoryItem* Item, UInt16 StackID, InvItemStackList CompareList) {
    GFxMovieRoot* root = InfoObj->objectInterface->view->movieRoot;

    GFxValue SWFPath;
    root->GetVariable(&SWFPath, "root.loaderInfo.url");
    bool isExamineMenu = (!_stricmp("Interface/ExamineMenu.swf", SWFPath.GetString()));

    if (!Item)
        return;

    if (!Item->form)
        return;

    if (!InfoObj->IsArray())
        return;

    switch (Item->form->formType) {
    case kFormType_AMMO: {
        ItemCard::ReformatCondition(InfoObj, root, "$charge", false);
        break;
    }

    case kFormType_ARMO: {
        ItemCard::ReformatCondition(InfoObj, root, "$health", isExamineMenu);
        break;
    }

    case kFormType_WEAP: {
        ExtraDataList* extraDataList = GetExtraDataListByStackID(Item, StackID);
        if (!extraDataList)
            return;

        float Value = GetWeaponConditionPercent(WeaponConditionData(Item->form, extraDataList));
        if (Value == -1)
            return;

        GFxValue InfoCardEntry;
        root->CreateObject(&InfoCardEntry);
        SetGFxValue(&InfoCardEntry, root, "text", "$CND");
        SetGFxValue(&InfoCardEntry, "value", Value);
        SetGFxValue(&InfoCardEntry, "max", 1.0);
        SetGFxValue(&InfoCardEntry, "IsMeter", true);
        InfoObj->PushBack(&InfoCardEntry);

        if (isExamineMenu) {
            float maximum = GetWeaponConditionMaximum(WeaponConditionData(Item->form, extraDataList));
            if (maximum != -1.0) {
                GFxValue MaxCondEntry;
                root->CreateObject(&MaxCondEntry);
                SetGFxValue(&MaxCondEntry, root, "text", "$MAX CND");
                SetGFxValue(&MaxCondEntry, "value", maximum);
                SetGFxValue(&MaxCondEntry, "IsMeterMax", true);
                InfoObj->PushBack(&MaxCondEntry);
            }
        }
        break;
    }

    default:
        break;
    }
}

void PopulateItemCard_Hook(GFxValue* InfoObj, BGSInventoryItem* Item, UInt16 StackID, InvItemStackList CompareList) {
    PopulateItemCard_Original(InfoObj, Item, StackID, CompareList);

    if (InfoObj->GetType() == GFxValue::kType_Object) {
        GFxValue ItemCardInfoList;
        InfoObj->GetMember("ItemCardInfoList", &ItemCardInfoList);
        PopulateItemCard_Custom(&ItemCardInfoList, Item, StackID, CompareList);
    } else {
        PopulateItemCard_Custom(InfoObj, Item, StackID, CompareList);
    }
}

EventResult MenuOpenCloseEventHandler::ReceiveEvent(MenuOpenCloseEvent* evn, void* dispatcher) {
    if (!evn)
        return kEvent_Continue;

    if (evn->menuName == BSFixedString("HUDMenu")) {
        if (evn->isOpen) {
            IMenu* menu = (*g_ui)->GetMenu(evn->menuName);
            if (!menu)
                return kEvent_Continue;

            GFxMovieRoot* root = menu->movie->movieRoot;
            root->SetVariable("root.LeftMeters_mc.HPMeter_mc.RadsBar_mc.visible", &GFxValue(false));
        }
    }

    else if (evn->menuName == BSFixedString("LevelUpMenu")) {
        if (evn->isOpen) {
            IMenu* menu = (*g_ui)->GetMenu(evn->menuName);
            if (!menu)
                return kEvent_Continue;

            GFxMovieRoot* root = menu->movie->movieRoot;

            Menus::BlurMenu::OpenMenu();
            RegisterLevelUpMenuInput(true);

            GFxValue BGSCodeObj;
            root->GetVariable(&BGSCodeObj, "root.Menu_mc.BGSCodeObj");
            RegisterFunction<GFX::AddPerks>(&BGSCodeObj, root, "AddPerks");
            RegisterFunction<GFX::AllowTextInput>(&BGSCodeObj, root, "AllowTextInput");
            RegisterFunction<GFX::FilterSearch>(&BGSCodeObj, root, "FilterSearch");
            RegisterFunction<GFX::ModSkills>(&BGSCodeObj, root, "ModSkills");
            RegisterFunction<GFX::TagSkills>(&BGSCodeObj, root, "TagSkills");
            root->Invoke("root.Menu_mc.onCodeObjUpdate", nullptr, nullptr, 0);

            GFxValue Args[5];
            GFxValue* Result = LevelUpObj.BuildEntryList(root, Args);
            root->Invoke("root.Menu_mc.Init", nullptr, Result, 5);
        } else {
            Menus::BlurMenu::CloseMenu();
            RegisterLevelUpMenuInput(false);
        }
    }

    else if (evn->menuName == BSFixedString("LockpickingMenu")) {
        if (evn->isOpen) {
            IMenu* menu = (*g_ui)->GetMenu(evn->menuName);
            if (!menu)
                return kEvent_Continue;

            GFxMovieRoot* root = menu->movie->movieRoot;

            RegisterLockpickingMenuInput(true);

            GFxValue BGSCodeObj;
            root->GetVariable(&BGSCodeObj, "root.Menu_mc.BGSCodeObj");
            RegisterFunction<GFX::ForceUnlock>(&BGSCodeObj, root, "ForceUnlock");
            RegisterFunction<GFX::LockpickLevel>(&BGSCodeObj, root, "LockpickLevel");
            root->Invoke("root.Menu_mc.onCodeObjUpdate", nullptr, nullptr, 0);
        } else {
            RegisterLockpickingMenuInput(false);
        }
    }

    else if (evn->menuName == BSFixedString("PipboyMenu")) {
        if (evn->isOpen) {
            IMenu* menu = (*g_ui)->GetMenu(evn->menuName);
            if (!menu)
                return kEvent_Continue;

            GFxMovieRoot* root = menu->movie->movieRoot;

            RegisterPipboyMenuInput(true);

            GFxValue BGSCodeObj;
            root->GetVariable(&BGSCodeObj, "root.Menu_mc.BGSCodeObj");
            RegisterFunction<GFX::ActiveEffects>(&BGSCodeObj, root, "ActiveEffects");
            RegisterFunction<GFX::DoctorBagCount>(&BGSCodeObj, root, "DoctorBagCount");
            RegisterFunction<GFX::GetDescription>(&BGSCodeObj, root, "GetDescription");
            RegisterFunction<GFX::HardcoreValues>(&BGSCodeObj, root, "HardcoreValues");
            RegisterFunction<GFX::InvFilter>(&BGSCodeObj, root, "InvFilter");
            RegisterFunction<GFX::InvItems>(&BGSCodeObj, root, "InvItems");
            RegisterFunction<GFX::PerksList>(&BGSCodeObj, root, "PerksList");
            RegisterFunction<GFX::RadiationValues>(&BGSCodeObj, root, "RadiationValues");
            RegisterFunction<GFX::RadXCount>(&BGSCodeObj, root, "RadXCount");
            RegisterFunction<GFX::SkillsList>(&BGSCodeObj, root, "SkillsList");
            RegisterFunction<GFX::StimpakCount>(&BGSCodeObj, root, "StimpakCount");
            RegisterFunction<GFX::UseDoctorBag>(&BGSCodeObj, root, "UseDoctorBag");
            RegisterFunction<GFX::UseRadX>(&BGSCodeObj, root, "UseRadX");
            RegisterFunction<GFX::UseStimpak>(&BGSCodeObj, root, "UseStimpak");
            root->Invoke("root.Menu_mc.onCodeObjUpdate", nullptr, nullptr, 0);
        } else {
            RegisterPipboyMenuInput(false);
        }
    }

    return kEvent_Continue;
}
MenuOpenCloseEventHandler g_MenuOpenCloseEventHandler;