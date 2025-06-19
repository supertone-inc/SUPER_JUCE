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

#if JUCE_MAC

namespace juce
{

//==============================================================================
/**
    macOS-specific implementation of global hotkeys using both Carbon RegisterEventHotKey
    and CGEventTap APIs.
    
    This implementation provides two strategies:
    1. Carbon RegisterEventHotKey API - simpler, requires fewer permissions, but has
       limitations on modern macOS versions (especially macOS Sequoia)
    2. CGEventTap API - more powerful but requires Input Monitoring permissions
    
    The implementation will attempt to use Carbon first, falling back to CGEventTap
    if Carbon fails or is disabled.
    
    @tags{GlobalHotKeys}
*/
struct GlobalHotKey::PlatformSpecificData
{
    PlatformSpecificData (GlobalHotKey& owner);
    ~PlatformSpecificData();
    
    bool registerHotKey (const KeyCode& keyCode, const ModifierKeys& modifiers);
    void unregisterHotKey();
    
    static bool isSupported();
    static bool requestPermissions();
    static bool hasPermissions();
    
private:
    GlobalHotKey& owner;
    bool isRegistered = false;
    bool usingCarbonAPI = false;
    bool usingCGEventTap = false;
    
    // Carbon API implementation
#if JUCE_GLOBAL_HOTKEYS_ENABLE_CARBON_API
    EventHotKeyRef carbonHotKeyRef = nullptr;
    EventHotKeyID carbonHotKeyID;
    
    bool registerWithCarbon (const KeyCode& keyCode, const ModifierKeys& modifiers);
    void unregisterFromCarbon();
    static UInt32 convertKeyCodeToCarbon (const KeyCode& keyCode);
    static UInt32 convertModifiersToCarbon (const ModifierKeys& modifiers);
#endif
    
    // CGEventTap API implementation
#if JUCE_GLOBAL_HOTKEYS_ENABLE_CGEVENTTAP_API
    CFMachPortRef eventTap = nullptr;
    CFRunLoopSourceRef eventSource = nullptr;
    KeyCode registeredKeyCode;
    ModifierKeys registeredModifiers;
    
    bool registerWithCGEventTap (const KeyCode& keyCode, const ModifierKeys& modifiers);
    void unregisterFromCGEventTap();
    static CGEventRef eventTapCallback (CGEventTapProxy proxy,
                                       CGEventType type,
                                       CGEventRef event,
                                       void* userData);
    static CGKeyCode convertKeyCodeToCG (const KeyCode& keyCode);
    static CGEventFlags convertModifiersToCG (const ModifierKeys& modifiers);
    bool matchesRegisteredHotkey (CGEventRef event) const;
#endif
    
    static std::unordered_map<EventHotKeyID*, PlatformSpecificData*> carbonHotkeys;
    static std::unordered_map<PlatformSpecificData*, PlatformSpecificData*> cgEventTapHotkeys;
    static std::atomic<UInt32> nextCarbonHotkeyId;
    static EventHandlerRef globalCarbonEventHandlerRef;
    
    static OSStatus globalCarbonEventHandler (EventHandlerCallRef nextHandler, 
                                            EventRef event, 
                                            void* userData);
};

} // namespace juce

#endif // JUCE_MAC 