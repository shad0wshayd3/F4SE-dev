#pragma once
#include "Forms.h"

#include <sstream>

struct RepairMenuEntry {
    std::string     text;
    UInt32          HandleID;
    UInt32          StackID;
    float           Condition;
};

struct EffectEntry {
	EffectSetting				* Setting;
	UInt32						FormID;
	float						Magnitude;
	float						Duration;
};

struct ActiveEffectEntry {
	std::string					Name;
	std::vector<EffectEntry>	EffectsB;
	std::vector<EffectEntry>	EffectsD;
	UInt32						FormID;
	UInt32						Type;
};

namespace Menus {
    DECLARE_CUSTOM_MENU(BlurMenu);
    DECLARE_CUSTOM_MENU(RepairMenu);
}

void OpenLevelUpMenu(int ID, int Level, int PointsBase, int PointsUsed);

void LevelUpMenuPrompt_Hook();
bool CharGenMenuPrompt();

void ContainerMenuInvoke_Hook(ContainerMenuBase* menu, GFxFunctionHandler::Args* args);
void ExamineMenuInvoke_Hook(ExamineMenu* menu, GFxFunctionHandler::Args* args);
void PipboyMenuInvoke_Hook(PipboyMenu* menu, GFxFunctionHandler::Args* args);
void PopulateItemCard_Custom(GFxValue* InfoObj, BGSInventoryItem* Item, UInt16 StackID, InvItemStackList CompareList);
void PopulateItemCard_Hook(GFxValue* InfoObj, BGSInventoryItem* Item, UInt16 StackID, InvItemStackList CompareList);

DECLARE_EVENT_CLASS(MenuOpenCloseEvent);