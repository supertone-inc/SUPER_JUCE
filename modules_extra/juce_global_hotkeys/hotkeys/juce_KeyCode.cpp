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

#include "../juce_global_hotkeys.h"

namespace juce
{

//==============================================================================
// Key code constants (compatible with JUCE KeyPress values)
enum
{
    spaceKey        = ' ',
    returnKey       = 0x1000d,
    escapeKey       = 0x1001b,
    backspaceKey    = 0x10008,
    deleteKey       = 0x1007f,
    tabKey          = 0x10009,
    leftKey         = 0x10012,
    rightKey        = 0x10014,
    upKey           = 0x10013,
    downKey         = 0x10015,
    homeKey         = 0x10010,
    endKey          = 0x10011,
    pageUpKey       = 0x10016,
    pageDownKey     = 0x10017,
    insertKey       = 0x10019,
    F1Key           = 0x20001,
    F2Key           = 0x20002,
    F3Key           = 0x20003,
    F4Key           = 0x20004,
    F5Key           = 0x20005,
    F6Key           = 0x20006,
    F7Key           = 0x20007,
    F8Key           = 0x20008,
    F9Key           = 0x20009,
    F10Key          = 0x2000a,
    F11Key          = 0x2000b,
    F12Key          = 0x2000c
};

//==============================================================================
KeyCode::KeyCode() noexcept : keyCode (0) {}

KeyCode::KeyCode (int keyChar) noexcept : keyCode (keyChar) {}

//==============================================================================
KeyCode KeyCode::fromPlatformKeyCode (int platformKeyCode) noexcept
{
#if JUCE_WINDOWS
    // Convert Windows virtual key code to JUCE key code
    if (platformKeyCode >= 'A' && platformKeyCode <= 'Z')
        return KeyCode (platformKeyCode, true);
    if (platformKeyCode >= '0' && platformKeyCode <= '9')
        return KeyCode (platformKeyCode, true);
    
    // Function keys
    if (platformKeyCode >= VK_F1 && platformKeyCode <= VK_F12)
        return KeyCode (F1Key + (platformKeyCode - VK_F1), true);
    
    // Special keys
    switch (platformKeyCode)
    {
        case VK_SPACE:      return KeyCode (spaceKey, true);
        case VK_RETURN:     return KeyCode (returnKey, true);
        case VK_ESCAPE:     return KeyCode (escapeKey, true);
        case VK_BACK:       return KeyCode (backspaceKey, true);
        case VK_DELETE:     return KeyCode (deleteKey, true);
        case VK_TAB:        return KeyCode (tabKey, true);
        case VK_LEFT:       return KeyCode (leftKey, true);
        case VK_RIGHT:      return KeyCode (rightKey, true);
        case VK_UP:         return KeyCode (upKey, true);
        case VK_DOWN:       return KeyCode (downKey, true);
        case VK_HOME:       return KeyCode (homeKey, true);
        case VK_END:        return KeyCode (endKey, true);
        case VK_PRIOR:      return KeyCode (pageUpKey, true);
        case VK_NEXT:       return KeyCode (pageDownKey, true);
        case VK_INSERT:     return KeyCode (insertKey, true);
        default:            return KeyCode();
    }
    
#elif JUCE_MAC
    // Convert macOS virtual key code to JUCE key code using correct mapping
    switch (platformKeyCode)
    {
        // Letters
        case 0x00: return KeyCode ('A', true);  case 0x01: return KeyCode ('S', true);
        case 0x02: return KeyCode ('D', true);  case 0x03: return KeyCode ('F', true);
        case 0x04: return KeyCode ('H', true);  case 0x05: return KeyCode ('G', true);
        case 0x06: return KeyCode ('Z', true);  case 0x07: return KeyCode ('X', true);
        case 0x08: return KeyCode ('C', true);  case 0x09: return KeyCode ('V', true);
        case 0x0B: return KeyCode ('B', true);  case 0x0C: return KeyCode ('Q', true);
        case 0x0D: return KeyCode ('W', true);  case 0x0E: return KeyCode ('E', true);
        case 0x0F: return KeyCode ('R', true);  case 0x10: return KeyCode ('Y', true);
        case 0x11: return KeyCode ('T', true);  case 0x1F: return KeyCode ('O', true);
        case 0x20: return KeyCode ('U', true);  case 0x22: return KeyCode ('I', true);
        case 0x23: return KeyCode ('P', true);  case 0x25: return KeyCode ('L', true);
        case 0x26: return KeyCode ('J', true);  case 0x28: return KeyCode ('K', true);
        case 0x2D: return KeyCode ('N', true);  case 0x2E: return KeyCode ('M', true);
        
        // Numbers
        case 0x12: return KeyCode ('1', true);  case 0x13: return KeyCode ('2', true);
        case 0x14: return KeyCode ('3', true);  case 0x15: return KeyCode ('4', true);
        case 0x16: return KeyCode ('6', true);  case 0x17: return KeyCode ('5', true);
        case 0x19: return KeyCode ('9', true);  case 0x1A: return KeyCode ('7', true);
        case 0x1C: return KeyCode ('8', true);  case 0x1D: return KeyCode ('0', true);
    }
    
    // Function keys
    if (platformKeyCode >= kVK_F1 && platformKeyCode <= kVK_F12)
        return KeyCode (F1Key + (platformKeyCode - kVK_F1), true);
    
    // Special keys
    switch (platformKeyCode)
    {
        case kVK_Space:         return KeyCode (spaceKey, true);
        case kVK_Return:        return KeyCode (returnKey, true);
        case kVK_Escape:        return KeyCode (escapeKey, true);
        case kVK_Delete:        return KeyCode (backspaceKey, true);
        case kVK_ForwardDelete: return KeyCode (deleteKey, true);
        case kVK_Tab:           return KeyCode (tabKey, true);
        case kVK_LeftArrow:     return KeyCode (leftKey, true);
        case kVK_RightArrow:    return KeyCode (rightKey, true);
        case kVK_UpArrow:       return KeyCode (upKey, true);
        case kVK_DownArrow:     return KeyCode (downKey, true);
        case kVK_Home:          return KeyCode (homeKey, true);
        case kVK_End:           return KeyCode (endKey, true);
        case kVK_PageUp:        return KeyCode (pageUpKey, true);
        case kVK_PageDown:      return KeyCode (pageDownKey, true);
        default:                return KeyCode();
    }
    
#else
    // For other platforms, assume direct mapping for now
    return KeyCode (platformKeyCode, true);
#endif
}

KeyCode KeyCode::fromJuceKeyCode (int juceKeyCode) noexcept
{
    return KeyCode (juceKeyCode, true);
}

//==============================================================================
int KeyCode::getPlatformKeyCode() const noexcept
{
#if JUCE_WINDOWS
    // Convert JUCE key code to Windows virtual key code
    if (keyCode >= 'A' && keyCode <= 'Z')
        return keyCode;
    if (keyCode >= '0' && keyCode <= '9')
        return keyCode;
    
    // Function keys
    if (keyCode >= F1Key && keyCode <= F12Key)
        return VK_F1 + (keyCode - F1Key);
    
    // Special keys
    switch (keyCode)
    {
        case spaceKey:     return VK_SPACE;
        case returnKey:    return VK_RETURN;
        case escapeKey:    return VK_ESCAPE;
        case backspaceKey: return VK_BACK;
        case deleteKey:    return VK_DELETE;
        case tabKey:       return VK_TAB;
        case leftKey:      return VK_LEFT;
        case rightKey:     return VK_RIGHT;
        case upKey:        return VK_UP;
        case downKey:      return VK_DOWN;
        case homeKey:      return VK_HOME;
        case endKey:       return VK_END;
        case pageUpKey:    return VK_PRIOR;
        case pageDownKey:  return VK_NEXT;
        case insertKey:    return VK_INSERT;
        default:           return 0;
    }
    
#elif JUCE_MAC
    // Convert JUCE key code to macOS virtual key code using correct mapping
    if (keyCode >= 'A' && keyCode <= 'Z')
    {
        switch (keyCode)
        {
            case 'A': return 0x00;  case 'S': return 0x01;  case 'D': return 0x02;
            case 'F': return 0x03;  case 'H': return 0x04;  case 'G': return 0x05;
            case 'Z': return 0x06;  case 'X': return 0x07;  case 'C': return 0x08;
            case 'V': return 0x09;  case 'B': return 0x0B;  case 'Q': return 0x0C;
            case 'W': return 0x0D;  case 'E': return 0x0E;  case 'R': return 0x0F;
            case 'Y': return 0x10;  case 'T': return 0x11;  case 'O': return 0x1F;
            case 'U': return 0x20;  case 'I': return 0x22;  case 'P': return 0x23;
            case 'L': return 0x25;  case 'J': return 0x26;  case 'K': return 0x28;
            case 'N': return 0x2D;  case 'M': return 0x2E;  default: return 0;
        }
    }
    if (keyCode >= '0' && keyCode <= '9')
    {
        switch (keyCode)
        {
            case '1': return 0x12;  case '2': return 0x13;  case '3': return 0x14;
            case '4': return 0x15;  case '6': return 0x16;  case '5': return 0x17;
            case '9': return 0x19;  case '7': return 0x1A;  case '8': return 0x1C;
            case '0': return 0x1D;  default: return 0;
        }
    }
    
    // Function keys
    if (keyCode >= F1Key && keyCode <= F12Key)
        return kVK_F1 + (keyCode - F1Key);
    
    // Special keys
    switch (keyCode)
    {
        case spaceKey:     return kVK_Space;
        case returnKey:    return kVK_Return;
        case escapeKey:    return kVK_Escape;
        case backspaceKey: return kVK_Delete;
        case deleteKey:    return kVK_ForwardDelete;
        case tabKey:       return kVK_Tab;
        case leftKey:      return kVK_LeftArrow;
        case rightKey:     return kVK_RightArrow;
        case upKey:        return kVK_UpArrow;
        case downKey:      return kVK_DownArrow;
        case homeKey:      return kVK_Home;
        case endKey:       return kVK_End;
        case pageUpKey:    return kVK_PageUp;
        case pageDownKey:  return kVK_PageDown;
        default:           return 0;
    }
    
#else
    // For other platforms, return the key code as-is
    return keyCode;
#endif
}

String KeyCode::toString() const
{
    if (keyCode >= 'A' && keyCode <= 'Z')
        return String::charToString (keyCode);
    if (keyCode >= '0' && keyCode <= '9')
        return String::charToString (keyCode);
    
    // Function keys
    if (keyCode >= F1Key && keyCode <= F12Key)
        return "F" + String (keyCode - F1Key + 1);
    
    // Special keys
    switch (keyCode)
    {
        case spaceKey:     return "Space";
        case returnKey:    return "Return";
        case escapeKey:    return "Escape";
        case backspaceKey: return "Backspace";
        case deleteKey:    return "Delete";
        case tabKey:       return "Tab";
        case leftKey:      return "Left";
        case rightKey:     return "Right";
        case upKey:        return "Up";
        case downKey:      return "Down";
        case homeKey:      return "Home";
        case endKey:       return "End";
        case pageUpKey:    return "Page Up";
        case pageDownKey:  return "Page Down";
        case insertKey:    return "Insert";
        default:           return "Unknown";
    }
}

//==============================================================================
// Static key code constants
const KeyCode KeyCode::A ('A', true);
const KeyCode KeyCode::B ('B', true);
const KeyCode KeyCode::C ('C', true);
const KeyCode KeyCode::D ('D', true);
const KeyCode KeyCode::E ('E', true);
const KeyCode KeyCode::F ('F', true);
const KeyCode KeyCode::G ('G', true);
const KeyCode KeyCode::H ('H', true);
const KeyCode KeyCode::I ('I', true);
const KeyCode KeyCode::J ('J', true);
const KeyCode KeyCode::K ('K', true);
const KeyCode KeyCode::L ('L', true);
const KeyCode KeyCode::M ('M', true);
const KeyCode KeyCode::N ('N', true);
const KeyCode KeyCode::O ('O', true);
const KeyCode KeyCode::P ('P', true);
const KeyCode KeyCode::Q ('Q', true);
const KeyCode KeyCode::R ('R', true);
const KeyCode KeyCode::S ('S', true);
const KeyCode KeyCode::T ('T', true);
const KeyCode KeyCode::U ('U', true);
const KeyCode KeyCode::V ('V', true);
const KeyCode KeyCode::W ('W', true);
const KeyCode KeyCode::X ('X', true);
const KeyCode KeyCode::Y ('Y', true);
const KeyCode KeyCode::Z ('Z', true);

const KeyCode KeyCode::Key0 ('0', true);
const KeyCode KeyCode::Key1 ('1', true);
const KeyCode KeyCode::Key2 ('2', true);
const KeyCode KeyCode::Key3 ('3', true);
const KeyCode KeyCode::Key4 ('4', true);
const KeyCode KeyCode::Key5 ('5', true);
const KeyCode KeyCode::Key6 ('6', true);
const KeyCode KeyCode::Key7 ('7', true);
const KeyCode KeyCode::Key8 ('8', true);
const KeyCode KeyCode::Key9 ('9', true);

const KeyCode KeyCode::F1 (F1Key, true);
const KeyCode KeyCode::F2 (F2Key, true);
const KeyCode KeyCode::F3 (F3Key, true);
const KeyCode KeyCode::F4 (F4Key, true);
const KeyCode KeyCode::F5 (F5Key, true);
const KeyCode KeyCode::F6 (F6Key, true);
const KeyCode KeyCode::F7 (F7Key, true);
const KeyCode KeyCode::F8 (F8Key, true);
const KeyCode KeyCode::F9 (F9Key, true);
const KeyCode KeyCode::F10 (F10Key, true);
const KeyCode KeyCode::F11 (F11Key, true);
const KeyCode KeyCode::F12 (F12Key, true);

const KeyCode KeyCode::Space (spaceKey, true);
const KeyCode KeyCode::Return (returnKey, true);
const KeyCode KeyCode::Escape (escapeKey, true);
const KeyCode KeyCode::Backspace (backspaceKey, true);
const KeyCode KeyCode::Delete (deleteKey, true);
const KeyCode KeyCode::Tab (tabKey, true);
const KeyCode KeyCode::LeftArrow (leftKey, true);
const KeyCode KeyCode::RightArrow (rightKey, true);
const KeyCode KeyCode::UpArrow (upKey, true);
const KeyCode KeyCode::DownArrow (downKey, true);
const KeyCode KeyCode::Home (homeKey, true);
const KeyCode KeyCode::End (endKey, true);
const KeyCode KeyCode::PageUp (pageUpKey, true);
const KeyCode KeyCode::PageDown (pageDownKey, true);
const KeyCode KeyCode::Insert (insertKey, true);

} // namespace juce 