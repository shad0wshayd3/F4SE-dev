#pragma once

#include "MenuDefinitions.h"
#include "f4se/GameExtraData.h"
#include "f4se/GameObjects.h"

class ActiveEffect {
public:
    enum { kTypeID = kFormType_ActiveMagicEffect };

    enum Flags {
        kFlag_Inactive = 0x8000
    };

    void*               unk00;          // 00
    UInt32              unk08;          // 08
    void*               unk10;          // 10
    UInt64              unk18;          // 18
    UInt32              unk20;          // 20
    UInt32              unk24;          // 24
    UInt32              unk28;          // 28
    UInt32              unk2C;          // 2C
    UInt32              unk30;          // 30
    UInt16              unk34;          // 34
    UInt32              unk38;          // 38
    UInt64              unk40;          // 40
    TESForm             * spell;        // 48

    struct Data {
        float               magnitude;
        UInt32              area;
        UInt32              duration;
        EffectSetting       * setting;
        float               baseCost;
        Condition           * condition;
    };

    Data                * effects;      // 50
    TESForm             * target;       // 58
    TESForm             * source;       // 60
    UInt64              unk68;          // 68
    UInt64              unk70;          // 70
    float               elapsed;        // 78
    float               duration;       // 7C
    float               magnitude;      // 80
    UInt32              flags;          // 84
    UInt32              unk88;          // 88
    UInt16              unk8C;          // 8C
    UInt32              unk90;          // 90
};
STATIC_ASSERT(sizeof(ActiveEffect) == 0x98);

class BGSInventoryItemEx {
public:
    class StackDataCompareFunctor {
    public:
        virtual bool CompareData(BGSInventoryItem::Stack* stack) = 0;
        virtual bool UseDefaultStackIfNoMatch() {
            return true;
        }
    };

    class CheckStackIDFunctor: public BGSInventoryItemEx::StackDataCompareFunctor {
    public:
                                    CheckStackIDFunctor(UInt32 index);
        bool                        CompareData(BGSInventoryItem::Stack* stack) override;
                                    
        UInt32                      index;
        UInt8                       pad[4];
    };

    class StackDataWriteFunctor {
    public:
        virtual void                WriteDataImpl(TESBoundObject* baseObject, BGSInventoryItem::Stack* stack) = 0;

        bool                        bShouldSplitStacks;
        bool                        bTransferEquippedToSplitStack;
        UInt8                       pad0A[6];
    };

    class ModifyModDataFunctor: public BGSInventoryItemEx::StackDataWriteFunctor {
    public:
                                    ModifyModDataFunctor(BGSMod::Attachment::Mod* mod, UInt8 index, bool doAttach, bool* modified);
        void                        WriteDataImpl(TESBoundObject* baseObject, BGSInventoryItem::Stack* stack) override;

        BGSMod::Attachment::Mod     * mod;
        TESBoundObject              * baseObject;
        bool                        * modified;
        UInt8                       index;
        bool                        doAttach;
        bool                        locked;
        UInt8                       pad2B[4];
    };

    class SetHealthFunctor: public BGSInventoryItemEx::StackDataWriteFunctor {
    public:
                                    SetHealthFunctor(float health);
        void                        WriteDataImpl(TESBoundObject* baseObject, BGSInventoryItem::Stack* stack) override;

        float                       health;
        UInt8                       pad14[4];
    };

    class WriteTextExtra: public BGSInventoryItemEx::StackDataWriteFunctor {
    public:
                                    WriteTextExtra(const char* name);
        void                        WriteDataImpl(TESBoundObject* baseObject, BGSInventoryItem::Stack* stack) override;

        const char                  * name;
    };
};

class BGSMessage : public TESForm {
public:
    enum { kTypeID = kFormType_MESG };

    TESFullName                 fullName;
    TESDescription              description;
    TESIcon                     * icon;
    TESQuest                    * owningQuest;
    UInt64                      unk00;
    UInt64                      unk01;
    BSFixedString               swfPath;
    BSFixedString               shortName;
    UInt32                      unk02;
    UInt32                      displayTime;
};
STATIC_ASSERT(sizeof(BGSMessage) == 0x80);

class BGSNote : public TESBoundObject {
public:
    enum { kTypeID = kFormType_NOTE };

    TESWeightForm               weight;
    TESValueForm                value;
    TESModel                    model;
    TESFullName                 fullName;
    TESIcon                     icon;
    BGSPickupPutdownSounds      pickupSounds;
    BSFixedString               swfPath;
    UInt32                      unk00[3];
};
STATIC_ASSERT(sizeof(BGSNote) == 0x108);

class BGSObjectInstance {
public:
    BGSObjectInstance(TESForm* form, TBO_InstanceData* instance);

    TESForm             * object;
    TBO_InstanceData    * instanceData;
};
STATIC_ASSERT(sizeof(BGSObjectInstance) == 0x10);

template <typename T>
class BGSObjectInstanceT: public BGSObjectInstance {
public:
    BGSObjectInstanceT(T form, TBO_InstanceData* instance);
};
STATIC_ASSERT(sizeof(BGSObjectInstanceT<TESForm*>) == 0x10);

class ExtraCharge : public BSExtraData {
public:
    float       charge;
    UInt32      pad1C;

    static ExtraCharge* Create(float value);
};
STATIC_ASSERT(sizeof(ExtraCharge) == 0x20);

class ExtraObjectHealth : public BSExtraData {
public:
    float       health;
    UInt32      pad1C;

    static ExtraObjectHealth* Create(float value);
};
STATIC_ASSERT(sizeof(ExtraObjectHealth) == 0x20);

class Interface3D {
public:
    class Renderer {
    public:
        UInt64              unk00[3];
        UInt64              unk18[3];
        BSReadWriteLock     m_lock;
        NiColorA            clearColor;
        float               alpha;
        bool                enabled;
        bool                offscreen3DEnabled;
        bool                hideScreenWhenDisabled;
        bool                usePremultAlpha;
        bool                clearRenderTarget;
        bool                clearDepthStencilMain;
        bool                clearDepthStencilOff;
        bool                enableAA;
        bool                useFullPremultAlpha;
        bool                renderWhenEnabled;
        bool                renderMainScreen;
        bool                enableAO;
        bool                useFarCamera;
        bool                needsLightMain;
        bool                needsLightOff;
        UInt8               pad5B;
        UInt32              backgroundMode;
        UInt32              postEffect;
        UInt32              highlightEffect;
        UInt32              offscreenMenuSize;
        UInt32              screenMode;
        UInt32              offscreenMenuBlendMode;
        UInt32              depth;
        NiAVObject          * worldElementRoot;
        NiAVObject          * screenElementMain;
        NiAVObject          * screenElementOff;
        UInt64              unk90[3];
        NiAVObject          * fullViewElement;
        NiAVObject          * highlightedElement;
        bool                highlightOffscreen;
        UInt8               unkB9[231];
        void                * unk1A0;
        void                * unk1A8;
        void                * unk1B0;
        void                * unk1B8;
        UInt32              customRenderTarget;
        UInt32              customSwapTarget;
        UInt64              unk1C8[3];
        UInt64              unk1E0[3];
        float               menuDiffuseIntensity1;
        float               menuEmitIntensity1;
        float               menuDiffuseIntensity2;
        float               menuEmitIntensity2;
        BSFixedString       name;
        BSFixedString       screenGeometryName;
        BSFixedString       screenMaterialName;
        BSFixedString       maskGeometryName;
        BSFixedString       maskMaterialName;
        void                * unk238;
        void                * unk240;
        void                * unk248;
    };
};
STATIC_ASSERT(sizeof(Interface3D::Renderer) == 0x248);
STATIC_ASSERT(offsetof(Interface3D::Renderer, name) == 0x208);

class TESIdleForm : public TESForm {
public:
    enum { kTypeID = kFormType_IDLE };
};

class TESKey : public TESObjectMISC {
public:
    enum { kTypeID = kFormType_KEYM };
};

class TESObjectBOOK : public TESBoundObject {
public:
    enum { kTypeID = kFormType_BOOK };

    TESFullName                 fullName;
    BGSModelMaterialSwap        materialSwap;
    TESIcon                     icon;
    TESValueForm                value;
    TESWeightForm               weight;
    TESDescription              description;
    BGSDestructibleObjectForm   destructible;
    BGSMessageIcon              messageIcon;
    BGSPickupPutdownSounds      pickupSounds;
    BGSKeywordForm              keywordForm;
    BGSFeaturedItemMessage      featured;

    struct Data {
        enum Flags {
            kType_None          = 0x00,
            kType_Skill         = 0x01,
            kType_CantBeTaken   = 0x02,
            kType_Spell         = 0x04,
            kType_HasBeenRead   = 0x08,
            kType_Perk          = 0x10,
        };

        UInt8   flags;
        UInt8   type;
        UInt16  unk00;

        union {
            ActorValueInfo  * skill;
            SpellItem       * spell;
            BGSPerk         * perk;
        } teaches;

        UInt32 unk01[2];
        UInt32 GetSanitizedType();
    };

    Data                        data;
    UInt64                      unk00[4];
};
STATIC_ASSERT(sizeof(TESObjectBOOK) == 0x1A8);

class TESWorldSpace: public TESForm {
public:
    enum { kTypeID = kFormType_WRLD };

    TESFullName                 fullName;
    TESTexture                  texture;

    UInt64                      unk40[87];
};
STATIC_ASSERT(sizeof(TESWorldSpace) == 0x2F8);