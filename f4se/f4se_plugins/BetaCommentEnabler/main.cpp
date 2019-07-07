#include "config.h"
#include "f4se_globals/Globals.h"

#include "f4se/GameData.h"
#include "f4se/GameReferences.h"
#include "f4se/GameRTTI.h"
#include "f4se/ObScript.h"
#include "f4se/PluginAPI.h"

#include "f4se_common/SafeWrite.h"

#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <time.h>

#define TIMESTAMP "%m/%d/%Y (%I:%M)"
#define INFO_SIZE 0x7FFF

ILog            g_Log           = ILog(PLUGIN_NAME_SHORT);
PluginHandle    g_PluginHandle  = kPluginHandle_Invalid;

namespace Settings {
    std::string Get_Internal(std::string settingNameAndSection) {
        auto delimiter = settingNameAndSection.find_first_of(':');
        std::string settingNameStr = settingNameAndSection.substr(0, delimiter);
        std::string settingSectionStr = settingNameAndSection.substr(delimiter + 1);

        char value[1024];
        GetPrivateProfileString(settingSectionStr.c_str(), settingNameStr.c_str(), NULL, value, sizeof(value), INI_LOCATION);

        return std::string(value);
    }

    std::string GetString(std::string settingName, std::string def) {
        auto value = Get_Internal(settingName);
        return (value != "") ? value : def;
    }
}

namespace ObScript {
    static ObScriptCommand* obBetaComment = nullptr;
    char* s_argString;

    bool BetaComment_Execute(void* paramInfo, void* scriptData, TESObjectREFR* thisObj, void* containingObj, void* scriptObj, void* locals, double* result, void* opcodeOffsetPtr) {
        if (!ExtractArgs(paramInfo, scriptData, opcodeOffsetPtr, thisObj, containingObj, scriptObj, locals, s_argString)) { return true; }

        if (!thisObj) {
            Console_Print("No reference has been selected!");
            return true;
        }

        // Get Current System Time
        std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::stringstream CST; CST << std::put_time(std::localtime(&now), TIMESTAMP);

        // Get Plugin Name
        if (!thisObj->unk08) {
            Console_Print("Selected reference is not from a file!");
            return true;
        }

        const char* fileName = thisObj->unk08->entries[thisObj->unk08->size - 1]->name;
        const char* runtimeDir = GetRuntimeDirectory().c_str();

        // Get Plugin Last Modified Time
        char pluginPath[MAX_PATH];
        sprintf_s(pluginPath, sizeof(pluginPath), "%sData\\%s", runtimeDir, fileName);

        struct stat resultTime; std::stringstream LFT;
        if (stat(pluginPath, &resultTime) == 0) {
            LFT << std::put_time(std::localtime(&resultTime.st_mtime), TIMESTAMP);
        }
        else {
            Console_Print("Couldn't get file time? I hope you're not reading this.");
            return true;
        }

        // Get Computer Name
        TCHAR computerName[INFO_SIZE]; DWORD bufCharCount = INFO_SIZE;
        if (!GetComputerName(computerName, &bufCharCount)) {
            Console_Print("Couldn't get computer name? I hope you're not reading this.");
            return true;
        }

        // Get EditorID
        std::string EDID = thisObj->GetEditorID();
        if (EDID.empty()) {
            EDID = thisObj->baseForm->GetEditorID();
            if (EDID.empty()) {
                EDID = "UNKNOWN";
            }
        }

        // Get Worldspace Name
        std::string cellName;
        if (!thisObj->parentCell) {
            Console_Print("Selected reference has no parent cell!");
            return true;
        }

        TESForm* thisWorld = nullptr;
        if (thisObj->parentCell->worldSpace) {
            thisWorld = DYNAMIC_CAST(thisObj->parentCell->worldSpace, TESWorldSpace, TESForm);
        }

        if (!thisWorld) {
            if (thisObj->parentCell->flags & thisObj->parentCell->kFlag_IsInterior) {
                cellName = thisObj->parentCell->GetEditorID();
            }
            else {
                Console_Print("Selected reference is in a worldspace that doesn't exist!");
                return true;
            }
        }
        else { cellName = thisWorld->GetEditorID(); }
        if (cellName.empty()) { cellName = "UNKNOWN"; }

        // Output
        const char* commentFilePath = Settings::GetString("sFileName:General", "BetaComments.txt").c_str();

        char filePath[MAX_PATH];
        sprintf_s(filePath, sizeof(filePath), "%s%s", runtimeDir, commentFilePath);

        char comment[MAX_PATH];
        sprintf_s(comment, sizeof(comment), "%s\t%s\t%s\t%s\t%08X\t%s\t%s\t%08X\t%i\t%i\t%i\t0\t-0\t0\t%s\n", CST.str().c_str(), fileName, LFT.str().c_str(), computerName,
            thisObj->formID, EDID.c_str(), cellName.c_str(), thisObj->parentCell->formID, (int)thisObj->pos.x, (int)thisObj->pos.y, (int)thisObj->pos.z, s_argString);

        std::ofstream commentFile;
        commentFile.open(filePath, std::ios::binary | std::ios::app);
        commentFile << comment;
        commentFile.close();

        return true;
    }

    bool Hook_Init() {
        for (ObScriptCommand* iter = g_firstConsoleCommand; iter->opcode < (kObScript_NumConsoleCommands + kObScript_ConsoleOpBase); ++iter) {
            if (!strcmp(iter->longName, "BetaComment")) {
                obBetaComment = iter;
                break;
            }
        }

        if (!obBetaComment) { return false; }
        return true;
    }

    bool Hook_Commit() {
        ObScriptCommand BC = *obBetaComment; {
            BC.execute = BetaComment_Execute;
        }

        SafeWriteBuf((uintptr_t)obBetaComment, &BC, sizeof(BC));
        return true;
    }
}

extern "C" {
    bool F4SEPlugin_Query(const F4SEInterface* F4SE, PluginInfo* Info) {
        ITimeKeeper startupClock = ITimeKeeper();
        startupClock.Start();

		g_Log.LogMessage("%s log opened (PC-64)", PLUGIN_NAME_LONG);
		g_Log.LogMessage("This is a plugin log only and does not contain information on any other part of the game, including crashes.");

        Info->infoVersion   = PluginInfo::kInfoVersion;
        Info->name          = PLUGIN_NAME_LONG;
        Info->version       = PLUGIN_VERSION;

        g_PluginHandle      = F4SE->GetPluginHandle();

        if (F4SE->runtimeVersion != SUPPORTED_RUNTIME_VERSION) {
			g_Log.LogError("Unsupported runtime version v%d.%d.%d.%d. This DLL is built for v%d.%d.%d.%d only. Plugin will be disabled.",
                GET_EXE_VERSION_MAJOR (F4SE->runtimeVersion),
                GET_EXE_VERSION_MINOR (F4SE->runtimeVersion),
                GET_EXE_VERSION_BUILD (F4SE->runtimeVersion),
                GET_EXE_VERSION_SUB   (F4SE->runtimeVersion),
                GET_EXE_VERSION_MAJOR (SUPPORTED_RUNTIME_VERSION),
                GET_EXE_VERSION_MINOR (SUPPORTED_RUNTIME_VERSION),
                GET_EXE_VERSION_BUILD (SUPPORTED_RUNTIME_VERSION),
                GET_EXE_VERSION_SUB   (SUPPORTED_RUNTIME_VERSION));

            return false;
        }

        if (!ObScript::Hook_Init()) {
			g_Log.LogError("Failed to hook ObScript functions. Plugin will be disabled.");
            return false;
        }

		g_Log.LogMessage("F4SEPlugin_Query Time: %fms", startupClock.Format(ITimeKeeper::Milli));
        return true;
    }

    bool F4SEPlugin_Load(const F4SEInterface* F4SE) {
        if (!ObScript::Hook_Commit()) {
			g_Log.LogWarning("Failed to commit ObScript functions.");
            return false;
        }

        ObScript::s_argString = (char*)malloc(0x4000);
        return true;
    }
}