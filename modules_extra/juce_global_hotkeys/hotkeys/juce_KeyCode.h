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
    Represents a keyboard key code that can be used for global hotkeys.
    
    This class provides a cross-platform way to represent keyboard keys,
    handling the differences between Windows, macOS, and Linux key codes.
    
    @tags{GlobalHotKeys}
*/
class KeyCode
{
public:
    //==============================================================================
    /** Creates an invalid key code. */
    KeyCode() noexcept;
    
    /** Creates a key code from a character. */
    KeyCode (int keyChar) noexcept;
    
    /** Creates a key code from platform-specific key code. */
    static KeyCode fromPlatformKeyCode (int platformKeyCode) noexcept;
    
    /** Creates a key code from a JUCE KeyPress key code. */
    static KeyCode fromJuceKeyCode (int juceKeyCode) noexcept;
    
    //==============================================================================
    /** Returns true if this represents a valid key code. */
    bool isValid() const noexcept { return keyCode != 0; }
    
    /** Returns the platform-specific key code for this key. */
    int getPlatformKeyCode() const noexcept;
    
    /** Returns the JUCE KeyPress compatible key code. */
    int getJuceKeyCode() const noexcept { return keyCode; }
    
    /** Returns a human-readable string representation of this key. */
    String toString() const;
    
    //==============================================================================
    // Common key codes
    static const KeyCode A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z;
    static const KeyCode Key0, Key1, Key2, Key3, Key4, Key5, Key6, Key7, Key8, Key9;
    static const KeyCode F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12;
    static const KeyCode Space, Return, Escape, Backspace, Delete, Tab;
    static const KeyCode LeftArrow, RightArrow, UpArrow, DownArrow;
    static const KeyCode Home, End, PageUp, PageDown, Insert;
    
    //==============================================================================
    bool operator== (const KeyCode& other) const noexcept { return keyCode == other.keyCode; }
    bool operator!= (const KeyCode& other) const noexcept { return keyCode != other.keyCode; }
    bool operator<  (const KeyCode& other) const noexcept { return keyCode < other.keyCode; }

private:
    int keyCode = 0;
    
    explicit KeyCode (int code, bool) noexcept : keyCode (code) {}
    
    JUCE_LEAK_DETECTOR (KeyCode)
};

} // namespace juce 