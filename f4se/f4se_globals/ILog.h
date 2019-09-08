#pragma once

class ILog {
public:
    static void     Open(const char* logName);

    static void     MessageNT(const char* messageText, va_list args, const char* messagePrefix = NULL);
    static void     MessageTS(const char* messageText, va_list args, const char* messagePrefix = NULL);

    static int      m_indentLevel;

private:
    static void     PrintMessage(const char* message);

    static void     SeekCursor(int position);
    static void     PrintSpaces(int numSpaces);
    static void     PrintText(const char* buf);
    static void     NewLine();
    static int      TabSize();

    static FILE*    m_logFile;
    
    static int      m_cursorPos;
};

inline void _LogIndent() {
    ILog::m_indentLevel++;
}

inline void _LogOutdent() {
    if (ILog::m_indentLevel)
        ILog::m_indentLevel--;
}

inline void _LogMessage(const char* messageText, ...) {
    va_list args; va_start(args, messageText);
    ILog::MessageTS(messageText, args);
    va_end(args);
}

inline void _LogWarning(const char* messageText, ...) {
    va_list args; va_start(args, messageText);
    ILog::MessageTS(messageText, args, "Warning: ");
    va_end(args);
}

inline void _LogError(const char* messageText, ...) {
    va_list args; va_start(args, messageText);
    ILog::MessageTS(messageText, args, "Error: ");
    va_end(args);
}

inline void _LogMessageNT(const char* messageText, ...) {
    va_list args; va_start(args, messageText);
    ILog::MessageNT(messageText, args);
    va_end(args);
}

inline void _LogWarningNT(const char* messageText, ...) {
    va_list args; va_start(args, messageText);
    ILog::MessageNT(messageText, args, "Warning: ");
    va_end(args);
}

inline void _LogErrorNT(const char* messageText, ...) {
    va_list args; va_start(args, messageText);
    ILog::MessageNT(messageText, args, "Error: ");
    va_end(args);
}