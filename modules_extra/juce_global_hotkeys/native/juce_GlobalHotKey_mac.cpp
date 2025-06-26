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
// Static member initialization
std::unordered_map<EventHotKeyID*, GlobalHotKey::PlatformSpecificData*> GlobalHotKey::PlatformSpecificData::carbonHotkeys;
std::unordered_map<GlobalHotKey::PlatformSpecificData*, GlobalHotKey::PlatformSpecificData*> GlobalHotKey::PlatformSpecificData::cgEventTapHotkeys;
std::atomic<UInt32> GlobalHotKey::PlatformSpecificData::nextCarbonHotkeyId {1};
EventHandlerRef GlobalHotKey::PlatformSpecificData::globalCarbonEventHandlerRef = nullptr;

//==============================================================================
GlobalHotKey::PlatformSpecificData::PlatformSpecificData (GlobalHotKey& owner)
    : owner (owner)
{
}

GlobalHotKey::PlatformSpecificData::~PlatformSpecificData()
{
    unregisterHotKey();
}

//==============================================================================
bool GlobalHotKey::PlatformSpecificData::registerHotKey (const KeyCode& keyCode, const ModifierKeys& modifiers)
{
    if (isRegistered)
    {
#if JUCE_GLOBAL_HOTKEYS_DEBUG_LOGGING
        DBG ("GlobalHotKey: Already registered, unregistering first");
#endif
        return false;
    }
    
#if JUCE_GLOBAL_HOTKEYS_DEBUG_LOGGING
    DBG ("GlobalHotKey: Attempting to register hotkey - Key: " + String(keyCode.getJuceKeyCode()) + 
         ", Modifiers: " + String(modifiers.getRawFlags()));
#endif
    
    
    // Fall back to CGEventTap API (if enabled)
#if JUCE_GLOBAL_HOTKEYS_ENABLE_CGEVENTTAP_API
    DBG ("GlobalHotKey: Trying CGEventTap API...");
    if (registerWithCGEventTap (keyCode, modifiers))
    {
        usingCGEventTap = true;
        isRegistered = true;
        DBG ("GlobalHotKey: Successfully registered with CGEventTap API");
        return true;
    }
    else
    {
        DBG ("GlobalHotKey: CGEventTap API registration failed");
    }
#endif
    
    // Try Carbon API first (if enabled)
#if JUCE_GLOBAL_HOTKEYS_ENABLE_CARBON_API
    DBG ("GlobalHotKey: Trying Carbon API...");
    if (registerWithCarbon (keyCode, modifiers))
    {
        usingCarbonAPI = true;
        isRegistered = true;
        DBG ("GlobalHotKey: Successfully registered with Carbon API");
        return true;
    }
    else
    {
        DBG ("GlobalHotKey: Carbon API registration failed");
    }
#endif
    
    DBG ("GlobalHotKey: All registration methods failed");
    return false;
}

void GlobalHotKey::PlatformSpecificData::unregisterHotKey()
{
    if (!isRegistered)
    {
        DBG ("GlobalHotKey: Not registered, nothing to unregister");
        return;
    }
    
    DBG ("GlobalHotKey: Unregistering hotkey");
    
#if JUCE_GLOBAL_HOTKEYS_ENABLE_CARBON_API
    if (usingCarbonAPI)
    {
        DBG ("GlobalHotKey: Unregistering from Carbon API");
        unregisterFromCarbon();
    }
#endif
    
#if JUCE_GLOBAL_HOTKEYS_ENABLE_CGEVENTTAP_API
    if (usingCGEventTap)
    {
        DBG ("GlobalHotKey: Unregistering from CGEventTap API");
        unregisterFromCGEventTap();
    }
#endif
    
    usingCarbonAPI = false;
    usingCGEventTap = false;
    isRegistered = false;
    DBG ("GlobalHotKey: Successfully unregistered");
}

//==============================================================================
bool GlobalHotKey::PlatformSpecificData::isSupported()
{
#if JUCE_GLOBAL_HOTKEYS_ENABLE_CARBON_API || JUCE_GLOBAL_HOTKEYS_ENABLE_CGEVENTTAP_API
    return true;
#else
    return false;
#endif
}

bool GlobalHotKey::PlatformSpecificData::requestPermissions()
{
#if JUCE_GLOBAL_HOTKEYS_ENABLE_CGEVENTTAP_API
    DBG ("GlobalHotKey: Requesting permissions for CGEventTap API");
    
    // Check if we already have permissions
    if (hasPermissions())
    {
        DBG ("GlobalHotKey: Permissions already granted");
        return true;
    }
    
    DBG ("GlobalHotKey: Permissions not available, attempting to trigger permission dialog");
    
    // For CGEventTap, we need to trigger the system permission dialog
    // This is done by attempting to create an event tap
    static auto dummyCallback = [](CGEventTapProxy, CGEventType, CGEventRef event, void*) -> CGEventRef {
        return event;
    };
    
    auto testTap = CGEventTapCreate(kCGSessionEventTap, 
                                   kCGHeadInsertEventTap, 
                                   kCGEventTapOptionDefault,
                                   CGEventMaskBit(kCGEventKeyDown),
                                   dummyCallback, 
                                   nullptr);
    
    if (testTap != nullptr)
    {
        DBG ("GlobalHotKey: Permission test succeeded");
        CFRelease(testTap);
        return true;
    }
    
    DBG ("GlobalHotKey: Permission test failed, likely denied");
    
    // If we reach here, permission was likely denied
    // Show dialog to guide user to system preferences
    if (JUCE_GLOBAL_HOTKEYS_SHOW_PERMISSION_DIALOG)
    {
        DBG ("GlobalHotKey: Showing permission dialog");
        // This will be implemented in the .mm file
        return showPermissionDialog();
    }
    
    return false;
#else
    DBG ("GlobalHotKey: Using Carbon API, no special permissions required");
    // Carbon API doesn't require special permissions
    return true;
#endif
}

bool GlobalHotKey::PlatformSpecificData::hasPermissions()
{
#if JUCE_GLOBAL_HOTKEYS_ENABLE_CGEVENTTAP_API
    // Use a simple test callback for permission checking
    static auto testCallback = [](CGEventTapProxy, CGEventType, CGEventRef event, void*) -> CGEventRef {
        return event;  // Pass through all events
    };
    
    // Test permissions by creating a minimal event tap
    auto testEventTap = CGEventTapCreate(kCGSessionEventTap, 
                                        kCGTailAppendEventTap,  // Less intrusive
                                        kCGEventTapOptionListenOnly,  // Read-only
                                        CGEventMaskBit(kCGEventNull),  // Minimal event mask
                                        testCallback,
                                        nullptr);
    
    if (testEventTap != nullptr)
    {
        CFRelease(testEventTap);
        return true;
    }
    
    return false;
#else
    // Carbon API doesn't require special permissions
    return true;
#endif
}

//==============================================================================
#if JUCE_GLOBAL_HOTKEYS_ENABLE_CARBON_API

bool GlobalHotKey::PlatformSpecificData::registerWithCarbon (const KeyCode& keyCode, const ModifierKeys& modifiers)
{
    carbonHotKeyID.signature = 'JUCE';
    carbonHotKeyID.id = nextCarbonHotkeyId++;
    
    const auto carbonKey = convertKeyCodeToCarbon (keyCode);
    const auto carbonModifiers = convertModifiersToCarbon (modifiers);
    
    DBG ("GlobalHotKey: Carbon conversion - Key: " + String(carbonKey) + 
         ", Modifiers: " + String(carbonModifiers));
    
    if (carbonKey == 0)
    {
        DBG ("GlobalHotKey: Invalid key code for Carbon API");
        return false;
    }
    
    // Register the hotkey
    DBG ("GlobalHotKey: Calling RegisterEventHotKey...");
    OSStatus status = RegisterEventHotKey (carbonKey, carbonModifiers, carbonHotKeyID,
                                          GetApplicationEventTarget(), 0, &carbonHotKeyRef);
    
    if (status != noErr)
    {
        DBG ("GlobalHotKey: RegisterEventHotKey failed with status: " + String(status));
        return false;
    }
    else
    {
        DBG ("GlobalHotKey: RegisterEventHotKey succeeded");
    }
    
    // Install global event handler if not already installed
    DBG ("GlobalHotKey: Installing Carbon event handler...");
    if (carbonHotkeys.empty())
    {
        const EventTypeSpec eventType = { kEventClassKeyboard, kEventHotKeyPressed };
        status = InstallApplicationEventHandler (&globalCarbonEventHandler, 1, &eventType, 
                                               nullptr, &globalCarbonEventHandlerRef);
        
        if (status != noErr)
        {
            DBG ("GlobalHotKey: InstallApplicationEventHandler failed with status: " + String(status));
            UnregisterEventHotKey (carbonHotKeyRef);
            carbonHotKeyRef = nullptr;
            return false;
        }
        DBG ("GlobalHotKey: Global Carbon event handler installed successfully");
    }
    else
    {
        DBG ("GlobalHotKey: Using existing global Carbon event handler");
    }
    
    carbonHotkeys[&carbonHotKeyID] = this;
    return true;
}

void GlobalHotKey::PlatformSpecificData::unregisterFromCarbon()
{
    if (carbonHotKeyRef != nullptr)
    {
        UnregisterEventHotKey (carbonHotKeyRef);
        carbonHotKeyRef = nullptr;
    }
    
    carbonHotkeys.erase (&carbonHotKeyID);
    
    // Remove global event handler if this was the last hotkey
    if (carbonHotkeys.empty() && globalCarbonEventHandlerRef != nullptr)
    {
        DBG ("GlobalHotKey: Removing global Carbon event handler");
        RemoveEventHandler (globalCarbonEventHandlerRef);
        globalCarbonEventHandlerRef = nullptr;
    }
}



OSStatus GlobalHotKey::PlatformSpecificData::globalCarbonEventHandler (EventHandlerCallRef nextHandler, 
                                                                      EventRef event, 
                                                                      void* userData)
{
#if JUCE_GLOBAL_HOTKEYS_DEBUG_LOGGING
    DBG ("GlobalHotKey: Global Carbon event handler called - Class: " + String(GetEventClass(event)) + 
         ", Kind: " + String(GetEventKind(event)));
#endif
    
    if (GetEventClass (event) == kEventClassKeyboard && 
        GetEventKind (event) == kEventHotKeyPressed)
    {
        EventHotKeyID hotKeyID;
        OSStatus status = GetEventParameter (event, kEventParamDirectObject, typeEventHotKeyID, 
                                           nullptr, sizeof(hotKeyID), nullptr, &hotKeyID);
        
#if JUCE_GLOBAL_HOTKEYS_DEBUG_LOGGING
        DBG ("GlobalHotKey: Global Carbon hotkey event - GetEventParameter status: " + String(status) +
             ", Signature: " + String(hotKeyID.signature) + 
             ", ID: " + String(hotKeyID.id));
#endif
        
        if (status == noErr && hotKeyID.signature == 'JUCE')
        {
            // Find the corresponding hotkey instance by searching through all registered hotkeys
            for (auto& pair : carbonHotkeys)
            {
                if (pair.first->signature == hotKeyID.signature && pair.first->id == hotKeyID.id)
                {
                    auto* hotkeyData = pair.second;
#if JUCE_GLOBAL_HOTKEYS_DEBUG_LOGGING
                    DBG ("GlobalHotKey: Found matching hotkey, triggering callback");
#endif
                    // Forward to JUCE message thread
                    MessageManager::callAsync ([hotkeyData]()
                    {
                        hotkeyData->owner.triggerCallback();
                    });
                    return noErr;
                }
            }
#if JUCE_GLOBAL_HOTKEYS_DEBUG_LOGGING
            DBG ("GlobalHotKey: No matching hotkey found for ID: " + String(hotKeyID.id));
#endif
        }
    }
    
    return CallNextEventHandler (nextHandler, event);
}

UInt32 GlobalHotKey::PlatformSpecificData::convertKeyCodeToCarbon (const KeyCode& keyCode)
{
    const auto juceKey = keyCode.getJuceKeyCode();
    
    // Map JUCE key codes to Carbon virtual key codes using correct mapping
    // Based on HIToolbox/Events.h virtual key codes
    if (juceKey >= 'A' && juceKey <= 'Z')
    {
        // Use the correct virtual key code mapping from translateVirtualToAsciiKeyCode
        switch (juceKey)
        {
            case 'A': return 0x00;  // kVK_ANSI_A
            case 'S': return 0x01;  // kVK_ANSI_S  
            case 'D': return 0x02;  // kVK_ANSI_D
            case 'F': return 0x03;  // kVK_ANSI_F
            case 'H': return 0x04;  // kVK_ANSI_H
            case 'G': return 0x05;  // kVK_ANSI_G  ← 이것이 핵심!
            case 'Z': return 0x06;  // kVK_ANSI_Z
            case 'X': return 0x07;  // kVK_ANSI_X
            case 'C': return 0x08;  // kVK_ANSI_C
            case 'V': return 0x09;  // kVK_ANSI_V
            case 'B': return 0x0B;  // kVK_ANSI_B
            case 'Q': return 0x0C;  // kVK_ANSI_Q
            case 'W': return 0x0D;  // kVK_ANSI_W
            case 'E': return 0x0E;  // kVK_ANSI_E
            case 'R': return 0x0F;  // kVK_ANSI_R
            case 'Y': return 0x10;  // kVK_ANSI_Y
            case 'T': return 0x11;  // kVK_ANSI_T
            case 'O': return 0x1F;  // kVK_ANSI_O
            case 'U': return 0x20;  // kVK_ANSI_U
            case 'I': return 0x22;  // kVK_ANSI_I
            case 'P': return 0x23;  // kVK_ANSI_P
            case 'L': return 0x25;  // kVK_ANSI_L
            case 'J': return 0x26;  // kVK_ANSI_J
            case 'K': return 0x28;  // kVK_ANSI_K
            case 'N': return 0x2D;  // kVK_ANSI_N
            case 'M': return 0x2E;  // kVK_ANSI_M
            default: return 0;
        }
    }
    if (juceKey >= '0' && juceKey <= '9')
    {
        // 숫자 키도 정확한 매핑 사용
        switch (juceKey)
        {
            case '1': return 0x12;  // kVK_ANSI_1
            case '2': return 0x13;  // kVK_ANSI_2
            case '3': return 0x14;  // kVK_ANSI_3
            case '4': return 0x15;  // kVK_ANSI_4
            case '6': return 0x16;  // kVK_ANSI_6
            case '5': return 0x17;  // kVK_ANSI_5
            case '9': return 0x19;  // kVK_ANSI_9
            case '7': return 0x1A;  // kVK_ANSI_7
            case '8': return 0x1C;  // kVK_ANSI_8
            case '0': return 0x1D;  // kVK_ANSI_0
            default: return 0;
        }
    }
    
    // Function keys (using our own constants)
    if (juceKey >= 0x20001 && juceKey <= 0x2000c)  // F1Key to F12Key
        return kVK_F1 + (juceKey - 0x20001);
    
    // Punctuation and special characters
    switch (juceKey)
    {
        // Punctuation marks
        case '[':        return 0x21;  // kVK_ANSI_LeftBracket
        case ']':        return 0x1E;  // kVK_ANSI_RightBracket
        case ';':        return 0x29;  // kVK_ANSI_Semicolon
        case '\'':       return 0x27;  // kVK_ANSI_Quote
        case ',':        return 0x2B;  // kVK_ANSI_Comma
        case '.':        return 0x2F;  // kVK_ANSI_Period
        case '/':        return 0x2C;  // kVK_ANSI_Slash
        case '\\':       return 0x2A;  // kVK_ANSI_Backslash
        case '`':        return 0x32;  // kVK_ANSI_Grave
        case '-':        return 0x1B;  // kVK_ANSI_Minus
        case '=':        return 0x18;  // kVK_ANSI_Equal
        
        // Additional special characters that might be used
        // These are accessible with Shift on various keys
        // Note: These are the base key codes, actual character depends on modifiers
        
        // Keypad numbers (if someone wants to use numpad for hotkeys)
        // These have different key codes from main number keys
        case 0x60000:    return 0x52;  // kVK_ANSI_Keypad0
        case 0x60001:    return 0x53;  // kVK_ANSI_Keypad1
        case 0x60002:    return 0x54;  // kVK_ANSI_Keypad2
        case 0x60003:    return 0x55;  // kVK_ANSI_Keypad3
        case 0x60004:    return 0x56;  // kVK_ANSI_Keypad4
        case 0x60005:    return 0x57;  // kVK_ANSI_Keypad5
        case 0x60006:    return 0x58;  // kVK_ANSI_Keypad6
        case 0x60007:    return 0x59;  // kVK_ANSI_Keypad7
        case 0x60008:    return 0x5B;  // kVK_ANSI_Keypad8
        case 0x60009:    return 0x5C;  // kVK_ANSI_Keypad9
        
        // Special keys
        case ' ':        return kVK_Space;      // spaceKey
        case 0x1000d:    return kVK_Return;     // returnKey
        case 0x1001b:    return kVK_Escape;     // escapeKey
        case 0x10008:    return kVK_Delete;     // backspaceKey
        case 0x1007f:    return kVK_ForwardDelete; // deleteKey
        case 0x10009:    return kVK_Tab;        // tabKey
        case 0x10012:    return kVK_LeftArrow;  // leftKey
        case 0x10014:    return kVK_RightArrow; // rightKey
        case 0x10013:    return kVK_UpArrow;    // upKey
        case 0x10015:    return kVK_DownArrow;  // downKey
        case 0x10010:    return kVK_Home;       // homeKey
        case 0x10011:    return kVK_End;        // endKey
        case 0x10016:    return kVK_PageUp;     // pageUpKey
        case 0x10017:    return kVK_PageDown;   // pageDownKey
        default:         return 0;
    }
}

UInt32 GlobalHotKey::PlatformSpecificData::convertModifiersToCarbon (const ModifierKeys& modifiers)
{
    UInt32 carbonModifiers = 0;
    
    // On macOS, Command key is the primary modifier
    if (modifiers.isCommandDown())
        carbonModifiers |= cmdKey;
    if (modifiers.isShiftDown())
        carbonModifiers |= shiftKey;
    if (modifiers.isAltDown())
        carbonModifiers |= optionKey;
    // Control key (actual Ctrl key, not Command)
    if (modifiers.isCtrlDown())
        carbonModifiers |= controlKey;
    
#if JUCE_GLOBAL_HOTKEYS_DEBUG_LOGGING
    DBG ("GlobalHotKey: Modifier conversion - Command: " + String(modifiers.isCommandDown()?"Yes":"No") +
         ", Shift: " + String(modifiers.isShiftDown()?"Yes":"No") +
         ", Alt: " + String(modifiers.isAltDown()?"Yes":"No") +
         ", Ctrl: " + String(modifiers.isCtrlDown()?"Yes":"No") +
         ", Carbon flags: " + String(carbonModifiers));
#endif
    
    return carbonModifiers;
}

#endif // JUCE_GLOBAL_HOTKEYS_ENABLE_CARBON_API

//==============================================================================
#if JUCE_GLOBAL_HOTKEYS_ENABLE_CGEVENTTAP_API

bool GlobalHotKey::PlatformSpecificData::registerWithCGEventTap (const KeyCode& keyCode, const ModifierKeys& modifiers)
{
    registeredKeyCode = keyCode;
    registeredModifiers = modifiers;
    
    const auto cgKey = convertKeyCodeToCG (keyCode);
    const auto cgModifiers = convertModifiersToCG (modifiers);
    
    DBG ("GlobalHotKey: CGEventTap conversion - Key: " + String(cgKey) + 
         ", Modifiers: " + String(cgModifiers));
    
    // Create event tap
    DBG ("GlobalHotKey: Creating CGEventTap...");
    eventTap = CGEventTapCreate (kCGSessionEventTap, 
                                kCGHeadInsertEventTap, 
                                kCGEventTapOptionDefault,
                                CGEventMaskBit(kCGEventKeyDown),
                                eventTapCallback, 
                                this);
    
    if (eventTap == nullptr)
    {
        DBG ("GlobalHotKey: CGEventTapCreate failed - possibly due to lack of Input Monitoring permission");
        return false;
    }
    else
    {
        DBG ("GlobalHotKey: CGEventTap created successfully");
    }
    
    // Create run loop source
    DBG ("GlobalHotKey: Creating run loop source...");
    eventSource = CFMachPortCreateRunLoopSource (kCFAllocatorDefault, eventTap, 0);
    
    if (eventSource == nullptr)
    {
        DBG ("GlobalHotKey: CFMachPortCreateRunLoopSource failed");
        CFRelease (eventTap);
        eventTap = nullptr;
        return false;
    }
    
    DBG ("GlobalHotKey: Run loop source created, adding to current run loop");
    // Add to run loop
    CFRunLoopAddSource (CFRunLoopGetCurrent(), eventSource, kCFRunLoopCommonModes);
    
    // Enable the event tap
    DBG ("GlobalHotKey: Enabling event tap");
    CGEventTapEnable (eventTap, true);
    
    cgEventTapHotkeys[this] = this;
    DBG ("GlobalHotKey: CGEventTap registration completed successfully");
    return true;
}

void GlobalHotKey::PlatformSpecificData::unregisterFromCGEventTap()
{
    if (eventSource != nullptr)
    {
        CFRunLoopRemoveSource (CFRunLoopGetCurrent(), eventSource, kCFRunLoopCommonModes);
        CFRelease (eventSource);
        eventSource = nullptr;
    }
    
    if (eventTap != nullptr)
    {
        CGEventTapEnable (eventTap, false);
        CFRelease (eventTap);
        eventTap = nullptr;
    }
    
    cgEventTapHotkeys.erase (this);
}

CGEventRef GlobalHotKey::PlatformSpecificData::eventTapCallback (CGEventTapProxy proxy,
                                                               CGEventType type,
                                                               CGEventRef event,
                                                               void* userData)
{
    auto* self = static_cast<PlatformSpecificData*>(userData);
    
    if (type == kCGEventKeyDown)
    {
        const auto keyCode = CGEventGetIntegerValueField (event, kCGKeyboardEventKeycode);
        const auto flags = CGEventGetFlags (event);
        
        DBG ("GlobalHotKey: CGEventTap key event - Key: " + String(keyCode) + 
             ", Flags: " + String(flags));
        
        if (self->matchesRegisteredHotkey (event))
        {
            DBG ("GlobalHotKey: Hotkey match found, triggering callback");
            // Forward to JUCE message thread
            MessageManager::callAsync ([self]()
            {
                self->owner.triggerCallback();
            });
            
            // Consume the event
            return nullptr;
        }
    }
    
    return event;
}

bool GlobalHotKey::PlatformSpecificData::matchesRegisteredHotkey (CGEventRef event) const
{
    const auto keyCode = CGEventGetIntegerValueField (event, kCGKeyboardEventKeycode);
    const auto flags = CGEventGetFlags (event);
    
    const auto expectedKeyCode = convertKeyCodeToCG (registeredKeyCode);
    const auto expectedFlags = convertModifiersToCG (registeredModifiers);
    
    return keyCode == expectedKeyCode && (flags & expectedFlags) == expectedFlags;
}

CGKeyCode GlobalHotKey::PlatformSpecificData::convertKeyCodeToCG (const KeyCode& keyCode)
{
    // Same mapping as Carbon since CGKeyCode uses the same values
    return convertKeyCodeToCarbon (keyCode);
}

CGEventFlags GlobalHotKey::PlatformSpecificData::convertModifiersToCG (const ModifierKeys& modifiers)
{
    CGEventFlags cgFlags = 0;
    
    // macOS에서 Command 키는 kCGEventFlagMaskCommand
    if (modifiers.isCommandDown())
        cgFlags |= kCGEventFlagMaskCommand;
    if (modifiers.isShiftDown())
        cgFlags |= kCGEventFlagMaskShift;
    if (modifiers.isAltDown())
        cgFlags |= kCGEventFlagMaskAlternate;
    // 실제 Control 키는 kCGEventFlagMaskControl
    if (modifiers.isCtrlDown())
        cgFlags |= kCGEventFlagMaskControl;
    
#if JUCE_GLOBAL_HOTKEYS_DEBUG_LOGGING
    DBG ("GlobalHotKey: CGEventTap modifier conversion - Command: " + String(modifiers.isCommandDown()?"Yes":"No") +
         ", Shift: " + String(modifiers.isShiftDown()?"Yes":"No") +
         ", Alt: " + String(modifiers.isAltDown()?"Yes":"No") +
         ", Ctrl: " + String(modifiers.isCtrlDown()?"Yes":"No") +
         ", CGEventFlags: " + String(cgFlags));
#endif
    
    return cgFlags;
}

#endif // JUCE_GLOBAL_HOTKEYS_ENABLE_CGEVENTTAP_API

} // namespace juce

#endif // JUCE_MAC 
