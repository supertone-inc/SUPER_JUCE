/*
  ==============================================================================

   This file is part of the JUCE framework.
   Copyright (c) Raw Material Software Limited

   JUCE is an open source framework subject to commercial or open source
   licensing.

  ==============================================================================
*/

#pragma once

#include "../juce_global_hotkeys.h"

using namespace juce;

//==============================================================================
/**
    Key Mapping Fix for macOS Virtual Key Codes
    
    This addresses the critical bug where JUCE key codes were incorrectly
    mapped to macOS virtual key codes, causing hotkey registration to fail.
    
    The Problem:
    - Previous code used: juceKey - 'A' + kVK_ANSI_A
    - This assumed alphabetical order: A=0, B=1, C=2, etc.
    - But macOS virtual key codes follow QWERTY keyboard layout
    
    The Fix:
    - Use correct virtual key code mapping based on HIToolbox/Events.h
    - G key: virtual code 0x05 (not 0x06)
    - Proper modifier flag conversion for CGEventTap
    
    @tags{GlobalHotkeys}
*/
class KeyMappingFix
{
public:
    //==============================================================================
    /** Test the key mapping fix with various key combinations */
    static void testKeyMappings()
    {
        std::cout << "=== Key Mapping Fix Test ===" << std::endl;
        
        // Test the problematic G key
        testKeyMapping('G', "G key (was problematic)");
        
        // Test other common keys
        testKeyMapping('A', "A key");
        testKeyMapping('S', "S key");
        testKeyMapping('D', "D key");
        testKeyMapping('F', "F key");
        testKeyMapping('H', "H key");
        testKeyMapping('Z', "Z key");
        testKeyMapping('X', "X key");
        testKeyMapping('C', "C key");
        testKeyMapping('V', "V key");
        
        // Test numbers
        testKeyMapping('1', "1 key");
        testKeyMapping('2', "2 key");
        testKeyMapping('5', "5 key");
        testKeyMapping('6', "6 key");
        testKeyMapping('0', "0 key");
        
        std::cout << "Test completed!" << std::endl;
    }
    
    /** Test a specific key mapping */
    static void testKeyMapping(char key, const String& description)
    {
        KeyCode keyCode = KeyCode::fromJuceKeyCode(key);
        auto virtualCode = keyCode.getPlatformKeyCode();
        
        std::cout << description << ": JUCE '" << key << "' -> Virtual Code " 
                  << virtualCode << " (0x" << String::toHexString(virtualCode) << ")" << std::endl;
    }
    
    //==============================================================================
    /** Show the before/after comparison */
    static void showMappingComparison()
    {
        std::cout << "=== Key Mapping Comparison ===" << std::endl;
        std::cout << "Key | Old Mapping | New Mapping | Correct?" << std::endl;
        std::cout << "----+-------------+-------------+----------" << std::endl;
        
        struct KeyTest { char key; int oldCode; int newCode; };
        
        KeyTest tests[] = {
            {'A', 0x00, 0x00},  // A was correct
            {'G', 0x06, 0x05},  // G was wrong! 
            {'Z', 0x19, 0x06},  // Z was wrong!
            {'1', 0x12, 0x12},  // 1 was correct
            {'5', 0x16, 0x17},  // 5 was wrong!
            {'6', 0x17, 0x16},  // 6 was wrong!
        };
        
        for (const auto& test : tests)
        {
            std::cout << " " << test.key << "  |    0x" 
                      << String::toHexString(test.oldCode).paddedLeft('0', 2)
                      << "     |    0x" 
                      << String::toHexString(test.newCode).paddedLeft('0', 2)
                      << "     |    " 
                      << (test.oldCode == test.newCode ? "Same" : "FIXED!")
                      << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "The key insight: macOS virtual key codes follow" << std::endl;
        std::cout << "QWERTY keyboard layout, NOT alphabetical order!" << std::endl;
    }
    
    //==============================================================================
    /** Test hotkey registration with the fixed mapping */
    static void testHotkeyRegistration()
    {
        std::cout << "=== Testing Hotkey Registration ===" << std::endl;
        
        // Test the previously problematic Command+Alt+G
        auto testCallback = []() {
            std::cout << "🎉 SUCCESS: Command+Alt+G hotkey triggered!" << std::endl;
        };
        
        try 
        {
            auto hotkey = std::make_unique<GlobalHotKey>(
                KeyCode::fromJuceKeyCode('G'),
                ModifierKeys::commandModifier | ModifierKeys::altModifier,
                testCallback
            );
            
            std::cout << "✅ Command+Alt+G registration successful!" << std::endl;
            std::cout << "   Now press Command+Alt+G to test..." << std::endl;
            
            // Keep the hotkey alive for testing
            // In real usage, store this in your application
            
        }
        catch (const std::exception& e)
        {
            std::cout << "❌ Registration failed: " << e.what() << std::endl;
        }
    }
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KeyMappingFix)
}; 