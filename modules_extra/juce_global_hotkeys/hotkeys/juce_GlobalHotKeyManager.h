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
    A manager class for handling multiple global hotkeys.
    
    This class provides a convenient way to register and manage multiple global
    hotkeys for an application. It also handles permission requests and provides
    utility functions for common hotkey operations.
    
    Example usage:
    @code
    class MyApplication : public JUCEApplicationBase
    {
    public:
        void initialise (const String& commandLine) override
        {
            // Request permissions early in the application lifecycle
            GlobalHotKeyManager::requestPermissions();
            
            // Register hotkeys
            hotKeyManager.registerHotKey ("showWindow", KeyCode::G, 
                                         ModifierKeys::ctrlKey() | ModifierKeys::shiftKey(),
                                         [this] { showMainWindow(); });
            
            hotKeyManager.registerHotKey ("hideWindow", KeyCode::H,
                                         ModifierKeys::ctrlKey() | ModifierKeys::shiftKey(),
                                         [this] { hideMainWindow(); });
        }
        
    private:
        GlobalHotKeyManager hotKeyManager;
    };
    @endcode
    
    @tags{GlobalHotKeys}
*/
class GlobalHotKeyManager
{
public:
    //==============================================================================
    /** Creates a GlobalHotKeyManager. */
    GlobalHotKeyManager() = default;
    
    /** Destructor - automatically unregisters all hotkeys. */
    ~GlobalHotKeyManager();
    
    //==============================================================================
    /** Registers a new global hotkey with a string identifier.
        
        @param identifier   A unique string identifier for this hotkey
        @param keyCode      The key code to register
        @param modifiers    The modifier keys that must be pressed
        @param callback     The function to call when the hotkey is triggered
        @returns            true if the hotkey was successfully registered
    */
    bool registerHotKey (const juce::String& identifier,
                        const KeyCode& keyCode,
                        const ModifierKeys& modifiers,
                        std::function<void()> callback);
    
    /** Unregisters a hotkey by its identifier.
        
        @param identifier   The identifier of the hotkey to unregister
        @returns            true if the hotkey was found and unregistered
    */
    bool unregisterHotKey (const juce::String& identifier);
    
    /** Unregisters all hotkeys. */
    void unregisterAllHotKeys();
    
    //==============================================================================
    /** Returns true if a hotkey with the given identifier is registered. */
    bool isHotKeyRegistered (const juce::String& identifier) const;
    
    /** Returns a list of all registered hotkey identifiers. */
    juce::StringArray getRegisteredHotKeyIdentifiers() const;
    
    /** Returns the number of registered hotkeys. */
    int getNumRegisteredHotKeys() const;
    
    //==============================================================================
    /** Manually triggers a hotkey by its identifier (for testing purposes).
        
        @param identifier   The identifier of the hotkey to trigger
        @returns            true if the hotkey was found and triggered
    */
    bool triggerHotKey (const juce::String& identifier);
    
    //==============================================================================
    /** Returns true if global hotkeys are supported on this platform. */
    static bool isSupported();
    
    /** Requests necessary permissions for global hotkeys.
        
        On macOS, this will show a permission dialog if needed.
        On other platforms, this will return true immediately.
        
        @returns true if permissions are granted or not required
    */
    static bool requestPermissions();
    
    /** Returns true if the application has the necessary permissions for global hotkeys. */
    static bool hasPermissions();
    
    //==============================================================================
    /** A structure that holds information about a registered hotkey. */
    struct HotKeyInfo
    {
        juce::String identifier;
        KeyCode keyCode;
        ModifierKeys modifiers;
        std::function<void()> callback;
        bool isRegistered;
        
        /** Returns a human-readable string representation of this hotkey. */
        juce::String toString() const;
    };
    
    /** Returns information about a registered hotkey.
        
        @param identifier   The identifier of the hotkey
        @returns            A pointer to the HotKeyInfo, or nullptr if not found
    */
    const HotKeyInfo* getHotKeyInfo (const juce::String& identifier) const;
    
    /** Returns information about all registered hotkeys. */
    juce::Array<HotKeyInfo> getAllHotKeyInfo() const;

private:
    //==============================================================================
    std::map<juce::String, std::unique_ptr<GlobalHotKey>> registeredHotKeys;
    std::map<juce::String, HotKeyInfo> hotKeyInfos;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GlobalHotKeyManager)
};

} // namespace juce 