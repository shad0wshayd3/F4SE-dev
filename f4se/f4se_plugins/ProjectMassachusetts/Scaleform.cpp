#include "Scaleform.h"

#include "Data.h"
#include "f4se_globals/Globals.h"

#include "f4se/GameReferences.h"
#include "f4se/PapyrusEvents.h"

#include <regex>

namespace GFxHelperFunctions {
    void RegisterString(GFxValue* dst, GFxMovieRoot* root, const char* name, const char* str) {
        GFxValue fxValue;
        root->CreateString(&fxValue, str);
        dst->SetMember(name, &fxValue);
    }

    void RegisterNumber(GFxValue* dst, const char* name, double value) {
        GFxValue fxValue;
        fxValue.SetNumber(value);
        dst->SetMember(name, &fxValue);
    }

    void RegisterInt(GFxValue* dst, const char* name, int value) {
        GFxValue fxValue;
        fxValue.SetInt(value);
        dst->SetMember(name, &fxValue);
    }

    void RegisterBool(GFxValue* dst, const char* name, bool value) {
        GFxValue fxValue;
        fxValue.SetBool(value);
        dst->SetMember(name, &fxValue);
    }
}

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

namespace Regex {
    using namespace std::regex_constants;

    std::regex BSComment   ("(<!--[A-Za-z ]*-->)(\r?\n)*",   ECMAScript | icase);
    std::regex BSPagebreak ("([<\\[](br|pagebreak)+[\\]>])", ECMAScript | icase);
    std::regex BSSimple    ("(</?(i|b|u|ul|li|p|font)+>)",   ECMAScript | icase);

    std::regex BSEmbed ("(<(font|img|p)+ [A-Z0-9=_'\":./$# ]+>)(\r?\n)*",               ECMAScript | icase);
    std::regex BSAlias ("(<(Alias|BaseName|Global|Relationship|Token)+[A-Z0-9=_. ]*>)", ECMAScript | icase);

    std::regex BSSingleLine ("(\r?\n)");
    std::regex BSDoubleLine ("(\r?\n)(\r?\n)+");

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
        bookText = std::regex_replace(bookText, BSComment,   "");
        bookText = std::regex_replace(bookText, BSSimple,    "");
        bookText = std::regex_replace(bookText, BSEmbed,     "");
        bookText = std::regex_replace(bookText, BSSingleLine, "\n");
        bookText = std::regex_replace(bookText, BSDoubleLine, "\n\n");

        return bookText;
    }
}

namespace Scaleform {
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

    class PlayMenuSound : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            if (args->args[0].IsString())
                PlayUISound(args->args[0].GetString());
        }
    };

    class StimpakCount : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            std::string itemCount = std::to_string(GetItemCount((*g_player), g_Data.ObjectTypeStimpak));
            args->movie->movieRoot->CreateString(args->result, itemCount.c_str());
        }
    };

    class UseStimpak : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            for (int i = 0; i < g_Data.StimpakOrder->forms.count; i++) {
                TESForm* thisForm = g_Data.StimpakOrder->forms[i];

                if (GetItemCount((*g_player), thisForm) > 0) {
                    EquipItem((*g_player), thisForm, false, true);
                    break;
                }
            }
        }
    };

    class RadXCount : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            std::string itemCount = std::to_string(GetItemCount((*g_player), g_Data.RadX));
            args->movie->movieRoot->CreateString(args->result, itemCount.c_str());
        }
    };

    class UseRadX : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            EquipItem((*g_player), g_Data.RadX, false, true);
        }
    };

    class DoctorBagCount : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            std::string itemCount = std::to_string(GetItemCount((*g_player), g_Data.DoctorsBag));
            args->movie->movieRoot->CreateString(args->result, itemCount.c_str());
        }
    };

    class UseDoctorBag : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            EquipItem((*g_player), g_Data.DoctorsBag, false, true);
        }
    };

    class SetLimbTarget : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            g_Data.LimbTarget->value = args->args[0].GetUInt();
        }
    };

    class ConsoleLimbSelect : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            g_Data.LimbTarget->value = 7.0;
            EquipItem((*g_player), g_Data.DoctorsBag, false, true);
        }
    };

    class HardcoreData : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            args->movie->movieRoot->CreateObject(args->result, "Object");

            if (g_GameSettings.GetBool("Rule_HardcoreNeeds")) {
                GFxHelperFunctions::RegisterInt(args->result, "H2O", (int)(*g_player)->actorValueOwner.GetValue(g_Data.Dehydration));
                GFxHelperFunctions::RegisterInt(args->result, "FOD", (int)(*g_player)->actorValueOwner.GetValue(g_Data.Starvation));
                GFxHelperFunctions::RegisterInt(args->result, "SLP", (int)(*g_player)->actorValueOwner.GetValue(g_Data.SleepDeprivation));
            }
            else
                GFxHelperFunctions::RegisterInt(args->result, "H2O", -1);
        }
    };

    class RadiationData : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            float cRads = (*g_player)->actorValueOwner.GetValue(g_Data.Rads);
            float cMax = g_GameSettings.GetFloat("RadiationThreshold05");

            args->movie->movieRoot->CreateObject(args->result, "Object");
            GFxHelperFunctions::RegisterInt(args->result, "current", (int)cRads);
            GFxHelperFunctions::RegisterInt(args->result, "next", (int)cMax);
            GFxHelperFunctions::RegisterInt(args->result, "max", (int)cMax);

            for (int i = 1; i < 6; i++) {
                char RadiationThreshold[21]; sprintf_s(RadiationThreshold,
                    sizeof(RadiationThreshold), "RadiationThreshold0%i", i);

                float value = g_GameSettings.GetFloat(RadiationThreshold);
                if (cRads < value) {
                    GFxHelperFunctions::RegisterInt(args->result, "next", value);
                    break;
                }
            }

            GFxHelperFunctions::RegisterInt(args->result, "resist", (int)ceilf((*g_player)->actorValueOwner.GetValue(g_Data.RadResistExposure)));
        }
    };

    class ExperienceData : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            int pLevel = GetLevel(*g_player);
            int xpReq = GetXPForLevel(pLevel);

            float cXP = (*g_player)->actorValueOwner.GetValue(g_Data.Experience) - xpReq;
            float cMax = GetXPForLevel(pLevel + 1) - xpReq;

            args->movie->movieRoot->CreateObject(args->result, "Object");
            GFxHelperFunctions::RegisterInt(args->result, "current", (int)cXP);
            GFxHelperFunctions::RegisterInt(args->result, "max", (int)cMax);
        }
    };

    class SkillList : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            GFxMovieRoot* root = args->movie->movieRoot;
            root->CreateArray(args->result);

            ActorValueOwner* owner = DYNAMIC_CAST((*g_player), PlayerCharacter, ActorValueOwner);

            for (auto ActorValue : Perks::SkillList) {
                GFxValue Skill;
                root->CreateObject(&Skill);

                float value = owner->GetValue(ActorValue);

                GFxHelperFunctions::RegisterString(&Skill, root, "text", ActorValue->fullName.name);
                GFxHelperFunctions::RegisterString(&Skill, root, "EditorID", ActorValue->GetEditorID());

                BSString description;
                TESDescription* avDesc = DYNAMIC_CAST(ActorValue, ActorValueInfo, TESDescription);
                CALL_MEMBER_FN(avDesc, Get)(&description, nullptr);

                GFxHelperFunctions::RegisterString(&Skill, root, "description", description.Get());
                GFxHelperFunctions::RegisterInt(&Skill, "value", (int)floorf(value));

                float modifier = owner->GetMod(0, ActorValue) + (owner->GetMod(0, Calculate::GetDependent(ActorValue)) * 2) + floorf(owner->GetMod(0, g_Data.Luck));
                GFxHelperFunctions::RegisterNumber(&Skill, "modifier", (double)modifier);

                args->result->PushBack(&Skill);
            }
        }
    };

    class PerkList : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            GFxMovieRoot* root = args->movie->movieRoot;
            root->CreateArray(args->result);

            for (auto Perk : Perks::EntirePerkList) {
                if (Perk->hidden || (Perk->numRanks == 0))
                    continue;

                if (HasPerk((*g_player), Perk)) {
                    std::vector<BGSPerk*> Ranks;
                    if (Perk->numRanks > 1) {
                        Ranks = Perks::GetRankList(Perk);
                        if (Ranks[0] != Perk)
                            continue;
                    }
                    else
                        Ranks.emplace_back(Perk);

                    GFxValue GFxPerk;
                    GFxValue GFxDesc;
                    int NumRanks = 0;

                    root->CreateArray(&GFxDesc);
                    root->CreateObject(&GFxPerk);

                    for (auto PerkRank : Ranks) {
                        if (HasPerk((*g_player), PerkRank))
                            NumRanks++;

                        BSString description;
                        TESDescription* perkDesc = DYNAMIC_CAST(Perk, BGSPerk, TESDescription);
                        CALL_MEMBER_FN(perkDesc, Get)(&description, nullptr);

                        GFxValue nDesc(description.Get());
                        GFxDesc.PushBack(&nDesc);
                    }

                    std::string PerkName = Perk->fullName.name.c_str();
                    if (PerkName == "")
                        continue;

                    GFxHelperFunctions::RegisterString(&GFxPerk, root, "text", PerkName.c_str());
                    GFxHelperFunctions::RegisterString(&GFxPerk, root, "SWFFile", Perk->swfPath.c_str());
                    GFxPerk.SetMember("descriptions", &GFxDesc);

                    GFxHelperFunctions::RegisterInt(&GFxPerk, "FormID", Perk->formID);
                    GFxHelperFunctions::RegisterInt(&GFxPerk, "maxRank", Ranks.size());
                    GFxHelperFunctions::RegisterInt(&GFxPerk, "rank", NumRanks);

                    args->result->PushBack(&GFxPerk);
                }
            }
        }
    };

    class AddPerks : public GFxFunctionHandler {
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

    class ModSkills : public GFxFunctionHandler {
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

    class TagSkills : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            int count = args->args[0].GetArraySize();

            GFxValue ArrayElement;
            GFxValue ObjElementFormID;

            if (Perks::PlayerTags.Barter)
                (*g_player)->actorValueOwner.Mod(1, g_Data.Barter, -15);

            if (Perks::PlayerTags.EnergyWeapons)
                (*g_player)->actorValueOwner.Mod(1, g_Data.EnergyWeapons, -15);

            if (Perks::PlayerTags.Explosives)
                (*g_player)->actorValueOwner.Mod(1, g_Data.Explosives, -15);

            if (Perks::PlayerTags.Guns)
                (*g_player)->actorValueOwner.Mod(1, g_Data.Guns, -15);

            if (Perks::PlayerTags.Lockpick)
                (*g_player)->actorValueOwner.Mod(1, g_Data.Lockpick, -15);

            if (Perks::PlayerTags.Medicine)
                (*g_player)->actorValueOwner.Mod(1, g_Data.Medicine, -15);

            if (Perks::PlayerTags.MeleeWeapons)
                (*g_player)->actorValueOwner.Mod(1, g_Data.MeleeWeapons, -15);

            if (Perks::PlayerTags.Repair)
                (*g_player)->actorValueOwner.Mod(1, g_Data.Repair, -15);

            if (Perks::PlayerTags.Science)
                (*g_player)->actorValueOwner.Mod(1, g_Data.Science, -15);

            if (Perks::PlayerTags.Sneak)
                (*g_player)->actorValueOwner.Mod(1, g_Data.Sneak, -15);

            if (Perks::PlayerTags.Speech)
                (*g_player)->actorValueOwner.Mod(1, g_Data.Speech, -15);

            if (Perks::PlayerTags.Survival)
                (*g_player)->actorValueOwner.Mod(1, g_Data.Survival, -15);

            if (Perks::PlayerTags.Unarmed)
                (*g_player)->actorValueOwner.Mod(1, g_Data.Unarmed, -15);

            Perks::PlayerTags = Perks::DefaultTags;

            for (int i = 0; i < count; i++) {
                args->args[0].GetElement(i, &ArrayElement);
                ArrayElement.GetMember("FormID", &ObjElementFormID);

                if (ObjElementFormID.GetInt() == g_Data.Barter->formID)
                    Perks::PlayerTags.Barter = true;

                else if (ObjElementFormID.GetInt() == g_Data.EnergyWeapons->formID)
                    Perks::PlayerTags.EnergyWeapons = true;

                else if (ObjElementFormID.GetInt() == g_Data.Explosives->formID)
                    Perks::PlayerTags.Explosives = true;

                else if (ObjElementFormID.GetInt() == g_Data.Guns->formID)
                    Perks::PlayerTags.Guns = true;

                else if (ObjElementFormID.GetInt() == g_Data.Lockpick->formID)
                    Perks::PlayerTags.Lockpick = true;

                else if (ObjElementFormID.GetInt() == g_Data.Medicine->formID)
                    Perks::PlayerTags.Medicine = true;

                else if (ObjElementFormID.GetInt() == g_Data.MeleeWeapons->formID)
                    Perks::PlayerTags.MeleeWeapons = true;

                else if (ObjElementFormID.GetInt() == g_Data.Repair->formID)
                    Perks::PlayerTags.Repair = true;

                else if (ObjElementFormID.GetInt() == g_Data.Science->formID)
                    Perks::PlayerTags.Science = true;

                else if (ObjElementFormID.GetInt() == g_Data.Sneak->formID)
                    Perks::PlayerTags.Sneak = true;

                else if (ObjElementFormID.GetInt() == g_Data.Speech->formID)
                    Perks::PlayerTags.Speech = true;

                else if (ObjElementFormID.GetInt() == g_Data.Survival->formID)
                    Perks::PlayerTags.Survival = true;

                else if (ObjElementFormID.GetInt() == g_Data.Unarmed->formID)
                    Perks::PlayerTags.Unarmed = true;

                ActorValueInfo* ActorValue = DYNAMIC_CAST(LookupFormByID(ObjElementFormID.GetInt()), TESForm, ActorValueInfo);
                (*g_player)->actorValueOwner.Mod(1, ActorValue, 15);
            }
        }
    };

    class BlurBackground : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            ApplyImagespaceModifier(g_Data.PipboyMenuImod, 1.0, NULL);
        }
    };

    class UnblurBackground : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            RemoveImagespaceModifier(g_Data.PipboyMenuImod);
        }
    };

    class CloseLevelUpMenu : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            BSFixedString LevelUpMenu("LevelUpMenu");
            CALL_MEMBER_FN(*g_uiMessageManager, SendUIMessage)(LevelUpMenu, kMessage_Close);
            LevelUpMenuInput::RegisterForInput(false);
            RemoveImagespaceModifier(g_Data.PipboyMenuImod);
        }
    };

    class ForceUnlock : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            BSFixedString LockpickingMenu("LockpickingMenu");
            CALL_MEMBER_FN(*g_uiMessageManager, SendUIMessage)(LockpickingMenu, kMessage_Close);
            VMArray<VMVariable> pArgs; CallFunctionNoWait<TESQuest>(g_Data.PerksQuest, "OnForceLock", pArgs);
        }
    };

    class GetLockpickLevel : public GFxFunctionHandler {
    public:
        virtual void Invoke(Args* args) {
            std::string level = std::to_string((*g_player)->actorValueOwner.GetValue(g_Data.Lockpick));
            args->movie->movieRoot->CreateString(args->result, level.c_str());
        }
    };

    class UpdateInvFilter : public GFxFunctionHandler {
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
            std::string bText = Regex::GetBookText(LookupFormByID(FormID.GetUInt()));
            args->args[0].SetMember("description", &GFxValue(bText.c_str()));
        }
    };
}

bool Scaleform::RegisterFunctions(GFxMovieView* view, GFxValue* F4SERoot) {
    GFxMovieRoot* movieRoot = view->movieRoot;

    GFxValue currentSWFPath;
    std::string currentSWFPathString = "";

    if (movieRoot->GetVariable(&currentSWFPath, "root.loaderInfo.url")) {
        currentSWFPathString = currentSWFPath.GetString();

        if (currentSWFPathString.find("HUDMenu.swf") != std::string::npos)
            movieRoot->SetVariable("root.LeftMeters_mc.HPMeter_mc.RadsBar_mc.visible", &GFxValue(false));

        else if (currentSWFPathString.find("LevelUpMenu.swf") != std::string::npos) {
            RegisterFunction<PlayMenuSound>     (F4SERoot, movieRoot, "PlaySound");
            RegisterFunction<BlurBackground>    (F4SERoot, movieRoot, "BlurBackground");
            RegisterFunction<UnblurBackground>  (F4SERoot, movieRoot, "UnblurBackground");
            RegisterFunction<CloseLevelUpMenu>  (F4SERoot, movieRoot, "CloseLevelUpMenu");
            RegisterFunction<AddPerks>          (F4SERoot, movieRoot, "AddPerks");
            RegisterFunction<ModSkills>         (F4SERoot, movieRoot, "ModSkills");
            RegisterFunction<TagSkills>         (F4SERoot, movieRoot, "TagSkills");

            Perks::ProcessPerkList(movieRoot, Perks::CurrentList);
            LevelUpMenuInput::RegisterForInput(true);
        }

        else if (currentSWFPathString.find("LockpickingMenu.swf") != std::string::npos) {
            RegisterFunction<ForceUnlock>       (F4SERoot, movieRoot, "ForceUnlock");
            RegisterFunction<GetLockpickLevel>  (F4SERoot, movieRoot, "GetLockpickLevel");
            LockpickingMenuInput::RegisterForInput(true);
        }

        else if (currentSWFPathString.find("Pipboy") != std::string::npos) {
            RegisterFunction<StimpakCount>      (F4SERoot, movieRoot, "StimpakCount");
            RegisterFunction<UseStimpak>        (F4SERoot, movieRoot, "UseStimpak");
            RegisterFunction<RadXCount>         (F4SERoot, movieRoot, "RadXCount");
            RegisterFunction<UseRadX>           (F4SERoot, movieRoot, "UseRadX");
            RegisterFunction<DoctorBagCount>    (F4SERoot, movieRoot, "DoctorBagCount");
            RegisterFunction<UseDoctorBag>      (F4SERoot, movieRoot, "UseDoctorBag");
            RegisterFunction<SetLimbTarget>     (F4SERoot, movieRoot, "SetLimbTarget");
            RegisterFunction<ConsoleLimbSelect> (F4SERoot, movieRoot, "ConsoleLimbSelect");
            RegisterFunction<HardcoreData>      (F4SERoot, movieRoot, "HardcoreData");
            RegisterFunction<RadiationData>     (F4SERoot, movieRoot, "RadiationData");
            RegisterFunction<ExperienceData>    (F4SERoot, movieRoot, "ExperienceData");
            RegisterFunction<SkillList>         (F4SERoot, movieRoot, "SkillList");
            RegisterFunction<PerkList>          (F4SERoot, movieRoot, "PerkList");
            RegisterFunction<UpdateInvFilter>   (F4SERoot, movieRoot, "UpdateInvFilter");
            RegisterFunction<GFxGetBookText>    (F4SERoot, movieRoot, "GetBookText");
        }
    }

    return true;
}
