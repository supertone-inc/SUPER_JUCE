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

namespace juce
{

//==============================================================================
/**
    Windows-specific implementation of global hotkeys using RegisterHotKey API.
    
    This implementation uses the Windows RegisterHotKey API to register global
    system hotkeys that can be triggered even when the application is in the
    background. The implementation handles WM_HOTKEY messages and forwards
    them to the JUCE message thread.
    
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
    HWND messageWindow = nullptr;
    int hotkeyId = 0;
    bool isRegistered = false;
    
    static int getNextHotkeyId();
    static UINT convertKeyCodeToWin32 (const KeyCode& keyCode);
    static UINT convertModifiersToWin32 (const ModifierKeys& modifiers);
    
    static LRESULT CALLBACK windowProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void handleHotkeyMessage();
    
    static std::unordered_map<int, PlatformSpecificData*> registeredHotkeys;
    static std::atomic<int> nextHotkeyId;
};

} // namespace juce

#endif // JUCE_WINDOWS 