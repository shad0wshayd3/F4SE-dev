#include "Globals.h"

// ------------------------------------------------------------------------------------------------
// InitializePlugin
// ------------------------------------------------------------------------------------------------

void InitializePlugin(const char* logName) {
    ILog::Open(logName);
}

void InitializePlugin(const char* logName, const char* configPath) {
    ILog::Open(logName);
    ISettings::Open(configPath);
}

void InitializePlugin(const char* logName, const char* configPath, const char* configPrefix) {
    ILog::Open(logName);
    ISettings::Open(configPath);
    IGlobalSettings::Open(configPrefix);
}