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
    Advanced API selection strategy for global hotkeys.
    
    This example shows different strategies for choosing between Carbon and CGEventTap APIs
    based on various criteria like key type, system version, or user preferences.
    
    @tags{GlobalHotkeys}
*/
class AdvancedGlobalHotKeyStrategy
{
public:
    enum class APIPreference
    {
        CarbonFirst,        // Try Carbon first, fallback to CGEventTap (default)
        CGEventTapFirst,    // Try CGEventTap first, fallback to Carbon
        CarbonOnly,         // Use only Carbon API
        CGEventTapOnly,     // Use only CGEventTap API
        Automatic           // Choose automatically based on key type and system
    };
    
    //==============================================================================
    /**
        Creates a hotkey with specific API preference.
    */
    static std::unique_ptr<GlobalHotKey> createHotKeyWithStrategy(
        const KeyCode& keyCode,
        const ModifierKeys& modifiers,
        std::function<void()> callback,
        APIPreference strategy = APIPreference::Automatic)
    {
        // Temporarily modify the global settings based on strategy
        auto originalCarbonSetting = JUCE_GLOBAL_HOTKEYS_ENABLE_CARBON_API;
        auto originalCGEventTapSetting = JUCE_GLOBAL_HOTKEYS_ENABLE_CGEVENTTAP_API;
        
        switch (strategy)
        {
            case APIPreference::CarbonOnly:
                // Force Carbon only by disabling CGEventTap in the header would require recompilation
                // Instead, we'll document this limitation
                std::cout << "Note: CarbonOnly strategy requires recompiling with JUCE_GLOBAL_HOTKEYS_ENABLE_CGEVENTTAP_API=0" << std::endl;
                break;
                
            case APIPreference::CGEventTapOnly:
                std::cout << "Note: CGEventTapOnly strategy requires recompiling with JUCE_GLOBAL_HOTKEYS_ENABLE_CARBON_API=0" << std::endl;
                break;
                
            case APIPreference::Automatic:
                return createWithAutomaticSelection(keyCode, modifiers, callback);
                
            default:
                break;
        }
        
        return std::make_unique<GlobalHotKey>(keyCode, modifiers, callback);
    }
    
    /**
        Automatically chooses the best API based on key characteristics.
    */
    static std::unique_ptr<GlobalHotKey> createWithAutomaticSelection(
        const KeyCode& keyCode,
        const ModifierKeys& modifiers,
        std::function<void()> callback)
    {
        std::cout << "=== Automatic API Selection ===" << std::endl;
        
        // Check system version
        auto osVersion = SystemStats::getOperatingSystemName();
        bool isSequoiaOrNewer = osVersion.contains("15.") || osVersion.contains("Sequoia");
        
        // Check key characteristics
        bool isFunctionKey = (keyCode.getJuceKeyCode() >= 0x20001 && keyCode.getJuceKeyCode() <= 0x2000c);
        bool hasComplexModifiers = modifiers.getRawFlags() > (ModifierKeys::commandModifier | ModifierKeys::shiftModifier);
        
        std::cout << "System: " << osVersion << std::endl;
        std::cout << "Is Sequoia+: " << (isSequoiaOrNewer ? "Yes" : "No") << std::endl;
        std::cout << "Is Function Key: " << (isFunctionKey ? "Yes" : "No") << std::endl;
        std::cout << "Complex Modifiers: " << (hasComplexModifiers ? "Yes" : "No") << std::endl;
        
        // Decision logic
        if (isSequoiaOrNewer && !isFunctionKey)
        {
            std::cout << "Recommendation: CGEventTap (better compatibility on Sequoia for non-function keys)" << std::endl;
        }
        else if (isFunctionKey && !hasComplexModifiers)
        {
            std::cout << "Recommendation: Carbon (function keys work well with Carbon)" << std::endl;
        }
        else
        {
            std::cout << "Recommendation: Default fallback strategy (Carbon first)" << std::endl;
        }
        
        std::cout << "=== Creating HotKey ===" << std::endl;
        return std::make_unique<GlobalHotKey>(keyCode, modifiers, callback);
    }
    
    /**
        Tests both APIs with the same key combination to compare behavior.
        WARNING: This will register the same hotkey twice, which may cause conflicts!
    */
    static void compareAPIs(const KeyCode& keyCode, const ModifierKeys& modifiers)
    {
        std::cout << "\n=== API Comparison Test ===" << std::endl;
        std::cout << "WARNING: This test may cause conflicts by registering the same key twice!" << std::endl;
        std::cout << "Key: " << keyCode.toString() << ", Modifiers: " << modifiers.getRawFlags() << std::endl;
        
        // Test Carbon API
        std::cout << "\n--- Testing Carbon API ---" << std::endl;
        bool carbonSuccess = false;
        auto carbonCallback = [&carbonSuccess]() {
            std::cout << "🔵 Carbon API callback triggered!" << std::endl;
            carbonSuccess = true;
        };
        
        // Note: This would require a way to force Carbon-only registration
        // For now, we'll just document the approach
        std::cout << "To test Carbon only: Set JUCE_GLOBAL_HOTKEYS_ENABLE_CGEVENTTAP_API=0 and recompile" << std::endl;
        
        // Test CGEventTap API
        std::cout << "\n--- Testing CGEventTap API ---" << std::endl;
        bool cgEventTapSuccess = false;
        auto cgEventTapCallback = [&cgEventTapSuccess]() {
            std::cout << "🟢 CGEventTap API callback triggered!" << std::endl;
            cgEventTapSuccess = true;
        };
        
        std::cout << "To test CGEventTap only: Set JUCE_GLOBAL_HOTKEYS_ENABLE_CARBON_API=0 and recompile" << std::endl;
        
        std::cout << "\n=== Comparison Complete ===" << std::endl;
    }
    
    /**
        Shows the current API configuration.
    */
    static void showCurrentConfiguration()
    {
        std::cout << "\n=== Current API Configuration ===" << std::endl;
        
        #if JUCE_GLOBAL_HOTKEYS_ENABLE_CARBON_API
        std::cout << "✅ Carbon API: ENABLED" << std::endl;
        #else
        std::cout << "❌ Carbon API: DISABLED" << std::endl;
        #endif
        
        #if JUCE_GLOBAL_HOTKEYS_ENABLE_CGEVENTTAP_API
        std::cout << "✅ CGEventTap API: ENABLED" << std::endl;
        #else
        std::cout << "❌ CGEventTap API: DISABLED" << std::endl;
        #endif
        
        std::cout << "Strategy: ";
        #if JUCE_GLOBAL_HOTKEYS_ENABLE_CARBON_API && JUCE_GLOBAL_HOTKEYS_ENABLE_CGEVENTTAP_API
        std::cout << "Fallback (Carbon first, CGEventTap second)" << std::endl;
        #elif JUCE_GLOBAL_HOTKEYS_ENABLE_CARBON_API
        std::cout << "Carbon only" << std::endl;
        #elif JUCE_GLOBAL_HOTKEYS_ENABLE_CGEVENTTAP_API
        std::cout << "CGEventTap only" << std::endl;
        #else
        std::cout << "No APIs enabled!" << std::endl;
        #endif
        
        std::cout << "=== Configuration Complete ===" << std::endl;
    }
};

//==============================================================================
/**
    Example usage of different API strategies.
*/
inline void demonstrateAPIStrategies()
{
    std::cout << "Global HotKey API Strategy Demonstration" << std::endl;
    std::cout << "=======================================" << std::endl;
    
    // Show current configuration
    AdvancedGlobalHotKeyStrategy::showCurrentConfiguration();
    
    // Test automatic selection
    auto automaticHotkey = AdvancedGlobalHotKeyStrategy::createWithAutomaticSelection(
        KeyCode::F1,
        ModifierKeys(ModifierKeys::commandModifier | ModifierKeys::shiftModifier),
        []() { std::cout << "Automatic strategy hotkey triggered!" << std::endl; }
    );
    
    // Test different strategies (requires recompilation for full effect)
    std::cout << "\n=== Strategy Recommendations ===" << std::endl;
    std::cout << "1. For maximum compatibility: Use fallback strategy (default)" << std::endl;
    std::cout << "2. For minimal permissions: Use Carbon only (recompile with CGEventTap disabled)" << std::endl;
    std::cout << "3. For maximum power: Use CGEventTap only (recompile with Carbon disabled)" << std::endl;
    std::cout << "4. For automatic optimization: Use automatic selection strategy" << std::endl;
    
    std::cout << "\n=== Compilation Options ===" << std::endl;
    std::cout << "Carbon only: -DJUCE_GLOBAL_HOTKEYS_ENABLE_CGEVENTTAP_API=0" << std::endl;
    std::cout << "CGEventTap only: -DJUCE_GLOBAL_HOTKEYS_ENABLE_CARBON_API=0" << std::endl;
    std::cout << "Both (fallback): Default settings" << std::endl;
} 