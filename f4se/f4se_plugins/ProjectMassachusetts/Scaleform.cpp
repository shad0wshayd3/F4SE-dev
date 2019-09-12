#include "Scaleform.h"

#include "Condition.h"
#include "InputHandler.h"
#include "Perks.h"

#include "f4se/ScaleformLoader.h"
#include "f4se/ScaleformValue.h"

#include <chrono>
#include <regex>
#include <sstream>
#include <thread>
#include <vector>

#define PipboyInventoryObjects (*g_PipboyDataManager)->inventoryData.inventoryObjects

EntryList LevelUpObj;

struct EffectTest {
	std::string			Name;
	std::string			Description;
	UInt32				FormID;
	bool				HasDuration;
	int					Type;
};

void ActiveEffect_TEST() {
	std::vector<EffectTest> EffectsV;

	auto TActiveEffects = (*g_player)->magicTarget.GetActiveEffects();
	for (int i = 0; i < TActiveEffects->count; i++) {
		ActiveEffect* Effect = TActiveEffects->entries[i];

		if (Effect->flags & ActiveEffect::kFlag_Inactive)
			continue;

		if (Effect->data->setting->flags & EffectSetting::kFlag_HideInUI)
			continue;

		TESForm* SourceForm;
		if (Effect->unk60)
			SourceForm = Effect->unk60;
		else if (Effect->unk48)
			SourceForm = Effect->unk48;

		EffectTest NewEffect;
		for (auto iter : EffectsV) {
			if (SourceForm->formID == iter.FormID) {
				NewEffect = iter;
				break;
			}
		}

		_LogMessageNT("Effect Name: %s", Effect->data->setting->GetFullName());
		if (Effect->unk60)
			_LogMessageNT("Source Item: %s", Effect->unk60->GetFullName());
		else if (Effect->unk48)
			_LogMessageNT("Source Spell: %s", Effect->unk48->GetFullName());
		_LogMessageNT("%f | %f | %f | %f", Effect->unk78, Effect->unk7C, Effect->unk80, Effect->data->unk00);
		_LogMessageNT("");
	}
}

void OpenLevelUpMenu(int ID, int Level, int PointsBase, int PointsUsed) {
    LevelUpObj = EntryList(ID, Level, PointsBase, PointsUsed);

    BSFixedString LevelUpMenu("LevelUpMenu");
    CALL_MEMBER_FN(*g_uiMessageManager, SendUIMessage)(LevelUpMenu, kMessage_Open);
}

namespace BSRegex {
    using namespace std::regex_constants;

	std::regex Face("(face='\\$[A-Za-z0-9_]+')",										ECMAScript | icase);
	std::regex Size("(size='(([0-1]?[0-9])|(2[0-3]))')",								ECMAScript | icase);
	std::regex Color("(color='#[A-Fa-f0-9]+')",											ECMAScript | icase);
    std::regex Alias("(<(Alias|BaseName|Global|Relationship|Token)+[A-Z0-9=_. ]*>)",	ECMAScript | icase);
	std::regex Pagebreak("([<\\[](br|pagebreak)+[\\]>])",								ECMAScript | icase);

    std::regex SingleLine("(\r?\n)");
    std::regex DoubleLine("(\r?\n)(\r?\n)+");
}

namespace GFX {
    bool RepairList_Sort(RepairMenuEntry Entry1, RepairMenuEntry Entry2) {
        const char* Name1 = Entry1.text.c_str();
        const char* Name2 = Entry2.text.c_str();
        int Length = max(strlen(Name1), strlen(Name2));

        for (int i = 0; i < Length; i++)
            if (Entry1.text[i] != Entry2.text[i])
                return (Entry1.text[i] < Entry2.text[i]);

        if (Entry1.text != Entry2.text)
            return (Entry1.text < Entry2.text);

        return (Entry1.Condition > Entry2.Condition);
    }

	class ActiveEffects : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) {
			ActiveEffect_TEST();

			GFxMovieRoot* root = args->movie->movieRoot;
			root->CreateArray(args->result);

			GFxValue DataObj, ActiveEffects;
			root->GetVariable(&DataObj, "root.Menu_mc.DataObj");
			DataObj.GetMember("ActiveEffects", &ActiveEffects);

			std::vector<ActiveEffectEntry> ActiveEffectsV;
			for (int i = 0; i < ActiveEffects.GetArraySize(); i++) {
				GFxValue Entry, Text, Effects, Type;
				ActiveEffects.GetElement(i,	&Entry);
				Entry.GetMember("text",		&Text);
				Entry.GetMember("effects",	&Effects);
				Entry.GetMember("type",		&Type);

				std::vector<EffectEntry> EffectsV;
				for (int j = 0; j < Effects.GetArraySize(); j++) {
					GFxValue Effect;
					Effects.GetElement(j, &Effect);

					GFxValue Text, Duration, Value, ShowAsPercent;
					Effect.GetMember("text",			&Text);
					Effect.GetMember("duration",		&Duration);
					Effect.GetMember("value",			&Value);
					Effect.GetMember("showAsPercent",	&ShowAsPercent);

					EffectEntry nEffectEntry;
					nEffectEntry.text			= Text.GetString();
					nEffectEntry.Duration		= Duration.GetNumber();
					nEffectEntry.Value			= Value.GetNumber();
					nEffectEntry.ShowAsPercent	= ShowAsPercent.GetBool();

					EffectsV.emplace_back(nEffectEntry);
				}

				std::sort(EffectsV.begin(), EffectsV.end(),
					[](EffectEntry E1, EffectEntry E2) {
						const char* Name1 = E1.text.c_str();
						const char* Name2 = E2.text.c_str();
						int Length = max(strlen(Name1), strlen(Name2));

						for (int i = 0; i < Length; i++)
							if (E1.text[i] != E2.text[i])
								return (E1.text[i] < E2.text[i]);

						if (E1.text != E2.text)
							return (E1.text < E2.text);

						return (E1.Value > E2.Value);
					}
				);

				std::stringstream Description;
				bool HasDuration = false;
				
				for (auto iter : EffectsV) {
					for (auto& C : iter.text)
						C = toupper(C);

					Description << iter.text << " " << round(iter.Value) << ((iter.ShowAsPercent) ? "% " : " ");
					HasDuration = ((iter.Duration > 0) || HasDuration);
				}

				ActiveEffectEntry ActiveEffect;
				ActiveEffect.text			= Text.GetString();
				ActiveEffect.Description	= Description.str();
				ActiveEffect.HasDuration	= HasDuration;
				ActiveEffect.Type			= Type.GetUInt();

				ActiveEffectsV.emplace_back(ActiveEffect);
			}

			std::sort(ActiveEffectsV.begin(), ActiveEffectsV.end(),
				[](ActiveEffectEntry E1, ActiveEffectEntry E2) {
					if (E1.Type != E2.Type)
						return (E1.Type < E2.Type);

					const char* Name1 = E1.text.c_str();
					const char* Name2 = E2.text.c_str();
					int Length = max(strlen(Name1), strlen(Name2));

					for (int i = 0; i < Length; i++)
						if (E1.text[i] != E2.text[i])
							return (E1.text[i] < E2.text[i]);

					return (E1.text < E2.text);
				}
			);

			for (auto iter : ActiveEffectsV) {
				GFxValue Entry;
				root->CreateObject(&Entry);
				SetGFxValue(&Entry, root,	"text",			iter.text);
				SetGFxValue(&Entry, root,	"Original",		iter.text);
				SetGFxValue(&Entry, root,	"Description",	iter.Description);
				SetGFxValue(&Entry,			"HasDuration",	iter.HasDuration);
				SetGFxValue(&Entry,			"Type",			iter.Type);
				args->result->PushBack(&Entry);
			}
		}
	};

    class AddPerks : public GFxFunctionHandler {
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

    class AllowTextInput : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            (*g_inputMgr)->AllowTextInput(args->args[0].GetBool());
        }
    };

    class CloseMenu : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            BSFixedString MenuName(args->args[0].GetString());
            CALL_MEMBER_FN((*g_uiMessageManager), SendUIMessage)(MenuName, kMessage_Close);
        }
    };

    class DoctorBagCount : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            args->result->SetUInt(GetItemCount((*g_player), Forms::DoctorsBag));
        }
    };

    class FilterSearch : public GFxFunctionHandler {
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
                }
                else (Item.SetMember("filterFlag", &GFxValue(0)));

                Result[0].PushBack(&Item);
            }

            if (matchCount == 0) {
                GFxValue Default;
                root->CreateObject(&Default);
                SetGFxValue(&Default, root, "text",         "$NONE_MATCHING");
                SetGFxValue(&Default,       "filterFlag",   filterFlag);
                SetGFxValue(&Default,       "IsDefault",    true);
                Result[0].PushBack(&Default);
            }

            root->Invoke("root.Menu_mc.FilterSearchCallback", nullptr, Result, 2);
        }
    };

	class ForceUnlock : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) {
			//
		}
	};

	class GetDescription : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
			GFxMovieRoot* root = args->movie->movieRoot;

			GFxValue FormID;
			args->args[0].GetMember("formID", &FormID);

			std::string		Description;
			bool			HasFeatured	= false;

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

						Description = std::regex_replace(Description, BSRegex::Pagebreak,	"<br>");
						Description = std::regex_replace(Description, BSRegex::SingleLine,	"<br>");
						Description = std::regex_replace(Description, BSRegex::DoubleLine,	"<br>");
						Description = std::regex_replace(Description, BSRegex::Face,		"face='$MAIN_Font'");
						Description = std::regex_replace(Description, BSRegex::Size,		"size='24')");
						Description = std::regex_replace(Description, BSRegex::Color,		"color='#FFFFFF'");
					}

					Book->bounds1;
					Book->bounds2;
				}
			}

			bool HasDescription = (HasFeatured || !Description.empty());
			SetGFxValue(&args->args[0], root,	"Description",		Description);
			SetGFxValue(&args->args[0],			"HasDescription",	HasDescription);
        }
    };

    class HardcoreValues : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            args->movie->movieRoot->CreateObject(args->result);
            SetGFxValue(args->result, "H2O",        GetValueInt((*g_player), Forms::Dehydration));
            SetGFxValue(args->result, "FOD",        GetValueInt((*g_player), Forms::Starvation));
            SetGFxValue(args->result, "SLP",        GetValueInt((*g_player), Forms::SleepDeprivation));
            SetGFxValue(args->result, "IsEnabled",  false);
        }
    };

	class InvFilter : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) {
			GFxMovieRoot* root = args->movie->movieRoot;
			root->CreateArray(args->result);

			GFxValue DataObj, CurrentPage, CurrentTab;
			root->GetVariable(&DataObj, "root.Menu_mc.DataObj");
			DataObj.GetMember("CurrentPage",	&CurrentPage);
			DataObj.GetMember("CurrentTab",		&CurrentTab);

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

	class InvItems : public GFxFunctionHandler {
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

    class LockpickLevel : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            args->result->SetInt(GetValueInt((*g_player), Forms::Lockpick));
        }
    };

	class ModSkills : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            GFxValue Element, FormID, Value;

            for (int i = 0; i < args->args[0].GetArraySize(); i++) {
                args->args[0].GetElement(i, &Element);
                Element.GetMember("FormID", &FormID);
                Element.GetMember("Value",  &Value);

                ActorValueInfo* ActorValue = LookupTypeByID(FormID.GetUInt(), ActorValueInfo);
                ModPermValue((*g_player), ActorValue, Value.GetNumber());
            }
        }
    };

    class PerksList : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            GFxMovieRoot* root = args->movie->movieRoot;
            root->CreateArray(args->result);

            for (auto iter : Forms::ListMaster) {
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
                }
                else Ranks.emplace_back(iter);

                GFxValue Descriptions;
                root->CreateArray(&Descriptions);
                
                int NumRanks = 0;
                for (auto iterRank : Ranks) {
                    if (HasPerk((*g_player), iterRank))
                        NumRanks++;

                    BSString DescriptionText;
                    TESDescription* DescriptionForm = DYNAMIC_CAST(iter, BGSPerk, TESDescription);
                    CALL_MEMBER_FN(DescriptionForm, Get)(&DescriptionText, nullptr);
                    Descriptions.PushBack(&GFxValue(DescriptionText.Get()));
                }

                GFxValue Perk;
                root->CreateObject(&Perk);
                SetGFxValue(&Perk, root,    "text",         Name);
                SetGFxValue(&Perk, root,    "IconPath",     GetPerkIconPath(iter));
                SetGFxValue(&Perk,          "FormID",       iter->formID);
                SetGFxValue(&Perk,          "MaxRank",      iter->numRanks);
                SetGFxValue(&Perk,          "Rank",         NumRanks);

                Perk.SetMember("Descriptions", &Descriptions);
                args->result->PushBack(&Perk);
            }
        }
    };

    class PlaySoundUI : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            PlayUISound(args->args[0].GetString());
        }
    };

    class PopulateRepairItemCard : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            GFxMovieRoot* root = args->movie->movieRoot;

            UInt32 HandleID = args->args[0].GetUInt();
            UInt32 StackID  = args->args[1].GetUInt();

            BGSInventoryItem* Item = GetInventoryItemByHandleID(HandleID);
            if (!Item)
                return;

            if (!Item->form)
                return;

            BGSInventoryItem::Stack* Stack = GetStackByStackID(Item, StackID);
            if (!Stack)
                return;

            GFxValue CurrentCard, PreviewCard;
            args->args[2].GetMember("Current_mc",   &CurrentCard);
            args->args[2].GetMember("Preview_mc",   &PreviewCard);

            GFxValue CurrentInfo, PreviewInfo;
            CurrentCard.GetMember("InfoObj",        &CurrentInfo);
            PreviewCard.GetMember("InfoObj",        &PreviewInfo);

            if (!CurrentInfo.IsArray() || !PreviewInfo.IsArray())
                return;

            SimpleCollector<InvItemStack> Test{ 0, { } };
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

    class RadXCount : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            args->result->SetUInt(GetItemCount((*g_player), Forms::RadX));
        }
    };

    class RadiationValues : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            int Current = GetValueInt((*g_player), Forms::Rads);
            int Resists = GetValueInt((*g_player), Forms::RadResistExposure);
            int Maximum = ISettings::GetInteger("Radiation::Threshold05", 1000);

            std::map<int, const char*> RadiationThreshold;
            RadiationThreshold.emplace(200,     "Radiation::Threshold01");
            RadiationThreshold.emplace(400,     "Radiation::Threshold02");
            RadiationThreshold.emplace(600,     "Radiation::Threshold03");
            RadiationThreshold.emplace(800,     "Radiation::Threshold04");
            RadiationThreshold.emplace(1000,    "Radiation::Threshold05");

            int Next = 0;
            for (auto iter : RadiationThreshold) {
                Next = ISettings::GetInteger(iter.second, iter.first);
                if (Next > Current)
                    break;
            }

            args->movie->movieRoot->CreateObject(args->result);
            SetGFxValue(args->result, "Current", Current);
            SetGFxValue(args->result, "Resists", Resists);
            SetGFxValue(args->result, "Maximum", Maximum);
            SetGFxValue(args->result, "Next",    Next);
        }
    };

    class RepairList : public GFxFunctionHandler {
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
                        Entry.text          = GetTableValue(BSFixedString,  i,  "text");
                        Entry.HandleID      = GetHandleIDByIndex(i);
                        Entry.StackID       = GetStackIDByIndex(i);
                        Entry.Condition     = Percent;

                        if (Percent == 1.0)
                            RepairI.emplace_back(Entry);
                        else RepairV.emplace_back(Entry);
                        break;
                    }

                    default:
                        break;
                    }
                }
            }

            std::sort(RepairV.begin(), RepairV.end(), &RepairList_Sort);
            std::sort(RepairI.begin(), RepairI.end(), &RepairList_Sort);
            RepairV.insert(RepairV.end(), RepairI.begin(), RepairI.end());

            for (auto iter : RepairV) {
                GFxValue Entry;
                root->CreateObject(&Entry);
                SetGFxValue(&Entry, root,   "text",         iter.text);
                SetGFxValue(&Entry,         "HandleID",     iter.HandleID);
                SetGFxValue(&Entry,         "StackID",      iter.StackID);
                SetGFxValue(&Entry,         "Condition",    iter.Condition);
                args->result->PushBack(&Entry);
            }
        }
    };

    class RepairMenuInit : public GFxFunctionHandler {
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

    class SkillsList : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            GFxMovieRoot* root = args->movie->movieRoot;
            root->CreateArray(args->result);

            for (auto iter : Forms::ListSkills) {
                BSString DescriptionText;
                TESDescription* DescriptionForm = DYNAMIC_CAST(iter, ActorValueInfo, TESDescription);
                CALL_MEMBER_FN(DescriptionForm, Get)(&DescriptionText, nullptr);

                GFxValue Skill;
                root->CreateObject(&Skill);
                SetGFxValue(&Skill, root,   "text",         iter->GetFullName());
                SetGFxValue(&Skill, root,   "Description",  DescriptionText.Get());
                SetGFxValue(&Skill, root,   "IconPath",     GetSkillIconPath(iter));
                SetGFxValue(&Skill,         "Value",        GetValueInt((*g_player), iter));
                SetGFxValue(&Skill,         "Modifier",     GetTempMod((*g_player), iter));
                args->result->PushBack(&Skill);
            }
        }
    };

    class StimpakCount : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            args->result->SetUInt(GetItemCount((*g_player), Forms::StimpakOrder));
        }
    };

    class TagSkills : public GFxFunctionHandler {
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

    class UseDoctorBag : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) {
			EquipItem((*g_player), Forms::DoctorsBag, false);
		}
	};

	class UseRadX : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) {
			EquipItem((*g_player), Forms::RadX, false);
		}
	};

	class UseStimpak : public GFxFunctionHandler {
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
}

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

        SetGFxValue(Entry,  "value",    std::stoi(current));
        SetGFxValue(Entry,  "max",      std::stoi(maximum));
        SetGFxValue(Entry,  "IsMeter",  true);
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
                        SetGFxValue(&MaxCondEntry, root,    "text",         "$MAX CND");
                        SetGFxValue(&MaxCondEntry,          "value",        maximum);
                        SetGFxValue(&MaxCondEntry,          "IsMeterMax",   true);
                        InfoObj->PushBack(&MaxCondEntry);
                    }
                }
            }
        }
    }
}

namespace Menus {
    const char* BlurMenu::sMenuName     = "BlurMenu";
    const char* RepairMenu::sMenuName   = "RepairMenu";

    BlurMenu::BlurMenu() : GameMenuBase() {
        flags = kFlag_BlurBackground |
                kFlag_DisableInteractive;

        depth = 0x08;
    }

    BlurMenu::~BlurMenu() {
        //
    }

    void BlurMenu::RegisterFunctions() {
        //
    }

    void BlurMenu::DrawNextFrame(float unk0, void* unk1) {
        __super::DrawNextFrame(unk0, unk1);
    }

    RepairMenu::RepairMenu() : GameMenuBase() {
        flags = kFlag_PauseGame                     |
                kFlag_ShowCursor                    |
                kFlag_PreventGameLoad               |
                kFlag_HideOther                     |
                kFlag_UpdateCursorOnPlatformChange  |
                //kFlag_ApplyDropDownFilter           |
                kFlag_Unk10000                      |
                kFlag_BlurBackground;

        depth = 0x09;

        if (CALL_MEMBER_FN((*g_scaleformManager), LoadMovie)(this, movie, "RepairMenu", "root", 2)) {
            GFxMovieRoot* root = movie->movieRoot;
            RegisterRepairMenuInput(true);

            CreateBaseShaderTarget(shaderTarget, stage);
            shaderTarget->SetFilterColor(false);

            //InitializeButtonBar(this, shaderTarget, "root.Menu_mc.ButtonHintBar_mc", 2);

            GFxValue BGSCodeObj;
            root->GetVariable(&BGSCodeObj, "root.Menu_mc.BGSCodeObj");
            RegisterFunction<GFX::CloseMenu>                (&BGSCodeObj, root, "CloseMenu");
            RegisterFunction<GFX::PlaySoundUI>              (&BGSCodeObj, root, "PlaySound");
            RegisterFunction<GFX::PopulateRepairItemCard>   (&BGSCodeObj, root, "PopulateRepairItemCard");
            RegisterFunction<GFX::RepairList>               (&BGSCodeObj, root, "RepairList");
            RegisterFunction<GFX::RepairMenuInit>           (&BGSCodeObj, root, "RepairMenuInit");
            root->Invoke("root.Menu_mc.onCodeObjCreate", nullptr, nullptr, 0);
        }
    }

    RepairMenu::~RepairMenu() {
        //
    }

    void RepairMenu::RegisterFunctions() {
        //
    }

    void RepairMenu::DrawNextFrame(float unk0, void* unk1) {
        __super::DrawNextFrame(unk0, unk1);
    }
}

namespace Threads {
    bool IsBlockingMenuOpen(bool AllowDialogueMenu) {
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
}

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
            BGSInventoryItem*   Item    = GetInventoryItemByIndex(SelectedIndex);
            UInt32              StackID = GetStackIDByIndex(SelectedIndex);

            SimpleCollector<InvItemStack> Blank{ 0, { } };
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

    GFxValue SWFPath; root->GetVariable(&SWFPath, "root.loaderInfo.url");
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
        SetGFxValue(&InfoCardEntry, root,   "text",     "$CND");
        SetGFxValue(&InfoCardEntry,         "value",    Value);
        SetGFxValue(&InfoCardEntry,         "max",      1.0);
        SetGFxValue(&InfoCardEntry,         "IsMeter",  true);
        InfoObj->PushBack(&InfoCardEntry);

        if (isExamineMenu) {
            float maximum = GetWeaponConditionMaximum(WeaponConditionData(Item->form, extraDataList));
            if (maximum != -1.0) {
                GFxValue MaxCondEntry;
                root->CreateObject(&MaxCondEntry);
                SetGFxValue(&MaxCondEntry, root,    "text",         "$MAX CND");
                SetGFxValue(&MaxCondEntry,          "value",        maximum);
                SetGFxValue(&MaxCondEntry,          "IsMeterMax",   true);
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
    }
    else {
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
            RegisterFunction<GFX::AddPerks>         (&BGSCodeObj, root, "AddPerks");
            RegisterFunction<GFX::AllowTextInput>   (&BGSCodeObj, root, "AllowTextInput");
            RegisterFunction<GFX::FilterSearch>     (&BGSCodeObj, root, "FilterSearch");
            RegisterFunction<GFX::ModSkills>        (&BGSCodeObj, root, "ModSkills");
            RegisterFunction<GFX::TagSkills>        (&BGSCodeObj, root, "TagSkills");
            root->Invoke("root.Menu_mc.onCodeObjUpdate", nullptr, nullptr, 0);

            GFxValue Args[5];
            GFxValue* Result = LevelUpObj.BuildEntryList(root, Args);
            root->Invoke("root.Menu_mc.Init", nullptr, Result, 5);
        }
        else {
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
			RegisterFunction<GFX::ForceUnlock>		(&BGSCodeObj, root, "ForceUnlock");
            RegisterFunction<GFX::LockpickLevel>    (&BGSCodeObj, root, "LockpickLevel");
            root->Invoke("root.Menu_mc.onCodeObjUpdate", nullptr, nullptr, 0);
        }
        else {
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
			RegisterFunction<GFX::ActiveEffects>	(&BGSCodeObj, root, "ActiveEffects");
            RegisterFunction<GFX::DoctorBagCount>   (&BGSCodeObj, root, "DoctorBagCount");
			RegisterFunction<GFX::GetDescription>	(&BGSCodeObj, root, "GetDescription");
            RegisterFunction<GFX::HardcoreValues>   (&BGSCodeObj, root, "HardcoreValues");
			RegisterFunction<GFX::InvFilter>		(&BGSCodeObj, root, "InvFilter");
			RegisterFunction<GFX::InvItems>			(&BGSCodeObj, root, "InvItems");
            RegisterFunction<GFX::PerksList>        (&BGSCodeObj, root, "PerksList");
            RegisterFunction<GFX::RadiationValues>  (&BGSCodeObj, root, "RadiationValues");
            RegisterFunction<GFX::RadXCount>        (&BGSCodeObj, root, "RadXCount");
            RegisterFunction<GFX::SkillsList>       (&BGSCodeObj, root, "SkillsList");
            RegisterFunction<GFX::StimpakCount>     (&BGSCodeObj, root, "StimpakCount");
			RegisterFunction<GFX::UseDoctorBag>		(&BGSCodeObj, root, "UseDoctorBag");
			RegisterFunction<GFX::UseRadX>			(&BGSCodeObj, root, "UseRadX");
			RegisterFunction<GFX::UseStimpak>		(&BGSCodeObj, root, "UseStimpak");
            root->Invoke("root.Menu_mc.onCodeObjUpdate", nullptr, nullptr, 0);
        }
        else {
            RegisterPipboyMenuInput(false);
        }
    }

    return kEvent_Continue;
}
MenuOpenCloseEventHandler g_MenuOpenCloseEventHandler;