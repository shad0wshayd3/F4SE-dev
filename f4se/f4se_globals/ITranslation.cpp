#include "ITranslation.h"

#include <stdlib.h>

// This is the dumbest workaround.
#define _SKIP_IMAGE_LOADER 1
#include "f4se/ScaleformLoader.h"

bool ITranslation::Init() {
    if (!(*g_scaleformManager))
        return false;

    m_translator = (BSScaleformTranslator*)((*g_scaleformManager)->stateBag->GetStateAddRef(GFxState::kInterface_Translator));
    return (m_translator);
}

void ITranslation::AddTranslatedString(const wchar_t* key, const wchar_t* translation) {
    m_translator->translations.Add(new TranslationTableItem(BSFixedString(key), BSFixedStringW(translation)));
}

const char* ITranslation::GetTranslatedString(const wchar_t* key) {
    TranslationTableItem* item = m_translator->translations.Find(&BSFixedString(key));
    if (!item)
        return "";

    size_t i;
    wcstombs_s(&i, &translationBuf, 0x1000, item->translation.data->Get<wchar_t>(), _TRUNCATE);
    return &translationBuf;
}

BSScaleformTranslator*  ITranslation::m_translator      = nullptr;
char                    ITranslation::translationBuf    = 0;
