#pragma once

#include "f4se/ScaleformTranslator.h"

class ITranslation {
public:
    static bool                     Init();

    static void                     AddTranslatedString(const wchar_t* key, const wchar_t* translation);
    static const char*              GetTranslatedString(const wchar_t* key);

private:
    static BSScaleformTranslator*   m_translator;
    static char                     translationBuf;
};