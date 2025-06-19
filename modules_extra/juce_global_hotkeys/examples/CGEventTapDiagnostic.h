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
#include <CoreGraphics/CoreGraphics.h>
#include <ApplicationServices/ApplicationServices.h>
#include <AppKit/AppKit.h>

using namespace juce;

//==============================================================================
/**
    Comprehensive diagnostic tool for CGEventTapCreate failures.
    
    This helps identify why CGEventTapCreate returns nullptr even when
    Input Monitoring permissions appear to be granted.
    
    @tags{GlobalHotkeys}
*/
class CGEventTapDiagnostic
{
public:
    //==============================================================================
    /** Runs a comprehensive diagnostic of CGEventTap functionality. */
    static void runFullDiagnostic()
    {
        std::cout << "CGEventTap Comprehensive Diagnostic" << std::endl;
        std::cout << "===================================" << std::endl;
        std::cout << std::endl;
        
        // 1. Basic system information
        checkSystemInfo();
        
        // 2. Permission checks
        checkPermissions();
        
        // 3. Try different CGEventTapCreate parameters
        testDifferentParameters();
        
        // 4. Process and thread information
        checkProcessInfo();
        
        // 5. Security and SIP status
        checkSecuritySettings();
        
        // 6. Recommendations
        showRecommendations();
    }
    
private:
    //==============================================================================
    static void checkSystemInfo()
    {
        std::cout << "=== System Information ===" << std::endl;
        
        // macOS version
        NSOperatingSystemVersion version = [[NSProcessInfo processInfo] operatingSystemVersion];
        std::cout << "macOS Version: " << version.majorVersion << "." 
                  << version.minorVersion << "." << version.patchVersion << std::endl;
        
        // Process information
        NSString* processName = [[NSProcessInfo processInfo] processName];
        std::cout << "Process Name: " << [processName UTF8String] << std::endl;
        std::cout << "Process ID: " << getpid() << std::endl;
        
        // Architecture
        #if defined(__x86_64__)
            std::cout << "Architecture: x86_64 (Intel)" << std::endl;
        #elif defined(__arm64__)
            std::cout << "Architecture: arm64 (Apple Silicon)" << std::endl;
        #else
            std::cout << "Architecture: Unknown" << std::endl;
        #endif
        
        std::cout << std::endl;
    }
    
    static void checkPermissions()
    {
        std::cout << "=== Permission Analysis ===" << std::endl;
        
        // Test 1: Basic permission check with minimal parameters
        std::cout << "Test 1: Basic permission check..." << std::endl;
        testBasicPermissions();
        
        // Test 2: Different event types
        std::cout << "Test 2: Testing different event types..." << std::endl;
        testDifferentEventTypes();
        
        // Test 3: Different tap locations
        std::cout << "Test 3: Testing different tap locations..." << std::endl;
        testDifferentTapLocations();
        
        std::cout << std::endl;
    }
    
    static void testBasicPermissions()
    {
        auto testCallback = [](CGEventTapProxy, CGEventType, CGEventRef event, void*) -> CGEventRef {
            return event;
        };
        
        // Most basic test - listen only, minimal event mask
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
            std::cout << "✅ Basic permission test: SUCCESS" << std::endl;
            CFRelease(testTap);
        }
        else
        {
            std::cout << "❌ Basic permission test: FAILED" << std::endl;
            std::cout << "   This indicates Input Monitoring permission is not granted or available." << std::endl;
        }
    }
    
    static void testDifferentEventTypes()
    {
        auto testCallback = [](CGEventTapProxy, CGEventType, CGEventRef event, void*) -> CGEventRef {
            return event;
        };
        
        struct EventTest {
            CGEventMask mask;
            const char* description;
        };
        
        EventTest tests[] = {
            { CGEventMaskBit(kCGEventNull), "Null events" },
            { CGEventMaskBit(kCGEventKeyDown), "Key down events" },
            { CGEventMaskBit(kCGEventKeyUp), "Key up events" },
            { CGEventMaskBit(kCGEventFlagsChanged), "Modifier flag changes" },
            { CGEventMaskBit(kCGEventKeyDown) | CGEventMaskBit(kCGEventKeyUp), "Key down + up" },
            { kCGEventMaskForAllEvents, "All events" }
        };
        
        for (const auto& test : tests)
        {
            auto testTap = CGEventTapCreate(
                kCGSessionEventTap,
                kCGTailAppendEventTap,
                kCGEventTapOptionListenOnly,
                test.mask,
                testCallback,
                nullptr
            );
            
            if (testTap != nullptr)
            {
                std::cout << "✅ " << test.description << ": SUCCESS" << std::endl;
                CFRelease(testTap);
            }
            else
            {
                std::cout << "❌ " << test.description << ": FAILED" << std::endl;
            }
        }
    }
    
    static void testDifferentTapLocations()
    {
        auto testCallback = [](CGEventTapProxy, CGEventType, CGEventRef event, void*) -> CGEventRef {
            return event;
        };
        
        struct LocationTest {
            CGEventTapLocation location;
            const char* description;
        };
        
        LocationTest tests[] = {
            { kCGHIDEventTap, "HID Event Tap" },
            { kCGSessionEventTap, "Session Event Tap" },
            { kCGAnnotatedSessionEventTap, "Annotated Session Event Tap" }
        };
        
        for (const auto& test : tests)
        {
            auto testTap = CGEventTapCreate(
                test.location,
                kCGTailAppendEventTap,
                kCGEventTapOptionListenOnly,
                CGEventMaskBit(kCGEventKeyDown),
                testCallback,
                nullptr
            );
            
            if (testTap != nullptr)
            {
                std::cout << "✅ " << test.description << ": SUCCESS" << std::endl;
                CFRelease(testTap);
            }
            else
            {
                std::cout << "❌ " << test.description << ": FAILED" << std::endl;
            }
        }
    }
    
    static void testDifferentParameters()
    {
        std::cout << "=== Parameter Testing ===" << std::endl;
        
        auto testCallback = [](CGEventTapProxy, CGEventType, CGEventRef event, void*) -> CGEventRef {
            return event;
        };
        
        struct ParameterTest {
            CGEventTapLocation location;
            CGEventTapPlacement placement;
            CGEventTapOptions options;
            CGEventMask eventMask;
            const char* description;
        };
        
        ParameterTest tests[] = {
            // Most permissive first
            { kCGSessionEventTap, kCGTailAppendEventTap, kCGEventTapOptionListenOnly, 
              CGEventMaskBit(kCGEventNull), "Most permissive (Null events, listen-only)" },
            
            // Standard configurations
            { kCGSessionEventTap, kCGTailAppendEventTap, kCGEventTapOptionListenOnly, 
              CGEventMaskBit(kCGEventKeyDown), "Listen-only key events" },
            
            { kCGSessionEventTap, kCGHeadInsertEventTap, kCGEventTapOptionListenOnly, 
              CGEventMaskBit(kCGEventKeyDown), "Head insert, listen-only" },
            
            { kCGSessionEventTap, kCGHeadInsertEventTap, kCGEventTapOptionDefault, 
              CGEventMaskBit(kCGEventKeyDown), "Head insert, default options" },
            
            // More restrictive
            { kCGHIDEventTap, kCGHeadInsertEventTap, kCGEventTapOptionDefault, 
              CGEventMaskBit(kCGEventKeyDown), "HID tap, default options" },
        };
        
        for (const auto& test : tests)
        {
            std::cout << "Testing: " << test.description << std::endl;
            
            auto testTap = CGEventTapCreate(
                test.location,
                test.placement,
                test.options,
                test.eventMask,
                testCallback,
                nullptr
            );
            
            if (testTap != nullptr)
            {
                std::cout << "✅ SUCCESS" << std::endl;
                CFRelease(testTap);
            }
            else
            {
                std::cout << "❌ FAILED" << std::endl;
            }
            std::cout << std::endl;
        }
    }
    
    static void checkProcessInfo()
    {
        std::cout << "=== Process Information ===" << std::endl;
        
        // Check if running as root
        if (geteuid() == 0)
        {
            std::cout << "⚠️  Running as root - this may cause CGEventTap issues" << std::endl;
        }
        else
        {
            std::cout << "✅ Running as regular user (UID: " << geteuid() << ")" << std::endl;
        }
        
        // Check console vs GUI application
        if ([NSApplication sharedApplication] == nil)
        {
            std::cout << "⚠️  No NSApplication - running as console app may limit CGEventTap" << std::endl;
        }
        else
        {
            std::cout << "✅ NSApplication available - GUI app context" << std::endl;
        }
        
        // Check run loop
        CFRunLoopRef currentRunLoop = CFRunLoopGetCurrent();
        if (currentRunLoop != nullptr)
        {
            std::cout << "✅ CFRunLoop available" << std::endl;
        }
        else
        {
            std::cout << "❌ No CFRunLoop - CGEventTap requires run loop" << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    static void checkSecuritySettings()
    {
        std::cout << "=== Security Settings ===" << std::endl;
        
        // Check SIP status (if possible)
        std::cout << "Note: System Integrity Protection (SIP) status affects CGEventTap" << std::endl;
        std::cout << "Run 'csrutil status' in Terminal to check SIP status" << std::endl;
        
        // Check if running in sandbox
        NSString* sandboxPath = [NSHomeDirectory() stringByAppendingPathComponent:@"Library/Containers"];
        if ([[NSFileManager defaultManager] fileExistsAtPath:sandboxPath])
        {
            std::cout << "⚠️  App may be sandboxed - this can prevent CGEventTap" << std::endl;
        }
        else
        {
            std::cout << "✅ App does not appear to be sandboxed" << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    static void showRecommendations()
    {
        std::cout << "=== Recommendations ===" << std::endl;
        std::cout << std::endl;
        
        std::cout << "If CGEventTapCreate is still returning nullptr:" << std::endl;
        std::cout << std::endl;
        
        std::cout << "1. 🔐 PERMISSION ISSUES:" << std::endl;
        std::cout << "   • Go to System Preferences > Security & Privacy > Privacy > Input Monitoring" << std::endl;
        std::cout << "   • Remove your app from the list if present" << std::endl;
        std::cout << "   • Run your app again to trigger permission dialog" << std::endl;
        std::cout << "   • Grant permission when prompted" << std::endl;
        std::cout << std::endl;
        
        std::cout << "2. 🏃‍♂️ PROCESS CONTEXT:" << std::endl;
        std::cout << "   • Ensure you're running as a GUI application (not console)" << std::endl;
        std::cout << "   • Don't run as root/sudo" << std::endl;
        std::cout << "   • Make sure NSApplication is initialized" << std::endl;
        std::cout << std::endl;
        
        std::cout << "3. 🛡️ SECURITY RESTRICTIONS:" << std::endl;
        std::cout << "   • Check if app is sandboxed (sandboxing prevents CGEventTap)" << std::endl;
        std::cout << "   • Verify SIP (System Integrity Protection) settings" << std::endl;
        std::cout << "   • Some enterprise security software blocks CGEventTap" << std::endl;
        std::cout << std::endl;
        
        std::cout << "4. 🔄 ALTERNATIVE APPROACHES:" << std::endl;
        std::cout << "   • Use Carbon RegisterEventHotKey API instead" << std::endl;
        std::cout << "   • Try different CGEventTapCreate parameters" << std::endl;
        std::cout << "   • Consider app-specific hotkeys instead of global ones" << std::endl;
        std::cout << std::endl;
        
        std::cout << "5. 🐛 DEBUGGING STEPS:" << std::endl;
        std::cout << "   • Test with a minimal example app" << std::endl;
        std::cout << "   • Check Console.app for security-related error messages" << std::endl;
        std::cout << "   • Try running from different locations (not Downloads folder)" << std::endl;
        std::cout << "   • Test on different user accounts" << std::endl;
        std::cout << std::endl;
        
        std::cout << "6. 🍎 MACOS VERSION SPECIFIC:" << std::endl;
        std::cout << "   • macOS Sequoia (15.0+): Enhanced security may block CGEventTap" << std::endl;
        std::cout << "   • macOS Sonoma (14.0+): Stricter permission enforcement" << std::endl;
        std::cout << "   • macOS Ventura (13.0+): New privacy controls" << std::endl;
        std::cout << std::endl;
    }
    
public:
    //==============================================================================
    /** Quick test to check if CGEventTap is working at all. */
    static bool quickTest()
    {
        std::cout << "CGEventTap Quick Test" << std::endl;
        std::cout << "=====================" << std::endl;
        
        auto testCallback = [](CGEventTapProxy, CGEventType, CGEventRef event, void*) -> CGEventRef {
            return event;
        };
        
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
            std::cout << "✅ CGEventTap is working!" << std::endl;
            CFRelease(testTap);
            return true;
        }
        else
        {
            std::cout << "❌ CGEventTap failed - run full diagnostic for details" << std::endl;
            return false;
        }
    }
    
    /** Shows the current permission status in detail. */
    static void showDetailedPermissionStatus()
    {
        std::cout << "Detailed Permission Status" << std::endl;
        std::cout << "==========================" << std::endl;
        
        // Test with different permission levels
        struct PermissionTest {
            CGEventTapOptions options;
            const char* description;
            const char* permissionLevel;
        };
        
        PermissionTest tests[] = {
            { kCGEventTapOptionListenOnly, "Listen-only tap", "Basic" },
            { kCGEventTapOptionDefault, "Default tap", "Full Input Monitoring" }
        };
        
        auto testCallback = [](CGEventTapProxy, CGEventType, CGEventRef event, void*) -> CGEventRef {
            return event;
        };
        
        for (const auto& test : tests)
        {
            auto testTap = CGEventTapCreate(
                kCGSessionEventTap,
                kCGTailAppendEventTap,
                test.options,
                CGEventMaskBit(kCGEventKeyDown),
                testCallback,
                nullptr
            );
            
            std::cout << test.description << " (" << test.permissionLevel << "): ";
            
            if (testTap != nullptr)
            {
                std::cout << "✅ GRANTED" << std::endl;
                CFRelease(testTap);
            }
            else
            {
                std::cout << "❌ DENIED" << std::endl;
            }
        }
        
        std::cout << std::endl;
        std::cout << "If both tests fail, Input Monitoring permission is not granted." << std::endl;
        std::cout << "If only 'Default tap' fails, you have partial permissions." << std::endl;
    }
}; 