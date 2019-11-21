#include "Globals.h"

GLog g_Log = GLog();

// ------------------------------------------------------------------------------------------------
// InitializePlugin
// ------------------------------------------------------------------------------------------------

void InitializePlugin(const char* logName) {
	g_Log.Open(logName);
}

void InitializePlugin(const char* logName, const char* configPath) {
	g_Log.Open(logName);
    ISettings::Open(configPath);
}