#include "Native.h"

#define XBYAK_NO_OP_NAMES 1
#include "f4se_common/BranchTrampoline.h"
#include "xbyak/xbyak.h"

// ------------------------------------------------------------------------------------------------
// addresses
// ------------------------------------------------------------------------------------------------

RelocAddr   <_DoTokenReplacement>           DoTokenReplacement                  (0x000C0C70);   // 'welcomeText'                                                            | if (qword_) if (sub(qword))
RelocAddr   <_GetItemByHandleID>            GetItemByHandleID_Internal          (0x001A3650);
RelocAddr   <_GetFormByHandleID>            GetFormByHandleID_Internal          (0x001A3740);
RelocAddr   <_ExtractArgs>                  ExtractArgs                         (0x004E36B0);   // literally every ObScriptCommand with arguments
RelocAddr   <_CalculateDamageResist>        CalculateDamageResist               (0x0065FAC0);   // 'fPhysicalDamageBase'                                                    | dword xref xref
RelocAddr   <_GetXPForLevel>                GetXPForLevel                       (0x00664A10);
RelocAddr	<_ResetFaceMorph>				ResetFaceMorph						(0x00668660);
RelocAddr   <_Notification>                 Notification_internal               (0x00AE1D70);
RelocAddr   <_PopulateItemCard>             PopulateItemCard                    (0x00AED710);
RelocAddr   <_ContainerMenuInvoke>          ContainerMenuInvoke                 (0x00B0A280);   // const BarterMenu::`vftable'                                              | xref4, sub_2
RelocAddr   <_ExamineMenuInvoke>            ExamineMenuInvoke                   (0x00B18090);
RelocAddr   <_LevelUpPrompt>                LevelUpPrompt                       (0x00B3A5A0);   // 'UILevelUpText'                                                          | sub_2
RelocAddr   <_PipboyMenuInvoke>             PipboyMenuInvoke                    (0x00B93F60);   // const PipboyMenu::`vftable'                                              | xref4, sub_2
RelocAddr	<_IsInAir>						IsInAir								(0x00D72810);
RelocAddr   <_AddPerk>                      AddPerk                             (0x00DA60E0);
RelocAddr   <_RemovePerk>                   RemovePerk                          (0x00DA61F0);
RelocAddr   <_HasPerk>                      HasPerk                             (0x00DA64E0);
RelocAddr   <_EquipItem>                    EquipItem_Internal                  (0x00E1BBB0);
RelocAddr   <_UnequipItem>                  UnequipItem_Internal                (0x00E1BF90);
RelocAddr   <_PlayIdle>                     PlayIdle_Internal                   (0x00E353F0);
RelocAddr	<_ShowWaitMenu>					ShowWaitMenu						(0x00E9C0C0);
RelocAddr   <_GetItemCount>                 GetItemCount                        (0x013FB5E0);
RelocAddr   <_ExtraChargeVtbl>              ExtraChargeVtbl                     (0x02C52218);
RelocAddr   <_ExtraObjectHealthVtbl>        ExtraObjectHealthVtbl               (0x02C524B8);
RelocAddr   <_ActorValueDerivedVtbl>        ActorValueDerivedVtbl               (0x02CEDBC8);   // const std::_Func_impl<std::_Callable_fun<float (*const)(ActorValueOwner  | `vftable'
RelocAddr   <_ActorValueCalcVtbl>           ActorValueCalcVtbl                  (0x02CEDC08);   // const std::_Func_impl<std::_Callable_fun<void (*const)(Actor             | `vftable'
RelocPtr    <PipboyDataManager*>            g_PipboyDataManager                 (0x058D0AF0);   // const BSTEventSink<RadioManager::PipboyRadioTuningEvent>::`vftable'      | xref1, ret qword
RelocPtr    <InventoryInterface*>           g_InventoryInterface                (0x058D4980);   // const DropItemCommand::`vftable'                                         | sub_3, mid qword
RelocPtr    <_EquipManager>                 g_EquipManager                      (0x059D7598);
RelocAddr   <_ObScript_Parse>               ObScript_Parse                      (0x404E7CE0);   // literally every ObScriptCommand

// ------------------------------------------------------------------------------------------------
// originals
// ------------------------------------------------------------------------------------------------

_ContainerMenuInvoke ContainerMenuInvoke_Original;
void HookContainerMenuInvoke(void(*hookFunc)(ContainerMenuBase*, GFxFunctionHandler::Args*)) {
    struct ContainerMenuInvoke_Code : Xbyak::CodeGenerator {
        ContainerMenuInvoke_Code(void* buf) : Xbyak::CodeGenerator(4096, buf) {
            Xbyak::Label retnLabel;

            mov(r11, rsp);
            mov(ptr[r11 + 0x18], rbx);

            jmp(ptr[rip + retnLabel]);

            L(retnLabel);
            dq(ContainerMenuInvoke.GetUIntPtr() + 7);
        }
    };

    void* codeBuf = g_localTrampoline.StartAlloc();
    ContainerMenuInvoke_Code code(codeBuf);
    g_localTrampoline.EndAlloc(code.getCurr());

    ContainerMenuInvoke_Original = (_ContainerMenuInvoke)codeBuf;
    g_branchTrampoline.Write6Branch(ContainerMenuInvoke.GetUIntPtr(), (uintptr_t)hookFunc);
}

_ExamineMenuInvoke ExamineMenuInvoke_Original;
void HookExamineMenuInvoke(void(*hookFunc)(ExamineMenu*, GFxFunctionHandler::Args*)) {
    struct ExamineMenuInvoke_Code : Xbyak::CodeGenerator {
        ExamineMenuInvoke_Code(void* buf) : Xbyak::CodeGenerator(4096, buf) {
            Xbyak::Label retnLabel;

            mov(rax, rsp);
            mov(ptr[rax + 0x08], rbx);

            jmp(ptr[rip + retnLabel]);

            L(retnLabel);
            dq(ExamineMenuInvoke.GetUIntPtr() + 7);
        }
    };

    void* codeBuf = g_localTrampoline.StartAlloc();
    ExamineMenuInvoke_Code code(codeBuf);
    g_localTrampoline.EndAlloc(code.getCurr());

    ExamineMenuInvoke_Original = (_ExamineMenuInvoke)codeBuf;
    g_branchTrampoline.Write6Branch(ExamineMenuInvoke.GetUIntPtr(), (uintptr_t)hookFunc);
}

_PipboyMenuInvoke PipboyMenuInvoke_Original;
void HookPipboyMenuInvoke(void(*hookFunc)(PipboyMenu*, GFxFunctionHandler::Args*)) {
    struct PipboyMenuInvoke_Code : Xbyak::CodeGenerator {
        PipboyMenuInvoke_Code(void* buf) : Xbyak::CodeGenerator(4096, buf) {
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
    PipboyMenuInvoke_Code code(codeBuf);
    g_localTrampoline.EndAlloc(code.getCurr());

    PipboyMenuInvoke_Original = (_PipboyMenuInvoke)codeBuf;
    g_branchTrampoline.Write5Branch(PipboyMenuInvoke.GetUIntPtr(), (uintptr_t)hookFunc);
}

_PopulateItemCard PopulateItemCard_Original;
void HookPopulateItemCard(void(*hookFunc)(GFxValue*, BGSInventoryItem*, UInt16, InvItemStackList)) {
    struct PopulateItemCard_Code : Xbyak::CodeGenerator {
        PopulateItemCard_Code(void* buf) : Xbyak::CodeGenerator(4096, buf) {
            Xbyak::Label retnLabel;

            mov(rax, rsp);
            mov(ptr[rax + 0x20], r9);

            jmp(ptr[rip + retnLabel]);

            L(retnLabel);
            dq(PopulateItemCard.GetUIntPtr() + 7);
        }
    };

    void* codeBuf = g_localTrampoline.StartAlloc();
    PopulateItemCard_Code code(codeBuf);
    g_localTrampoline.EndAlloc(code.getCurr());

    PopulateItemCard_Original = (_PopulateItemCard)codeBuf;
    g_branchTrampoline.Write6Branch(PopulateItemCard.GetUIntPtr(), (uintptr_t)hookFunc);
}