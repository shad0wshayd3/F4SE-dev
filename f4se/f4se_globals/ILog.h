#pragma once

#include <fstream>
#include <string>

class GLog {
public:
					~GLog();

	void			Open(std::string logName);
	void			Message(const char* prefix, const char* message, va_list args, bool timestamp);
	void			Indent();
	void			Outdent();

private:
	void			Write(const char* message);

	std::ofstream	m_logFile;
	int				m_indentLevel;
};

extern GLog g_Log;

inline void _LogIndent() {
	g_Log.Indent();
}

inline void _LogOutdent() {
	g_Log.Outdent();
}

inline void _LogMessage(const char* message, ...) {
    va_list args; va_start(args, message);
	g_Log.Message("", message, args, true);
    va_end(args);
}

inline void _LogWarning(const char* message, ...) {
    va_list args; va_start(args, message);
	g_Log.Message("Warning: ", message, args, true);
    va_end(args);
}

inline void _LogError(const char* message, ...) {
    va_list args; va_start(args, message);
	g_Log.Message("Error: ", message, args, true);
    va_end(args);
}

inline void _LogMessageNT(const char* message, ...) {
	va_list args; va_start(args, message);
	g_Log.Message("", message, args, false);
    va_end(args);
}

inline void _LogWarningNT(const char* message, ...) {
	va_list args; va_start(args, message);
	g_Log.Message("Warning: ", message, args, false);
    va_end(args);
}

inline void _LogErrorNT(const char* message, ...) {
	va_list args; va_start(args, message);
	g_Log.Message("Error: ", message, args, false);
    va_end(args);
}