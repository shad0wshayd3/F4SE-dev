#pragma once

#include <fstream>
#include <string>

class GLog {
public:
    ~GLog();

    void Open(std::string logName);
    void Message(const char* prefix, const char* message, va_list args, bool timestamp, int level = 0);

    void Indent() { m_indentLevel++; }
    void Outdent() {
        if (m_indentLevel)
            m_indentLevel--;
    }

    void SetLogLevel(int logLevel) { m_logLevel = logLevel; }
    int GetLogLevel() { return m_logLevel; }

private:
    void Write(const char* message);

    std::ofstream m_logFile;
    int m_indentLevel;
    int m_logLevel = 0;
};

extern GLog g_Log;

inline void _LogIndent() {
    g_Log.Indent();
}

inline void _LogOutdent() {
    g_Log.Outdent();
}

inline void _LogLevel(int logLevel) {
    g_Log.SetLogLevel(logLevel);
}

inline void _LogMessage(const char* message, ...) {
    va_list args;
    va_start(args, message);
    g_Log.Message("", message, args, true);
    va_end(args);
}

inline void _LogWarning(const char* message, ...) {
    va_list args;
    va_start(args, message);
    g_Log.Message("Warning: ", message, args, true);
    va_end(args);
}

inline void _LogError(const char* message, ...) {
    va_list args;
    va_start(args, message);
    g_Log.Message("Error: ", message, args, true);
    va_end(args);
}

inline void _LogMessageNT(const char* message, ...) {
    va_list args;
    va_start(args, message);
    g_Log.Message("", message, args, false);
    va_end(args);
}

inline void _LogWarningNT(const char* message, ...) {
    va_list args;
    va_start(args, message);
    g_Log.Message("Warning: ", message, args, false);
    va_end(args);
}

inline void _LogErrorNT(const char* message, ...) {
    va_list args;
    va_start(args, message);
    g_Log.Message("Error: ", message, args, false);
    va_end(args);
}

inline void _DebugIndent() {
    if (g_Log.GetLogLevel() >= 1)
        g_Log.Indent();
}

inline void _DebugOutdent() {
    if (g_Log.GetLogLevel() >= 1)
        g_Log.Outdent();
}

inline void _DebugMessage(const char* message, ...) {
    va_list args;
    va_start(args, message);
    g_Log.Message("", message, args, true, 1);
    va_end(args);
}

inline void _DebugWarning(const char* message, ...) {
    va_list args;
    va_start(args, message);
    g_Log.Message("Warning: ", message, args, true, 1);
    va_end(args);
}

inline void _DebugError(const char* message, ...) {
    va_list args;
    va_start(args, message);
    g_Log.Message("Error: ", message, args, true, 1);
    va_end(args);
}

inline void _DebugMessageNT(const char* message, ...) {
    va_list args;
    va_start(args, message);
    g_Log.Message("", message, args, false, 1);
    va_end(args);
}

inline void _DebugWarningNT(const char* message, ...) {
    va_list args;
    va_start(args, message);
    g_Log.Message("Warning: ", message, args, false, 1);
    va_end(args);
}

inline void _DebugErrorNT(const char* message, ...) {
    va_list args;
    va_start(args, message);
    g_Log.Message("Error: ", message, args, false, 1);
    va_end(args);
}