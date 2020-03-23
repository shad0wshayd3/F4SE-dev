#pragma once

#include "ILog.h"
#include "IGlobalSettings.h"
#include "IObScript.h"
#include "ISettings.h"
#include "ITimeKeeper.h"
#include "ITranslation.h"

#include "Definitions.h"
#include "FormLoader.h"
#include "Messaging.h"
#include "Native.h"
#include "Scaleform.h"
#include "Utilities.h"
#include "Virtual.h"

void InitializePlugin(const char* logName);
void InitializePlugin(const char* logName, const char* configPath);