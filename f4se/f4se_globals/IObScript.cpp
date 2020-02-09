#include "IObScript.h"

bool Cmd_Default_Execute(EXECUTE_ARGS) {
    return true;
}

bool Cmd_Default_Eval(EVAL_ARGS) {
    return true;
}

bool Cmd_Default_Parse(PARSE_ARGS) {
    return ObScript_Parse(PASS_PARSE_ARGS);
}

// ------------------------------------------------------------------------------------------------
// IObScript
// ------------------------------------------------------------------------------------------------

bool IObScript::Init() {
    std::vector<int> m_opcodes = {kObScript_NumConsoleCommands + kObScript_ConsoleOpBase, kObScript_NumObScriptCommands + kObScript_ScriptOpBase};
    ObScriptCommandList m_init = {g_firstConsoleCommand, g_firstObScriptCommand};

    for (int i = 0; i < m_init.size(); ++i) {
        for (ObScriptCommand* iter = m_init[i]; iter->opcode < m_opcodes[i]; ++iter) {
            m_commands.emplace_back(iter);
        }
    }

    return true;
}

ObScriptCommand* IObScript::GetCommand(const char* name) {
    for (auto iter: m_commands)
        if (!_stricmp(iter->longName, name) || !_stricmp(iter->shortName, name))
            return iter;

    return nullptr;
}

ObScriptCommand* IObScript::GetCommand(UInt32 opcode) {
    for (auto iter: m_commands)
        if (iter->opcode == opcode)
            return iter;

    return nullptr;
}

void IObScript::IterCommands(void (*iterFunction)(ObScriptCommand* command)) {
    for (auto iter: m_commands)
        iterFunction(iter);
}

bool IObScript::OverloadCommand(ObScriptCommand* command, ObScriptCommand overload) {
    ObScriptCommand commandUpdate = *command;
    commandUpdate.longName = overload.longName;
    commandUpdate.shortName = overload.shortName;
    commandUpdate.helpText = overload.helpText;
    commandUpdate.needsParent = overload.needsParent;
    commandUpdate.numParams = overload.numParams;
    commandUpdate.params = overload.params;
    commandUpdate.execute = (overload.execute) ? overload.execute : command->execute;
    //commandUpdate.parse             = (overload.parse)      ? overload.parse    : command->parse;
    commandUpdate.eval = (overload.eval) ? overload.eval : command->eval;
    commandUpdate.flags = overload.flags;

    SafeWriteBuf((uintptr_t)command, &commandUpdate, sizeof(commandUpdate));
    return true;
}

bool IObScript::OverloadCommand(const char* name, ObScriptCommand overload) {
    ObScriptCommand* command = GetCommand(name);
    if (!command)
        return false;

    return OverloadCommand(command, overload);
}

bool IObScript::OverloadCommand(UInt32 opcode, ObScriptCommand overload) {
    ObScriptCommand* command = GetCommand(opcode);
    if (!command)
        return false;

    return OverloadCommand(command, overload);
}

// ------------------------------------------------------------------------------------------------
// Initial Statics
// ------------------------------------------------------------------------------------------------

ObScriptCommandList IObScript::m_commands;