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

namespace juce
{

//==============================================================================
/**
    A cross-platform global hotkey that can be triggered even when the application
    is in the background.
    
    This class provides a unified interface for registering global system hotkeys
    on Windows, macOS, and Linux. The hotkey will trigger its callback function
    whenever the specified key combination is pressed, regardless of which
    application currently has focus.
    
    Example usage:
    @code
    auto hotkey = std::make_unique<GlobalHotKey> (
        KeyCode::G,
        ModifierKeys::ctrlKey() | ModifierKeys::shiftKey(),
        [this] { showApplicationWindow(); }
    );
    @endcode
    
    @tags{GlobalHotKeys}
*/
class GlobalHotKey
{
public:
    //==============================================================================
    /** A callback function that gets called when the hotkey is triggered. */
    using HotKeyCallback = std::function<void()>;
    
    //==============================================================================
    /** Creates a global hotkey with the specified key combination and callback.
        
        @param keyCode      The key code to register
        @param modifiers    The modifier keys that must be pressed
        @param callback     The function to call when the hotkey is triggered
    */
    GlobalHotKey (const KeyCode& keyCode, 
                  const ModifierKeys& modifiers,
                  HotKeyCallback callback);
    
    /** Destructor - automatically unregisters the hotkey. */
    ~GlobalHotKey();
    
    //==============================================================================
    /** Returns true if the hotkey was successfully registered. */
    bool isRegistered() const noexcept { return registered; }
    
    /** Returns the key code for this hotkey. */
    const KeyCode& getKeyCode() const noexcept { return keyCode; }
    
    /** Returns the modifier keys for this hotkey. */
    const ModifierKeys& getModifierKeys() const noexcept { return modifiers; }
    
    /** Manually triggers the hotkey callback (for testing purposes). */
    void triggerCallback();
    
    //==============================================================================
    /** Checks if the system supports global hotkeys on this platform. */
    static bool isSupported();
    
    /** Requests necessary permissions for global hotkeys (macOS only).
        Returns true if permissions are granted or not required.
    */
    static bool requestPermissions();
    
    /** Returns true if the application has the necessary permissions for global hotkeys. */
    static bool hasPermissions();
    
    /** Shows a permission dialog to guide users to system preferences.
        On macOS, this shows a native dialog with option to open System Preferences.
        On other platforms, this returns false as no permissions are needed.
    */
    static bool showPermissionDialog();

private:
    //==============================================================================
    KeyCode keyCode;
    ModifierKeys modifiers;
    HotKeyCallback callback;
    bool registered = false;
    
    struct PlatformSpecificData;
    std::unique_ptr<PlatformSpecificData> platformData;
    
    void registerHotKey();
    void unregisterHotKey();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GlobalHotKey)
};

} // namespace juce 