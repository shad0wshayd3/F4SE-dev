#include "InputHandler.h"

void ProcessUserEvent(const char* menuName, const char* control, bool down) {
    BSFixedString MenuName(menuName);

    IMenu* Menu = (*g_ui)->GetMenu(MenuName);
    if (Menu) {
        GFxValue args[2];
        args[0] = GFxValue(control);
        args[1] = GFxValue(down);

        Menu->movie->movieRoot->Invoke("root.Menu_mc.ProcessUserEvent", nullptr, args, 2);
    }
}

class LevelUpMenu_Input: public BSInputEventUser {
public:
    LevelUpMenu_Input(): BSInputEventUser(true) {}
    virtual void OnButtonEvent(ButtonEvent* evn) {
        std::string control;

        float timer = evn->timer;
        bool keyDown = evn->isDown == 1.0f && timer == 0.0f;
        bool keyUp = evn->isDown == 0.0f && timer != 0.0f;

        switch (evn->deviceType) {
        case InputEvent::kDeviceType_Keyboard:
            switch (evn->keyMask) {
            case kScanCode_TAB:
            case kScanCode_ESCAPE:
                control = "Cancel";
                break;

            case kScanCode_T:
                control = "Reset";
                break;

            case kScanCode_X:
                control = "Confirm";
                break;

            default:
                return;
            }
            break;

        case InputEvent::kDeviceType_Gamepad:
            switch (evn->keyMask) {
            case kScanCode_GP_B:
                control = "Cancel";
                break;

            case kScanCode_GP_Y:
                control = "Reset";
                break;

            case kScanCode_GP_X:
                control = "Confirm";
                break;

            case kScanCode_GP_RTH:
                control = "Search";
                break;

            default:
                return;
            }
            break;

        default:
            return;
        }

        if (keyDown)
            ProcessUserEvent("LevelUpMenu", control.c_str(), true);
        else if (keyUp)
            ProcessUserEvent("LevelUpMenu", control.c_str(), false);
    }
};
LevelUpMenu_Input g_LevelUpMenuHandler;

class LockpickingMenu_Input: public BSInputEventUser {
public:
    LockpickingMenu_Input(): BSInputEventUser(true) {}
    virtual void OnButtonEvent(ButtonEvent* evn) {
        std::string control;

        float timer = evn->timer;
        bool keyDown = evn->isDown == 1.0f && timer == 0.0f;
        bool keyUp = evn->isDown == 0.0f && timer != 0.0f;

        switch (evn->deviceType) {
        case InputEvent::kDeviceType_Keyboard:
            switch (evn->keyMask) {
            case kScanCode_F:
                control = "Force";
                break;

            default:
                return;
            }
            break;

        case InputEvent::kDeviceType_Gamepad:
            switch (evn->keyMask) {
            case kScanCode_GP_X:
                control = "Force";
                break;

            default:
                return;
            }
            break;

        default:
            return;
        }

        if (keyDown)
            ProcessUserEvent("LockpickingMenu", control.c_str(), true);
        else if (keyUp)
            ProcessUserEvent("LockpickingMenu", control.c_str(), false);
    }
};
LockpickingMenu_Input g_LockpickingMenuHandler;

class PipboyMenu_Input: public BSInputEventUser {
public:
    PipboyMenu_Input(): BSInputEventUser(true) {}
    virtual void OnButtonEvent(ButtonEvent* evn) {
        std::string control;

        float timer = evn->timer;
        bool keyDown = evn->isDown == 1.0f && timer == 0.0f;
        bool keyUp = evn->isDown == 0.0f && timer != 0.0f;

        switch (evn->deviceType) {
        case InputEvent::kDeviceType_Keyboard:
            switch (evn->keyMask) {
            case kScanCode_F:
                control = "Repair";
                break;

            default:
                return;
            }
            break;

        case InputEvent::kDeviceType_Gamepad:
            switch (evn->keyMask) {
            case kScanCode_GP_X:
                control = "Repair";
                break;

            default:
                return;
            }
            break;

        default:
            return;
        }

        if (keyDown)
            ProcessUserEvent("PipboyMenu", control.c_str(), true);
        else if (keyUp)
            ProcessUserEvent("PipboyMenu", control.c_str(), false);
    }
};
PipboyMenu_Input g_PipboyMenuHandler;

class RepairMenu_Input: public BSInputEventUser {
public:
    RepairMenu_Input(): BSInputEventUser(true) {}
    virtual void OnButtonEvent(ButtonEvent* evn) {
        std::string control;

        float timer = evn->timer;
        bool keyDown = evn->isDown == 1.0f && timer == 0.0f;
        bool keyUp = evn->isDown == 0.0f && timer != 0.0f;

        switch (evn->deviceType) {
        case InputEvent::kDeviceType_Keyboard:
            switch (evn->keyMask) {
            case kScanCode_TAB:
            case kScanCode_ESCAPE:
                control = "Cancel";
                break;

            default:
                return;
            }
            break;

        case InputEvent::kDeviceType_Gamepad:
            switch (evn->keyMask) {
            case kScanCode_GP_B:
                control = "Cancel";
                break;

            default:
                return;
            }
            break;

        default:
            return;
        }

        if (keyDown)
            ProcessUserEvent("RepairMenu", control.c_str(), true);
        else if (keyUp)
            ProcessUserEvent("RepairMenu", control.c_str(), false);
    }
};
RepairMenu_Input g_RepairMenuHandler;

void RegisterLevelUpMenuInput(bool enable) {
    g_LevelUpMenuHandler.enabled = enable;

    if (enable) {
        tArray<BSInputEventUser*>* inputEvents = &((*g_menuControls)->inputEvents);
        BSInputEventUser* inputHandlerPtr = &g_LevelUpMenuHandler;

        if (inputEvents->GetItemIndex(inputHandlerPtr) == -1)
            inputEvents->Push(inputHandlerPtr);
    }
}

void RegisterLockpickingMenuInput(bool enable) {
    g_LockpickingMenuHandler.enabled = enable;

    if (enable) {
        tArray<BSInputEventUser*>* inputEvents = &((*g_menuControls)->inputEvents);
        BSInputEventUser* inputHandlerPtr = &g_LockpickingMenuHandler;

        if (inputEvents->GetItemIndex(inputHandlerPtr) == -1)
            inputEvents->Push(inputHandlerPtr);
    }
}

void RegisterPipboyMenuInput(bool enable) {
    g_PipboyMenuHandler.enabled = enable;

    if (enable) {
        tArray<BSInputEventUser*>* inputEvents = &((*g_menuControls)->inputEvents);
        BSInputEventUser* inputHandlerPtr = &g_PipboyMenuHandler;

        if (inputEvents->GetItemIndex(inputHandlerPtr) == -1)
            inputEvents->Push(inputHandlerPtr);
    }
}

void RegisterRepairMenuInput(bool enable) {
    g_RepairMenuHandler.enabled = enable;

    if (enable) {
        tArray<BSInputEventUser*>* inputEvents = &((*g_menuControls)->inputEvents);
        BSInputEventUser* inputHandlerPtr = &g_RepairMenuHandler;

        if (inputEvents->GetItemIndex(inputHandlerPtr) == -1)
            inputEvents->Push(inputHandlerPtr);
    }
}