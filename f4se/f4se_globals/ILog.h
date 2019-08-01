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

    void    LogMessage	(const char* messageText, ...);
    void    LogWarning	(const char* messageText, ...);
    void    LogError	(const char* messageText, ...);

    void    LogMessageNT(const char* messageText, ...);

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

    FILE*   logFile;
    int     indentLevel;
    int     cursorPos;
};
