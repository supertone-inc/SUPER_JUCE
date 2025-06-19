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
    Quick test functions for debugging global hotkeys.
    
    These functions provide simple ways to test hotkey functionality
    without creating a full application.
    
    @tags{GlobalHotkeys}
*/

/** 
    Quick test function that registers a simple hotkey and provides feedback.
    This is useful for debugging KeyCode conversion issues.
*/
inline void testSingleHotkey(const KeyCode& key, const ModifierKeys& modifiers, const String& description)
{
    std::cout << "\n=== Testing: " << description << " ===" << std::endl;
    
    // Show what we're trying to register
    std::cout << "Key code (JUCE): " << key.getJuceKeyCode() << std::endl;
    std::cout << "Key code (Platform): " << key.getPlatformKeyCode() << std::endl;
    std::cout << "Key description: " << key.toString() << std::endl;
    std::cout << "Modifiers (raw): " << modifiers.getRawFlags() << std::endl;
    std::cout << "Command: " << (modifiers.isCommandDown() ? "YES" : "NO") << std::endl;
    std::cout << "Shift: " << (modifiers.isShiftDown() ? "YES" : "NO") << std::endl;
    std::cout << "Alt: " << (modifiers.isAltDown() ? "YES" : "NO") << std::endl;
    std::cout << "Ctrl: " << (modifiers.isCtrlDown() ? "YES" : "NO") << std::endl;
    std::cout << std::endl;
    
    // Test registration
    bool callbackTriggered = false;
    auto callback = [&callbackTriggered, description]() {
        std::cout << "🎉 SUCCESS: " << description << " hotkey triggered!" << std::endl;
        callbackTriggered = true;
    };
    
    auto hotkey = std::make_unique<GlobalHotKey>(key, modifiers, callback);
    
    if (hotkey->isRegistered())
    {
        std::cout << "✅ Hotkey registered successfully!" << std::endl;
        std::cout << "Try pressing the hotkey now..." << std::endl;
        
        // Wait for user input
        std::cout << "Press Enter when you've tested the hotkey...";
        std::cin.get();
        
        if (callbackTriggered)
        {
            std::cout << "✅ Hotkey is working correctly!" << std::endl;
        }
        else
        {
            std::cout << "❌ Hotkey registered but callback was not triggered." << std::endl;
            std::cout << "This indicates a KeyCode conversion or event handling issue." << std::endl;
        }
    }
    else
    {
        std::cout << "❌ Failed to register hotkey!" << std::endl;
    }
    
    std::cout << "=== Test Complete ===" << std::endl;
}

/**
    Runs a series of quick tests with different key combinations.
*/
inline void runQuickTests()
{
    std::cout << "Running Quick Global HotKey Tests" << std::endl;
    std::cout << "==================================" << std::endl;
    
    // Test 1: Simple function key
    testSingleHotkey(KeyCode::F1, 
                    ModifierKeys(ModifierKeys::commandModifier | ModifierKeys::shiftModifier),
                    "Cmd+Shift+F1");
    
    // Test 2: Letter key
    testSingleHotkey(KeyCode::G, 
                    ModifierKeys(ModifierKeys::commandModifier | ModifierKeys::shiftModifier),
                    "Cmd+Shift+G");
    
    // Test 3: Number key
    testSingleHotkey(KeyCode::Key1, 
                    ModifierKeys(ModifierKeys::commandModifier),
                    "Cmd+1");
    
    // Test 4: Just Command modifier
    testSingleHotkey(KeyCode::F12, 
                    ModifierKeys(ModifierKeys::commandModifier),
                    "Cmd+F12");
    
    std::cout << "\nAll quick tests completed!" << std::endl;
}

/**
    Tests KeyCode conversion specifically.
*/
inline void testKeyCodeConversion()
{
    std::cout << "\n=== KeyCode Conversion Test ===" << std::endl;
    
    struct TestCase {
        KeyCode key;
        String description;
    };
    
    TestCase testCases[] = {
        { KeyCode::A, "Letter A" },
        { KeyCode::Z, "Letter Z" },
        { KeyCode::Key0, "Number 0" },
        { KeyCode::Key9, "Number 9" },
        { KeyCode::F1, "Function F1" },
        { KeyCode::F12, "Function F12" },
        { KeyCode::Space, "Space" },
        { KeyCode::Return, "Return" },
        { KeyCode::Escape, "Escape" }
    };
    
    for (const auto& testCase : testCases)
    {
        std::cout << testCase.description << ":" << std::endl;
        std::cout << "  JUCE Code: " << testCase.key.getJuceKeyCode() << std::endl;
        std::cout << "  Platform Code: " << testCase.key.getPlatformKeyCode() << std::endl;
        std::cout << "  String: " << testCase.key.toString() << std::endl;
        std::cout << std::endl;
    }
    
    std::cout << "=== Conversion Test Complete ===" << std::endl;
}

/**
    Tests modifier key combinations.
*/
inline void testModifierKeys()
{
    std::cout << "\n=== Modifier Keys Test ===" << std::endl;
    
    struct TestCase {
        ModifierKeys modifiers;
        String description;
    };
    
    TestCase testCases[] = {
        { ModifierKeys(ModifierKeys::commandModifier), "Command only" },
        { ModifierKeys(ModifierKeys::shiftModifier), "Shift only" },
        { ModifierKeys(ModifierKeys::altModifier), "Alt only" },
        { ModifierKeys(ModifierKeys::ctrlModifier), "Ctrl only" },
        { ModifierKeys(ModifierKeys::commandModifier | ModifierKeys::shiftModifier), "Command+Shift" },
        { ModifierKeys(ModifierKeys::commandModifier | ModifierKeys::altModifier), "Command+Alt" },
        { ModifierKeys(ModifierKeys::commandModifier | ModifierKeys::shiftModifier | ModifierKeys::altModifier), "Command+Shift+Alt" }
    };
    
    for (const auto& testCase : testCases)
    {
        std::cout << testCase.description << ":" << std::endl;
        std::cout << "  Raw flags: " << testCase.modifiers.getRawFlags() << std::endl;
        std::cout << "  Command: " << (testCase.modifiers.isCommandDown() ? "YES" : "NO") << std::endl;
        std::cout << "  Shift: " << (testCase.modifiers.isShiftDown() ? "YES" : "NO") << std::endl;
        std::cout << "  Alt: " << (testCase.modifiers.isAltDown() ? "YES" : "NO") << std::endl;
        std::cout << "  Ctrl: " << (testCase.modifiers.isCtrlDown() ? "YES" : "NO") << std::endl;
        std::cout << std::endl;
    }
    
    std::cout << "=== Modifier Test Complete ===" << std::endl;
} 