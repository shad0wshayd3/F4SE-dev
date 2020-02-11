#include "Scaleform.h"

void SetGFxValue(GFxValue* dst, GFxMovieRoot* root, const char* name, const char* str) {
    GFxValue Value;
    root->CreateString(&Value, str);
    dst->SetMember(name, &Value);
}

void SetGFxValue(GFxValue* dst, GFxMovieRoot* root, const char* name, std::string str) {
    GFxValue Value;
    root->CreateString(&Value, str.c_str());
    dst->SetMember(name, &Value);
}

void SetGFxValue(GFxValue* dst, const char* name, UInt32 value) {
    GFxValue Value;
    Value.SetUInt(value);
    dst->SetMember(name, &Value);
}

void SetGFxValue(GFxValue* dst, const char* name, double value) {
    GFxValue Value;
    Value.SetNumber(value);
    dst->SetMember(name, &Value);
}

void SetGFxValue(GFxValue* dst, const char* name, bool value) {
    GFxValue Value;
    Value.SetBool(value);
    dst->SetMember(name, &Value);
}

void SetGFxValue(GFxValue* dst, const char* name, int value) {
    GFxValue Value;
    Value.SetInt(value);
    dst->SetMember(name, &Value);
}

void SetGFxValue(GFxValue* dst, const char* name, GFxValue value) {
    dst->SetMember(name, &value);
}

void SetGFxValue(GFxValue* dst, const char* name, GFxValue* value) {
    dst->SetMember(name, value);
}