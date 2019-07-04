#include "ILog.h"

#include <chrono>
#include <iomanip>
#include <ShlObj.h>
#include <sstream>

ILog::ILog(const char* logName) {
	char logPath[MAX_PATH];
	indentLevel = 0; cursorPos = 0;
	logFile = NULL;

    sprintf_s(logPath, sizeof(logPath), "\\My Games\\Fallout4\\F4SE\\%s.log", logName);
    OpenRelative(CSIDL_MYDOCUMENTS, logPath);
}

ILog::~ILog() {
	if (logFile)
		fclose(logFile);
}

void ILog::Open(const char* logPath) {
	logFile = _fsopen(logPath, "w", _SH_DENYWR);

	if (!logFile) {
		UInt32	id = 0;
		char	name[1024];

		do {
			sprintf_s(name, sizeof(name), "%s%d", logPath, id);
			id++;

			logFile = NULL;
			logFile = _fsopen(name, "w", _SH_DENYWR);
		} while (!logFile && (id < 5));
	}
}

void ILog::OpenRelative(int folderID, const char* relativePath) {
	char	logPath[MAX_PATH];

	HRESULT err = SHGetFolderPath(NULL, folderID | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, logPath);
	if (!SUCCEEDED(err)) {
		_FATALERROR("SHGetFolderPath %08X failed (result = %08X lasterr = %08X)", folderID, err, GetLastError());
	}

	ASSERT_CODE(SUCCEEDED(err), err);

	strcat_s(logPath, sizeof(logPath), relativePath);
	IFileStream::MakeAllDirs(logPath);
	Open(logPath);
}

void ILog::Indent(void) {
	indentLevel++;
}

void ILog::Outdent(void) {
	if (indentLevel)
		indentLevel--;
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

// ------------------------------------------------------------------------------------------------
// Internal Functions
// ------------------------------------------------------------------------------------------------

void ILog::Message(const char* message, bool newLine) {
	SeekCursor(indentLevel * 4);
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

void ILog::SeekCursor(int position) {
	if (position > cursorPos)
		PrintSpaces(position - cursorPos);
}

void ILog::PrintSpaces(int numSpaces) {
	int	originalNumSpaces = numSpaces;

	if (logFile) {
		while (numSpaces > 0) {
			if (numSpaces >= TabSize()) {
				numSpaces -= TabSize();
				fputc('\t', logFile);
			}
			else {
				numSpaces--;
				fputc(' ', logFile);
			}
		}
	}

	cursorPos += originalNumSpaces;
}

void ILog::PrintText(const char* buf) {
	if (logFile) {
		fputs(buf, logFile);
		fflush(logFile);
	}

	const char* traverse = buf;
	char		data;

	while (data = *traverse++) {
		if (data == '\t')
			cursorPos += TabSize();
		else
			cursorPos++;
	}
}

void ILog::NewLine() {
	if (logFile) {
		fputc('\n', logFile);
		fflush(logFile);
	}

	cursorPos = 0;
}

int ILog::TabSize() {
	return ((~cursorPos) & 3) + 1;
}