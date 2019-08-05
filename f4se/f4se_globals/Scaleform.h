#pragma once

#include "f4se/ScaleformMovie.h"
#include "f4se/ScaleformValue.h"

namespace GFxHelperFunctions {
    extern void RegisterString(GFxValue* dst, GFxMovieRoot* root, const char* name, const char* str);
    extern void RegisterNumber(GFxValue* dst, const char* name, double value);
    extern void RegisterInt(GFxValue* dst, const char* name, int value);
    extern void RegisterBool(GFxValue* dst, const char* name, bool value);
}