#include "IObScript.h"

#include "Native.h"

#include "f4se_common/SafeWrite.h"

bool ObScript_Default_Parse(UInt32 numParams, void* paramInfo, void* lineBuf, void* scriptBuf) {
    return ObScript_Parse(numParams, paramInfo, lineBuf, scriptBuf);
}

bool IObScript::Init() {
    std::vector<int>    m_opcodes   = { kObScript_NumConsoleCommands + kObScript_ConsoleOpBase, kObScript_NumObScriptCommands + kObScript_ScriptOpBase };
    ObScriptCommandList m_init      = { g_firstConsoleCommand, g_firstObScriptCommand };

    for (int i = 0; i < m_init.size(); ++i) {
        for (ObScriptCommand* iter = m_init[i]; iter->opcode < m_opcodes[i]; ++iter) {
            m_commands.emplace_back(iter);
        }
    }

    return true;
}

ObScriptCommand* IObScript::GetCommand(const char* name) {
    for (auto iter : m_commands) {
        if (!strcmp(iter->longName, name) || !strcmp(iter->shortName, name))
            return iter;
    }

    return nullptr;
}

ObScriptCommandList IObScript::GetMatchingCommands(bool(*filterFunction)(ObScriptCommand* command)) {
    ObScriptCommandList Result;
    for (auto iter : m_commands) {
        if (filterFunction(iter))
            Result.emplace_back(iter);
    }

    return Result;
}

bool IObScript::OverloadCommand(ObScriptCommand* command, ObScriptCommand overload) {
    ObScriptCommand commandUpdate   = *command;
    commandUpdate.longName          = overload.longName;
    commandUpdate.shortName         = overload.shortName;
    commandUpdate.helpText          = overload.helpText;
    commandUpdate.needsParent       = overload.needsParent;
    commandUpdate.numParams         = overload.numParams;
    commandUpdate.params            = overload.params;
    commandUpdate.flags             = overload.flags;

    if (overload.execute)
        commandUpdate.execute = overload.execute;

    if (overload.eval)
        commandUpdate.eval = overload.eval;

    SafeWriteBuf((uintptr_t)command, &commandUpdate, sizeof(commandUpdate));
    return true;
}

bool IObScript::OverloadCommand(const char* name, ObScriptCommand overload) {
    ObScriptCommand* command = GetCommand(name);
    if (!command)
        return false;

    return OverloadCommand(command, overload);
}
