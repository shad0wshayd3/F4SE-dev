#pragma once

/**
    Reimplimentation of IDebugLog with support for multiple active classes, and less garbage.
*/
class ILog {
public:
            ILog(const char* logName);
            ~ILog();

    void    Open(const char* logPath);
    void    OpenRelative(int folderID, const char* relativePath);

    void    Indent();
    void    Outdent();

    void    LogMessage(const char* messageText, ...);
    void    LogWarning(const char* messageText, ...);
    void    LogError(const char* messageText, ...);

    void    LogMessage(const char* messageText, va_list args);
    void    LogWarning(const char* messageText, va_list args);
    void    LogError(const char* messageText, va_list args);

    void    LogMessageNT(const char* messageText, ...);
    void    LogWarningNT(const char* messageText, ...);
    void    LogErrorNT(const char* messageText, ...);

    void    LogMessageNT(const char* messageText, va_list args);
    void    LogWarningNT(const char* messageText, va_list args);
    void    LogErrorNT(const char* messageText, va_list args);

private:
    void    Message(const char* message, bool newLine = true);

    void    MessageNT(const char* messageText, const char* messagePrefix = NULL);
    void    MessageNT(const char* messageText, va_list args, const char* messagePrefix = NULL);

    void    TimestampedMessage(const char* messageText, const char* messagePrefix = NULL);
    void    TimestampedMessage(const char* messageText, va_list args, const char* messagePrefix = NULL);

    void    SeekCursor(int position);
    void    PrintSpaces(int numSpaces);
    void    PrintText(const char* buf);
    void    NewLine();
    int     TabSize();

    FILE*   m_logFile;
    int     m_indentLevel;
    int     m_cursorPos;
};

extern ILog g_Log;

inline void _LOGMESSAGE(const char* messageText, ...) {
    va_list args; va_start(args, messageText);
    g_Log.LogMessage(messageText, args);
    va_end(args);
}

inline void _LOGWARNING(const char* messageText, ...) {
    va_list args; va_start(args, messageText);
    g_Log.LogWarning(messageText, args);
    va_end(args);
}

inline void _LOGERROR(const char* messageText, ...) {
    va_list args; va_start(args, messageText);
    g_Log.LogError(messageText, args);
    va_end(args);
}

inline void _LOGMESSAGENT(const char* messageText, ...) {
    va_list args; va_start(args, messageText);
    g_Log.LogMessageNT(messageText, args);
    va_end(args);
}

inline void _LOGWARNINGNT(const char* messageText, ...) {
    va_list args; va_start(args, messageText);
    g_Log.LogWarningNT(messageText, args);
    va_end(args);
}

inline void _LOGERRORNT(const char* messageText, ...) {
    va_list args; va_start(args, messageText);
    g_Log.LogErrorNT(messageText, args);
    va_end(args);
}