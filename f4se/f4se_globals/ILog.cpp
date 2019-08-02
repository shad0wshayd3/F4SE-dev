#include "ILog.h"

#include "common/IFileStream.h"

#include <chrono>
#include <iomanip>
#include <ShlObj.h>
#include <sstream>

ILog::ILog(const char* logName) {
    m_logFile       = NULL;
    m_indentLevel   = 0;
    m_cursorPos     = 0;

    char logPath[MAX_PATH];
    sprintf_s(logPath, sizeof(logPath), "\\My Games\\Fallout4\\F4SE\\%s.log", logName);
    OpenRelative(CSIDL_MYDOCUMENTS, logPath);
}

ILog::~ILog() {
    if (m_logFile)
        fclose(m_logFile);
}

void ILog::Open(const char* logPath) {
    m_logFile = _fsopen(logPath, "w", _SH_DENYWR);

    if (!m_logFile) {
        UInt32 id = 0;
        char name[1024];

        do {
            sprintf_s(name, sizeof(name), "%s%d", logPath, id);
            id++;

            m_logFile = NULL;
            m_logFile = _fsopen(name, "w", _SH_DENYWR);
        } while (!m_logFile && (id < 5));
    }
}

void ILog::OpenRelative(int folderID, const char* relativePath) {
    char logPath[MAX_PATH];

    HRESULT err = SHGetFolderPath(NULL, folderID | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, logPath);
    if (!SUCCEEDED(err))
        _FATALERROR("SHGetFolderPath %08X failed (result = %08X lasterr = %08X)", folderID, err, GetLastError());

    ASSERT_CODE(SUCCEEDED(err), err);

    strcat_s(logPath, sizeof(logPath), relativePath);
    IFileStream::MakeAllDirs(logPath);
    Open(logPath);
}

void ILog::Indent() {
    m_indentLevel++;
}

void ILog::Outdent() {
    if (m_indentLevel)
        m_indentLevel--;
}

void ILog::LogMessage(const char* messageText, ...) {
    va_list args; va_start(args, messageText);
    TimestampedMessage(messageText, args);
    va_end(args);
}

void ILog::LogWarning(const char* messageText, ...) {
    va_list args; va_start(args, messageText);
    TimestampedMessage(messageText, args, "Warning: ");
    va_end(args);
}

void ILog::LogError(const char* messageText, ...) {
    va_list args; va_start(args, messageText);
    TimestampedMessage(messageText, args, "Error: ");
    va_end(args);
}

void ILog::LogMessage(const char* messageText, va_list args) {
    TimestampedMessage(messageText, args);
}

void ILog::LogWarning(const char* messageText, va_list args) {
    TimestampedMessage(messageText, args, "Warning: ");
}

void ILog::LogError(const char* messageText, va_list args) {
    TimestampedMessage(messageText, args, "Error: ");
}

void ILog::LogMessageNT(const char* messageText, ...) {
    va_list args; va_start(args, messageText);
    MessageNT(messageText, args);
    va_end(args);
}

void ILog::LogWarningNT(const char* messageText, ...) {
    va_list args; va_start(args, messageText);
    MessageNT(messageText, args, "Warning: ");
    va_end(args);
}

void ILog::LogErrorNT(const char* messageText, ...) {
    va_list args; va_start(args, messageText);
    MessageNT(messageText, args, "Error: ");
    va_end(args);
}

void ILog::LogMessageNT(const char* messageText, va_list args) {
    MessageNT(messageText, args);
}

void ILog::LogWarningNT(const char* messageText, va_list args) {
    MessageNT(messageText, args, "Warning: ");
}

void ILog::LogErrorNT(const char* messageText, va_list args) {
    MessageNT(messageText, args, "Error: ");
}

// ------------------------------------------------------------------------------------------------
// Internal Functions
// ------------------------------------------------------------------------------------------------

void ILog::Message(const char* message, bool newLine) {
    SeekCursor(m_indentLevel * 4);
    PrintText(message);

    if (newLine)
        NewLine();
}

void ILog::TimestampedMessage(const char* messageText, const char* messagePrefix) {
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    tm Timestamp; localtime_s(&Timestamp, &now);

    std::stringstream MessageStream;
    MessageStream << std::put_time(&Timestamp, "[%m/%d/%Y - %I:%M:%S%p] ");

    if (messagePrefix)
        MessageStream << messagePrefix;

    MessageStream << messageText;
    Message(MessageStream.str().c_str());
}

void ILog::TimestampedMessage(const char* messageText, va_list args, const char* messagePrefix) {
    char formatBuf[8192];
    vsprintf_s(formatBuf, sizeof(formatBuf), messageText, args);
    TimestampedMessage(formatBuf, messagePrefix);
}

void ILog::MessageNT(const char* messageText, const char* messagePrefix) {
    std::stringstream MessageStream;

    if (messagePrefix)
        MessageStream << messagePrefix;

    MessageStream << messageText;
    Message(MessageStream.str().c_str());
}

void ILog::MessageNT(const char* messageText, va_list args, const char* messagePrefix) {
    char formatBuf[8192];
    vsprintf_s(formatBuf, sizeof(formatBuf), messageText, args);
    MessageNT(formatBuf, messagePrefix);
}

void ILog::SeekCursor(int position) {
    if (position > m_cursorPos)
        PrintSpaces(position - m_cursorPos);
}

void ILog::PrintSpaces(int numSpaces) {
    int	originalNumSpaces = numSpaces;

    if (m_logFile) {
        while (numSpaces > 0) {
            if (numSpaces >= TabSize()) {
                numSpaces -= TabSize();
                fputc('\t', m_logFile);
            }
            else {
                numSpaces--;
                fputc(' ', m_logFile);
            }
        }
    }

    m_cursorPos += originalNumSpaces;
}

void ILog::PrintText(const char* buf) {
    if (m_logFile) {
        fputs(buf, m_logFile);
        fflush(m_logFile);
    }

    const char* traverse = buf;
    char data;

    while (data = *traverse++) {
        if (data == '\t')
            m_cursorPos += TabSize();
        else
            m_cursorPos++;
    }
}

void ILog::NewLine() {
    if (m_logFile) {
        fputc('\n', m_logFile);
        fflush(m_logFile);
    }

    m_cursorPos = 0;
}

int ILog::TabSize() {
    return ((~m_cursorPos) & 3) + 1;
}
