#include "Papyrus.h"

#include <algorithm>
#include <string>

namespace BakaUtil {
    VMArray<TESObjectREFR*> FilterReferencesByKeywords(StaticFunctionTag* base, VMArray<TESObjectREFR*> refrs, VMArray<BGSKeyword*> kywds, bool filterHas) {
        VMArray<TESObjectREFR*> result;
        result.SetNone(true);

        if ((refrs.Length() == 0) || (kywds.Length() == 0))
            return result;

        result.SetNone(false);
        for (auto i = 0; i < refrs.Length(); i++) {
            TESObjectREFR* refr; refrs.Get(&refr, i);
            if (!refr)
                continue;

            for (auto j = 0; j < kywds.Length(); j++) {
                BGSKeyword* kywd; kywds.Get(&kywd, j);
                if (!kywd)
                    continue;

                if (HasKeyword(refr, kywd) && filterHas) {
                    result.Push(&refr);
                    break;
                }

                if (!HasKeyword(refr, kywd) && !filterHas) {
                    result.Push(&refr);
                    break;
                }
            }
        }

        return result;
    }
}

namespace StringUtil {
    struct SingleCharStar {
        char m_c[2];
        SingleCharStar(char c) {
            m_c[0] = c; m_c[1] = '\0';
        }
        SingleCharStar& operator=(char c) {
            m_c[0] = c;
            return *this;
        }
        const char* c_str() {
            return m_c;
        }
    };

    UInt32 GetLength(StaticFunctionTag* base, BSFixedString string) {
        if (!string.data->data)
            return 0;
        return strlen(string.data->data);
    }

    BSFixedString GetNthCharacter(StaticFunctionTag* base, BSFixedString string, UInt32 index) {
        if ((0 > index) || (index > strlen(string.data->data)))
            return BSFixedString("");
        return BSFixedString(SingleCharStar(string.data->data[index]).c_str());
    }

    bool IsLetter(StaticFunctionTag* base, BSFixedString string) {
        if (!string.data->data)
            return false;
        return std::isalpha(string.data->data[0]);
    }

    bool IsDigit(StaticFunctionTag* base, BSFixedString string) {
        if (!string.data->data)
            return false;
        return std::isdigit(string.data->data[0]);
    }

    bool IsPunctuation(StaticFunctionTag* base, BSFixedString string) {
        if (!string.data->data)
            return false;
        return std::ispunct(string.data->data[0]);
    }

    bool IsPrintable(StaticFunctionTag* base, BSFixedString string) {
        if (!string.data->data)
            return false;
        return std::isprint(string.data->data[0]);
    }

    UInt32 AsInt(StaticFunctionTag* thisInput, BSFixedString string) {
        return string.data->data[0];
    }

    BSFixedString AsChar(StaticFunctionTag* thisInput, UInt32 index) {
        if (index > 255)
            return BSFixedString("");
        char result = index;
        return BSFixedString(SingleCharStar(index).c_str());
    }

    UInt32 Find(StaticFunctionTag* base, BSFixedString string, BSFixedString toFind, UInt32 startIndex) {
        if (startIndex < 0)
            startIndex = 0;

        std::string str(string.data->data);

        if (startIndex >= str.length())
            return -1;
        StringToUpper(str);

        std::string find(toFind.data->data);
        StringToUpper(find);

        auto index = str.find(find, startIndex);
        return (index == std::string::npos) ? -1 : index;
    }

    BSFixedString Substring(StaticFunctionTag* base, BSFixedString string, UInt32 startIndex, UInt32 length) {
        if (startIndex < 0)
            return BSFixedString("");

        std::string str(string.data->data);
        if (startIndex >= str.length())
            return BSFixedString("");

        std::string sub = (length) ? str.substr(startIndex, length) : str.substr(startIndex);
        return BSFixedString(sub.c_str());
    }

    VMArray<BSFixedString> StringToArray(StaticFunctionTag* base, BSFixedString string, BSFixedString delim) {
        VMArray<BSFixedString> result;
        result.SetNone(true);

        std::string str(string.data->data);
        if (str.length() == 0)
            return result;

        result.SetNone(false);

        if (strlen(delim.data->data) == 0) {
            for (auto iter : str) {
                result.Push(&BSFixedString(&iter));
            }
        }
        else {
            std::string upperStr(str);
            std::string delimStr(delim.data->data);
            StringToUpper(upperStr);
            StringToUpper(delimStr);

            auto lastIdx = upperStr.find_first_not_of(delimStr, 0);
            auto thisIdx = upperStr.find_first_of(delimStr, lastIdx);

            while ((std::string::npos != thisIdx) || (std::string::npos != lastIdx)) {
                std::string token = str.substr(lastIdx, thisIdx - lastIdx);

                result.Push(&BSFixedString(token.c_str()));
                lastIdx = upperStr.find_first_not_of(delimStr, thisIdx);
                thisIdx = upperStr.find_first_of(delimStr, lastIdx);
            }
        }

        return result;
    }

    BSFixedString ArrayToString(StaticFunctionTag* base, VMArray<BSFixedString> array, BSFixedString delim) {
        if (array.Length() == 0)
            return BSFixedString("");

        std::string result;
        for (auto i = 0; i < array.Length(); i++) {
            BSFixedString nChar; array.Get(&nChar, i);
            result.append(nChar.data->data).append(delim.data->data);
        }

        return BSFixedString(result.c_str());
    }
}

// std::vector<UInt32> SearchFormList(BGSListForm* FormList, TESForm* Query, std::vector<UInt32> Result) {
//     for (int i = 0; i < FormList->forms.count; i++) {
//         TESForm* Form = FormList->forms[i];
//         if (Form->formID == Query->formID) {
//             Result.emplace_back(i); // Result.insert(Result.begin(), i);
//             return Result;
//         }
// 
//         if (Form->formType == FormType::kFormType_FLST) {
//             std::vector<UInt32> nResult = SearchFormList(DYNAMIC_CAST(Form, TESForm, BGSListForm), Query, Result);
//             if (Result.size() != nResult.size()) {
//                 nResult.emplace_back(i);
//                 return nResult;
//             }
//         }
//     }
// 
//     return Result;
// }

// VMArray<UInt32> FindNestedForm(StaticFunctionTag* base, BGSListForm* RootList, TESForm* Query) {
//     VMArray<UInt32> Result;
//     Result.SetNone(true);
// 
//     if (!RootList || !Query)
//         return Result;
// 
//     Result.SetNone(false);
// 
//     std::vector<UInt32> result_t;
//     result_t = SearchFormList(RootList, Query, result_t);
// 
//     for (UInt32 ret : result_t)
//         Result.Push(&ret);
// 
//     return Result;
// }

bool Papyrus::Init(VirtualMachine* VM) {
    // VM->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMArray<UInt32>, BGSListForm*, TESForm*>
    //     ("FindNestedForm", PAPYRUS_SCRIPT_NAME, FindNestedForm, VM));

    VM->RegisterFunction(new NativeFunction3<StaticFunctionTag, VMArray<TESObjectREFR*>, VMArray<TESObjectREFR*>, VMArray<BGSKeyword*>, bool>
        ("FilterReferencesByKeywords",  "BakaUtil",     BakaUtil::FilterReferencesByKeywords,   VM));

    // StringUtil
    VM->RegisterFunction(new NativeFunction1<StaticFunctionTag, UInt32, BSFixedString>
        ("GetLength",       "StringUtil",   StringUtil::GetLength,          VM));

    VM->RegisterFunction(new NativeFunction2<StaticFunctionTag, BSFixedString, BSFixedString, UInt32>
        ("GetNthCharacter", "StringUtil",   StringUtil::GetNthCharacter,    VM));

    VM->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, BSFixedString>
        ("IsLetter",        "StringUtil",   StringUtil::IsLetter,           VM));

    VM->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, BSFixedString>
        ("IsDigit",         "StringUtil",   StringUtil::IsDigit,            VM));

    VM->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, BSFixedString>
        ("IsPunctuation",   "StringUtil",   StringUtil::IsPunctuation,      VM));

    VM->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, BSFixedString>
        ("IsPrintable",     "StringUtil",   StringUtil::IsPrintable,        VM));

    VM->RegisterFunction(new NativeFunction1<StaticFunctionTag, UInt32, BSFixedString>
        ("AsInt",           "StringUtil",   StringUtil::AsInt,              VM));

    VM->RegisterFunction(new NativeFunction1<StaticFunctionTag, BSFixedString, UInt32>
        ("AsChar",          "StringUtil",   StringUtil::AsChar,             VM));

    VM->RegisterFunction(new NativeFunction3<StaticFunctionTag, UInt32, BSFixedString, BSFixedString, UInt32>
        ("Find",            "StringUtil",   StringUtil::Find,               VM));

    VM->RegisterFunction(new NativeFunction3<StaticFunctionTag, BSFixedString, BSFixedString, UInt32, UInt32>
        ("Substring",       "StringUtil",   StringUtil::Substring,          VM));

    VM->RegisterFunction(new NativeFunction2<StaticFunctionTag, VMArray<BSFixedString>, BSFixedString, BSFixedString>
        ("StringToArray",   "StringUtil",   StringUtil::StringToArray,      VM));

    VM->RegisterFunction(new NativeFunction2<StaticFunctionTag, BSFixedString, VMArray<BSFixedString>, BSFixedString>
        ("ArrayToString",   "StringUtil",   StringUtil::ArrayToString,      VM));

    VM->SetFunctionFlags("StringUtil",  "GetLength",        IFunction::kFunctionFlag_NoWait);
    VM->SetFunctionFlags("StringUtil",  "GetNthCharacter",  IFunction::kFunctionFlag_NoWait);
    VM->SetFunctionFlags("StringUtil",  "IsLetter",         IFunction::kFunctionFlag_NoWait);
    VM->SetFunctionFlags("StringUtil",  "IsDigit",          IFunction::kFunctionFlag_NoWait);
    VM->SetFunctionFlags("StringUtil",  "IsPunctuation",    IFunction::kFunctionFlag_NoWait);
    VM->SetFunctionFlags("StringUtil",  "IsPrintable",      IFunction::kFunctionFlag_NoWait);
    VM->SetFunctionFlags("StringUtil",  "AsInt",            IFunction::kFunctionFlag_NoWait);
    VM->SetFunctionFlags("StringUtil",  "AsChar",           IFunction::kFunctionFlag_NoWait);
    VM->SetFunctionFlags("StringUtil",  "Find",             IFunction::kFunctionFlag_NoWait);
    VM->SetFunctionFlags("StringUtil",  "Substring",        IFunction::kFunctionFlag_NoWait);
    VM->SetFunctionFlags("StringUtil",  "StringToArray",    IFunction::kFunctionFlag_NoWait);
    VM->SetFunctionFlags("StringUtil",  "ArrayToString",    IFunction::kFunctionFlag_NoWait);

    return true;
}