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
#include <mutex>

namespace juce
{

//==============================================================================
// Static member initialization
void* GlobalHotKey::PlatformSpecificData::keyboardHook = nullptr;
std::unordered_map<int, GlobalHotKey::PlatformSpecificData::HotKeyInfo> GlobalHotKey::PlatformSpecificData::registeredHotkeys;
std::mutex GlobalHotKey::PlatformSpecificData::registeredHotkeysMutex;
std::atomic<int> GlobalHotKey::PlatformSpecificData::nextHotkeyId {1};
std::atomic<int> GlobalHotKey::PlatformSpecificData::hookRefCount {0};

//==============================================================================
GlobalHotKey::PlatformSpecificData::PlatformSpecificData (GlobalHotKey& owner)
    : owner (owner)
{
    hotKeyInfo.owner = this;
    hotKeyInfo.id = -1;
    
    DBG ("GlobalHotKey: PlatformSpecificData created");
}

GlobalHotKey::PlatformSpecificData::~PlatformSpecificData()
{
    unregisterHotKey();
    DBG ("GlobalHotKey: PlatformSpecificData destroyed");
}

//==============================================================================
bool GlobalHotKey::PlatformSpecificData::registerHotKey (const KeyCode& keyCode, const ModifierKeys& modifiers)
{
    if (isRegistered)
    {
        DBG ("GlobalHotKey: Already registered");
        return false;
    }
    
    // Install hook if not already installed
    if (!installHook())
    {
        DBG ("GlobalHotKey: Failed to install keyboard hook");
        return false;
    }
    
    // Setup hotkey info
    hotKeyInfo.keyCode = keyCode;
    hotKeyInfo.modifiers = modifiers;
    hotKeyInfo.id = getNextHotkeyId();
    
    // Register the hotkey
    {
        std::lock_guard<std::mutex> lock (registeredHotkeysMutex);
        registeredHotkeys[hotKeyInfo.id] = hotKeyInfo;
    }
    
    isRegistered = true;
    
    DBG ("GlobalHotKey: Successfully registered hotkey with ID: " << hotKeyInfo.id
         << ", JUCE Key: " << keyCode.getJuceKeyCode()
         << ", Modifiers: " << modifiers.getRawFlags());
    
    return true;
}

void GlobalHotKey::PlatformSpecificData::unregisterHotKey()
{
    if (!isRegistered)
        return;
    
    // Remove from registered hotkeys
    {
        std::lock_guard<std::mutex> lock (registeredHotkeysMutex);
        registeredHotkeys.erase (hotKeyInfo.id);
    }
    
    isRegistered = false;
    
    // Uninstall hook if no more hotkeys are registered
    if (registeredHotkeys.empty())
    {
        uninstallHook();
    }
    
    DBG ("GlobalHotKey: Unregistered hotkey with ID: " << hotKeyInfo.id);
}

//==============================================================================
int GlobalHotKey::PlatformSpecificData::getNextHotkeyId()
{
    return nextHotkeyId++;
}

bool GlobalHotKey::PlatformSpecificData::installHook()
{
    std::lock_guard<std::mutex> lock (registeredHotkeysMutex);
    
    if (keyboardHook != nullptr)
    {
        ++hookRefCount;
        return true;  // Hook already installed
    }
    
    // Install low-level keyboard hook
    auto hook = SetWindowsHookEx (
        WH_KEYBOARD_LL,
        reinterpret_cast<HOOKPROC>(keyboardHookProc),
        GetModuleHandle (nullptr),
        0  // Global hook
    );
    
    if (hook != nullptr)
    {
        keyboardHook = hook;
        hookRefCount = 1;
        DBG ("GlobalHotKey: Keyboard hook installed successfully");
        return true;
    }
    else
    {
        auto error = GetLastError();
        DBG ("GlobalHotKey: Failed to install keyboard hook. Error: " << error);
        return false;
    }
}

void GlobalHotKey::PlatformSpecificData::uninstallHook()
{
    std::lock_guard<std::mutex> lock (registeredHotkeysMutex);
    
    if (keyboardHook == nullptr)
        return;
    
    --hookRefCount;
    
    if (hookRefCount <= 0)
    {
        if (UnhookWindowsHookEx (static_cast<HHOOK>(keyboardHook)))
        {
            DBG ("GlobalHotKey: Keyboard hook uninstalled successfully");
        }
        else
        {
            auto error = GetLastError();
            DBG ("GlobalHotKey: Failed to uninstall keyboard hook. Error: " << error);
        }
        
        keyboardHook = nullptr;
        hookRefCount = 0;
    }
}

//==============================================================================
long __stdcall GlobalHotKey::PlatformSpecificData::keyboardHookProc (int nCode, void* wParam, void* lParam)
{
    if (nCode >= 0)
    {
        auto message = static_cast<UINT>(reinterpret_cast<uintptr_t>(wParam));
        auto kbStruct = static_cast<KBDLLHOOKSTRUCT*>(lParam);
        
        // Only handle key down events
        if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN)
        {
            int vkCode = kbStruct->vkCode;
            auto currentModifiers = getCurrentModifiers();
            
            // Check all registered hotkeys
            std::lock_guard<std::mutex> lock (registeredHotkeysMutex);
            
            for (const auto& pair : registeredHotkeys)
            {
                const auto& hotkey = pair.second;
                
                if (matchesHotKey (hotkey, vkCode, currentModifiers))
                {
                    DBG ("GlobalHotKey: Hotkey matched! ID: " << hotkey.id 
                         << ", VK: " << vkCode 
                         << ", Modifiers: " << currentModifiers.getRawFlags());
                    
                    // Call the callback asynchronously on the JUCE message thread
                    MessageManager::callAsync ([hotkeyOwner = hotkey.owner]()
                    {
                        if (hotkeyOwner != nullptr)
                        {
                            DBG ("GlobalHotKey: Executing hotkey callback");
                            hotkeyOwner->handleHotkeyMessage();
                        }
                    });
                    
                    // Note: We don't return here - allow multiple hotkeys to match
                    // and don't consume the key event (let other apps receive it)
                }
            }
        }
    }
    
    // Always call next hook - don't consume the key event
    return CallNextHookEx (static_cast<HHOOK>(keyboardHook), nCode, 
                          reinterpret_cast<WPARAM>(wParam), reinterpret_cast<LPARAM>(lParam));
}

//==============================================================================
bool GlobalHotKey::PlatformSpecificData::matchesHotKey (const HotKeyInfo& hotkey, int vkCode, const ModifierKeys& currentModifiers)
{
    // Use KeyCode's built-in platform conversion method
    int expectedVkCode = hotkey.keyCode.getPlatformKeyCode();
    
    if (expectedVkCode == 0)
        return false;
    
    // Check if the key matches
    if (vkCode != expectedVkCode)
        return false;
    
    // Check if modifiers match
    return currentModifiers.getRawFlags() == hotkey.modifiers.getRawFlags();
}

ModifierKeys GlobalHotKey::PlatformSpecificData::getCurrentModifiers()
{
    int flags = 0;
    
    if (GetAsyncKeyState (VK_CONTROL) & 0x8000)
        flags |= ModifierKeys::ctrlModifier;
    
    if (GetAsyncKeyState (VK_SHIFT) & 0x8000)
        flags |= ModifierKeys::shiftModifier;
    
    if (GetAsyncKeyState (VK_MENU) & 0x8000)  // Alt key
        flags |= ModifierKeys::altModifier;
    
    if ((GetAsyncKeyState (VK_LWIN) & 0x8000) || (GetAsyncKeyState (VK_RWIN) & 0x8000))
        flags |= ModifierKeys::commandModifier;
    
    return ModifierKeys (flags);
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