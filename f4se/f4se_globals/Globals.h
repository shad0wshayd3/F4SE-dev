#pragma once

#include "ILog.h"
#include "IGlobalSettings.h"
#include "IObScript.h"
#include "ISettings.h"
#include "ITimeKeeper.h"

#include "Definitions.h"
#include "Native.h"
#include "Scaleform.h"
#include "Utilities.h"

void InitializePlugin(const char* logName);
void InitializePlugin(const char* logName, const char* configPath);
void InitializePlugin(const char* logName, const char* configPath, const char* configPrefix);