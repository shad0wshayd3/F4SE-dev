#include "ILog.h"

#include "common/IFileStream.h"

#include <chrono>
#include <iomanip>
#include <ShlObj.h>
#include <sstream>

// ------------------------------------------------------------------------------------------------
// ILog
// ------------------------------------------------------------------------------------------------

void ILog::Open(const char* logName) {
    char logPath[MAX_PATH], relativePath[MAX_PATH];

    sprintf_s(relativePath, sizeof(relativePath), "\\My Games\\Fallout4\\F4SE\\%s.log", logName);
    HRESULT err = SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, logPath);
    if (!SUCCEEDED(err))
        HALT("Could not create log file.");

    strcat_s(logPath, sizeof(logPath), relativePath);
    IFileStream::MakeAllDirs(logPath);

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

void ILog::MessageNT(const char* messageText, va_list args, const char* messagePrefix) {
    std::stringstream MessageStream;

    if (messagePrefix)
        MessageStream << messagePrefix;

    char formatBuf[8192];
    vsprintf_s(formatBuf, sizeof(formatBuf), messageText, args);

    MessageStream << formatBuf;
    PrintMessage(MessageStream.str().c_str());
}

void ILog::MessageTS(const char* messageText, va_list args, const char* messagePrefix) {
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    tm Timestamp; localtime_s(&Timestamp, &now);

    std::stringstream MessageStream;
    MessageStream << std::put_time(&Timestamp, "[%m/%d/%Y - %I:%M:%S%p] ");

    if (messagePrefix)
        MessageStream << messagePrefix;

    char formatBuf[8192];
    vsprintf_s(formatBuf, sizeof(formatBuf), messageText, args);

    MessageStream << formatBuf;
    PrintMessage(MessageStream.str().c_str());
}

// ------------------------------------------------------------------------------------------------
// Internal Functions
// ------------------------------------------------------------------------------------------------

void ILog::PrintMessage(const char* message) {
    SeekCursor(m_indentLevel * 4);
    PrintText(message);
    NewLine();
}

void ILog::SeekCursor(int position) {
    if (position > m_cursorPos)
        PrintSpaces(position - m_cursorPos);
}

void ILog::PrintSpaces(int numSpaces) {
    int	originalNumSpaces = numSpaces;

    if (m_logFile) {
        while (numSpaces > 0) {
            numSpaces--;
            fputc(' ', m_logFile);
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

// ------------------------------------------------------------------------------------------------
// Initialize
// ------------------------------------------------------------------------------------------------

int     ILog::m_indentLevel = 0;

FILE*   ILog::m_logFile       = NULL;
int     ILog::m_cursorPos     = 0;