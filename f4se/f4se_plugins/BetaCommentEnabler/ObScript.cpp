#include "ObScript.h"

#include "f4se/GameData.h"
#include "f4se/GameRTTI.h"

#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace ObScript {
    TCHAR runtimeDir[MAX_PATH], filePath[MAX_PATH], computerName[INFO_SIZE];
    char* s_argString;

    bool InitializeBetaComment() {
        // Get Runtime Directory
        sprintf_s(runtimeDir, sizeof(runtimeDir), "%s", GetRuntimeDirectory().c_str());

        // Get Computer Name
        DWORD bufCharCount = INFO_SIZE;
        if (!GetComputerName(computerName, &bufCharCount)) {
            _LOGERROR("Couldn't get computer name.");
            return false;
        }

        // Get Output File
        std::string commentFilePath = g_Settings.GetString("sFileName:General", "BetaComments.txt");
        sprintf_s(filePath, sizeof(filePath), "%s%s", runtimeDir, commentFilePath.c_str());

        _LOGMESSAGE("Using output path: %s", filePath);
        s_argString = (char*)malloc(0x4000);
        return true;
    }

    bool Cmd_BetaComment_Execute(EXECUTE_ARGS) {
        if (!ExtractArgs(EXTRACT_ARGS, s_argString))
            return true;

        // Check Plugin Name
        if (!thisObj->unk08) {
            Console_Print("Selected reference is not from a file!");
            return true;
        }

        // Get Plugin Name
        const char* fileName = thisObj->unk08->entries[thisObj->unk08->size - 1]->name;

        // Check Worldspace Name
        std::string cellName;
        if (!thisObj->parentCell) {
            Console_Print("Selected reference has no parent cell!");
            return true;
        }

        // Get Worldspace Form
        TESForm* thisWorld = nullptr;
        if (thisObj->parentCell->worldSpace)
            thisWorld = DYNAMIC_CAST(thisObj->parentCell->worldSpace, TESWorldSpace, TESForm);

        // Get Worldspace or Interior Cell name
        if (!thisWorld) {
            if (thisObj->parentCell->flags & thisObj->parentCell->kFlag_IsInterior) {
                cellName = thisObj->parentCell->GetEditorID();
            }
            else {
                Console_Print("Selected reference is in a worldspace that doesn't exist!");
                return true;
            }
        }
        else {
            cellName = thisWorld->GetEditorID();
        }

        if (cellName.empty())
            cellName = "UNKNOWN";

        // Get System Time
        std::stringstream systemTime;
        std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        systemTime << std::put_time(std::localtime(&now), TIMESTAMP);

        // Get Plugin File path
        char pluginPath[MAX_PATH];
        sprintf_s(pluginPath, sizeof(pluginPath), "%sData\\%s", runtimeDir, fileName);

        // Get Plugin File last modified time
        struct stat resultTime;
        std::stringstream fileTime;
        if (stat(pluginPath, &resultTime) == 0)
            fileTime << std::put_time(std::localtime(&resultTime.st_mtime), TIMESTAMP);

        // Get EditorID
        std::string EDID = thisObj->GetEditorID();
        if (EDID.empty()) {
            EDID = thisObj->baseForm->GetEditorID();
            if (EDID.empty())
                EDID = "UNKNOWN";
        }

        // Output
        char comment[MAX_PATH];
        sprintf_s(comment, sizeof(comment), "%s\t%s\t%s\t%s\t%08X\t%s\t%s\t%08X\t%i\t%i\t%i\t0\t-0\t0\t%s\n", systemTime.str().c_str(), fileName, fileTime.str().c_str(),
            computerName, thisObj->formID, EDID.c_str(), cellName.c_str(), thisObj->parentCell->formID, (int)thisObj->pos.x, (int)thisObj->pos.y, (int)thisObj->pos.z, s_argString);

        std::ofstream commentFile;
        commentFile.open(filePath, std::ios::binary | std::ios::app);
        commentFile << comment;
        commentFile.close();

        return true;
    }

    bool Hook_Commit() {
        if (!g_ObScript.OverloadCommand("BetaComment", kCommand_BetaComment)) {
            return false;
        }

        return true;
    }
}
