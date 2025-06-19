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
    Utility class for debugging global hotkey issues on macOS.
    
    This class provides methods to diagnose common problems with global hotkey
    registration and provides guidance on resolving them.
    
    @tags{GlobalHotkeys}
*/
class GlobalHotKeyDebugger
{
public:
    //==============================================================================
    /** Runs a comprehensive diagnostic check for global hotkey functionality. */
    static void runDiagnostics()
    {
        std::cout << "=== Global HotKey Diagnostics ===" << std::endl;
        std::cout << std::endl;
        
        // Check platform support
        checkPlatformSupport();
        
        // Check permissions
        checkPermissions();
        
        // Check for system conflicts
        checkSystemConflicts();
        
        // Test basic functionality
        testBasicFunctionality();
        
        std::cout << "=== Diagnostics Complete ===" << std::endl;
        std::cout << std::endl;
        
        printDebuggingTips();
    }
    
    /** Shows information about current permission status. */
    static void checkPermissions()
    {
        std::cout << "--- Permission Check ---" << std::endl;
        
        bool hasPermissions = GlobalHotKey::hasPermissions();
        std::cout << "Input Monitoring Permission: " << (hasPermissions ? "GRANTED" : "DENIED") << std::endl;
        
        if (!hasPermissions)
        {
            std::cout << "⚠️  INPUT MONITORING PERMISSION REQUIRED" << std::endl;
            std::cout << "To grant permission:" << std::endl;
            std::cout << "1. Open System Preferences" << std::endl;
            std::cout << "2. Go to Security & Privacy > Privacy" << std::endl;
            std::cout << "3. Select 'Input Monitoring' from the list" << std::endl;
            std::cout << "4. Add your application to the list" << std::endl;
            std::cout << "5. Restart your application" << std::endl;
            std::cout << std::endl;
            
            std::cout << "Or call GlobalHotKey::requestPermissions() to show the system dialog." << std::endl;
        }
        else
        {
            std::cout << "✅ Permission granted - hotkeys should work!" << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    /** Checks if the platform supports global hotkeys. */
    static void checkPlatformSupport()
    {
        std::cout << "--- Platform Support Check ---" << std::endl;
        
        bool isSupported = GlobalHotKey::isSupported();
        std::cout << "Platform Support: " << (isSupported ? "YES" : "NO") << std::endl;
        
        #if JUCE_MAC
        std::cout << "Platform: macOS" << std::endl;
        
        #if JUCE_GLOBAL_HOTKEYS_ENABLE_CARBON_API
        std::cout << "Carbon API: ENABLED" << std::endl;
        #else
        std::cout << "Carbon API: DISABLED" << std::endl;
        #endif
        
        #if JUCE_GLOBAL_HOTKEYS_ENABLE_CGEVENTTAP_API  
        std::cout << "CGEventTap API: ENABLED" << std::endl;
        #else
        std::cout << "CGEventTap API: DISABLED" << std::endl;
        #endif
        
        #endif
        
        std::cout << std::endl;
    }
    
    /** Tests basic hotkey registration and unregistration. */
    static void testBasicFunctionality()
    {
        std::cout << "--- Basic Functionality Test ---" << std::endl;
        
        // Test with a safe key combination (Cmd+Shift+F12)
        KeyCode testKey = KeyCode::F12;
        ModifierKeys testModifiers = ModifierKeys (ModifierKeys::commandModifier | ModifierKeys::shiftModifier);
        
        bool callbackTriggered = false;
        auto callback = [&callbackTriggered]() 
        { 
            callbackTriggered = true; 
            std::cout << "✅ Test hotkey callback triggered!" << std::endl;
        };
        
        auto hotkey = std::make_unique<GlobalHotKey>(testKey, testModifiers, callback);
        
        std::cout << "Attempting to register test hotkey (Cmd+Shift+F12)..." << std::endl;
        
        if (hotkey->isRegistered())
        {
            std::cout << "✅ Test hotkey registered successfully!" << std::endl;
            std::cout << "Try pressing Cmd+Shift+F12 to test..." << std::endl;
            
            // Wait a bit for user to test
            std::cout << "Waiting 5 seconds for test..." << std::endl;
            Thread::sleep(5000);
            
            if (callbackTriggered)
            {
                std::cout << "✅ Hotkey is working correctly!" << std::endl;
            }
            else
            {
                std::cout << "⚠️  Hotkey registered but callback not triggered." << std::endl;
                std::cout << "This could indicate a permissions or system conflict issue." << std::endl;
            }
        }
        else
        {
            std::cout << "❌ Failed to register test hotkey!" << std::endl;
            std::cout << "Check permissions and system conflicts." << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    /** Checks for common system conflicts. */
    static void checkSystemConflicts()
    {
        std::cout << "--- System Conflicts Check ---" << std::endl;
        std::cout << "Common hotkey conflicts to check:" << std::endl;
        std::cout << "1. System Preferences > Keyboard > Shortcuts" << std::endl;
        std::cout << "2. Third-party applications (Alfred, BetterTouchTool, etc.)" << std::endl;
        std::cout << "3. Accessibility applications" << std::endl;
        std::cout << "4. Window managers" << std::endl;
        std::cout << std::endl;
        
        std::cout << "To check for conflicts:" << std::endl;
        std::cout << "1. Open Console.app" << std::endl;
        std::cout << "2. Filter by your app name" << std::endl;
        std::cout << "3. Look for hotkey-related error messages" << std::endl;
        std::cout << std::endl;
    }
    
    /** Prints useful debugging tips. */
    static void printDebuggingTips()
    {
        std::cout << "=== Debugging Tips ===" << std::endl;
        std::cout << std::endl;
        
        std::cout << "1. ENABLE DEBUG LOGGING:" << std::endl;
        std::cout << "   Set JUCE_GLOBAL_HOTKEYS_DEBUG_LOGGING=1 in your project" << std::endl;
        std::cout << "   This will show detailed registration and event logs." << std::endl;
        std::cout << std::endl;
        
        std::cout << "2. CHECK CONSOLE LOGS:" << std::endl;
        std::cout << "   Open Console.app and filter by your application name" << std::endl;
        std::cout << "   Look for GlobalHotKey messages and errors" << std::endl;
        std::cout << std::endl;
        
        std::cout << "3. VERIFY PERMISSIONS:" << std::endl;
        std::cout << "   System Preferences > Security & Privacy > Privacy > Input Monitoring" << std::endl;
        std::cout << "   Your app should be listed and enabled" << std::endl;
        std::cout << std::endl;
        
        std::cout << "4. CHECK FOR CONFLICTS:" << std::endl;
        std::cout << "   Try different key combinations to isolate conflicts" << std::endl;
        std::cout << "   Temporarily disable other hotkey applications" << std::endl;
        std::cout << std::endl;
        
        std::cout << "5. TEST WITH SIMPLE KEYS:" << std::endl;
        std::cout << "   Start with Function keys (F1-F12) which are less likely to conflict" << std::endl;
        std::cout << "   Use multiple modifiers (Cmd+Shift+Alt+Key) to reduce conflicts" << std::endl;
        std::cout << std::endl;
        
        std::cout << "6. RESTART AFTER PERMISSION CHANGES:" << std::endl;
        std::cout << "   macOS may require a restart after granting Input Monitoring permission" << std::endl;
        std::cout << std::endl;
        
        std::cout << "7. CHECK SYSTEM VERSION:" << std::endl;
        std::cout << "   Some features may not work on newer macOS versions (Sequoia+)" << std::endl;
        std::cout << "   Try both Carbon and CGEventTap APIs" << std::endl;
        std::cout << std::endl;
    }
    
    /** Shows the current macOS version and compatibility info. */
    static void showSystemInfo()
    {
        std::cout << "--- System Information ---" << std::endl;
        
        #if JUCE_MAC
        auto osVersion = SystemStats::getOperatingSystemName();
        std::cout << "Operating System: " << osVersion << std::endl;
        
        // Check if this is a potentially problematic version
        if (osVersion.contains("15.") || osVersion.contains("Sequoia"))
        {
            std::cout << "⚠️  macOS Sequoia detected - some limitations may apply" << std::endl;
            std::cout << "Consider using Carbon API if CGEventTap fails" << std::endl;
        }
        #endif
        
        std::cout << std::endl;
    }
};

//==============================================================================
/**
    Example usage of the debugging utilities.
    
    Call this function from your main application to run diagnostics.
*/
inline void runGlobalHotKeyDiagnostics()
{
    std::cout << "Running Global HotKey Diagnostics..." << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << std::endl;
    
    GlobalHotKeyDebugger::showSystemInfo();
    GlobalHotKeyDebugger::runDiagnostics();
    
    std::cout << "If you're still having issues:" << std::endl;
    std::cout << "1. Check the debug logs in your IDE console" << std::endl;
    std::cout << "2. Open Console.app and look for error messages" << std::endl;
    std::cout << "3. Try different key combinations" << std::endl;
    std::cout << "4. Restart your application after granting permissions" << std::endl;
    std::cout << std::endl;
} 