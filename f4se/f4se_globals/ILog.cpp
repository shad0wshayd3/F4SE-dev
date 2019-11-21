#include "ILog.h"

#include <chrono>
#include <filesystem>
#include <ShlObj.h>
#include <sstream>

// ------------------------------------------------------------------------------------------------
// GLog
// ------------------------------------------------------------------------------------------------

GLog::~GLog() {
	if (m_logFile.is_open())
		m_logFile.close();
}

void GLog::Open(std::string logName) {
	PWSTR documentsPath;
	HRESULT hr = SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &documentsPath);

	if (!SUCCEEDED(hr))
		HALT("Couldn't get My Documents folder.");

	auto path = std::filesystem::path(documentsPath);
	path.append("My Games\\Fallout4\\F4SE");
	path.append(logName + ".log");

	m_logFile.open(path, std::ofstream::trunc);
	if (!m_logFile.is_open()) {
		int index = 0;

		do {
			path.remove_filename();
			path.append(logName + std::to_string(index) + ".log");
			index++;

			m_logFile.open(path, std::ofstream::trunc);

		} while (!m_logFile.is_open());
	}
}

void GLog::Message(const char* prefix, const char* message, va_list args, bool timestamp) {
	std::stringstream stream;

	if (timestamp) {
		auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		tm Timestamp; localtime_s(&Timestamp, &time);

		stream << std::put_time(&Timestamp, "[%m/%d/%Y - %I:%M:%S%p] ");
	}

	if (prefix) {
		stream << prefix;
	}

    char formatBuf[8192];
    vsprintf_s(formatBuf, sizeof(formatBuf), message, args);

	stream << formatBuf;
    Write(stream.str().c_str());
}

void GLog::Indent() {
	m_indentLevel++;
}

void GLog::Outdent() {
	if (m_indentLevel)
		m_indentLevel--;
}

// ------------------------------------------------------------------------------------------------
// Internal Functions
// ------------------------------------------------------------------------------------------------

void GLog::Write(const char* message) {
	for (int i = 0; i < m_indentLevel; i++)
		m_logFile << "    ";
	m_logFile << message << "\n";
	m_logFile.flush();
}