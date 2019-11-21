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

	void*	unk00;		// 00
	UInt32	unk08;		// 08
	void*	unk10;		// 10
	UInt64	unk18;		// 18
	UInt32	unk20;		// 20
	UInt32	unk24;		// 24
	UInt32	unk28;		// 28
	UInt32	unk2C;		// 2C
	UInt32	unk30;		// 30
	UInt16  unk34;		// 34
	UInt32  unk38;		// 38
	UInt64  unk40;		// 40
	TESForm	* sourceSpell;	// 48

	struct Data {
		float			unk00;		// 00
		UInt32			unk04;		// 04
		UInt32			unk08;		// 08
		EffectSetting	* setting;	// 10
		// ...
	};

	Data	* data;		// 50

	UInt64  unk58;		// 58
	TESForm	* sourceItem;	// 60
	UInt64	unk68;		// 68
	UInt64	unk70;		// 70
	float	elapsed;		// 78
	float	duration;		// 7C
	float	magnitude;		// 80
	UInt32	flags;		// 84
	UInt32	unk88;		// 88
	UInt16	unk8C;		// 8C
	UInt32	unk90;		// 90
	// ...
};
STATIC_ASSERT(offsetof(ActiveEffect, sourceSpell) == 0x48);
STATIC_ASSERT(offsetof(ActiveEffect, data) == 0x50);
STATIC_ASSERT(offsetof(ActiveEffect, sourceItem) == 0x60);
STATIC_ASSERT(offsetof(ActiveEffect::Data, setting) == 0x10);

class ExtraCharge : public BSExtraData {
public:
    float charge;
    UInt32 pad1C;

    static ExtraCharge* Create(float value);
};
STATIC_ASSERT(sizeof(ExtraCharge) == 0x20);

class ExtraObjectHealth : public BSExtraData {
public:
    float health;    // 18
    UInt32 pad1C;    // 1C

    static ExtraObjectHealth* Create(float value);
};
STATIC_ASSERT(sizeof(ExtraObjectHealth) == 0x20);

class BGSMessage : public TESForm {
public:
	enum { kTypeID = kFormType_MESG };

	TESFullName					fullName;
	TESDescription				description;
};

class BGSNote : public TESBoundObject {
public:
    enum { kTypeID = kFormType_NOTE };

    TESWeightForm               weight;
    TESValueForm                value;
    TESModel                    model;
    TESFullName                 fullName;
    TESIcon                     icon;
    BGSPickupPutdownSounds      pickupSounds;
};

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
			kType_None			= 0x00,
			kType_Skill			= 0x01,
			kType_CantBeTaken	= 0x02,
			kType_Spell			= 0x04,
			kType_HasBeenRead	= 0x08,
			kType_Perk			= 0x10,
		};

		UInt8	flags;
		UInt8	type;
		UInt16	unk02;

		union {
			ActorValueInfo	* Skill;
			SpellItem		* Spell;
			BGSPerk			* Perk;
		} teaches;

		UInt32 GetSanitizedType();
	};

	Data						data;
};