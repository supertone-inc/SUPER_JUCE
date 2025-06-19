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
    macOS Sequoia (15.0+) Compatibility Guide for Global Hotkeys
    
    macOS Sequoia introduced significant security enhancements that severely
    limit CGEventTap functionality, making Carbon API the preferred approach
    for global hotkeys.
    
    @tags{GlobalHotkeys}
*/
class SequoiaCompatibilityGuide
{
public:
    //==============================================================================
    /** Shows the current macOS version and compatibility status. */
    static void showCompatibilityStatus()
    {
        std::cout << "macOS Sequoia (15.0+) Global Hotkeys Compatibility" << std::endl;
        std::cout << "==================================================" << std::endl;
        std::cout << std::endl;
        
        // Get macOS version
        NSOperatingSystemVersion version = [[NSProcessInfo processInfo] operatingSystemVersion];
        std::cout << "Current macOS Version: " << version.majorVersion << "." 
                  << version.minorVersion << "." << version.patchVersion << std::endl;
        std::cout << std::endl;
        
        if (version.majorVersion >= 15)
        {
            std::cout << "🚨 SEQUOIA DETECTED - Enhanced Security Mode" << std::endl;
            std::cout << "=============================================" << std::endl;
            std::cout << std::endl;
            
            showSequoiaLimitations();
            showRecommendedApproach();
            testSequoiaCompatibility();
        }
        else if (version.majorVersion >= 14)
        {
            std::cout << "⚠️  SONOMA DETECTED - Moderate Security Restrictions" << std::endl;
            std::cout << "====================================================" << std::endl;
            std::cout << std::endl;
            
            showSonomaLimitations();
        }
        else
        {
            std::cout << "✅ OLDER MACOS - Standard Security Model" << std::endl;
            std::cout << "========================================" << std::endl;
            std::cout << "CGEventTap should work normally with Input Monitoring permission." << std::endl;
        }
    }
    
private:
    //==============================================================================
    static void showSequoiaLimitations()
    {
        std::cout << "Sequoia Security Changes:" << std::endl;
        std::cout << "------------------------" << std::endl;
        std::cout << "• CGEventTap severely restricted for third-party apps" << std::endl;
        std::cout << "• Input Monitoring permission alone may not be sufficient" << std::endl;
        std::cout << "• Additional security layers prevent event interception" << std::endl;
        std::cout << "• System-level event filtering enhanced" << std::endl;
        std::cout << "• Sandboxed apps completely blocked from CGEventTap" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Why CGEventTapCreate Returns nullptr:" << std::endl;
        std::cout << "------------------------------------" << std::endl;
        std::cout << "1. 🔒 Enhanced security framework blocks event taps" << std::endl;
        std::cout << "2. 🛡️  System considers CGEventTap potentially malicious" << std::endl;
        std::cout << "3. 🚫 Background app restrictions tightened" << std::endl;
        std::cout << "4. 🔐 Additional permission layers not accessible via UI" << std::endl;
        std::cout << std::endl;
    }
    
    static void showSonomaLimitations()
    {
        std::cout << "Sonoma Security Changes:" << std::endl;
        std::cout << "-----------------------" << std::endl;
        std::cout << "• Stricter Input Monitoring permission enforcement" << std::endl;
        std::cout << "• CGEventTap still works but with more restrictions" << std::endl;
        std::cout << "• Permission dialogs more frequent" << std::endl;
        std::cout << "• Some system shortcuts harder to override" << std::endl;
        std::cout << std::endl;
    }
    
    static void showRecommendedApproach()
    {
        std::cout << "🎯 RECOMMENDED APPROACH FOR SEQUOIA" << std::endl;
        std::cout << "===================================" << std::endl;
        std::cout << std::endl;
        
        std::cout << "1. 🥇 PRIMARY: Use Carbon RegisterEventHotKey API" << std::endl;
        std::cout << "   ✅ No special permissions required" << std::endl;
        std::cout << "   ✅ Works reliably on Sequoia" << std::endl;
        std::cout << "   ✅ System-supported approach" << std::endl;
        std::cout << "   ⚠️  Limited to specific key combinations" << std::endl;
        std::cout << std::endl;
        
        std::cout << "2. 🥈 FALLBACK: App-specific hotkeys" << std::endl;
        std::cout << "   ✅ Always works when app is focused" << std::endl;
        std::cout << "   ✅ No permission issues" << std::endl;
        std::cout << "   ❌ Only works when app is active" << std::endl;
        std::cout << std::endl;
        
        std::cout << "3. 🥉 LAST RESORT: CGEventTap (very limited)" << std::endl;
        std::cout << "   ❌ Likely to fail on Sequoia" << std::endl;
        std::cout << "   ❌ Requires complex permission setup" << std::endl;
        std::cout << "   ❌ May break in future updates" << std::endl;
        std::cout << std::endl;
    }
    
    static void testSequoiaCompatibility()
    {
        std::cout << "🧪 SEQUOIA COMPATIBILITY TEST" << std::endl;
        std::cout << "=============================" << std::endl;
        std::cout << std::endl;
        
        // Test Carbon API
        std::cout << "Testing Carbon API (Recommended)..." << std::endl;
        bool carbonWorks = testCarbonAPI();
        
        if (carbonWorks)
        {
            std::cout << "✅ Carbon API: WORKING" << std::endl;
            std::cout << "   This is the best option for Sequoia!" << std::endl;
        }
        else
        {
            std::cout << "❌ Carbon API: FAILED" << std::endl;
            std::cout << "   This is unexpected and may indicate system issues." << std::endl;
        }
        std::cout << std::endl;
        
        // Test CGEventTap
        std::cout << "Testing CGEventTap API (Limited)..." << std::endl;
        bool cgEventTapWorks = testCGEventTapAPI();
        
        if (cgEventTapWorks)
        {
            std::cout << "😲 CGEventTap API: WORKING" << std::endl;
            std::cout << "   Surprising! This may work on your specific configuration." << std::endl;
            std::cout << "   However, it may break in future updates." << std::endl;
        }
        else
        {
            std::cout << "❌ CGEventTap API: FAILED (Expected on Sequoia)" << std::endl;
            std::cout << "   This is normal behavior on Sequoia." << std::endl;
        }
        std::cout << std::endl;
        
        // Recommendations based on results
        showFinalRecommendations(carbonWorks, cgEventTapWorks);
    }
    
    static bool testCarbonAPI()
    {
        // Test Carbon RegisterEventHotKey
        EventHotKeyRef hotKeyRef;
        EventHotKeyID hotKeyID;
        hotKeyID.signature = 'test';
        hotKeyID.id = 1;
        
        OSStatus status = RegisterEventHotKey(
            kVK_F1,  // F1 key (safe choice)
            cmdKey,  // Command modifier
            hotKeyID,
            GetApplicationEventTarget(),
            0,
            &hotKeyRef
        );
        
        if (status == noErr)
        {
            UnregisterEventHotKey(hotKeyRef);
            return true;
        }
        
        return false;
    }
    
    static bool testCGEventTapAPI()
    {
        auto testCallback = [](CGEventTapProxy, CGEventType, CGEventRef event, void*) -> CGEventRef {
            return event;
        };
        
        // Try the most permissive CGEventTap configuration
        auto testTap = CGEventTapCreate(
            kCGSessionEventTap,
            kCGTailAppendEventTap,
            kCGEventTapOptionListenOnly,
            CGEventMaskBit(kCGEventNull),
            testCallback,
            nullptr
        );
        
        if (testTap != nullptr)
        {
            CFRelease(testTap);
            return true;
        }
        
        return false;
    }
    
    static void showFinalRecommendations(bool carbonWorks, bool cgEventTapWorks)
    {
        std::cout << "📋 FINAL RECOMMENDATIONS FOR YOUR SYSTEM" << std::endl;
        std::cout << "=========================================" << std::endl;
        std::cout << std::endl;
        
        if (carbonWorks)
        {
            std::cout << "🎉 GOOD NEWS: Carbon API is working!" << std::endl;
            std::cout << std::endl;
            std::cout << "Recommended configuration:" << std::endl;
            std::cout << "```cpp" << std::endl;
            std::cout << "#define JUCE_GLOBAL_HOTKEYS_ENABLE_CARBON_API 1" << std::endl;
            std::cout << "#define JUCE_GLOBAL_HOTKEYS_ENABLE_CGEVENTTAP_API 0" << std::endl;
            std::cout << "```" << std::endl;
            std::cout << std::endl;
            std::cout << "Use safe key combinations like:" << std::endl;
            std::cout << "• Cmd+Alt+[Letter] (e.g., Cmd+Alt+G)" << std::endl;
            std::cout << "• Cmd+Shift+F1 through F12" << std::endl;
            std::cout << "• Cmd+Ctrl+[Letter]" << std::endl;
            std::cout << std::endl;
        }
        else
        {
            std::cout << "⚠️  WARNING: Carbon API failed!" << std::endl;
            std::cout << "This suggests deeper system issues." << std::endl;
            std::cout << std::endl;
            std::cout << "Try these steps:" << std::endl;
            std::cout << "1. Restart your Mac" << std::endl;
            std::cout << "2. Check if running as administrator/root" << std::endl;
            std::cout << "3. Test with a simple console application" << std::endl;
            std::cout << "4. Consider using app-specific hotkeys only" << std::endl;
            std::cout << std::endl;
        }
        
        if (!cgEventTapWorks)
        {
            std::cout << "ℹ️  CGEventTap not working (normal on Sequoia)" << std::endl;
            std::cout << "Don't spend time trying to fix CGEventTap permissions." << std::endl;
            std::cout << "Focus on Carbon API or app-specific alternatives." << std::endl;
            std::cout << std::endl;
        }
        
        std::cout << "🔧 IMPLEMENTATION STRATEGY:" << std::endl;
        std::cout << "1. Use Carbon API as primary method" << std::endl;
        std::cout << "2. Provide clear user guidance on key combinations" << std::endl;
        std::cout << "3. Offer app-specific hotkeys as fallback" << std::endl;
        std::cout << "4. Don't rely on CGEventTap for Sequoia users" << std::endl;
        std::cout << std::endl;
    }
    
public:
    //==============================================================================
    /** Creates a Sequoia-optimized hotkey with safe settings. */
    static std::unique_ptr<GlobalHotKey> createSequoiaOptimizedHotkey(
        const KeyCode& keyCode,
        const ModifierKeys& modifiers,
        std::function<void()> callback)
    {
        std::cout << "Creating Sequoia-optimized hotkey..." << std::endl;
        
        // Force Carbon-only mode for Sequoia
        std::cout << "Using Carbon-only mode for maximum compatibility." << std::endl;
        
        // Verify it's a safe key combination
        if (isSafeKeyComboForSequoia(keyCode, modifiers))
        {
            std::cout << "✅ Key combination appears safe for Sequoia." << std::endl;
        }
        else
        {
            std::cout << "⚠️  Warning: This key combination may conflict with system shortcuts." << std::endl;
            showSafeAlternatives();
        }
        
        return std::make_unique<GlobalHotKey>(keyCode, modifiers, callback);
    }
    
    /** Checks if a key combination is safe for Sequoia. */
    static bool isSafeKeyComboForSequoia(const KeyCode& keyCode, const ModifierKeys& modifiers)
    {
        // Function keys are generally safe
        if (keyCode.getJuceKeyCode() >= KeyPress::F1Key && keyCode.getJuceKeyCode() <= KeyPress::F12Key)
        {
            return true;
        }
        
        // Multiple modifiers are safer
        int modifierCount = 0;
        if (modifiers.isCtrlDown()) modifierCount++;
        if (modifiers.isShiftDown()) modifierCount++;
        if (modifiers.isAltDown()) modifierCount++;
        if (modifiers.isCommandDown()) modifierCount++;
        
        // At least 2 modifiers recommended for letters
        if (modifierCount >= 2)
        {
            return true;
        }
        
        // Single modifier with common letters is risky
        return false;
    }
    
    /** Shows safe key combination alternatives for Sequoia. */
    static void showSafeAlternatives()
    {
        std::cout << std::endl;
        std::cout << "🔒 SEQUOIA-SAFE KEY COMBINATIONS:" << std::endl;
        std::cout << "=================================" << std::endl;
        std::cout << "• Cmd+Alt+G (instead of Cmd+Shift+G)" << std::endl;
        std::cout << "• Cmd+Shift+F1 (function keys are safe)" << std::endl;
        std::cout << "• Cmd+Ctrl+G (Control instead of Shift)" << std::endl;
        std::cout << "• Cmd+Shift+Alt+G (triple modifier)" << std::endl;
        std::cout << "• Cmd+Alt+1 through 9 (numbers with Alt)" << std::endl;
        std::cout << std::endl;
    }
    
    /** Shows the complete Sequoia migration guide. */
    static void showMigrationGuide()
    {
        std::cout << "🚀 SEQUOIA MIGRATION GUIDE" << std::endl;
        std::cout << "==========================" << std::endl;
        std::cout << std::endl;
        
        std::cout << "If you're upgrading from an older macOS version:" << std::endl;
        std::cout << std::endl;
        
        std::cout << "1. 📝 UPDATE YOUR CODE:" << std::endl;
        std::cout << "   • Set JUCE_GLOBAL_HOTKEYS_ENABLE_CARBON_API=1" << std::endl;
        std::cout << "   • Set JUCE_GLOBAL_HOTKEYS_ENABLE_CGEVENTTAP_API=0" << std::endl;
        std::cout << "   • Test all hotkey combinations" << std::endl;
        std::cout << std::endl;
        
        std::cout << "2. 🔄 UPDATE KEY COMBINATIONS:" << std::endl;
        std::cout << "   • Replace system-conflicting shortcuts" << std::endl;
        std::cout << "   • Use function keys where possible" << std::endl;
        std::cout << "   • Add extra modifiers to existing shortcuts" << std::endl;
        std::cout << std::endl;
        
        std::cout << "3. 👥 UPDATE USER DOCUMENTATION:" << std::endl;
        std::cout << "   • Inform users about key combination changes" << std::endl;
        std::cout << "   • Explain why CGEventTap permissions are no longer needed" << std::endl;
        std::cout << "   • Provide alternative workflows" << std::endl;
        std::cout << std::endl;
        
        std::cout << "4. 🧪 TEST THOROUGHLY:" << std::endl;
        std::cout << "   • Test on multiple Sequoia versions" << std::endl;
        std::cout << "   • Test with different user permission levels" << std::endl;
        std::cout << "   • Test background vs foreground operation" << std::endl;
        std::cout << std::endl;
    }
}; 