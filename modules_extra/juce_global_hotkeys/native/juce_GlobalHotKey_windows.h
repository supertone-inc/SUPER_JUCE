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

#include <unordered_map>
#include <atomic>
#include <mutex>

namespace juce
{

//==============================================================================
/**
    Windows-specific implementation of global hotkeys using Low-Level Keyboard Hook.
    
    This implementation uses SetWindowsHookEx with WH_KEYBOARD_LL to monitor
    keyboard events globally. Unlike RegisterHotKey, this approach allows
    multiple applications to monitor the same key combinations without
    interfering with each other.
    
    @tags{GlobalHotKeys}
*/
struct GlobalHotKey::PlatformSpecificData
{
    PlatformSpecificData (GlobalHotKey& owner);
    ~PlatformSpecificData();
    
    bool registerHotKey (const KeyCode& keyCode, const ModifierKeys& modifiers);
    void unregisterHotKey();
    
    static bool isSupported() { return true; }
    static bool requestPermissions() { return true; }
    static bool hasPermissions() { return true; }
    
private:
    GlobalHotKey& owner;
    
    // Hook-based implementation
    struct HotKeyInfo
    {
        KeyCode keyCode;
        ModifierKeys modifiers;
        PlatformSpecificData* owner;
        int id;
    };
    
    HotKeyInfo hotKeyInfo;
    bool isRegistered = false;
    
    // Static hook management
    static void* keyboardHook;  // HHOOK as void*
    static std::unordered_map<int, HotKeyInfo> registeredHotkeys;
    static std::mutex registeredHotkeysMutex;
    static std::atomic<int> nextHotkeyId;
    static std::atomic<int> hookRefCount;
    
    static int getNextHotkeyId();
    static bool installHook();
    static void uninstallHook();
    
    // Hook procedure
    static long __stdcall keyboardHookProc(int nCode, void* wParam, void* lParam);
    
    // Key matching
    static bool matchesHotKey(const HotKeyInfo& hotkey, int vkCode, const ModifierKeys& currentModifiers);
    static ModifierKeys getCurrentModifiers();
    
    void handleHotkeyMessage();
};

} // namespace juce

#endif // JUCE_WINDOWS 