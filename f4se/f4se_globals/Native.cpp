#include "Native.h"

#include "f4se_common/BranchTrampoline.h"

#define XBYAK_NO_OP_NAMES 1
#include "xbyak/xbyak.h"

// ------------------------------------------------------------------------------------------------
// addresses
// ------------------------------------------------------------------------------------------------

RelocAddr   <_BGSInventoryItemCompareData>  CheckStackIDFunctorCompare      (0x001A7F80);
RelocAddr   <_BGSInventoryItemWriteImpl>    ModifyModDataFunctorWriteImpl   (0x001A8280);
RelocAddr   <_BGSInventoryItemWriteImpl>    SetHealthFunctorWriteImpl       (0x001A8160);
RelocAddr   <_BGSInventoryItemWriteImpl>    WriteTextExtraWriteImpl         (0x001A81B0);
RelocAddr   <_SetInventoryItemStackData>    SetInventoryItemStackData       (0x003FB430);

RelocAddr   <_GetHardcodedAV>               GetHardcodedAV                  (0x0006B1F0);
RelocAddr   <_DoTokenReplacement>           DoTokenReplacement              (0x000C0C70);
RelocAddr   <_GetItemByHandleID>            GetItemByHandleID_Internal      (0x001A3650);
RelocAddr   <_GetFormByHandleID>            GetFormByHandleID_Internal      (0x001A3740);
RelocAddr   <_BGSObjectInstanceCtor>        BGSObjectInstanceCtor           (0x002F7B50);
RelocAddr   <_ExtractArgs>                  ExtractArgs                     (0x004E37D0);
RelocAddr   <_CalculateDamageResist>        CalculateDamageResist           (0x0065FBE0);
RelocAddr   <_GetXPForLevel>                GetXPForLevel                   (0x00664B30);
RelocAddr   <_Notification>                 Notification_internal           (0x00AE1E90);
RelocAddr   <_PopulateItemCard>             PopulateItemCard                (0x00AED830);
RelocAddr   <_ContainerMenuInvoke>          ContainerMenuInvoke             (0x00B0A3A0);
RelocAddr   <_ExamineMenuInvoke>            ExamineMenuInvoke               (0x00B181B0);
RelocAddr   <_LevelUpPrompt>                LevelUpPrompt                   (0x00B3A6C0);
RelocAddr   <_PipboyMenuInvoke>             PipboyMenuInvoke                (0x00B94080);
RelocAddr   <_CalcSpellProperties>          CalcSpellProperties             (0x00C4E860);
RelocAddr   <_IsInAir>                      IsInAir                         (0x00D72930);
RelocAddr   <_CalcWeight>                   CalcWeight                      (0x00D8E930);
RelocAddr   <_AddPerk>                      AddPerk                         (0x00DA6200);
RelocAddr   <_RemovePerk>                   RemovePerk                      (0x00DA6310);
RelocAddr   <_HasPerk>                      HasPerk                         (0x00DA6600);
RelocAddr   <_AddSpell>                     AddSpell                        (0x00E10890);
RelocAddr   <_RemoveSpell>                  RemoveSpell                     (0x00E10A60);
RelocAddr   <_HasSpell>                     HasSpell                        (0x00E146B0);
RelocAddr   <_UpdateActor>                  UpdateActor                     (0x00E147E0);
RelocAddr   <_EquipItem>                    EquipItem_Internal              (0x00E1BCD0);
RelocAddr   <_UnequipItem>                  UnequipItem_Internal            (0x00E1C0B0);
RelocAddr   <_PlayIdle>                     PlayIdle_Internal               (0x00E35510);
RelocAddr   <_ShowWaitMenu>                 ShowWaitMenu                    (0x00E9C1E0);
RelocAddr   <_GetItemCount>                 GetItemCount                    (0x013FB700);
RelocAddr   <_GetCurrentGameTime>           GetCurrentGameTime              (0x014540D0);
RelocAddr   <uintptr_t>                     ExtraObjectInstanceVtbl         (0x02C4BE10);
RelocAddr   <uintptr_t>                     ExtraChargeVtbl                 (0x02C52228);
RelocAddr   <uintptr_t>                     ExtraObjectHealthVtbl           (0x02C524C8);
RelocAddr   <uintptr_t>                     ActorValueDerivedVtbl           (0x02CEDC58);
RelocAddr   <uintptr_t>                     ActorValueCalcVtbl              (0x02CEDC98);
RelocPtr    <PipboyDataManager*>            g_PipboyDataManager             (0x058D0AF0);
RelocPtr    <InventoryInterface*>           g_InventoryInterface            (0x058D4980);
RelocPtr    <void*>                         g_EquipManager                  (0x059D75C8);

// ------------------------------------------------------------------------------------------------
// originals
// ------------------------------------------------------------------------------------------------

_ContainerMenuInvoke ContainerMenuInvoke_Original;
void HookContainerMenuInvoke(void (*hookFunc)(ContainerMenuBase*, GFxFunctionHandler::Args*)) {
    struct HookCode: Xbyak::CodeGenerator {
        HookCode(void* buf): Xbyak::CodeGenerator(4096, buf) {
            Xbyak::Label retnLabel;

            mov(r11, rsp);
            mov(ptr[r11 + 0x18], rbx);

            jmp(ptr[rip + retnLabel]);

            L(retnLabel);
            dq(ContainerMenuInvoke.GetUIntPtr() + 7);
        }
    };

    void* codeBuf = g_localTrampoline.StartAlloc();
    HookCode code(codeBuf);
    g_localTrampoline.EndAlloc(code.getCurr());

    ContainerMenuInvoke_Original = (_ContainerMenuInvoke)codeBuf;
    g_branchTrampoline.Write6Branch(ContainerMenuInvoke.GetUIntPtr(), (uintptr_t)hookFunc);
}

_ExamineMenuInvoke ExamineMenuInvoke_Original;
void HookExamineMenuInvoke(void (*hookFunc)(ExamineMenu*, GFxFunctionHandler::Args*)) {
    struct HookCode: Xbyak::CodeGenerator {
        HookCode(void* buf): Xbyak::CodeGenerator(4096, buf) {
            Xbyak::Label retnLabel;

            mov(rax, rsp);
            mov(ptr[rax + 0x08], rbx);

            jmp(ptr[rip + retnLabel]);

            L(retnLabel);
            dq(ExamineMenuInvoke.GetUIntPtr() + 7);
        }
    };

    void* codeBuf = g_localTrampoline.StartAlloc();
    HookCode code(codeBuf);
    g_localTrampoline.EndAlloc(code.getCurr());

    ExamineMenuInvoke_Original = (_ExamineMenuInvoke)codeBuf;
    g_branchTrampoline.Write6Branch(ExamineMenuInvoke.GetUIntPtr(), (uintptr_t)hookFunc);
}

_PipboyMenuInvoke PipboyMenuInvoke_Original;
void HookPipboyMenuInvoke(void (*hookFunc)(PipboyMenu*, GFxFunctionHandler::Args*)) {
    struct HookCode: Xbyak::CodeGenerator {
        HookCode(void* buf): Xbyak::CodeGenerator(4096, buf) {
            Xbyak::Label retnLabel;

            mov(r11, rsp);
            push(rbp);
            push(rbx);
            jmp(ptr[rip + retnLabel]);

            L(retnLabel);
            dq(PipboyMenuInvoke.GetUIntPtr() + 5);
        }
    };

    void* codeBuf = g_localTrampoline.StartAlloc();
    HookCode code(codeBuf);
    g_localTrampoline.EndAlloc(code.getCurr());

    PipboyMenuInvoke_Original = (_PipboyMenuInvoke)codeBuf;
    g_branchTrampoline.Write5Branch(PipboyMenuInvoke.GetUIntPtr(), (uintptr_t)hookFunc);
}

_PopulateItemCard PopulateItemCard_Original;
void HookPopulateItemCard(void (*hookFunc)(GFxValue*, BGSInventoryItem*, UInt16, InvItemStackList)) {
    struct HookCode: Xbyak::CodeGenerator {
        HookCode(void* buf): Xbyak::CodeGenerator(4096, buf) {
            Xbyak::Label retnLabel;

            mov(rax, rsp);
            mov(ptr[rax + 0x20], r9);

            jmp(ptr[rip + retnLabel]);

            L(retnLabel);
            dq(PopulateItemCard.GetUIntPtr() + 7);
        }
    };

    void* codeBuf = g_localTrampoline.StartAlloc();
    HookCode code(codeBuf);
    g_localTrampoline.EndAlloc(code.getCurr());

    PopulateItemCard_Original = (_PopulateItemCard)codeBuf;
    g_branchTrampoline.Write6Branch(PopulateItemCard.GetUIntPtr(), (uintptr_t)hookFunc);
}

_UpdateActor UpdateActor_Original;
void HookUpdateActor(void (*hookFunc)(Actor*, float)) {
    struct HookCode: Xbyak::CodeGenerator {
        HookCode(void* buf): Xbyak::CodeGenerator(4096, buf) {
            Xbyak::Label retnLabel;

            push(rdi);
            sub(rsp, 0x60);

            jmp(ptr[rip + retnLabel]);

            L(retnLabel);
            dq(UpdateActor.GetUIntPtr() + 6);
        }
    };

    void* codeBuf = g_localTrampoline.StartAlloc();
    HookCode code(codeBuf);
    g_localTrampoline.EndAlloc(code.getCurr());

    UpdateActor_Original = (_UpdateActor)codeBuf;
    g_branchTrampoline.Write6Branch(UpdateActor.GetUIntPtr(), (uintptr_t)hookFunc);
}
