/*
  ==============================================================================

   This file is part of the JUCE framework.
   Copyright (c) Raw Material Software Limited

   JUCE is an open source framework subject to commercial or open source
   licensing.

   By downloading, installing, or using the JUCE framework, or combining the
   JUCE framework with any other source code, object code, content or any other
   copyrightable work, you agree to the terms of the JUCE End User Licence
   Agreement, and all incorporated terms including the JUCE Privacy Policy and
   the JUCE Website Terms of Service, as applicable, which will bind you. If you
   do not agree to the terms of these agreements, we will not license the JUCE
   framework to you, and you must discontinue the installation or download
   process and cease use of the JUCE framework.

   JUCE End User Licence Agreement: https://juce.com/legal/juce-8-licence/
   JUCE Privacy Policy: https://juce.com/juce-privacy-policy
   JUCE Website Terms of Service: https://juce.com/juce-website-terms-of-service/

   Or:

   You may also use this code under the terms of the AGPLv3:
   https://www.gnu.org/licenses/agpl-3.0.en.html

   THE JUCE FRAMEWORK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL
   WARRANTIES, WHETHER EXPRESSED OR IMPLIED, INCLUDING WARRANTY OF
   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

#if JUCE_WINDOWS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
namespace juce
{

//==============================================================================
// Static member initialization
std::unordered_map<int, GlobalHotKey::PlatformSpecificData*> GlobalHotKey::PlatformSpecificData::registeredHotkeys;
std::atomic<int> GlobalHotKey::PlatformSpecificData::nextHotkeyId {1};

//==============================================================================
GlobalHotKey::PlatformSpecificData::PlatformSpecificData (GlobalHotKey& owner)
    : owner (owner)
{
    // Create a message-only window to receive WM_HOTKEY messages
    static bool windowClassRegistered = false;
    
    if (!windowClassRegistered)
    {
        WNDCLASSW wc = {};
        wc.lpfnWndProc = windowProc;
        wc.hInstance = GetModuleHandle (nullptr);
        wc.lpszClassName = L"JuceGlobalHotKeyWindow";
        
        if (RegisterClassW (&wc) != 0)
            windowClassRegistered = true;
    }
    
    if (windowClassRegistered)
    {
        messageWindow = CreateWindowW (
            L"JuceGlobalHotKeyWindow",
            L"JUCE Global HotKey",
            0, 0, 0, 0, 0,
            HWND_MESSAGE,  // Message-only window
            nullptr,
            GetModuleHandle (nullptr),
            nullptr
        );
        
        if (messageWindow != nullptr)
            SetWindowLongPtr (messageWindow, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    }
}

GlobalHotKey::PlatformSpecificData::~PlatformSpecificData()
{
    unregisterHotKey();
    
    if (messageWindow != nullptr)
    {
        DestroyWindow (messageWindow);
        messageWindow = nullptr;
    }
}

//==============================================================================
bool GlobalHotKey::PlatformSpecificData::registerHotKey (const KeyCode& keyCode, const ModifierKeys& modifiers)
{
    if (isRegistered || messageWindow == nullptr)
        return false;
    
    hotkeyId = getNextHotkeyId();
    
    const auto win32Key = convertKeyCodeToWin32 (keyCode);
    const auto win32Modifiers = convertModifiersToWin32 (modifiers);
    
    if (win32Key == 0)  // Invalid key code
        return false;
    
    if (RegisterHotKey (messageWindow, hotkeyId, win32Modifiers, win32Key))
    {
        registeredHotkeys[hotkeyId] = this;
        isRegistered = true;
        return true;
    }
    
    // If registration failed, common reasons:
    // ERROR_HOTKEY_ALREADY_REGISTERED (1409) - hotkey already in use
    // ERROR_HOTKEY_NOT_REGISTERED (1419) - invalid combination
    const auto error = GetLastError();
    (void) error;  // Suppress unused variable warning in release builds
    
    return false;
}

void GlobalHotKey::PlatformSpecificData::unregisterHotKey()
{
    if (isRegistered && messageWindow != nullptr)
    {
        UnregisterHotKey (messageWindow, hotkeyId);
        registeredHotkeys.erase (hotkeyId);
        isRegistered = false;
    }
}

//==============================================================================
int GlobalHotKey::PlatformSpecificData::getNextHotkeyId()
{
    return nextHotkeyId++;
}

UINT GlobalHotKey::PlatformSpecificData::convertKeyCodeToWin32 (const KeyCode& keyCode)
{
    const auto juceKey = keyCode.getJuceKeyCode();
    
    // Convert common JUCE key codes to Windows virtual key codes
    if (juceKey >= 'A' && juceKey <= 'Z')
        return juceKey;
    if (juceKey >= '0' && juceKey <= '9')
        return juceKey;
    
    // Function keys (using our own constants)
    if (juceKey >= 0x20001 && juceKey <= 0x2000c)  // F1Key to F12Key
        return VK_F1 + (juceKey - 0x20001);
    
    // Punctuation and special characters
    switch (juceKey)
    {
        // Punctuation marks (Windows VK codes)
        case '[':        return VK_OEM_4;      // Left bracket
        case ']':        return VK_OEM_6;      // Right bracket
        case ';':        return VK_OEM_1;      // Semicolon
        case '\'':       return VK_OEM_7;      // Quote/apostrophe
        case ',':        return VK_OEM_COMMA;  // Comma
        case '.':        return VK_OEM_PERIOD; // Period
        case '/':        return VK_OEM_2;      // Forward slash
        case '\\':       return VK_OEM_5;      // Backslash
        case '`':        return VK_OEM_3;      // Grave accent/backtick
        case '-':        return VK_OEM_MINUS;  // Minus
        case '=':        return VK_OEM_PLUS;   // Plus/equals
        
        // Keypad numbers (different from main number keys)
        case 0x60000:    return VK_NUMPAD0;    // Numpad 0
        case 0x60001:    return VK_NUMPAD1;    // Numpad 1
        case 0x60002:    return VK_NUMPAD2;    // Numpad 2
        case 0x60003:    return VK_NUMPAD3;    // Numpad 3
        case 0x60004:    return VK_NUMPAD4;    // Numpad 4
        case 0x60005:    return VK_NUMPAD5;    // Numpad 5
        case 0x60006:    return VK_NUMPAD6;    // Numpad 6
        case 0x60007:    return VK_NUMPAD7;    // Numpad 7
        case 0x60008:    return VK_NUMPAD8;    // Numpad 8
        case 0x60009:    return VK_NUMPAD9;    // Numpad 9
        
        // Keypad operators
        case 0x70000:    return VK_MULTIPLY;   // Numpad *
        case 0x70001:    return VK_ADD;        // Numpad +
        case 0x70002:    return VK_SUBTRACT;   // Numpad -
        case 0x70003:    return VK_DIVIDE;     // Numpad /
        case 0x70004:    return VK_DECIMAL;    // Numpad .
        
        // Special keys
        case ' ':        return VK_SPACE;      // spaceKey
        case 0x1000d:    return VK_RETURN;     // returnKey
        case 0x1001b:    return VK_ESCAPE;     // escapeKey
        case 0x10008:    return VK_BACK;       // backspaceKey
        case 0x1007f:    return VK_DELETE;     // deleteKey
        case 0x10009:    return VK_TAB;        // tabKey
        case 0x10012:    return VK_LEFT;       // leftKey
        case 0x10014:    return VK_RIGHT;      // rightKey
        case 0x10013:    return VK_UP;         // upKey
        case 0x10015:    return VK_DOWN;       // downKey
        case 0x10010:    return VK_HOME;       // homeKey
        case 0x10011:    return VK_END;        // endKey
        case 0x10016:    return VK_PRIOR;      // pageUpKey
        case 0x10017:    return VK_NEXT;       // pageDownKey
        case 0x10019:    return VK_INSERT;     // insertKey
        default:         return 0;
    }
}

UINT GlobalHotKey::PlatformSpecificData::convertModifiersToWin32 (const ModifierKeys& modifiers)
{
    UINT win32Modifiers = MOD_NOREPEAT;  // Prevent repeated WM_HOTKEY messages
    
    if (modifiers.isCtrlDown())
        win32Modifiers |= MOD_CONTROL;
    if (modifiers.isShiftDown())
        win32Modifiers |= MOD_SHIFT;
    if (modifiers.isAltDown())
        win32Modifiers |= MOD_ALT;
    if (modifiers.isCommandDown())
        win32Modifiers |= MOD_WIN;
    
    return win32Modifiers;
}

//==============================================================================
LRESULT CALLBACK GlobalHotKey::PlatformSpecificData::windowProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_HOTKEY)
    {
        const auto hotkeyId = static_cast<int>(wParam);
        
        // Thread-safe lookup
        const auto it = registeredHotkeys.find (hotkeyId);
        
        if (it != registeredHotkeys.end() && it->second != nullptr)
        {
            // Capture the pointer for safe async call
            auto* hotkey = it->second;
            
            // Forward the hotkey event to the JUCE message thread
            MessageManager::callAsync ([hotkey]()
            {
                // Double-check the hotkey is still valid
                if (hotkey != nullptr)
                    hotkey->handleHotkeyMessage();
            });
        }
        
        return 0;
    }
    
    return DefWindowProc (hwnd, msg, wParam, lParam);
}

void GlobalHotKey::PlatformSpecificData::handleHotkeyMessage()
{
    owner.triggerCallback();
}

//==============================================================================
// Windows doesn't require permission dialogs for global hotkeys
bool GlobalHotKey::showPermissionDialog()
{
    return false; // No permissions needed on Windows
}

} // namespace juce

#endif // JUCE_WINDOWS 