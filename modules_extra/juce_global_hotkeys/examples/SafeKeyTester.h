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
    Tests safe key combinations that don't conflict with macOS system shortcuts.
    
    This helps identify which key combinations work reliably on macOS without
    being intercepted by the system.
    
    @tags{GlobalHotkeys}
*/
class SafeKeyTester
{
public:
    //==============================================================================
    /** Tests a series of safe key combinations that typically don't conflict with system shortcuts. */
    static void testSafeKeyCombinations()
    {
        std::cout << "Testing Safe Key Combinations for macOS" << std::endl;
        std::cout << "=======================================" << std::endl;
        std::cout << "Note: These combinations are designed to avoid system shortcut conflicts." << std::endl;
        std::cout << std::endl;
        
        struct SafeCombo {
            KeyCode key;
            ModifierKeys modifiers;
            String description;
            String reasoning;
        };
        
        SafeCombo safeCombos[] = {
            // Function keys with multiple modifiers (very safe)
            { KeyCode::F1, ModifierKeys(ModifierKeys::commandModifier | ModifierKeys::shiftModifier), 
              "Cmd+Shift+F1", "Function keys with 2+ modifiers rarely conflict" },
            
            { KeyCode::F2, ModifierKeys(ModifierKeys::commandModifier | ModifierKeys::altModifier), 
              "Cmd+Alt+F2", "Alt modifier reduces conflicts" },
            
            { KeyCode::F12, ModifierKeys(ModifierKeys::commandModifier | ModifierKeys::shiftModifier | ModifierKeys::altModifier), 
              "Cmd+Shift+Alt+F12", "Triple modifier is very safe" },
            
            // Letters with triple modifiers (safe)
            { KeyCode::G, ModifierKeys(ModifierKeys::commandModifier | ModifierKeys::shiftModifier | ModifierKeys::altModifier), 
              "Cmd+Shift+Alt+G", "Triple modifier avoids Cmd+Shift+G conflict" },
            
            { KeyCode::H, ModifierKeys(ModifierKeys::commandModifier | ModifierKeys::ctrlModifier), 
              "Cmd+Ctrl+H", "Ctrl modifier avoids Cmd+H conflict" },
            
            // Numbers with modifiers (usually safe)
            { KeyCode::Key1, ModifierKeys(ModifierKeys::commandModifier | ModifierKeys::shiftModifier), 
              "Cmd+Shift+1", "Numbers with modifiers rarely conflict" },
            
            { KeyCode::Key9, ModifierKeys(ModifierKeys::commandModifier | ModifierKeys::altModifier), 
              "Cmd+Alt+9", "High numbers less likely to conflict" },
            
            // Special keys (often safe)
            { KeyCode::Space, ModifierKeys(ModifierKeys::commandModifier | ModifierKeys::shiftModifier | ModifierKeys::altModifier), 
              "Cmd+Shift+Alt+Space", "Triple modifier makes Space safe" },
            
            { KeyCode::Return, ModifierKeys(ModifierKeys::commandModifier | ModifierKeys::altModifier), 
              "Cmd+Alt+Return", "Alt+Return combinations usually available" },
            
            // Arrow keys with multiple modifiers
            { KeyCode::LeftArrow, ModifierKeys(ModifierKeys::commandModifier | ModifierKeys::shiftModifier | ModifierKeys::altModifier), 
              "Cmd+Shift+Alt+Left", "Triple modifier arrow keys are safe" }
        };
        
        for (const auto& combo : safeCombos)
        {
            testSingleCombination(combo.key, combo.modifiers, combo.description, combo.reasoning);
            std::cout << std::endl;
        }
        
        std::cout << "=== Testing Complete ===" << std::endl;
        showConflictingCombinations();
    }
    
    /** Shows known conflicting combinations to avoid. */
    static void showConflictingCombinations()
    {
        std::cout << "\n=== Known Conflicting Combinations (AVOID) ===" << std::endl;
        
        struct ConflictingCombo {
            String combination;
            String systemFunction;
        };
        
        ConflictingCombo conflicts[] = {
            { "Cmd+G", "Find Next" },
            { "Cmd+Shift+G", "Find Previous / Go to Folder" },
            { "Cmd+H", "Hide Application" },
            { "Cmd+M", "Minimize Window" },
            { "Cmd+Q", "Quit Application" },
            { "Cmd+W", "Close Window" },
            { "Cmd+Tab", "Application Switcher" },
            { "Cmd+Space", "Spotlight Search" },
            { "Cmd+Shift+3", "Screenshot" },
            { "Cmd+Shift+4", "Screenshot Selection" },
            { "Cmd+Shift+5", "Screenshot Utility" },
            { "Cmd+F", "Find" },
            { "Cmd+A", "Select All" },
            { "Cmd+C", "Copy" },
            { "Cmd+V", "Paste" },
            { "Cmd+X", "Cut" },
            { "Cmd+Z", "Undo" },
            { "Cmd+Shift+Z", "Redo" },
            { "Cmd+S", "Save" },
            { "Cmd+O", "Open" },
            { "Cmd+N", "New" },
            { "Cmd+P", "Print" }
        };
        
        for (const auto& conflict : conflicts)
        {
            std::cout << "❌ " << conflict.combination << " - " << conflict.systemFunction << std::endl;
        }
        
        std::cout << "\n💡 Tips for avoiding conflicts:" << std::endl;
        std::cout << "1. Use Function keys (F1-F12) with modifiers" << std::endl;
        std::cout << "2. Add extra modifiers (Ctrl, Alt) to common combinations" << std::endl;
        std::cout << "3. Use numbers (especially 6-9) with modifiers" << std::endl;
        std::cout << "4. Test thoroughly on target macOS versions" << std::endl;
        std::cout << "5. Provide alternative key combinations for users" << std::endl;
    }
    
    /** Tests a single key combination and reports results. */
    static void testSingleCombination(const KeyCode& key, const ModifierKeys& modifiers, 
                                    const String& description, const String& reasoning)
    {
        std::cout << "Testing: " << description << std::endl;
        std::cout << "Reasoning: " << reasoning << std::endl;
        
        bool callbackTriggered = false;
        auto callback = [&callbackTriggered, description]() {
            std::cout << "✅ SUCCESS: " << description << " callback triggered!" << std::endl;
            callbackTriggered = true;
        };
        
        auto hotkey = std::make_unique<GlobalHotKey>(key, modifiers, callback);
        
        if (hotkey->isRegistered())
        {
            std::cout << "✅ Registration: SUCCESS" << std::endl;
            std::cout << "Press " << description << " to test, then press Enter...";
            std::cin.get();
            
            if (callbackTriggered)
            {
                std::cout << "✅ Trigger: SUCCESS - This combination works!" << std::endl;
            }
            else
            {
                std::cout << "❌ Trigger: FAILED - Likely system conflict" << std::endl;
            }
        }
        else
        {
            std::cout << "❌ Registration: FAILED" << std::endl;
        }
    }
    
    /** Provides recommendations for safe key combinations. */
    static void showRecommendations()
    {
        std::cout << "\n=== Recommended Safe Combinations ===" << std::endl;
        std::cout << "For your application, consider these alternatives to Cmd+Shift+G:" << std::endl;
        std::cout << std::endl;
        
        std::cout << "🟢 HIGHLY RECOMMENDED:" << std::endl;
        std::cout << "  • Cmd+Shift+F1 through F12" << std::endl;
        std::cout << "  • Cmd+Alt+[Letter]" << std::endl;
        std::cout << "  • Cmd+Shift+Alt+[Letter]" << std::endl;
        std::cout << std::endl;
        
        std::cout << "🟡 MODERATELY SAFE:" << std::endl;
        std::cout << "  • Cmd+Ctrl+[Letter]" << std::endl;
        std::cout << "  • Cmd+Shift+[Number 6-9]" << std::endl;
        std::cout << "  • Cmd+Alt+[Number]" << std::endl;
        std::cout << std::endl;
        
        std::cout << "🔴 AVOID:" << std::endl;
        std::cout << "  • Any single Cmd+[Key] combinations" << std::endl;
        std::cout << "  • Cmd+Shift+[Common letters like G, A, C, etc.]" << std::endl;
        std::cout << "  • System screenshot combinations" << std::endl;
        std::cout << std::endl;
        
        std::cout << "💡 SPECIFIC ALTERNATIVES TO Cmd+Shift+G:" << std::endl;
        std::cout << "  • Cmd+Shift+Alt+G (adds Alt modifier)" << std::endl;
        std::cout << "  • Cmd+Ctrl+G (uses Ctrl instead of Shift)" << std::endl;
        std::cout << "  • Cmd+Shift+F1 (uses Function key)" << std::endl;
        std::cout << "  • Cmd+Alt+G (uses Alt instead of Shift)" << std::endl;
    }
};

//==============================================================================
/** Quick test function for the problematic Cmd+Shift+G combination. */
inline void testProblematicCombination()
{
    std::cout << "Testing the Problematic Combination: Cmd+Shift+G" << std::endl;
    std::cout << "================================================" << std::endl;
    std::cout << "This will demonstrate why Cmd+Shift+G doesn't work." << std::endl;
    std::cout << std::endl;
    
    bool callbackTriggered = false;
    auto callback = [&callbackTriggered]() {
        std::cout << "🎉 Cmd+Shift+G callback triggered!" << std::endl;
        callbackTriggered = true;
    };
    
    auto hotkey = std::make_unique<GlobalHotKey>(
        KeyCode::G,
        ModifierKeys(ModifierKeys::commandModifier | ModifierKeys::shiftModifier),
        callback
    );
    
    if (hotkey->isRegistered())
    {
        std::cout << "✅ Hotkey registered successfully (this is expected)" << std::endl;
        std::cout << "Now press Cmd+Shift+G..." << std::endl;
        std::cout << "You'll likely see a 'Go to Folder' dialog instead of our callback." << std::endl;
        std::cout << "Press Enter when done testing...";
        std::cin.get();
        
        if (callbackTriggered)
        {
            std::cout << "😲 Surprising! The callback was triggered." << std::endl;
            std::cout << "This might work on your system configuration." << std::endl;
        }
        else
        {
            std::cout << "❌ As expected: callback was NOT triggered." << std::endl;
            std::cout << "The system intercepted Cmd+Shift+G for 'Go to Folder'." << std::endl;
        }
    }
    else
    {
        std::cout << "❌ Registration failed (this would be unusual)" << std::endl;
    }
    
    std::cout << std::endl;
    SafeKeyTester::showRecommendations();
} 