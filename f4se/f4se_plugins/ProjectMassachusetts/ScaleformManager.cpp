#include "ScaleformManager.h"

#include "PerkManager.h"
#include "ValueManager.h"

#include "f4se/PapyrusEvents.h"

#include <regex>

namespace LevelUpMenuInput {
    void ProcessUserEvent(const char* controlName, bool isDown) {
        BSFixedString LevelUpMenu("LevelUpMenu");
        if ((*g_ui)->IsMenuOpen(LevelUpMenu)) {
            IMenu* LUM = (*g_ui)->GetMenu(LevelUpMenu);
            GFxMovieRoot* movieRoot = LUM->movie->movieRoot;

            GFxValue args[2];
            args[0].SetString(controlName);
            args[1].SetBool(isDown);

            movieRoot->Invoke("root.Menu_mc.ProcessUserEvent", nullptr, args, 2);
        }
    }

    void RegisterForInput(bool enable);
    class LUMInputHandler : public BSInputEventUser {
    public:
        LUMInputHandler() : BSInputEventUser(true) { }

        virtual void OnButtonEvent(ButtonEvent* inputEvent) {
            std::string control = "";

            float timer = inputEvent->timer;
            bool isDown = inputEvent->isDown == 1.0f && timer == 0.0f;
            bool isUp = inputEvent->isDown == 0.0f && timer != 0.0f;

            switch (inputEvent->keyMask) {
            case 0x0D:
            case 0x45:
            case 4096:
                control = "Accept";
                break;

            case 0x52:
            case 16384:
                control = "Continue";
                break;

            case 0x54:
            case 32768:
                control = "Reset";
                break;
            }

            if (isDown)
                ProcessUserEvent(control.c_str(), true);
            else if (isUp)
                ProcessUserEvent(control.c_str(), false);
        }
    };
    LUMInputHandler g_scaleformInputHandler;

    void RegisterForInput(bool enable) {
        if (enable) {
            g_scaleformInputHandler.enabled = true;
            tArray<BSInputEventUser*>* inputEvents = &((*g_menuControls)->inputEvents);
            BSInputEventUser* inputHandler = &g_scaleformInputHandler;

            if (inputEvents->GetItemIndex(inputHandler) == -1)
                inputEvents->Push(&g_scaleformInputHandler);
        }

        else
            g_scaleformInputHandler.enabled = false;
    }
}

namespace LockpickingMenuInput {
    void ProcessUserEvent(const char* controlName, bool isDown) {
        BSFixedString LockpickingMenu("LockpickingMenu");
        if ((*g_ui)->IsMenuOpen(LockpickingMenu)) {
            IMenu* LPM = (*g_ui)->GetMenu(LockpickingMenu);
            GFxMovieRoot* movieRoot = LPM->movie->movieRoot;

            GFxValue args[2];
            args[0].SetString(controlName);
            args[1].SetBool(isDown);

            movieRoot->Invoke("root.LockpickingMenu_mc.ProcessUserEvent", nullptr, args, 2);
        }
    }

    void RegisterForInput(bool enable);
    class LPMInputHandler : public BSInputEventUser {
    public:
        LPMInputHandler() : BSInputEventUser(true) { }

        virtual void OnButtonEvent(ButtonEvent* inputEvent) {
            std::string control = "";

            float timer = inputEvent->timer;
            bool isDown = inputEvent->isDown == 1.0f && timer == 0.0f;
            bool isUp = inputEvent->isDown == 0.0f && timer != 0.0f;

            switch (inputEvent->keyMask) {
            case 0x1B:
            case 8192:
                RegisterForInput(false);
                return;

            case 0x46:
            case 16384:
                control = "Force";
                break;
            }

            if (isDown)
                ProcessUserEvent(control.c_str(), true);
            else if (isUp)
                ProcessUserEvent(control.c_str(), false);
        }
    };
    LPMInputHandler g_scaleformInputHandler;

    void RegisterForInput(bool enable) {
        if (enable) {
            g_scaleformInputHandler.enabled = true;
            tArray<BSInputEventUser*>* inputEvents = &((*g_menuControls)->inputEvents);
            BSInputEventUser* inputHandler = &g_scaleformInputHandler;

            if (inputEvents->GetItemIndex(inputHandler) == -1)
                inputEvents->Push(&g_scaleformInputHandler);
        }

        else
            g_scaleformInputHandler.enabled = false;
    }
}

namespace BSRegex {
    using namespace std::regex_constants;

    std::regex BSComment("(<!--[A-Za-z ]*-->)(\r?\n)*", ECMAScript | icase);
    std::regex BSPagebreak("([<\\[](br|pagebreak)+[\\]>])", ECMAScript | icase);
    std::regex BSSimple("(</?(i|b|u|ul|li|p|font)+>)", ECMAScript | icase);

    std::regex BSEmbed("(<(font|img|p)+ [A-Z0-9=_'\":./$# ]+>)(\r?\n)*", ECMAScript | icase);
    std::regex BSAlias("(<(Alias|BaseName|Global|Relationship|Token)+[A-Z0-9=_. ]*>)", ECMAScript | icase);

    std::regex BSSingleLine("(\r?\n)");
    std::regex BSDoubleLine("(\r?\n)(\r?\n)+");

    std::string GetBookText(TESForm* bookForm) {
        if (!bookForm)
            return "";

        TESObjectBOOK* thisBook = DYNAMIC_CAST(bookForm, TESForm, TESObjectBOOK);
        if (!thisBook)
            return "";

        BSString descText;
        TESDescription* bookDesc = DYNAMIC_CAST(thisBook, TESObjectBOOK, TESDescription);
        CALL_MEMBER_FN(bookDesc, Get)(&descText, nullptr);

        std::string bookText = descText.Get();

        if (std::regex_search(bookText, BSAlias)) {
            auto inventory = (*g_player)->inventoryList;

            if (inventory) {
                inventory->inventoryLock.LockForRead();

                for (int i = 0; i < inventory->items.count; i++) {
                    BGSInventoryItem item;
                    inventory->items.GetNthItem(i, item);

                    if (item.form->formID == bookForm->formID) {
                        BSExtraData* rawData = item.stack->extraData->GetByType(kExtraData_TextDisplayData);

                        if (rawData) {
                            ExtraTextDisplayData* textData = DYNAMIC_CAST(rawData, BSExtraData, ExtraTextDisplayData);

                            if (textData) {
                                DoTokenReplacement_Internal(textData, descText);
                                bookText = descText.Get();
                            }
                        }

                        break;
                    }
                }

                inventory->inventoryLock.Unlock();
            }
        }

        bookText = std::regex_replace(bookText, BSPagebreak, "\n");
        bookText = std::regex_replace(bookText, BSComment, "");
        bookText = std::regex_replace(bookText, BSSimple, "");
        bookText = std::regex_replace(bookText, BSEmbed, "");
        bookText = std::regex_replace(bookText, BSSingleLine, "\n");
        bookText = std::regex_replace(bookText, BSDoubleLine, "\n\n");

        return bookText;
    }
}

namespace ScaleformFunctions {
    void EquipItem(Actor* actor, TESForm* form, bool preventRemoval, bool silent) {
        // Obviously, this implimentation is terrible. But it's less bad than alternative. Sort of.
        // The alternative being Scaleform -> F4SE -> Papyrus -> Papyrus, this just cuts out an ExternalEvent registration.
        VMArray<VMVariable> args;
        VMVariable akItem, abPrevent, abSilent;

        akItem.Set(&form);
        abPrevent.Set(&preventRemoval);
        abSilent.Set(&silent);

        args.Push(&akItem);
        args.Push(&abPrevent);
        args.Push(&abSilent);

        CallFunctionNoWait<Actor>(actor, "EquipItem", args);
    }

    class GFxPlayMenuSound : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            if (args->args[0].IsString())
                PlayUISound(args->args[0].GetString());
        }
    };

    class GFxStimpakCount : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            std::string itemCount = std::to_string(GetItemCount((*g_player), DataManager::ObjectTypeStimpak));
            args->movie->movieRoot->CreateString(args->result, itemCount.c_str());
        }
    };

    class GFxUseStimpak : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            for (int i = 0; i < DataManager::StimpakOrder->forms.count; i++) {
                TESForm* thisForm = DataManager::StimpakOrder->forms[i];

                if (GetItemCount((*g_player), thisForm) > 0) {
                    EquipItem((*g_player), thisForm, false, true);
                    break;
                }
            }
        }
    };

    class GFxRadXCount : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            std::string itemCount = std::to_string(GetItemCount((*g_player), DataManager::RadX));
            args->movie->movieRoot->CreateString(args->result, itemCount.c_str());
        }
    };

    class GFxUseRadX : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            EquipItem((*g_player), DataManager::RadX, false, true);
        }
    };

    class GFxDoctorBagCount : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            std::string itemCount = std::to_string(GetItemCount((*g_player), DataManager::DoctorsBag));
            args->movie->movieRoot->CreateString(args->result, itemCount.c_str());
        }
    };

    class GFxUseDoctorBag : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            EquipItem((*g_player), DataManager::DoctorsBag, false, true);
        }
    };

    class GFxSetLimbTarget : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            DataManager::LimbTarget->value = args->args[0].GetUInt();
        }
    };

    class GFxConsoleLimbSelect : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            DataManager::LimbTarget->value = 7.0;
            EquipItem((*g_player), DataManager::DoctorsBag, false, true);
        }
    };

    class GFxHardcoreData : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            args->movie->movieRoot->CreateObject(args->result, "Object");

            if (g_GlobalSettings.GetBool("Rule_HardcoreNeeds")) {
                GFxHelperFunctions::RegisterInt(args->result, "H2O", (int)(*g_player)->actorValueOwner.GetValue(DataManager::Dehydration));
                GFxHelperFunctions::RegisterInt(args->result, "FOD", (int)(*g_player)->actorValueOwner.GetValue(DataManager::Starvation));
                GFxHelperFunctions::RegisterInt(args->result, "SLP", (int)(*g_player)->actorValueOwner.GetValue(DataManager::SleepDeprivation));
            }
            else {
                GFxHelperFunctions::RegisterInt(args->result, "H2O", -1);
                GFxHelperFunctions::RegisterInt(args->result, "FOD", -1);
                GFxHelperFunctions::RegisterInt(args->result, "SLP", -1);
            }
        }
    };

    class GFxRadiationData : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            float CurrentRads = (*g_player)->actorValueOwner.GetValue(DataManager::Rads);
            float CurrentMax = g_GlobalSettings.GetFloat("RadiationThreshold05");

            args->movie->movieRoot->CreateObject(args->result, "Object");
            GFxHelperFunctions::RegisterInt(args->result, "current", (int)CurrentRads);
            GFxHelperFunctions::RegisterInt(args->result, "next", (int)CurrentMax);
            GFxHelperFunctions::RegisterInt(args->result, "max", (int)CurrentMax);

            for (int i = 1; i < 6; i++) {
                char RadiationThreshold[21]; sprintf_s(RadiationThreshold,
                    sizeof(RadiationThreshold), "RadiationThreshold0%i", i);

                float value = g_GlobalSettings.GetFloat(RadiationThreshold);
                if (CurrentRads < value) {
                    GFxHelperFunctions::RegisterInt(args->result, "next", value);
                    break;
                }
            }

            GFxHelperFunctions::RegisterInt(args->result, "resist", (int)ceilf((*g_player)->actorValueOwner.GetValue(DataManager::RadResistExposure)));
        }
    };

    class GFxExperienceData : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            int PlayerLevel = GetLevel(*g_player);
            int ExperenceRequirement = GetXPForLevel(PlayerLevel);

            float CurrentXP = (*g_player)->actorValueOwner.GetValue(DataManager::Experience) - ExperenceRequirement;
            float CurrentMax = GetXPForLevel(PlayerLevel + 1) - ExperenceRequirement;

            args->movie->movieRoot->CreateObject(args->result, "Object");
            GFxHelperFunctions::RegisterInt(args->result, "current", (int)CurrentXP);
            GFxHelperFunctions::RegisterInt(args->result, "max", (int)CurrentMax);
        }
    };

    class GFxSkillList : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            GFxMovieRoot* root = args->movie->movieRoot;
            root->CreateArray(args->result);

            ActorValueOwner* owner = DYNAMIC_CAST((*g_player), PlayerCharacter, ActorValueOwner);

            for (auto iter : DataManager::m_SkillList) {
                GFxValue Skill;
                root->CreateObject(&Skill);

                float value = owner->GetValue(iter);

                GFxHelperFunctions::RegisterString(&Skill, root, "text", iter->GetFullName());
                GFxHelperFunctions::RegisterString(&Skill, root, "EditorID", iter->GetEditorID());

                BSString description;
                TESDescription* avDesc = DYNAMIC_CAST(iter, ActorValueInfo, TESDescription);
                CALL_MEMBER_FN(avDesc, Get)(&description, nullptr);

                GFxHelperFunctions::RegisterString(&Skill, root, "description", description.Get());
                GFxHelperFunctions::RegisterInt(&Skill, "value", (int)floorf(value));

                float modifier = owner->GetMod(0, iter) + (owner->GetMod(0, ValueManager::GetDependent(iter)) * 2) + floorf(owner->GetMod(0, DataManager::Luck));
                GFxHelperFunctions::RegisterNumber(&Skill, "modifier", (double)modifier);

                args->result->PushBack(&Skill);
            }
        }
    };

    class GFxPerkList : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            GFxMovieRoot* root = args->movie->movieRoot;
            root->CreateArray(args->result);

            for (auto iter : DataManager::m_MasterPerkList) {
                if (iter->hidden || (iter->numRanks == 0))
                    continue;

                if (HasPerk((*g_player), iter)) {
                    std::vector<BGSPerk*> Ranks;
                    if (iter->numRanks > 1) {
                        Ranks = PerkManager::GetRankList(iter);
                        if (Ranks[0] != iter)
                            continue;
                    }
                    else
                        Ranks.emplace_back(iter);

                    GFxValue GFxPerk;
                    GFxValue GFxDesc;
                    int NumRanks = 0;

                    root->CreateArray(&GFxDesc);
                    root->CreateObject(&GFxPerk);

                    for (auto iterRank : Ranks) {
                        if (HasPerk((*g_player), iterRank))
                            NumRanks++;

                        BSString DescText;
                        TESDescription* DescriptionForm = DYNAMIC_CAST(iter, BGSPerk, TESDescription);
                        CALL_MEMBER_FN(DescriptionForm, Get)(&DescText, nullptr);

                        GFxValue nDesc(DescText.Get());
                        GFxDesc.PushBack(&nDesc);
                    }

                    std::string PerkName = iter->GetFullName();
                    if (PerkName.empty())
                        continue;

                    GFxHelperFunctions::RegisterString(&GFxPerk, root, "text", PerkName.c_str());
                    GFxHelperFunctions::RegisterString(&GFxPerk, root, "SWFFile", iter->swfPath.c_str());
                    GFxPerk.SetMember("descriptions", &GFxDesc);

                    GFxHelperFunctions::RegisterInt(&GFxPerk, "FormID", iter->formID);
                    GFxHelperFunctions::RegisterInt(&GFxPerk, "maxRank", Ranks.size());
                    GFxHelperFunctions::RegisterInt(&GFxPerk, "rank", NumRanks);

                    args->result->PushBack(&GFxPerk);
                }
            }
        }
    };

    class GFxAddPerks : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            int count = args->args[0].GetArraySize();

            GFxValue ArrayElement;
            GFxValue ObjElementFormID;

            for (int i = 0; i < count; i++) {
                args->args[0].GetElement(i, &ArrayElement);
                ArrayElement.GetMember("FormID", &ObjElementFormID);

                BGSPerk* newPerk = DYNAMIC_CAST(LookupFormByID(ObjElementFormID.GetUInt()), TESForm, BGSPerk);
                if (!HasPerk((*g_player), newPerk))
                    AddPerk((*g_player), newPerk, false);
            }
        }
    };

    class GFxModSkills : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            int count = args->args[0].GetArraySize();

            GFxValue ArrayElement;
            GFxValue ObjElementFormID;
            GFxValue ObjElementValue;

            for (int i = 0; i < count; i++) {
                args->args[0].GetElement(i, &ArrayElement);
                ArrayElement.GetMember("FormID", &ObjElementFormID);
                ArrayElement.GetMember("Value", &ObjElementValue);

                ActorValueInfo* ActorValue = DYNAMIC_CAST(LookupFormByID(ObjElementFormID.GetInt()), TESForm, ActorValueInfo);
                (*g_player)->actorValueOwner.Mod(1, ActorValue, ObjElementValue.GetNumber());
            }
        }
    };

    class GFxTagSkills : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            int count = args->args[0].GetArraySize();

            GFxValue ArrayElement;
            GFxValue ObjElementFormID;

            if (PerkManager::m_PlayerTags.Barter)
                (*g_player)->actorValueOwner.Mod(1, DataManager::Barter, -15);

            if (PerkManager::m_PlayerTags.EnergyWeapons)
                (*g_player)->actorValueOwner.Mod(1, DataManager::EnergyWeapons, -15);

            if (PerkManager::m_PlayerTags.Explosives)
                (*g_player)->actorValueOwner.Mod(1, DataManager::Explosives, -15);

            if (PerkManager::m_PlayerTags.Guns)
                (*g_player)->actorValueOwner.Mod(1, DataManager::Guns, -15);

            if (PerkManager::m_PlayerTags.Lockpick)
                (*g_player)->actorValueOwner.Mod(1, DataManager::Lockpick, -15);

            if (PerkManager::m_PlayerTags.Medicine)
                (*g_player)->actorValueOwner.Mod(1, DataManager::Medicine, -15);

            if (PerkManager::m_PlayerTags.MeleeWeapons)
                (*g_player)->actorValueOwner.Mod(1, DataManager::MeleeWeapons, -15);

            if (PerkManager::m_PlayerTags.Repair)
                (*g_player)->actorValueOwner.Mod(1, DataManager::Repair, -15);

            if (PerkManager::m_PlayerTags.Science)
                (*g_player)->actorValueOwner.Mod(1, DataManager::Science, -15);

            if (PerkManager::m_PlayerTags.Sneak)
                (*g_player)->actorValueOwner.Mod(1, DataManager::Sneak, -15);

            if (PerkManager::m_PlayerTags.Speech)
                (*g_player)->actorValueOwner.Mod(1, DataManager::Speech, -15);

            if (PerkManager::m_PlayerTags.Survival)
                (*g_player)->actorValueOwner.Mod(1, DataManager::Survival, -15);

            if (PerkManager::m_PlayerTags.Unarmed)
                (*g_player)->actorValueOwner.Mod(1, DataManager::Unarmed, -15);

            PerkManager::m_PlayerTags = PerkManager::m_DefaultTags;

            for (int i = 0; i < count; i++) {
                args->args[0].GetElement(i, &ArrayElement);
                ArrayElement.GetMember("FormID", &ObjElementFormID);

                if (ObjElementFormID.GetInt() == DataManager::Barter->formID)
                    PerkManager::m_PlayerTags.Barter = true;

                else if (ObjElementFormID.GetInt() == DataManager::EnergyWeapons->formID)
                    PerkManager::m_PlayerTags.EnergyWeapons = true;

                else if (ObjElementFormID.GetInt() == DataManager::Explosives->formID)
                    PerkManager::m_PlayerTags.Explosives = true;

                else if (ObjElementFormID.GetInt() == DataManager::Guns->formID)
                    PerkManager::m_PlayerTags.Guns = true;

                else if (ObjElementFormID.GetInt() == DataManager::Lockpick->formID)
                    PerkManager::m_PlayerTags.Lockpick = true;

                else if (ObjElementFormID.GetInt() == DataManager::Medicine->formID)
                    PerkManager::m_PlayerTags.Medicine = true;

                else if (ObjElementFormID.GetInt() == DataManager::MeleeWeapons->formID)
                    PerkManager::m_PlayerTags.MeleeWeapons = true;

                else if (ObjElementFormID.GetInt() == DataManager::Repair->formID)
                    PerkManager::m_PlayerTags.Repair = true;

                else if (ObjElementFormID.GetInt() == DataManager::Science->formID)
                    PerkManager::m_PlayerTags.Science = true;

                else if (ObjElementFormID.GetInt() == DataManager::Sneak->formID)
                    PerkManager::m_PlayerTags.Sneak = true;

                else if (ObjElementFormID.GetInt() == DataManager::Speech->formID)
                    PerkManager::m_PlayerTags.Speech = true;

                else if (ObjElementFormID.GetInt() == DataManager::Survival->formID)
                    PerkManager::m_PlayerTags.Survival = true;

                else if (ObjElementFormID.GetInt() == DataManager::Unarmed->formID)
                    PerkManager::m_PlayerTags.Unarmed = true;

                ActorValueInfo* ActorValue = DYNAMIC_CAST(LookupFormByID(ObjElementFormID.GetInt()), TESForm, ActorValueInfo);
                (*g_player)->actorValueOwner.Mod(1, ActorValue, 15);
            }
        }
    };

    class GFxBlurBackground : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            ApplyImagespaceModifier(DataManager::PipboyMenuIMOD, 1.0, NULL);
        }
    };

    class GFxUnblurBackground : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            RemoveImagespaceModifier(DataManager::PipboyMenuIMOD);
        }
    };

    class GFxCloseLevelUpMenu : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            BSFixedString LevelUpMenu("LevelUpMenu");
            CALL_MEMBER_FN(*g_uiMessageManager, SendUIMessage)(LevelUpMenu, kMessage_Close);
            LevelUpMenuInput::RegisterForInput(false);
            RemoveImagespaceModifier(DataManager::PipboyMenuIMOD);
        }
    };

    class GFxForceUnlock : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            BSFixedString LockpickingMenu("LockpickingMenu");
            CALL_MEMBER_FN(*g_uiMessageManager, SendUIMessage)(LockpickingMenu, kMessage_Close);
            VMArray<VMVariable> pArgs; CallFunctionNoWait<TESQuest>(DataManager::PerksQuest, "OnForceLock", pArgs);
        }
    };

    class GFxGetLockpickLevel : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            std::string level = std::to_string((*g_player)->actorValueOwner.GetValue(DataManager::Lockpick));
            args->movie->movieRoot->CreateString(args->result, level.c_str());
        }
    };

    class GFxUpdateInvFilter : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            args->movie->movieRoot->CreateArray(args->result);

            for (int i = 0; i < args->args[0].GetArraySize(); i++) {
                GFxValue item;
                GFxValue Filter;
                GFxValue FormID;
                GFxValue Holotape;

                args->args[0].GetElement(i, &item);
                item.GetMember("filterFlag", &Filter);
                Holotape.SetBool(false);

                switch (Filter.GetUInt()) {
                case 8704:
                    Holotape.SetBool(true);

                case 640:
                    Filter.SetUInt(16384);
                    item.SetMember("filterFlag", &Filter);
                    item.SetMember("isHolotape", &Holotape);
                    break;

                case 512:
                    item.GetMember("formID", &FormID);
                    if (LookupFormByID(FormID.GetUInt())->formType == FormType::kFormType_KEYM)
                        Filter.SetUInt(32768); item.SetMember("filterFlag", &Filter);
                    break;

                default:
                    break;
                }

                args->result->PushBack(&item);
            }
        }
    };

    class GFxGetBookText : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            GFxValue FormID; args->args[0].GetMember("formID", &FormID);
            std::string bText = BSRegex::GetBookText(LookupFormByID(FormID.GetUInt()));
            args->args[0].SetMember("description", &GFxValue(bText.c_str()));
        }
    };
}

bool ScaleformManager::Init(GFxMovieView* View, GFxValue* F4SERoot) {
    using namespace ScaleformFunctions;

    GFxValue currentSWFPath;
    std::string currentSWFPathString = "";
    GFxMovieRoot* movieRoot = View->movieRoot;

    if (movieRoot->GetVariable(&currentSWFPath, "root.loaderInfo.url")) {
        currentSWFPathString = currentSWFPath.GetString();

        if (currentSWFPathString.find("HUDMenu.swf") != std::string::npos)
            movieRoot->SetVariable("root.LeftMeters_mc.HPMeter_mc.RadsBar_mc.visible", &GFxValue(false));

        else if (currentSWFPathString.find("LevelUpMenu.swf") != std::string::npos) {
            RegisterFunction<GFxPlayMenuSound>      (F4SERoot, movieRoot, "PlaySound");
            RegisterFunction<GFxBlurBackground>     (F4SERoot, movieRoot, "BlurBackground");
            RegisterFunction<GFxUnblurBackground>   (F4SERoot, movieRoot, "UnblurBackground");
            RegisterFunction<GFxCloseLevelUpMenu>   (F4SERoot, movieRoot, "CloseLevelUpMenu");
            RegisterFunction<GFxAddPerks>           (F4SERoot, movieRoot, "AddPerks");
            RegisterFunction<GFxModSkills>          (F4SERoot, movieRoot, "ModSkills");
            RegisterFunction<GFxTagSkills>          (F4SERoot, movieRoot, "TagSkills");

            PerkManager::ProcessPerkList(movieRoot);
            LevelUpMenuInput::RegisterForInput(true);
        }

        else if (currentSWFPathString.find("LockpickingMenu.swf") != std::string::npos) {
            RegisterFunction<GFxForceUnlock>        (F4SERoot, movieRoot, "ForceUnlock");
            RegisterFunction<GFxGetLockpickLevel>   (F4SERoot, movieRoot, "GetLockpickLevel");
            LockpickingMenuInput::RegisterForInput(true);
        }

        else if (currentSWFPathString.find("Pipboy") != std::string::npos) {
            RegisterFunction<GFxStimpakCount>       (F4SERoot, movieRoot, "StimpakCount");
            RegisterFunction<GFxUseStimpak>         (F4SERoot, movieRoot, "UseStimpak");
            RegisterFunction<GFxRadXCount>          (F4SERoot, movieRoot, "RadXCount");
            RegisterFunction<GFxUseRadX>            (F4SERoot, movieRoot, "UseRadX");
            RegisterFunction<GFxDoctorBagCount>     (F4SERoot, movieRoot, "DoctorBagCount");
            RegisterFunction<GFxUseDoctorBag>       (F4SERoot, movieRoot, "UseDoctorBag");
            RegisterFunction<GFxSetLimbTarget>      (F4SERoot, movieRoot, "SetLimbTarget");
            RegisterFunction<GFxConsoleLimbSelect>  (F4SERoot, movieRoot, "ConsoleLimbSelect");
            RegisterFunction<GFxHardcoreData>       (F4SERoot, movieRoot, "HardcoreData");
            RegisterFunction<GFxRadiationData>      (F4SERoot, movieRoot, "RadiationData");
            RegisterFunction<GFxExperienceData>     (F4SERoot, movieRoot, "ExperienceData");
            RegisterFunction<GFxSkillList>          (F4SERoot, movieRoot, "SkillList");
            RegisterFunction<GFxPerkList>           (F4SERoot, movieRoot, "PerkList");
            RegisterFunction<GFxUpdateInvFilter>    (F4SERoot, movieRoot, "UpdateInvFilter");
            RegisterFunction<GFxGetBookText>        (F4SERoot, movieRoot, "GetBookText");
        }
    }

    return true;
}