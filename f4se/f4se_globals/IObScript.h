#pragma once

#include "IObScriptParams.h"
#include "Native.h"

#include "f4se/GameReferences.h"
#include "f4se_common/SafeWrite.h"

#include <vector>

typedef std::vector<ObScriptCommand*> ObScriptCommandList;

#define PASS_EXTRACT_ARGS   paramInfo, scriptData, opcodeOffsetPtr, thisObj, containingObj, scriptObj, locals
#define EXECUTE_ARGS        void* paramInfo, void* scriptData, TESObjectREFR* thisObj, void* containingObj, void* scriptObj, void* locals, double* result, void* opcodeOffsetPtr
#define PASS_EXECUTE_ARGS   paramInfo, scriptData, thisObj, containingObj, scriptObj, locals, result, opcodeOffsetPtr
#define EVAL_ARGS           TESObjectREFR* thisObj, void* arg1, void* arg2, double* result
#define PASS_EVAL_ARGS      thisObj, arg1, arg2, result
#define PARSE_ARGS          UInt32 numParams, void* paramInfo, void* lineBuf, void* scriptBuf
#define PASS_PARSE_ARGS     numParams, paramInfo, lineBuf, scriptBuf

typedef bool (*Cmd_Execute)(EXECUTE_ARGS);
bool Cmd_Default_Execute(EXECUTE_ARGS);

typedef bool (*Cmd_Eval)(EVAL_ARGS);
bool Cmd_Default_Eval(EVAL_ARGS);

#define DEFINE_CMD_FULL(longName, shortName, description, refRequired, paramInfo)\
    extern bool Cmd_ ## longName ## _Execute(EXECUTE_ARGS);\
    static ObScriptCommand (kObScriptCommand_ ## longName) = {\
        #longName,\
        #shortName,\
        0,\
        0,\
        #description,\
        refRequired,\
        0,\
        (sizeof(paramInfo) / sizeof(ObScriptParam)),\
        0,\
        paramInfo,\
        Cmd_ ## longName ## _Execute,\
        NULL,\
        NULL,\
        0,\
        0\
    }

#define DEFINE_CMD_EVAL_FULL(longName, shortName, description, refRequired, paramInfo)\
    extern bool Cmd_ ## longName ## _Execute(EXECUTE_ARGS);\
    extern bool Cmd_ ## longName ## _Eval(EVAL_ARGS);\
    static ObScriptCommand (kObScriptCommand_ ## longName) = {\
        #longName,\
        #shortName,\
        0,\
        0,\
        #description,\
        refRequired,\
        0,\
        (sizeof(paramInfo) / sizeof(ObScriptParam)),\
        0,\
        paramInfo,\
        Cmd_ ## longName ## _Execute,\
        NULL,\
        Cmd_ ## longName ## _Eval,\
        1,\
        0\
    }

class IObScript {
public:
    static bool                 Init();

    static ObScriptCommand*     GetCommand(const char* name);
    static ObScriptCommand*     GetCommand(UInt32 opcode);

    static void                 IterCommands(void(*iterFunction)(ObScriptCommand* command));

    static bool                 OverloadCommand(ObScriptCommand* command, ObScriptCommand overload);
    static bool                 OverloadCommand(const char* name, ObScriptCommand overload);
    static bool                 OverloadCommand(UInt32 opcode, ObScriptCommand overload);

private:
    static ObScriptCommandList  m_commands;
};