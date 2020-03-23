#include "ObScript.h"

#include "Forms.h"

#include "f4se/GameData.h"
#include "f4se/GameRTTI.h"

#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace ObScript {
    TCHAR runtimeDir[MAX_PATH], filePath[MAX_PATH];
    char* s_argString;

    bool InitializeBetaComment() {
        // Get Runtime Directory
        sprintf_s(runtimeDir, sizeof(runtimeDir), "%s", GetRuntimeDirectory().c_str());

        // Get Output File
        std::string commentFilePath = ISettings::GetString("BetaComment:sFileName", "BetaComments.txt");
        sprintf_s(filePath, sizeof(filePath), "%s%s", runtimeDir, commentFilePath.c_str());

        _LogMessage("Using output path: %s", filePath);
        s_argString = (char*)malloc(0x4000);
        return true;
    }

    bool Cmd_BetaComment_Execute(EXECUTE_ARGS) {
        if (!ExtractArgs(PASS_EXTRACT_ARGS, s_argString))
            return true;

        if (!thisObj) {
            Console_Print("No Reference selected, using Player.");
            thisObj = (*g_player);
        }

        NiPoint3 rotation = thisObj->rot;
        NiPoint3 position = thisObj->pos;
        if (thisObj->formID == 0x14)
            position.z += 140;

        std::ofstream commentFile(filePath, std::ios::binary | std::ios::app);
        if (!commentFile.is_open()) {
            Console_Print("Couldn't open BetaComment file: %s", filePath);
            return true;
        }

        char printBuf;

        // Add System Time
        _SYSTEMTIME systemTime; GetLocalTime(&systemTime);
        sprintf_s(&printBuf, 0x1000, "%u/%u/%u (%02u:%02u)\t",
                  systemTime.wMonth, systemTime.wDay, systemTime.wYear, systemTime.wHour, systemTime.wMinute);
        commentFile.write(&printBuf, strlen(&printBuf));

        // Add File Info
        ModInfo* file = GetFile(thisObj);
        if (file) {
            _SYSTEMTIME fileModifiedTime; GetFileModifiedTime(file, fileModifiedTime);
            sprintf_s(&printBuf, 0x1000, "%s\t%u/%u/%u (%02u:%02u)\t", file->name,
                      fileModifiedTime.wMonth, fileModifiedTime.wDay, fileModifiedTime.wYear, fileModifiedTime.wHour, fileModifiedTime.wMinute);
        } else {
            sprintf_s(&printBuf, 0x1000, "%s\t%u/%u/%u (%02u:%02u)\t", "UNKNOWN", 1, 1, 1970, 0, 0);
        }
        commentFile.write(&printBuf, strlen(&printBuf));

        // Add Machine Name
        char machineName; DWORD bufCharCount = 0x1000;
        GetComputerName(&machineName, &bufCharCount);
        sprintf_s(&printBuf, 0x1000, "%s\t", &machineName);
        commentFile.write(&printBuf, strlen(&printBuf));

        // Add FormID
        sprintf_s(&printBuf, 0x1000, "%08X\t", thisObj->formID);
        commentFile.write(&printBuf, strlen(&printBuf));

        // Add Name
        const char* formName = thisObj->GetEditorID();
        if (strlen(formName) == 0) {
            formName = thisObj->baseForm->GetEditorID();
            if (strlen(formName) == 0) {
                formName = "UNKNOWN";
            }
        }
        sprintf_s(&printBuf, 0x1000, "%s\t", formName);
        commentFile.write(&printBuf, strlen(&printBuf));

        // Add Cell Info
        char cellName; UInt32 cellFormID = 0;
        if (thisObj->parentCell) {
            cellFormID = thisObj->parentCell->formID;
            if (thisObj->parentCell->flags & TESObjectCELL::kFlag_IsInterior) {
                if (strlen(thisObj->parentCell->GetEditorID()) != 0)
                    sprintf_s(&cellName, 0x1000, "%s", thisObj->parentCell->GetEditorID());
                else sprintf_s(&cellName, 0x1000, "%s", "UNKNOWN");
            }
            else if (thisObj->parentCell->worldSpace) {
                // F4SE has a nasty habit of overwriting "flags" with a new def, so this is compensation
                TESForm* parentCell = DYNAMIC_CAST(thisObj->parentCell, TESObjectCELL, TESForm);

                int cellX = 0, cellY = 0;
                if ((parentCell->flags >> 13) & 1) {
                    _LogMessageNT("BetaComment: I have no idea what flag this is, but 08X has it.", parentCell->formID);
                }
                else {
                    if (thisObj->parentCell->unk50) {
                        cellX = thisObj->parentCell->unk50->unk00;
                        cellY = thisObj->parentCell->unk50->unk04;
                    }
                }

                if (strlen(thisObj->parentCell->worldSpace->GetEditorID()) != 0)
                    sprintf_s(&cellName, 0x1000, "%s (%d,%d)",
                              thisObj->parentCell->worldSpace->GetEditorID(), cellX, cellY);
                else sprintf_s(&cellName, 0x1000, "%s (%d,%d)", "UNKNOWN", cellX, cellY);
            }
        }

        sprintf_s(&printBuf, 0x1000, "%s\t%08X\t", &cellName, cellFormID);
        commentFile.write(&printBuf, strlen(&printBuf));

        // Add X Pos
        sprintf_s(&printBuf, 0x1000, "%.0f\t", thisObj->pos.x);
        commentFile.write(&printBuf, strlen(&printBuf));

        // Add *Something*
        commentFile.write("\t", strlen("\t"));

        // Add Something from TESForm
        sprintf_s(&printBuf, 0x1000, "%.0f\t", (double)thisObj->unk18);
        commentFile.write(&printBuf, strlen(&printBuf));

        // Add X Rot
        sprintf_s(&printBuf, 0x1000, "%.0f\t", thisObj->rot.x);
        commentFile.write(&printBuf, strlen(&printBuf));

        // Add Number of Overwriting Files
        sprintf_s(&printBuf, 0x1000, "%.0f\t", (double)((thisObj->unk08) ? thisObj->unk08->size : 0));
        commentFile.write(&printBuf, strlen(&printBuf));

        // Add Flags from TESForm
        sprintf_s(&printBuf, 0x1000, "%.0f\t", (double)thisObj->flags);
        commentFile.write(&printBuf, strlen(&printBuf));

        // Add Position X, Y, Z
        sprintf_s(&printBuf, 0x1000, "%.0f\t%.0f\t%.0f\t", position.x, position.y, position.z);
        commentFile.write(&printBuf, strlen(&printBuf));

        // Add Camera Rot X, Y, Z
        sprintf_s(&printBuf, 0x1000, "%.0f\t%.0f\t%.0f\t", rotation.x, rotation.y, rotation.z);
        commentFile.write(&printBuf, strlen(&printBuf));

        // Add Comment, Newline
        sprintf_s(&printBuf, 0x1000, "\"%s\"\t\r\n", s_argString);
        commentFile.write(&printBuf, strlen(&printBuf));
        commentFile.close();

        Console_Print("Logged BetaComment.");
        return true;
    }

    bool Cmd_GetPermanentValue_Execute(EXECUTE_ARGS) {
        *result = 0.0;
        ActorValueInfo* avif = nullptr;

        if (!ExtractArgs(PASS_EXTRACT_ARGS, &avif)) {
            return true;
        }

        if (!thisObj || !avif) {
            return true;
        }

        *(float*)result = GetPermValue(thisObj, avif);
        Console_Print("GetPermanentValue: %s >> %0.2f", avif->avName, *(float*)result);
        return true;
    }

    bool Cmd_GetPermanentValue_Eval(EVAL_ARGS) {
        *result = 0.0;
        ActorValueInfo* avif = (ActorValueInfo*)arg1;

        if (!thisObj || !avif) {
            return true;
        }

        if (!thisObj->formID) {
            thisObj = (*g_player);
        }

        *(float*)result = GetPermValue(thisObj, avif);
        return true;
    }

    bool Hook_Commit() {
        if (!IObScript::OverloadCommand("BetaComment", kObScriptCommand_BetaComment)) {
            return false;
        }

        kObScriptCommand_GetPermanentValue.helpText = "Get the value of a property ignoring temporary modifiers [player.getpermanentvalue strength]";
        if (!IObScript::OverloadCommand("GetPermanentValue", kObScriptCommand_GetPermanentValue)) {
            return false;
        }

        return true;
    }
}